// WCSRequestHandler.cc

#include "config.h"

#include "WCSRequestHandler.h"
#include "BESResponseHandler.h"
#include "BESResponseNames.h"
#include "BESVersionInfo.h"
#include "BESTextInfo.h"

WCSRequestHandler::WCSRequestHandler( const string &name )
    : BESRequestHandler( name )
{
    add_handler( VERS_RESPONSE, WCSRequestHandler::sample_build_vers ) ;
    add_handler( HELP_RESPONSE, WCSRequestHandler::sample_build_help ) ;
}

WCSRequestHandler::~WCSRequestHandler()
{
}

bool
WCSRequestHandler::sample_build_vers( BESDataHandlerInterface &dhi )
{
    bool ret = true ;
    BESVersionInfo *info = dynamic_cast<BESVersionInfo *>(dhi.response_handler->get_response_object() ) ;
    info->addHandlerVersion( PACKAGE_NAME, PACKAGE_VERSION ) ;
    return ret ;
}

bool
WCSRequestHandler::sample_build_help( BESDataHandlerInterface &dhi )
{
    bool ret = true ;
    BESInfo *info = dynamic_cast<BESInfo *>(dhi.response_handler->get_response_object());

    info->begin_tag("Handler");
    info->add_tag("name", PACKAGE_NAME);
    info->add_tag("version", PACKAGE_STRING);
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

