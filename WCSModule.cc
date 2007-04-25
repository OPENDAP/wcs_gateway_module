// WCSModule.cc

#include <iostream>

using std::endl ;

#include "WCSModule.h"
#include "BESRequestHandlerList.h"
#include "WCSRequestHandler.h"
#include "BESDebug.h"
#include "BESResponseHandlerList.h"
#include "BESResponseNames.h"
#include "BESCommand.h"
#include "WCSResponseNames.h"

void
WCSModule::initialize( const string &modname )
{
    BESDEBUG( "Initializing WCS Handler:" << endl )

    BESDEBUG( "    adding " << modname << " request handler" << endl )
    BESRequestHandlerList::TheList()->add_handler( modname, new WCSRequestHandler( modname ) ) ;
}

void
WCSModule::terminate( const string &modname )
{
    BESDEBUG( "Removing WCS Handlers" << endl )
    BESRequestHandler *rh = BESRequestHandlerList::TheList()->remove_handler( modname ) ;
    if( rh ) delete rh ;
}

extern "C"
{
    BESAbstractModule *maker()
    {
	return new WCSModule ;
    }
}

void
WCSModule::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "WCSModule::dump - ("
			     << (void *)this << ")" << endl ;
}

