// WCSContainer.h

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

#ifndef WCSContainer_h_
#define WCSContainer_h_ 1

#include <list>
#include <string>

using std::list ;
using std::string ;

#include "BESContainer.h"

/** @brief Container representing a WCS request
 *
 * The real name of a WCSContainer is the actual WCS request. When the
 * access method is called the WCS request is made, the response cached if
 * successfull, and the target response returned as the real container that
 * a data handler would then open.
 *
 * @see WCSContainerStorage
 */
class WCSContainer : public BESContainer
{
private:
    string			_cacheDir ;
    string			_cacheTime ;
    string			_cacheName ;
    FILE *			_file_ptr ;

				WCSContainer()
				    : BESContainer(),
				      _file_ptr( 0 ) {}
protected:
    void			_duplicate( WCSContainer &copy_to ) ;
public:
    				WCSContainer( const string &sym_name,
					      const string &real_name ) ;

				WCSContainer( const WCSContainer &copy_from ) ;

    virtual			~WCSContainer() ;

    virtual BESContainer *	ptr_duplicate( ) ;

    virtual string		access() ;

    virtual bool		release() ;

    virtual void		dump( ostream &strm ) const ;
};

#endif // WCSContainer_h_

