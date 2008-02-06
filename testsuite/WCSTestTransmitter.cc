// WCSTestTransmitter.cc

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

#include <sstream>

using std::ostringstream;

#include "WCSTestTransmitter.h"
#include "BESDDSResponse.h"
#include "BESDataDDSResponse.h"
#include "BESReturnManager.h"
#include "BESTransmitterNames.h"
#include "BESInternalError.h"

#define DAS_TRANSMITTER "das"
#define DDS_TRANSMITTER "dds"
#define DDX_TRANSMITTER "ddx"
#define DATA_TRANSMITTER "data"
#define ASCII_TRANSMITTER "ascii"

WCSTestTransmitter::WCSTestTransmitter()
{
    add_method( DAS_TRANSMITTER, WCSTestTransmitter::send_basic_das ) ;
    add_method( DDS_TRANSMITTER, WCSTestTransmitter::send_basic_dds ) ;
    add_method( DDX_TRANSMITTER, WCSTestTransmitter::send_basic_ddx ) ;
    add_method( DATA_TRANSMITTER, WCSTestTransmitter::send_basic_data ) ;
    add_method( ASCII_TRANSMITTER, WCSTestTransmitter::send_basic_ascii ) ;
}

void
WCSTestTransmitter::send_basic_das(BESResponseObject * obj,
                               BESDataHandlerInterface & dhi)
{
    BESTransmitter *t = BESReturnManager::TheManager()->find_transmitter( BASIC_TRANSMITTER ) ;
    if( t )
    {
	t->send_response( DAS_TRANSMITTER, obj, dhi ) ;
    }
    else
    {
	string err = (string)"WCS Test Transmitter could not locate the basic "
	             + "transmitter" ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }
}

void
WCSTestTransmitter::send_basic_dds(BESResponseObject * obj,
                                    BESDataHandlerInterface & dhi)
{
    BESDDSResponse *bdds = dynamic_cast < BESDDSResponse * >(obj);
    DDS *dds = bdds->get_dds();
    dds->set_dataset_name( "test" ) ;

    BESTransmitter *t = BESReturnManager::TheManager()->find_transmitter( BASIC_TRANSMITTER ) ;
    if( t )
    {
	t->send_response( DDS_TRANSMITTER, obj, dhi ) ;
    }
    else
    {
	string err = (string)"WCS Test Transmitter could not locate the basic "
	             + "transmitter" ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }
}

void
WCSTestTransmitter::send_basic_data(BESResponseObject * obj,
                                     BESDataHandlerInterface & dhi)
{
    BESDataDDSResponse *bdds = dynamic_cast < BESDataDDSResponse * >(obj);
    DataDDS *dds = bdds->get_dds();
    dds->set_dataset_name( "test" ) ;

    BESTransmitter *t = BESReturnManager::TheManager()->find_transmitter( BASIC_TRANSMITTER ) ;
    if( t )
    {
	t->send_response( DATA_TRANSMITTER, obj, dhi ) ;
    }
    else
    {
	string err = (string)"WCS Test Transmitter could not locate the basic "
	             + "transmitter" ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }
}

void
WCSTestTransmitter::send_basic_ddx(BESResponseObject * obj,
                                    BESDataHandlerInterface & dhi)
{
    BESDDSResponse *bdds = dynamic_cast < BESDDSResponse * >(obj);
    DDS *dds = bdds->get_dds();
    dds->set_dataset_name( "test" ) ;

    BESTransmitter *t = BESReturnManager::TheManager()->find_transmitter( BASIC_TRANSMITTER ) ;
    if( t )
    {
	t->send_response( DDX_TRANSMITTER, obj, dhi ) ;
    }
    else
    {
	string err = (string)"WCS Test Transmitter could not locate the basic "
	             + "transmitter" ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }
}

void
WCSTestTransmitter::send_basic_ascii( BESResponseObject * obj,
                                    BESDataHandlerInterface & dhi )
{
    BESDataDDSResponse *bdds = dynamic_cast < BESDataDDSResponse * >(obj);
    DataDDS *dds = bdds->get_dds();
    dds->set_dataset_name( "test" ) ;

    BESTransmitter *t = BESReturnManager::TheManager()->find_transmitter( BASIC_TRANSMITTER ) ;
    if( t )
    {
	t->send_response( ASCII_TRANSMITTER, obj, dhi ) ;
    }
    else
    {
	string err = (string)"WCS Test Transmitter could not locate the basic "
	             + "transmitter" ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }
}

