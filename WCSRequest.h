// WCSRequest.h

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

#ifndef I_WCSRequest_H
#define I_WCSRequest_H 1

#include <vector>
#include <string>

using std::vector ;
using std::string ;

class WCSFile ;

/** @brief knows how to make a wcs request
 *
 * This class knows how to make a WCS request using libcurl given the wcs request, the
 * name of the target response file, the type of the target response, and the maximum time
 * a response is kept in the cache.
 *
 * @see WCSCache
 */
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
    string			make_request( const string &url,
					      const string &name,
					      const string &type,
					      const string &cacheTime ) ;
} ;

#endif // I_WCSRequest_H

