// WCSRequestHandler.h

#ifndef I_WCSRequestHandler_H
#define I_WCSRequestHandler_H

#include "BESRequestHandler.h"

class WCSRequestHandler : public BESRequestHandler {
public:
			WCSRequestHandler( string name ) ;
    virtual		~WCSRequestHandler( void ) ;

    virtual void	dump( ostream &strm ) const ;

    /*
    static bool		wcs_build_das( BESDataHandlerInterface &dhi ) ;
    static bool		wcs_build_dds( BESDataHandlerInterface &dhi ) ;
    static bool		wcs_build_data( BESDataHandlerInterface &dhi ) ;
    */
    static bool		wcs_redirect( BESDataHandlerInterface &dhi ) ;

    static bool		wcs_build_vers( BESDataHandlerInterface &dhi ) ;
    static bool		wcs_build_help( BESDataHandlerInterface &dhi ) ;
};

#endif // WCSRequestHandler.h

