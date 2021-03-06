// WCSGatewayRequestHandler.h

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

#ifndef I_WCSGatewayRequestHandler_H
#define I_WCSGatewayRequestHandler_H

#include "BESRequestHandler.h"

class WCSGatewayRequestHandler : public BESRequestHandler {
public:
			WCSGatewayRequestHandler( const string &name ) ;
    virtual		~WCSGatewayRequestHandler( void ) ;

    virtual void	dump( ostream &strm ) const ;

    static bool		sample_build_vers( BESDataHandlerInterface &dhi ) ;
    static bool		sample_build_help( BESDataHandlerInterface &dhi ) ;
};

#endif // WCSGatewayRequestHandler.h

