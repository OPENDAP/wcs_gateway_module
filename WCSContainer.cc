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
#include "WCSRequest.h"
#include "WCSUtils.h"
#include "BESContainerStorageException.h"
#include "TheBESKeys.h"

/** @brief Creates an instances of WCSContainer with symbolic name and real
 * name that is the WCS request
 *
 * The real_name is the WCS request URL. Within this url is the target file
 * name of the streamed response to the WCS request and the type of data being
 * streamed back (netCDF, etc...).
 *
 * The target data type is also specified in the WCS request with the format
 * parameter.
 *
 * For example, a WCS request might look like this:
 *
 * http://data.laits.gmu.edu/cgi-bin/ACCESS/wcs300?service=WCS&amp;version=1.0.0&amp;request=getCoverage&amp;coverage=/home/mmin/testdata/MOD021KM.A2002248.0140.003.2002248112526.hdf:Swath:MODIS_SWATH_Type_L1B:EV_1KM_Emissive&amp;crs=EPSG:4326&amp;bbox=-40,29,-39,30&amp;format=netCDF&amp;resx=0.01&amp;resy=0.01&amp;Band_1KM_Emissive=1
 *
 * The cache time, in seconds, is accessed from the BES configuration file.
 * If not set then set to 0.0.
 *
 * @param sym_name symbolic name representing this WCS container
 * @param real_name the WCS request
 * @throws BESContainerStorageException if the url does not validate
 * @see WCSUtils
 * @see BESKeys
 */
WCSContainer::WCSContainer( const string &sym_name,
			    const string &real_name )
    : BESContainer( sym_name, "", "" )
{
    // The real name passed is the wcs request string. Within the request is
    // the name of the file we will use to form the cached file name, and the
    // type of file being cached. We also need to verify that this is a
    // well formed WCS request.
    string type ;
    string err = WCSUtils::validate_url( real_name, _target, type ) ;
    if( !err.empty() )
    {
	throw BESContainerStorageException( err, __FILE__, __LINE__ ) ;
    }

    // remove the leading and trailing double quotes
    string new_real_name = real_name.substr( 1, real_name.size() - 2 ) ;
    set_real_name( new_real_name ) ;

    set_container_type( type ) ;

    // Retrieve the cache time from the BES configuration file
    bool found = false ;
    _cacheTime = TheBESKeys::TheKeys()->get_key( "WCS.CacheTime", found );
    if( !found || _cacheTime.empty() )
    {
	_cacheTime = "0.0" ;
    }
}

WCSContainer::WCSContainer( const WCSContainer &copy_from )
    : BESContainer( copy_from ),
      _target( copy_from._target ),
      _cacheTime( copy_from._cacheTime )
{
}

void
WCSContainer::_duplicate( WCSContainer &copy_to )
{
    copy_to._target = _target ;
    copy_to._cacheTime = _cacheTime ;
    BESContainer::_duplicate( copy_to ) ;
}

BESContainer *
WCSContainer::ptr_duplicate( )
{
    WCSContainer *container = new WCSContainer ;
    _duplicate( *container ) ;
    return container ;
}

/** @brief access the WCS target response by making the WCS request
 *
 * @return the target response
 * @throws WCSException if there is a problem making the WCS request
 */
string
WCSContainer::access()
{
    WCSRequest r ;
    return r.make_request( get_real_name(), _target,
			   get_container_type(), _cacheTime ) ;
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
    strm << BESIndent::LMarg << "target: " << _target << endl ;
    strm << BESIndent::LMarg << "cache time: " << _cacheTime << endl ;
    BESContainer::dump( strm ) ;
    BESIndent::UnIndent() ;
}

