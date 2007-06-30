// WCSFile.h

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

#ifndef I_WCSFile_H
#define I_WCSFile_H 1

#include <map>
#include <string>

using std::map ;
using std::string ;

#include "BESObj.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#define WCS_REQUEST "wcs-request"
#define WCS_RETURNTYPE "returnType"
#define WCS_CACHETIME "cacheTime"
#define WCS_DESCRIPTION "request-description"

#define WCS_XML_NAME "name"

class WCSFile : public BESObj
{
private:
    string			_filename ;
    string			_name ;
    string			_url ;
    string			_id ;
    map<string,string>		_properties ;

    void			handle_dataset_properties( xmlNode *a_node ) ;
    void			handle_property_element( xmlNode *a_prop ) ;
public:
    				WCSFile( const string &filename ) ;
    virtual		    	~WCSFile() ;

    virtual void		read( ) ;
    virtual string		get_property( const string &prop_name ) const ;
    virtual string		get_name() const { return _name ; }
    virtual string		get_url() const { return _url ; }
    virtual string		get_id() const { return _id ; }

    virtual void		dump( ostream &strm ) const ;
} ;

#endif // I_WCSFile_H

