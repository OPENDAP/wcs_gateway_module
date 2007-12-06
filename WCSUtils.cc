// WCSUtils.cc

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

#include "WCSUtils.h"
#include "BESUtil.h"
#include "TheBESKeys.h"
#include "BESHandlerException.h"

map<string,string> WCSUtils::type_list ;

void
WCSUtils::break_apart_types( const string &types )
{
    bool done = false ;
    string::size_type start = 0 ;
    while( !done )
    {
	string::size_type semi = types.find( ";", start ) ;
	if( semi == string::npos )
	{
	    done = true ;
	}
	else
	{
	    string::size_type colon = types.find( ":", start ) ;
	    if( colon == string::npos || colon > semi )
	    {
		string err = "Malformed wcs type list: " + types ;
		throw BESHandlerException( err, __FILE__, __LINE__ ) ;
	    }
	    else
	    {
		string wcs_type =
		    BESUtil::lowercase( types.substr( start, colon - start ) ) ;
		string bes_type = types.substr( colon+1, semi - colon - 1 ) ;
		type_list[wcs_type] = bes_type ;
	    }
	    start = semi + 1 ;
	    if( start == types.length() )
	    {
		done = true ;
	    }
	}
    }
}

/** @brief convert the wcs format to the bes type
 *
 * Use the configuration file parameter WCS.TypeList, which is formated as
 * wcs_type:bes_type; If the list does not exist then assume that it is a
 * direct mappiing.
 *
 * If the wcs_type cannot be found in the list then simply return the
 * wcs_type
 *
 * @param wcs_type wcs format to convert to bes format
 * @return corresponding bes_type
 */
string
WCSUtils::convert_wcs_type( const string &wcs_type )
{
    if( WCSUtils::type_list.size() == 0 )
    {
	bool found = false ;
	string types = TheBESKeys::TheKeys()->get_key( "WCS.TypeList", found ) ;
	if( found && !types.empty() )
	{
	    WCSUtils::break_apart_types( types ) ;
	}
    }
    string wcs_ltype = BESUtil::lowercase( wcs_type ) ;
    string bes_type = type_list[wcs_ltype] ;
    if( bes_type.empty() )
	bes_type = wcs_type ;
    return bes_type ;
}

/** @brief determine if the given url is a valid WCS request URL.
 *
 * looks for the following elements in the request:
 * 1. the request URL must begin with http://
 * 2. service=wcs - converted to all lower case, the service being requested must be wcs
 * 3. version=vers_number - a version number must be provided, not validating the version
 * 4. request=getCoverage - only thye getCoverage request is handled by the WCS module
 * 5. coverage=wcs_request - the coverage parameter contains the wcs request url
 * 6. format=data_format - the data format of the WCS request response file
 *
 * @param url the WCS request URL
 * @param target the name of the target file (not including the cache directory)
 * @param format the data type format of the WCS response
 * @return an error string if any problems are encountered. No exceptions are thrown
 */
string
WCSUtils::validate_url( const string &url, string &target, string &format )
{
    // look for the ?, then mandatory fields service=WCS,
    // version=<something>, request=getCoverage, format=<something>
    // http://data.laits.gmu.edu/cgi-bin/ACCESS/wcs300?service=WCS&amp;version=1.0.0&amp;request=getCoverage&amp;coverage=/home/mmin/testdata/MOD021KM.A2002248.0140.003.2002248112526.hdf:Swath:MODIS_SWATH_Type_L1B:EV_1KM_Emissive&amp;crs=EPSG:4326&amp;bbox=-40,29,-39,30&amp;format=netCDF&amp;resx=0.01&amp;resy=0.01&amp;Band_1KM_Emissive=1

    // spaces are not allowed in a url
    string::size_type spaces = url.find( " " ) ;
    if( spaces != string::npos )
    {
	return "Invalid WCS request, contains spaces" ;
    }

    // remove the double quotes
    if( (url[0] != '"' ) || (url[url.size() - 1] != '"' ) )
    {
	return "Invalid WCS request, must be enclosed in double quotes" ;
    }
    string newurl = url.substr( 1, url.size() - 2 ) ;

    // convert it all to lower case
    newurl = BESUtil::lowercase( newurl ) ;

    // url must begin with http://
    if( newurl.compare( 0, 7, "http://" ) )
    {
	return "Invalid WCS request, must begin with http://" ;
    }

    string::size_type qmark = newurl.find( "?" ) ;
    if( qmark == string::npos )
    {
	return "Invalid WCS request, missing parameter character '?'" ;
    }

    // a wcs request must have service=wcs. It is mandatory.
    string::size_type service = newurl.find( "service=wcs", qmark+1 ) ;
    if( service == string::npos )
    {
	return "Invalid WCS request, missing service name 'service=WCS'" ;
    }

    // a wcs request must have version information. It is mandatory.
    string::size_type version = newurl.find( "version=", qmark+1 ) ;
    if( version == string::npos )
    {
	return "Invalid WCS request, missing version information" ;
    }

    // a wcs request must have request=getcoverage. It is mandatory.
    string::size_type request = newurl.find( "request=getcoverage", qmark+1 ) ;
    if( request == string::npos )
    {
	return "Invalid WCS request, request should be for getCoverage" ;
    }

    // find the coverage information. This will tell us the name of the
    // target file
    string::size_type coverage = newurl.find( "coverage=" ) ;
    if( coverage == string::npos )
    {
	return "Invalid WCS request, missing coverage information" ;
    }
    string::size_type eq_index = newurl.find( "=", coverage ) ;
    if( eq_index == string::npos )
    {
	return "Invalid WCS request, malformed coverage information" ;
    }
    string::size_type col_index = newurl.find( ":", eq_index ) ;
    string::size_type cov_amp_index = newurl.find( "&", eq_index ) ;
    if( cov_amp_index == string::npos )
    {
	cov_amp_index = newurl.length() ;
    }
    if( col_index == string::npos || col_index > cov_amp_index )
    {
	col_index = cov_amp_index ;
    }
    // find the last slash in the coverage file name. May not have one
    string::size_type slash_index = newurl.rfind( "/", col_index ) ;
    if( slash_index == string::npos || slash_index < eq_index )
    {
	slash_index = eq_index ;
    }
    // This might include a different file extension than the target type.
    // For example, the coverage file might have a .hdf extension but the
    // target type is nc. So the resulting target will be .hdf.nc, which is
    // fine
    target = newurl.substr( slash_index+1, col_index - slash_index - 1 ) ;

    // find the format
    string::size_type format_index = newurl.find( "format" ) ;
    if( format_index == string::npos )
    {
	return "Invalid WCS request, missing format information" ;
    }
    eq_index = newurl.find( "=", format_index ) ;
    if( eq_index == string::npos )
    {
	return "Invalid WCS request, malformed format information, missing =" ;
    }
    string::size_type amp_index = newurl.find( "&", eq_index ) ;
    if( amp_index == string::npos )
    {
	amp_index = newurl.length() ;
    }
    // url and newurl are the same, except for case and the leading and
    // trailing double quote. We want the case for the format conversion.
    string wcs_format = url.substr( eq_index+2, amp_index - eq_index - 1 ) ;
    format = WCSUtils::convert_wcs_type( wcs_format ) ;

    // the format will be added later inside of WCSRequest::make_request, so
    // don't add it here. pcw 06/13/07
    //target = target + "." + format ;

    return "" ;
}

