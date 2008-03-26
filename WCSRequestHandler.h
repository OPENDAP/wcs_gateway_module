// WCSRequestHandler.h

#ifndef I_WCSRequestHandler_H
#define I_WCSRequestHandler_H

#include "BESRequestHandler.h"

class WCSRequestHandler : public BESRequestHandler {
public:
			WCSRequestHandler( const string &name ) ;
    virtual		~WCSRequestHandler( void ) ;

    virtual void	dump( ostream &strm ) const ;

    static bool		sample_build_vers( BESDataHandlerInterface &dhi ) ;
    static bool		sample_build_help( BESDataHandlerInterface &dhi ) ;
};

#endif // WCSRequestHandler.h

