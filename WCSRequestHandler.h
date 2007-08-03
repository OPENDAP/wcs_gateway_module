// WCSRequestHandler.h

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

#ifndef I_WCSRequestHandler_H
#define I_WCSRequestHandler_H

#include "BESRequestHandler.h"

/** @brief a data request handler that knows how to build responses for the .wcs data
 * type.
 *
 * The .wcs data type represents a WCS request that can return any number of data type
 * files, currently being netCDF and HDF4 data files. The wcs request handler first makes
 * the wcs request for the given container and then redirects the request to the data
 * handler that knows how to handle the resulting data file.
 *
 * Also knows how to add information for a help and version request for the WCS module
 *
 * @see WCSFile
 * @see WCSRequest
 * @see WCSContainer
 * @see WCSContainerStorage
 * @see WCSCache
 */
class WCSRequestHandler : public BESRequestHandler {
public:
			WCSRequestHandler( const string &name ) ;
    virtual		~WCSRequestHandler( void ) ;

    virtual void	dump( ostream &strm ) const ;

    static bool		wcs_redirect( BESDataHandlerInterface &dhi ) ;

    static bool		wcs_build_vers( BESDataHandlerInterface &dhi ) ;
    static bool		wcs_build_help( BESDataHandlerInterface &dhi ) ;
};

#endif // WCSRequestHandler.h

