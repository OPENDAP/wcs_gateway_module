// WCSRequest.h

#ifndef I_WCSRequest_H
#define I_WCSRequest_H 1

#include <vector>
#include <string>

using std::vector ;
using std::string ;

class WCSFile ;

class WCSRequest
{
private:
    vector<string>		_hdr_list ;
    static size_t		save_raw_http_headers( void *ptr, size_t size,
						       size_t nmemb,
						       void *resp_hdrs ) ;
public:
    				WCSRequest() {} ;
				~WCSRequest() {} ;
    string			make_request( const WCSFile &file ) ;
} ;

#endif // I_WCSRequest_H

