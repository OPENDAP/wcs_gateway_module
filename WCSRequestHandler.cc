// WCSRequestHandler.cc

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

WCSRequestHandler::WCSRequestHandler( string name )
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

    // Now that we have the request information ... make the wcs request.
    // This will return to us the name of a new file of a type that we
    // should be able to read.
    WCSRequest wcs ;
    string new_file = wcs.make_request( file ) ;

    // Now that we have this file, and we know the type of data being
    // handled for this new file, find the request handler that knows how to
    // handle that data type.

    string type = file.get_property( WCS_RETURNTYPE ) ;

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
    dhi.container->set_container_type( curr_type ) ;
    dhi.container->set_real_name( curr_name ) ;

    return ret ;
}

/*
bool
WCSRequestHandler::wcs_build_das( BESDataHandlerInterface &dhi )
{
    bool ret = true ;
    BESResponseObject *response =
    dhi.response_handler->get_response_object();
    BESDASResponse *bdas = dynamic_cast < BESDASResponse * >(response);
    DAS *das = bdas->get_das();


    return ret ;
}

bool
WCSRequestHandler::wcs_build_dds( BESDataHandlerInterface &dhi )
{
    bool ret = true ;
    BESResponseObject *response =
    dhi.response_handler->get_response_object();
    BESDDSResponse *bdds = dynamic_cast < BESDDSResponse * >(response);
    DDS *dds = bdds->get_dds();

    // Your code goes here

    return ret ;
}

bool
WCSRequestHandler::wcs_build_data( BESDataHandlerInterface &dhi )
{
    bool ret = true ;
    BESResponseObject *response =
    dhi.response_handler->get_response_object();
    BESDataDDSResponse *bdds = dynamic_cast < BESDataDDSResponse * >(response);
    DataDDS *dds = bdds->get_dds();

    // Your code goes here

    return ret ;
}
*/

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

