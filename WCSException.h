// WCSException.h

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

#ifndef WCSException_h_
#define WCSException_h_ 1

#include "BESException.h"

/** @brief exception sub class representing exceptions thrown within the WCS
 * module.
 *
 * Extends the BESException class. If a WCSException is thrown it is not
 * fatal, and therefor the return code is BES_DATA_HANDLER_PROBLEM.
 *
 * Two static methods are also provided that can read the target response
 * file if an error occurred, either an HTML error message or an XML error
 * message generated by the remote WCS system.
 *
 * @see BESException
 */
class WCSException : public BESException
{
protected:
      			WCSException() {}
public:
      			WCSException( const string &s,
			                     const string &file,
					     int line )
			    : BESException( s, file, line )
			{
			    set_context( "WCS Exception" ) ;
			    set_return_code( BES_DATA_HANDLER_PROBLEM ) ;
			}
    virtual		~WCSException() {}

    static void		read_error( const string &filename,
				    string &err,
				    const string &url ) ;

    static void		read_xml_error( const string &filename,
					string &err,
					const string &url ) ;
};

#endif // WCSException_h_ 

