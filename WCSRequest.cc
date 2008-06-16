// WCSRequest.cc

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

#include <HTTPConnect.h>
#include <RCReader.h>
#include <Error.h>

using namespace libdap ;

#include <BESInternalError.h>
#include <BESDebug.h>

#include "WCSRequest.h"
#include "WCSError.h"
#include "WCSUtils.h"
#include "config.h"

/** @brief make the WCS request against the given information
 *
 * function that makes a WCS request using libcurl given the WCS request
 * url, the target name of the file for the response, the type of data being
 * returned, and the maximum amount of time (in seconds) that a response
 * file can remain in the cache.
 *
 * First it is determined if the response file already exists in the WCS
 * cache. The cache directory is retrieved from the BES configuration file
 * using the parameter WCS.CacheDir. If not set or empty then /tmp is used.
 * The target response file can remain in the cache for cacheTime seconds.
 * If the target does not exist or the cache time has expired, then the
 * request is made, otherwise the response file in the cache is used.
 *
 * A temporary target file is used to store the information. If the request
 * was successful and the resulting file does not contain error information
 * then this temporary file is moved to the real target file.
 *
 * @param url WCS request url
 * @param type target file name data type, like nc or hdf4
 * @return HTTPResponse pointer for the wcs request response
 * @throws BESInternalError if there is a problem making the WCS request or the request fails
 */
HTTPResponse *
WCSRequest::make_request( const string &url )
{
    if( url.empty() )
    {
	string err = "WCS Request URL is empty" ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }

    BESDEBUG( "wcs", "WCSRequest::make_request" << endl )
    BESDEBUG( "wcs", "  request = " << url << endl )

    HTTPConnect connect( RCReader::instance() ) ;
    connect.set_cache_enabled( false ) ;

    HTTPResponse *response = 0 ;
    try
    {
	response = connect.fetch_url( url ) ;
    }
    catch( Error &e )
    {
	BESInternalError err( e.get_error_message(), __FILE__, __LINE__ ) ;
	throw err ;
    }
    catch( ... )
    {
	string msg = (string)"Unknown exception fetching wcs request " + url ;
	BESInternalError err( msg, __FILE__, __LINE__ ) ;
	throw err ;
    }

    if( !response )
    {
	string msg = (string)"Response empty fetching wcs request " + url ;
	BESInternalError err( msg, __FILE__, __LINE__ ) ;
	throw err ;
    }

    // A WCS request is successful if we get data or if there is an xml
    // error, in some cases. Sometimes an XML error also returns an HTTP
    // error, but not always. So ... check the headers from the HTTPResponse
    // object.

    // WCS request errors could come as a regular HTML response (not found,
    // whatever) or could come as a WCS request XML error. So we have to
    // check to make sure which kind of error it is.
    bool request_status = true ;
    bool xml_error = false ;
    if( response->get_status() != 200 )
    {
	request_status = false ;
    }

    vector<string> *hdrs = response->get_headers() ;
    if( hdrs )
    {
	vector<string>::const_iterator i = hdrs->begin() ;
	vector<string>::const_iterator e = hdrs->end() ;
	for( ; i != e; i++ )
	{
	    string hdr_line = (*i) ;
	    if( hdr_line.find( "text/xml" ) != string::npos )
	    {
		BESDEBUG( "wcs", "  found xml error" << endl )
		request_status = false ;
		xml_error = true ;
	    }
	}
    }

    if( request_status == false )
    {
	BESDEBUG( "wcs", " request FAILED" << endl )

	// get the error information from the temoorary file
	string err ;
	if( xml_error )
	{
	    BESDEBUG( "wcs", " reading xml error" << endl )
	    WCSError::read_xml_error( response->get_file(), err, url ) ;
	}
	else
	{
	    BESDEBUG( "wcs", " reading text error" << endl )
	    WCSError::read_error( response->get_file(), err, url ) ;
	}

	// toss the response
	delete response ;
	response = 0 ;

	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }

    BESDEBUG( "wcs", "WCSRequest::make_request - done" << endl )

    return response ;
}

