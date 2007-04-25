// WCSRequest.h

#ifndef I_WCSRequest_H
#define I_WCSRequest_H 1

#include <vector>
#include <string>
#include <map>

using std::vector ;
using std::string ;
using std::map ;

class WCSFile ;

class WCSRequest
{
private:
    static vector<string>	hdr_list ;
    static size_t		save_raw_http_headers( void *ptr, size_t size,
						       size_t nmemb,
						       void *resp_hdrs ) ;
public:
    static string		make_request( const WCSFile &file ) ;
} ;

#endif // I_WCSRequest_H

