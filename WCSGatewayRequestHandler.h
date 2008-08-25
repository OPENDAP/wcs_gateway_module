// WCSGatewayRequestHandler.h

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

