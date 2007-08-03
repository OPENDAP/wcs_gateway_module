// WCSCache.h

// This file is part of wcs_module, A C++ module that can be loaded in to
// the OPeNDAP Back-End Server (BES) and is able to handle wcs requests.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
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
// You can contact University Corporation for Atmospheric Research at
// 3080 Center Green Drive, Boulder, CO 80301
 
// (c) COPYRIGHT University Corporation for Atmospheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

#ifndef I_WCSCache_H
#define I_WCSCache_H 1

#include <string>

using std::string ;

#include "BESObj.h"

/** @brief This class represents a possibly cached wcs response.
 *
 * WCS responses are cached as target files on the file system. This class
 * represents a target response and can tell if it has been cached or not.
 * It does not cache the response.
 *
 * The cache time is represented in seconds.
 */
class WCSCache : public BESObj
{
private:
    string			_cacheDir ;
    double			_cacheTime ;
    string			_target ;
public:
    				WCSCache( const string &cacheDir,
					  const string &cacheTime,
					  const string &target ) ;
    virtual		    	~WCSCache() ;

    virtual bool		is_cached( ) ;

    virtual void		dump( ostream &strm ) const ;
} ;

#endif // I_WCSCache_H

