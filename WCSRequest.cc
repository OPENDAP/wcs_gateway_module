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

// For making the WCS request we use curl
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include <stdio.h>
#include <stdlib.h>

#include "WCSRequest.h"
#include "WCSFile.h"
#include "WCSCache.h"
#include "WCSException.h"
#include "BESDebug.h"
#include "TheBESKeys.h"
#include "config.h"

/* function passed to libcurl that knows how to save the raw http headers in the response
 */
size_t 
WCSRequest::save_raw_http_headers( void *ptr, size_t size,
				   size_t nmemb, void *resp_hdrs )
{
    vector<string> *hdrs = static_cast<vector<string> *>(resp_hdrs);

    // Grab the header, minus the trailing newline. Or \r\n pair.
    string complete_line;
    if (*(static_cast<char*>(ptr) + size * (nmemb-2)) == '\r')
        complete_line.assign(static_cast<char *>(ptr), size * nmemb - 2);
    else
        complete_line.assign(static_cast<char *>(ptr), size * nmemb - 1);
        
    // Store all non-empty headers that are not HTTP status codes
    hdrs->push_back( complete_line ) ;

    return size * nmemb;
}

/** @brief make the WCS request against the given information
 *
 * function that makes a WCS request using libcurl given the WCS request url, the target
 * name of the file for the response, the type of data being returned, and the maximum
 * amount of time (in seconds) that a response file can remain in the cache.
 *
 * First it is determined if the response file already exists in the WCS cache. The cache
 * directory is retrieved from the BES configuration file using the parameter
 * WCS.CacheDir. If not set or empty then /tmp is used. The target response file can
 * remain in the cache for cacheTime seconds. If the target does not exist or the cache
 * time has expired, then the request is made, otherwise the response file in the cache is
 * used.
 *
 * A temporary target file is used to store the information. If the request was successful
 * and the resulting file does not contain error information then this temporary file is
 * moved to the real target file.
 *
 * @param url WCS request url
 * @param name target file name of the WCS request response
 * @param type target file name data type, like nc or hdf4
 * @param cacheTime maximum time a response can remain in the cache
 * @return target response file name
 * @throws WCSException if there is a problem making the WCS request or the request fails
 */
