// WCSRequest.h

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.edu> and Jose Garcia <jgarcia@ucar.edu>
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
// You can contact University Corporation for Atmospheric Research at
// 3080 Center Green Drive, Boulder, CO 80301
 
// (c) COPYRIGHT University Corporation for Atmospheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>
//      jgarcia     Jose Garcia <jgarcia@ucar.edu>

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
    void			read_error( FILE *f, string &err,
					    const string &url ) ;
public:
    				WCSRequest() {} ;
				~WCSRequest() {} ;
    string			make_request( const string &url,
					      const string &name,
					      const string &type,
					      const string &cacheTime ) ;
} ;

#endif // I_WCSRequest_H

