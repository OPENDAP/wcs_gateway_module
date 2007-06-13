// WCSContainerStorage.cc

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

#include "WCSContainerStorage.h"
#include "WCSContainer.h"
#include "BESContainerStorageException.h"

/** @brief create an instance of this persistent store with the given name.
 *
 * Creates an instances of WCSContainerStorage with the given name.
 * Looks up the base directory and regular expressions in the dods
 * initialization file using TheBESKeys. THrows an exception if either of
 * these cannot be determined or if the regular expressions are incorrectly
 * formed.
 *
 * &lt;data type&gt;:&lt;reg exp&gt;;&lt;data type&gt;:&lt;reg exp&gt;;
 *
 * each type/reg expression pair is separated by a semicolon and ends with a
 * semicolon. The data type/expression pair itself is separated by a
 * semicolon.
 *
 * @param n name of this persistent store
 * @throws BESContainerStorageException if unable to find the base
 * directory or regular expressions in the dods initialization file. Also
 * thrown if the type matching expressions are malformed.
 * @see WCSContainer
 */
WCSContainerStorage::WCSContainerStorage( const string &n )
    : BESContainerStorageVolatile( n )
{
}

WCSContainerStorage::~WCSContainerStorage()
{ 
}

/** @brief adds a container with the provided information
 *
 * If a match is made with the real name passed then the type is set.
 *
 * The real name of the container (the file name) is constructed using the
 * root directory from the initialization file with the passed real name
 * appended to it.
 *
 * The information is then passed to the add_container method in the parent
 * class.
 *
 * @param s_name symbolic name for the container
 * @param r_name real name (full path to the file) for the container
 * @param type type of data represented by this container
 */
void
WCSContainerStorage::add_container( const string &s_name,
					   const string &r_name,
					   const string &type )
{
    BESContainer *c = new WCSContainer( s_name, r_name ) ;
    BESContainerStorageVolatile::add_container( c ) ;
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance along with information about
 * the "storage" of containers in a catalog.
 *
 * @param strm C++ i/o stream to dump the information to
 */
void
WCSContainerStorage::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "WCSContainerStorage::dump - ("
			     << (void *)this << ")" << endl ;
    BESIndent::Indent() ;
    BESContainerStorageVolatile::dump( strm ) ;
    BESIndent::UnIndent() ;
}

