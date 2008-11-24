// WCSContainer.cc

// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of wcs_module, A C++ module that can be loaded in to
// the OPeNDAP Back-End Server (BES) and is able to handle wcs requests.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Patrick West <pwest@ucar.edu>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      pcw       Patrick West <pwest@ucar.edu>

#include "WCSContainer.h"
#include "WCSGatewayRequest.h"
#include "WCSGatewayUtils.h"
#include "BESSyntaxUserError.h"
#include "BESInternalError.h"
#include "BESDebug.h"

/** @brief Creates an instances of WCSContainer with symbolic name and real
 * name, which is the WCS request.
 *
 * The real_name is the WCS request URL. Within this url we expect to find
 * certain elements: service=WCS, version=x, request=getCoverage,
 * format=netCDF|HDF.
 *
 * For example, a WCS request might look like this:
 *
 * http://data.laits.gmu.edu/cgi-bin/ACCESS/wcs300?service=WCS&amp;version=1.0.0&amp;request=getCoverage&amp;coverage=/home/mmin/testdata/MOD021KM.A2002248.0140.003.2002248112526.hdf:Swath:MODIS_SWATH_Type_L1B:EV_1KM_Emissive&amp;crs=EPSG:4326&amp;bbox=-40,29,-39,30&amp;format=netCDF&amp;resx=0.01&amp;resy=0.01&amp;Band_1KM_Emissive=1
 *
 * @param sym_name symbolic name representing this WCS container
 * @param real_name the WCS request URL
 * @throws BESSyntaxUserError if the url does not validate
 * @see WCSGatewayUtils
 */
WCSContainer::WCSContainer( const string &sym_name,
			    const string &real_name )
    : BESContainer( sym_name, "", "" ), _response( 0 )
{
    // The real name passed is the wcs request string. Within the request is
    // the name of the file we will use to form the cached file name, and the
    // type of file being cached. We also need to verify that this is a
    // well formed WCS request.
    string type ;
    string err = WCSGatewayUtils::validate_url( real_name, type ) ;
    if( !err.empty() )
    {
	throw BESSyntaxUserError( err, __FILE__, __LINE__ ) ;
    }

    // remove the leading and trailing double quotes
    /* With the xml request documents we don't have quotes anymore, so no
     * need to do this. pcw 09/09/2008
    string new_real_name = real_name.substr( 1, real_name.size() - 2 ) ;
    set_real_name( new_real_name ) ;
    */
    set_real_name( real_name ) ;

    set_container_type( type ) ;
}

WCSContainer::WCSContainer( const WCSContainer &copy_from )
    : BESContainer( copy_from ),
      _response( copy_from._response )
{
    // we can not make a copy of this container once the WCS request has
    // been made
    if( _response )
    {
	string err = (string)"The Container has already been accessed, "
	             + "can not create a copy of this container." ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }
}

void
WCSContainer::_duplicate( WCSContainer &copy_to )
{
    if( copy_to._response )
    {
	string err = (string)"The Container has already been accessed, "
	             + "can not duplicate this resource." ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }
    copy_to._response = _response ;
    BESContainer::_duplicate( copy_to ) ;
}

BESContainer *
WCSContainer::ptr_duplicate( )
{
    WCSContainer *container = new WCSContainer ;
    _duplicate( *container ) ;
    return container ;
}

WCSContainer::~WCSContainer()
{
    if( _response )
    {
	release() ;
    }
}

/** @brief access the WCS target response by making the WCS request
 *
 * @return full path to the WCS request response data file
 * @throws BESError if there is a problem making the WCS request
 */
string
WCSContainer::access()
{
    BESDEBUG( "wcs", "accessing " << get_real_name() << endl );
    string accessed ;
    if( !_response )
    {
	WCSGatewayRequest r ;
	_response = r.make_request( get_real_name() ) ;
	if( _response )
	    accessed = _response->get_file() ;
    }
    else
    {
	accessed = _response->get_file() ;
    }
    BESDEBUG( "wcs", "done accessing " << get_real_name() << " returning "
		     << accessed << endl );
    BESDEBUG( "wcs", "done accessing " << *this << endl );
    return accessed ;
}

/** @brief release the WCS cached resources
 *
 * Release the resource
 *
 * @return true if the resource is released successfully and false otherwise
 */
bool
WCSContainer::release()
{
    if( _response )
    {
	BESDEBUG( "wcs", "releasing wcs response" << endl );
	delete _response ;
	_response = 0 ;
    }

    BESDEBUG( "wcs", "done releasing wcs response" << endl );
    return true ;
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance along with information about
 * this container.
 *
 * @param strm C++ i/o stream to dump the information to
 */
void
WCSContainer::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "WCSContainer::dump - ("
			     << (void *)this << ")" << endl ;
    BESIndent::Indent() ;
    BESContainer::dump( strm ) ;
    if( _response )
    {
	strm << BESIndent::LMarg << "response file: " << _response->get_file()
	     << endl ;
	strm << BESIndent::LMarg << "response headers: " ;
	vector<string> *hdrs = _response->get_headers() ;
	if( hdrs )
	{
	    strm << endl ;
	    BESIndent::Indent() ;
	    vector<string>::const_iterator i = hdrs->begin() ;
	    vector<string>::const_iterator e = hdrs->end() ;
	    for( ; i != e; i++ )
	    {
		string hdr_line = (*i) ;
		strm << BESIndent::LMarg << hdr_line << endl ;
	    }
	    BESIndent::UnIndent() ;
	}
	else
	{
	    strm << "none" << endl ;
	}
    }
    else
    {
	strm << BESIndent::LMarg << "response not yet obtained" << endl ;
    }
    BESIndent::UnIndent() ;
}