string
WCSRequest::make_request( const string &url, const string &name,
			  const string &type, const string &cacheTime )
{
    bool found = false ;
    string cacheDir = TheBESKeys::TheKeys()->get_key( "WCS.CacheDir", found ) ;
    if( !found || cacheDir.empty() )
    {
	cacheDir = "/tmp" ;
    }

    if( url.empty() )
    {
	string err = "WCS Request URL is empty" ;
	throw WCSException( err, __FILE__, __LINE__ ) ;
    }
    if( name.empty() )
    {
	string err = "WCS Request target name is empty" ;
	throw WCSException( err, __FILE__, __LINE__ ) ;
    }
    if( type.empty() )
    {
	string err = "WCS Request target type is empty" ;
	throw WCSException( err, __FILE__, __LINE__ ) ;
    }

    string tmp_target = cacheDir + "/" + name + ".tmp" ;
    string target = cacheDir + "/" + name + "." + type ;

    BESDEBUG( "wcs", "WCSRequest::make_request - request = " << url << endl )
    BESDEBUG( "wcs", "WCSRequest::make_request - target = " << target << endl )
    BESDEBUG( "wcs", "WCSRequest::make_request - tmp target = " << tmp_target << endl )

    WCSCache cache( cacheDir, cacheTime, target ) ;
    bool is_cached = cache.is_cached( ) ;

    if( !is_cached )
    {
	BESDEBUG( "wcs", "WCSRequest::make_request - making the wcs request" << endl )
	// clear the header response list for this run
	_hdr_list.clear() ;

	CURL *d_curl = curl_easy_init();
	if( !d_curl )
	{
	    string err = "Unable to initialize curl" ;
	    throw WCSException( err, __FILE__, __LINE__ ) ;
	}

	char d_error_buffer[CURL_ERROR_SIZE] ;
	curl_easy_setopt( d_curl, CURLOPT_ERRORBUFFER, d_error_buffer ) ;

	// No proxy
	curl_easy_setopt(d_curl, CURLOPT_PROXY, 0);

	// This means libcurl will use Basic, Digest, GSS Negotiate, or NTLM,
	// choosing the the 'safest' one supported by the server.
	// This requires curl 7.10.6 which is still in pre-release. 07/25/03 jhrg
	curl_easy_setopt( d_curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_ANY ) ;

	curl_easy_setopt( d_curl, CURLOPT_NOPROGRESS, 1 ) ;
	curl_easy_setopt( d_curl, CURLOPT_NOSIGNAL, 1 ) ;
	curl_easy_setopt( d_curl, CURLOPT_HEADERFUNCTION, WCSRequest::save_raw_http_headers ) ;
	curl_easy_setopt( d_curl, CURLOPT_WRITEHEADER, &_hdr_list ) ;

	// We aren't using SSL
	curl_easy_setopt(d_curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(d_curl, CURLOPT_SSL_VERIFYHOST, 0);

	curl_easy_setopt( d_curl, CURLOPT_URL, url.c_str() ) ;

	FILE *stream = fopen( tmp_target.c_str(), "w+" ) ;
#ifdef WIN32
	curl_easy_setopt( d_curl, CURLOPT_FILE, stream ) ;
	curl_easy_setopt( d_curl, CURLOPT_WRITEFUNCTION, &fwrite ) ;
#else
	curl_easy_setopt( d_curl, CURLOPT_FILE, stream ) ;
#endif

	// set some information in the header
	struct curl_slist *req_headers = 0 ;
	string pragma = "Pragma:"  ;
	string ua = (string)"User-Agent: " + PACKAGE_NAME + "/" + PACKAGE_VERSION ;
	req_headers = curl_slist_append( req_headers, pragma.c_str() ) ;
	req_headers = curl_slist_append( req_headers, ua.c_str() ) ;
	curl_easy_setopt( d_curl, CURLOPT_HTTPHEADER, req_headers ) ;

	CURLcode res = curl_easy_perform(d_curl);
	if (res != 0)
	{
	    // close the temporary target file
	    fclose( stream ) ;

	    // remove the temporary target file here
	    if( remove( tmp_target.c_str() ) != 0 )
	    {
		perror( "Error deleting temporary target file" ) ;
	    }
	    string err = (string )"WCS request failed: " + d_error_buffer ;
	    throw WCSException( err, __FILE__, __LINE__ ) ;
	}

	// Free the header list and null the value in d_curl.
	curl_slist_free_all( req_headers ) ;
	curl_easy_setopt( d_curl, CURLOPT_HTTPHEADER, 0 ) ;

	long status;
	res = curl_easy_getinfo(d_curl, CURLINFO_HTTP_CODE, &status);
	if (res != 0)
	{
	    // close the temporary target file
	    fclose( stream ) ;

	    // remove the temporary target file here
	    if( remove( tmp_target.c_str() ) != 0 )
	    {
		perror( "Error deleting temporary target file" ) ;
	    }
	    string err = (string )"WCS request failed: " + d_error_buffer ;
	    throw WCSException( err, __FILE__, __LINE__ ) ;
	}

	curl_easy_cleanup(d_curl);

	// Determine if the resulting response file contains error information.
	bool request_status = false ;
	bool xml_error = false ;
	vector<string>::const_iterator iter = _hdr_list.begin() ;
	while( iter != _hdr_list.end() )
	{
	    string hdr_line = (*iter) ;
	    BESDEBUG( "wcs", "WCS Response Header: " << hdr_line << endl )

	    // if we found "200 OK" then the request was successfull, but
	    // still need to check if the content type is xml, which would
	    // signify an exception condition.
	    if( hdr_line.find( "200 OK" ) != string::npos )
	    {
		request_status = true ;
	    }

	    // this would be found after the "200 OK" so not worry about
	    // being set to true again
	    if( hdr_line.find( "text/xml" ) != string::npos )
	    {
		request_status = false ;
		xml_error = true ;
	    }

	    // move on to the next header
	    iter++ ;
	}

	if( request_status == false )
	{
	    // close the temporary target
	    fclose( stream ) ;
	    
	    // get the error information from the temoorary file
	    string err ;
	    if( xml_error )
	    {
		WCSException::read_xml_error( tmp_target, err, url ) ;
	    }
	    else
	    {
		WCSException::read_error( tmp_target, err, url ) ;
	    }

	    // need to remove the temporary target file here
	    if( remove( tmp_target.c_str() ) != 0 )
	    {
		perror( "Error deleting temporary target file" ) ;
	    }
	    throw WCSException( err, __FILE__, __LINE__ ) ;
	}

	int result= rename( tmp_target.c_str() , target.c_str() );
	if( result != 0 )
	{
	    // close the temporary target
	    fclose( stream ) ;
	    
	    string err = "Unable to rename temporary target file "
			 + tmp_target + " to " + target ;
	    throw WCSException( err, __FILE__, __LINE__ ) ;
	}

	fclose( stream ) ;

	BESDEBUG( "wcs", "WCSRequest::make_request - succeeded" << endl )
    }
    else
    {
	BESDEBUG( "wcs", "WCSRequest::make_request - target cached" << endl )
    }

    return target ;
}

