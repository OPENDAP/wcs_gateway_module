// WCSRequestHandler.cc

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

#include "config.h"

#include "WCSRequestHandler.h"
#include "BESResponseHandler.h"
#include "BESResponseException.h"
#include "BESResponseNames.h"
#include "WCSResponseNames.h"
#include "BESVersionInfo.h"
#include "BESTextInfo.h"
#include "BESDataDDSResponse.h"
#include "BESDDSResponse.h"
#include "BESDASResponse.h"
#include "BESConstraintFuncs.h"
#include "WCSFile.h"
#include "WCSRequest.h"
#include "BESRequestHandlerList.h"

WCSRequestHandler::WCSRequestHandler( const string &name )
    : BESRequestHandler( name )
{
    add_handler( DAS_RESPONSE, WCSRequestHandler::wcs_redirect ) ;
    add_handler( DDS_RESPONSE, WCSRequestHandler::wcs_redirect ) ;
    add_handler( DATA_RESPONSE, WCSRequestHandler::wcs_redirect ) ;

    add_handler( VERS_RESPONSE, WCSRequestHandler::wcs_build_vers ) ;
    add_handler( HELP_RESPONSE, WCSRequestHandler::wcs_build_help ) ;
}

WCSRequestHandler::~WCSRequestHandler()
{
}

bool
WCSRequestHandler::wcs_redirect( BESDataHandlerInterface &dhi )
{
    // this same logic can happen for all of the dap requests das, dds,
    // data, so could just register one function for all of them. The
    // redirect will be generic.

    bool ret = true ;

    // Make an external call to a WCS class that will load the .wcs file.
    // This file will be an xml document that will contain the wcs request,
    // the data type being returned, and caching information. Might need to
    // get some caching information from the configuration file, or a wcs
    // configuration file. The WCS class will return information for a new
    // container that can be passed to the proper request handler. If there
    // is a problem loading the .wcs xml file then an exception will be
    // thrown.
    WCSFile file( dhi.container->access() ) ;
    file.read() ;
    string url = file.get_property( WCS_REQUEST ) ;
    string name = file.get_name() ;
    string type = file.get_property( WCS_RETURNTYPE ) ;
    string cacheTime = file.get_property( WCS_CACHETIME ) ;

    // Now that we have the request information ... make the wcs request.
    // This will return to us the name of a new file of a type that we
    // should be able to read.

    WCSRequest wcs ;
    string new_file = wcs.make_request( url, name, type, cacheTime ) ;

    // Now that we have this file, and we know the type of data being
    // handled for this new file, find the request handler that knows how to
    // handle that data type.

    // Save off the current container information associated with the wcs
    // file and set the target file information in the container
    string curr_type = dhi.container->get_container_type() ;
    string curr_name = dhi.container->get_real_name() ;

    dhi.container->set_container_type( type ) ;
    dhi.container->set_real_name( new_file ) ;

    // handle this new container.
    BESRequestHandlerList::TheList()->execute_current( dhi ) ;

    // reset the container information (do we really  need to? Do it for
    // now, but I don't think this container information is used any more.
    //
    // Actually it is, when retrieving the dap-server information (ascii,
    // www, usage) it uses the dataset to read the data.
    //dhi.container->set_container_type( curr_type ) ;
    //dhi.container->set_real_name( curr_name ) ;

    return ret ;
}

bool
WCSRequestHandler::wcs_build_vers( BESDataHandlerInterface &dhi )
{
    bool ret = true ;
    BESVersionInfo *info = dynamic_cast<BESVersionInfo *>(dhi.response_handler->get_response_object() ) ;
    info->addHandlerVersion( PACKAGE_NAME, PACKAGE_VERSION ) ;
    return ret ;
}

bool
WCSRequestHandler::wcs_build_help( BESDataHandlerInterface &dhi )
{
    bool ret = true ;
    BESInfo *info = dynamic_cast<BESInfo *>(dhi.response_handler->get_response_object());

    info->begin_tag("Handler");
    info->add_tag("name", PACKAGE_NAME);
    info->add_tag("version", PACKAGE_STRING);
    info->add_data_from_file( "WCS.Help", "WCS Help" ) ;
    info->end_tag("Handler");

    return ret ;
}

void
WCSRequestHandler::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "WCSRequestHandler::dump - ("
			     << (void *)this << ")" << endl ;
    BESIndent::Indent() ;
    BESRequestHandler::dump( strm ) ;
    BESIndent::UnIndent() ;
}

