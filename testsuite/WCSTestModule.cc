// WCSTestModule.cc

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

#include <iostream>

using std::endl ;

#include "WCSTestModule.h"
#include "WCSTestTransmitter.h"
#include "BESReturnManager.h"
#include "BESDebug.h"

void
WCSTestModule::initialize( const string &modname )
{
    BESDEBUG( "wcs", "Initializing WCS Module " << modname << endl ) ;

    BESDEBUG( "wcs", "Registering WCS Test Transmitter" << endl ) ;
    BESReturnManager::TheManager()->add_transmitter( "test", new WCSTestTransmitter );

    BESDEBUG( "wcs", "Done Initializing WCS Module " << modname << endl ) ;
}

void
WCSTestModule::terminate( const string &modname )
{
    BESDEBUG( "wcs", "Cleaning WCS Module " << modname << endl ) ;

    BESDEBUG( "wcs", "Deleting WCS Test Transmitter" << endl ) ;
    BESReturnManager::TheManager()->del_transmitter( "test" ) ;

    BESDEBUG( "wcs", "Done Cleaning WCS Module " << modname << endl ) ;
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance along with information about
 * this module.
 *
 * @param strm C++ i/o stream to dump the information to
 */
void
WCSTestModule::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "WCSTestModule::dump - ("
			     << (void *)this << ")" << endl ;
}

extern "C"
{
    BESAbstractModule *maker()
    {
	return new WCSTestModule ;
    }
}

