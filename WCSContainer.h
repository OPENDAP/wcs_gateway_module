// WCSContainer.h

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

#ifndef WCSContainer_h_
#define WCSContainer_h_ 1

#include <list>
#include <string>

using std::list ;
using std::string ;

#include "BESContainer.h"

/** @brief Container representing a WCS request
 *
 * @see WCSContainerStorage
 */
class WCSContainer : public BESContainer
{
private:
    string			_target ;
    string			_cacheTime ;

				WCSContainer() : BESContainer() {}
protected:
    void			_duplicate( WCSContainer &copy_to ) ;
public:
    				WCSContainer( const string &sym_name,
					      const string &real_name ) ;

				WCSContainer( const WCSContainer &copy_from ) ;

    virtual			~WCSContainer() {}

    virtual BESContainer *	ptr_duplicate( ) ;

    virtual string		access() ;

    virtual void		dump( ostream &strm ) const ;
};

#endif // WCSContainer_h_

