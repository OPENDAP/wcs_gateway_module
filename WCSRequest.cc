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

#include <sstream>

using std::istringstream ;

#include "HTTPCache.h"
#include "util.h"
#include "util_mit.h"

using namespace libdap ;

#include "WCSRequest.h"
#include "WCSUtils.h"
#include "WCSParams.h"
#include "BESInternalError.h"
#include "WCSError.h"
#include "BESDebug.h"
#include "config.h"

/* function passed to libcurl to save the raw http headers from the response
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
        
    // Store all non-empty headers
    if( !complete_line.empty() )
	hdrs->push_back( complete_line ) ;

    return size * nmemb;
}

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
 * @param cacheName out parameter where the name of the cached file will be
 * stored for access
 * @return FILE pointer for the cached file represented by cacheName
 * @throws BESInternalError if there is a problem making the WCS request or the request fails
 * @see WCSParams
 */
FILE *
WCSRequest::make_request( const string &url, const string &type,
			  string &cacheName )
{
    if( url.empty() )
    {
	string err = "WCS Request URL is empty" ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }
    if( type.empty() )
    {
	string err = "WCS Request target type is empty" ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }

    string cacheDir = WCSParams::GetCacheDir() ;
    int cacheTime = WCSParams::GetCacheTime() ;
    int cacheSize = WCSParams::GetCacheSize() ;
    int entrySize = WCSParams::GetEntrySize() ;

    BESDEBUG( "wcs", "WCSRequest::make_request" << endl )
    BESDEBUG( "wcs", "  request = " << url << endl )

    HTTPCache *cache = HTTPCache::instance( cacheDir, false ) ;
    if( !cache )
    {
	string err = "Unable to get cache for cache directory " + cacheDir ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }

    cache->set_cache_enabled( true ) ;
    cache->set_expire_ignored( false ) ;
    cache->set_max_size( cacheSize ) ;
    cache->set_max_entry_size( entrySize ) ;
    cache->set_always_validate( false ) ;

    FILE *s = 0 ;
    BESDEBUG( "wcs", "  is url in cache? ..." << endl )
    if( cache && cache->is_url_in_cache( url ) )
    {
	BESDEBUG( "wcs", "yes" << endl )
	BESDEBUG( "wcs", "  is url valid? ..." << endl )
        if( cache->is_url_valid( url ) )
	{
	    BESDEBUG( "wcs", "yes" << endl )
	    vector<string> headers ;
            s = cache->get_cached_response( url, headers, cacheName ) ;
	    return s ;
	}
	else
	{
	    BESDEBUG( "wcs", "no" << endl )
	}
    }
    else
    {
	BESDEBUG( "wcs", "no" << endl )
    }

    // if it's not in the cache, or isn't valid, or we couldn't get it from
    // the cache, then make the WCS request
    if( !s )
    {
	BESDEBUG( "wcs", "  making WCS request" << endl )

	CURL *d_curl = curl_easy_init();
	if( !d_curl )
	{
	    string err = "Unable to initialize curl" ;
	    throw BESInternalError( err, __FILE__, __LINE__ ) ;
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

	vector<string> headers ;
	curl_easy_setopt( d_curl, CURLOPT_WRITEHEADER, &headers ) ;

	// We aren't using SSL
	curl_easy_setopt(d_curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(d_curl, CURLOPT_SSL_VERIFYHOST, 0);

	curl_easy_setopt( d_curl, CURLOPT_URL, url.c_str() ) ;

	FILE *stream = 0 ;
	string wcs_temp = get_temp_file( stream ) ;
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
	    close_temp( stream, wcs_temp ) ;
	    string err = (string )"WCS request failed: " + d_error_buffer ;
	    throw BESInternalError( err, __FILE__, __LINE__ ) ;
	}

	// Free the header list and null the value in d_curl.
	curl_slist_free_all( req_headers ) ;
	curl_easy_setopt( d_curl, CURLOPT_HTTPHEADER, 0 ) ;

	long status;
	res = curl_easy_getinfo(d_curl, CURLINFO_HTTP_CODE, &status);
	if (res != 0)
	{
	    // close the temporary target file
	    close_temp( stream, wcs_temp ) ;
	    string err = (string )"WCS request failed: " + d_error_buffer ;
	    throw BESInternalError( err, __FILE__, __LINE__ ) ;
	}

	curl_easy_cleanup(d_curl);

	// Add the Expires header using the cacheTime
	time_t now = time( 0 ) ; // When was the request made (now).
	time_t expires = now + cacheTime ;
	string expires_str = "Expires: " + date_time_str( &expires ) ;
	headers.push_back( expires_str ) ;

	// Determine if the resulting response file contains error information.
	bool request_status = false ;
	bool xml_error = false ;
	vector<string>::const_iterator iter = headers.begin() ;
	while( iter != headers.end() )
	{
	    fflush( stream ) ;
	    string hdr_line = (*iter) ;
	    BESDEBUG( "wcs", "  response header: \"" << hdr_line << "\"" << endl )

	    // if we found "200 OK" then the request was successfull, but
	    // still need to check if the content type is xml, which would
	    // signify an exception condition.
	    if( hdr_line.find( "200 OK" ) != string::npos )
	    {
		BESDEBUG( "wcs", "  found 200 OK" << endl )
		request_status = true ;
	    }

	    // this would be found after the "200 OK" so not worry about
	    // being set to true again
	    if( hdr_line.find( "text/xml" ) != string::npos )
	    {
		BESDEBUG( "wcs", "  found xml error" << endl )
		request_status = false ;
		xml_error = true ;
	    }

	    // move on to the next header
	    iter++ ;
	}

	if( request_status == false )
	{
	    BESDEBUG( "wcs", " request FAILED" << endl )

	    // get the error information from the temoorary file
	    string err ;
	    if( xml_error )
	    {
		BESDEBUG( "wcs", " reading xml error" << endl )
		WCSError::read_xml_error( wcs_temp, err, url ) ;
	    }
	    else
	    {
		BESDEBUG( "wcs", " reading text error" << endl )
		WCSError::read_error( wcs_temp, err, url ) ;
	    }

	    // close the temporary target
	    close_temp( stream, wcs_temp ) ;

	    throw BESInternalError( err, __FILE__, __LINE__ ) ;
	}

	BESDEBUG( "wcs", "  rewinding stream" << endl )
	rewind( stream ) ;

	// cache the response
	BESDEBUG( "wcs", "  caching response" << endl )
	cache->cache_response( url, now, headers, stream ) ;

	// close the temporary target
	BESDEBUG( "wcs", "  closing temporary file" << endl )
	close_temp( stream, wcs_temp ) ;
	fclose( stream ) ;

	// get the cached response and return that instead of the temporary
	// file
	BESDEBUG( "wcs", "  getting cached response" << endl )
	vector<string> cached_headers ;
	s = cache->get_cached_response( url, cached_headers, cacheName ) ;

	BESDEBUG( "wcs", "  request SUCCEEDED" << endl )
    }

    BESDEBUG( "wcs", "  returning stream " << (void *)s << " - name " << cacheName << endl )
    return s ;
}

/** Open a temporary file and return its name. This method opens a temporary
    file using get_tempfile_template(). The FILE* \c stream is opened for
    both reads and writes; if it already exists (highly unlikely), it is
    truncated. If used on Unix, it's the callers responsibility to unlink the
    named file so that when all descriptors to it are closed, it will be
    deleted. On Win32 platforms, this method pushes the name of the temporary
    file onto a vector which is used during object destruction to delete all
    the temporary files.

    @note Delete the returned char* using delete[].

    A private method.

    @param stream A value-result parameter; the open file descriptor is
    returned via this parameter.
    @return The name of the temporary file.
    @exception InternalErr thrown if the FILE* could not be opened. */

string
WCSRequest::get_temp_file( FILE *&stream )
{
    // get_tempfile_template() uses new, must call delete
    char *wcs_temp = WCSUtils::get_tempfile_template("wcsXXXXXX");

    // Open truncated for update. NB: mkstemp() returns a file descriptor.
#if defined(WIN32) || defined(TEST_WIN32_TEMPS)
    stream = fopen(_mktemp(wcs_temp), "w+b");
#else
    stream = fdopen(mkstemp(wcs_temp), "w+");
#endif

    string wcs_temp_s = wcs_temp;
    delete[] wcs_temp; wcs_temp = 0;

    if( !stream )
    {
        string err = "Failed to open a temporary file for WCS Request" ;
        throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }

    return wcs_temp_s;
}

/** @brief Close the temporary file opened for storing the results of the
 * WCS Request.
 *
 * @param stream FILE ptr to close
 * @param name filename of the temporary file to remove
 */
void
WCSRequest::close_temp( FILE *stream, const string &name )
{
    int res = fclose( stream ) ;
    if( res )
        BESDEBUG( "wcs", "Failed to close temp file " << (void *)stream << endl)

    unlink( name.c_str() ) ;
}

