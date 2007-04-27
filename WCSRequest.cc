#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <stdio.h>
#include <stdlib.h>

#include "WCSRequest.h"
#include "WCSFile.h"
#include "WCSCache.h"
#include "BESDebug.h"
#include "BESHandlerException.h"
#include "TheBESKeys.h"
#include "config.h"

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

string
WCSRequest::make_request( const WCSFile &file )
{
    string url = file.get_property( WCS_REQUEST ) ;
    string name = file.get_name() ;
    string type = file.get_property( WCS_RETURNTYPE ) ;
    string cacheTime = file.get_property( WCS_CACHETIME ) ;

    bool found = false ;
    string cacheDir = TheBESKeys::TheKeys()->get_key( "WCS.CacheDir", found ) ;
    if( !found || cacheDir.empty() )
    {
	cacheDir = "/tmp" ;
    }

    string tmp_target = cacheDir + "/" + name + ".tmp" ;
    string target = cacheDir + "/" + name + "." + type ;

    if( url.empty() )
    {
	string err = "WCS Request URL is empty" ;
	throw BESHandlerException( err, __FILE__, __LINE__ ) ;
    }
    if( name.empty() )
    {
	string err = "WCS Request target name is empty" ;
	throw BESHandlerException( err, __FILE__, __LINE__ ) ;
    }
    if( type.empty() )
    {
	string err = "WCS Request target type is empty" ;
	throw BESHandlerException( err, __FILE__, __LINE__ ) ;
    }
    BESDEBUG( "WCSRequest::make_request - request = " << url << endl )
    BESDEBUG( "WCSRequest::make_request - target = " << target << endl )
    BESDEBUG( "WCSRequest::make_request - tmp target = " << tmp_target << endl )

    WCSCache cache( cacheDir, cacheTime, target ) ;
    bool is_cached = cache.is_cached( ) ;

    if( !is_cached )
    {
	BESDEBUG( "WCSRequest::make_request - making the wcs request" << endl )
	// clear the header response list for this run
	_hdr_list.clear() ;

	CURL *d_curl = curl_easy_init();
	if( !d_curl )
	{
	    string err = "Unable to initialize curl" ;
	    throw BESHandlerException( err, __FILE__, __LINE__ ) ;
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

	FILE *stream = fopen( tmp_target.c_str(), "w" ) ;
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

	// Free the header list and null the value in d_curl.
	curl_slist_free_all( req_headers ) ;
	curl_easy_setopt( d_curl, CURLOPT_HTTPHEADER, 0 ) ;

	long status;
	res = curl_easy_getinfo(d_curl, CURLINFO_HTTP_CODE, &status);
	if (res != 0)
	{
	    string err = (string )"WCS request failed: " + d_error_buffer ;
	    throw BESHandlerException( err, __FILE__, __LINE__ ) ;
	}

	curl_easy_cleanup(d_curl);

	bool request_status = false ;
	vector<string>::const_iterator iter = _hdr_list.begin() ;
	while( iter != _hdr_list.end() )
	{
	    string hdr_line = (*iter) ;
	    BESDEBUG( "WCS Response Header: " << hdr_line << endl )
	    if( hdr_line.find( "200 OK" ) != string::npos )
		request_status = true ;
	    iter++ ;
	}

	if( request_status == false )
	{
	    // need to remove the temporary target file here
	    if( remove( tmp_target.c_str() ) != 0 )
	    {
		perror( "Error deleting temporary target file" ) ;
	    }
	    string err = "request failed, need to get the information" ;
	    throw BESHandlerException( err, __FILE__, __LINE__ ) ;
	}

	int result= rename( tmp_target.c_str() , target.c_str() );
	if( result != 0 )
	{
	    string err = "Unable to rename temporary target file "
			 + tmp_target + " to " + target ;
	    throw BESHandlerException( err, __FILE__, __LINE__ ) ;
	}

	BESDEBUG( "WCSRequest::make_request - succeeded" << endl )
    }
    else
    {
	BESDEBUG( "WCSRequest::make_request - target cached" << endl )
    }

    return target ;
}

