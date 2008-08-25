// WCSGatewayRequestHandler.cc

#include "config.h"

#include "WCSGatewayRequestHandler.h"
#include "BESResponseHandler.h"
#include "BESResponseNames.h"
#include "BESVersionInfo.h"
#include "BESTextInfo.h"

WCSGatewayRequestHandler::WCSGatewayRequestHandler( const string &name )
    : BESRequestHandler( name )
{
    add_handler( VERS_RESPONSE, WCSGatewayRequestHandler::sample_build_vers ) ;
    add_handler( HELP_RESPONSE, WCSGatewayRequestHandler::sample_build_help ) ;
}

WCSGatewayRequestHandler::~WCSGatewayRequestHandler()
{
}

bool
WCSGatewayRequestHandler::sample_build_vers( BESDataHandlerInterface &dhi )
{
    BESResponseObject *response = dhi.response_handler->get_response_object();
    BESVersionInfo *info = dynamic_cast < BESVersionInfo * >(response);
    if( !info )
	throw BESInternalError( "cast error", __FILE__, __LINE__ ) ;
  
    info->addHandlerVersion( PACKAGE_NAME, PACKAGE_VERSION ) ;

    return true ;
}

bool
WCSGatewayRequestHandler::sample_build_help( BESDataHandlerInterface &dhi )
{
    BESResponseObject *response = dhi.response_handler->get_response_object();
    BESInfo *info = dynamic_cast<BESInfo *>(response);
    if( !info )
	throw BESInternalError( "cast error", __FILE__, __LINE__ ) ;

    info->begin_tag("Handler");
    info->add_tag("name", PACKAGE_NAME);
    info->add_tag("version", PACKAGE_STRING);
    info->end_tag("Handler");

    return true ;
}

void
WCSGatewayRequestHandler::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "WCSGatewayRequestHandler::dump - ("
			     << (void *)this << ")" << endl ;
    BESIndent::Indent() ;
    BESRequestHandler::dump( strm ) ;
    BESIndent::UnIndent() ;
}

