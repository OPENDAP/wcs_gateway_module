// WCSGatewayUtils.h

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

#ifndef I_WCSGatewayUtils_H
#define I_WCSGatewayUtils_H 1

#include <map>
#include <string>

using std::map ;
using std::string ;

/** @brief utility class for the WCS request mechanism
 *
 * Current static functions available are:
 *   convert_wcs_type - converts the format parameter in the WCS request to a type that is
 *   understood by the BES using the WCS.TypeList parameter in the BES configuration file
 *
 *   validate_url - validates the WCS request URL to make sure all the information
 *   required to make the request are present.
 */
class WCSGatewayUtils
{
private:
    static map<string,string>	type_list ;
    static void			break_apart_types( const string &types ) ;
public:
    static string		convert_wcs_type( const string &wcs_type ) ;

    static string		validate_url( const string &url,
					      string &format ) ;
    static char *		get_tempfile_template( char *file_template ) ;
} ;

#endif // I_WCSGatewayUtils_H

