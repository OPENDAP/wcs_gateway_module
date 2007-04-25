// WCSModule.h

#ifndef I_WCSModule_H
#define I_WCSModule_H 1

#include "BESAbstractModule.h"

class WCSModule : public BESAbstractModule
{
public:
    				WCSModule() {}
    virtual		    	~WCSModule() {}
    virtual void		initialize( const string &modname ) ;
    virtual void		terminate( const string &modname ) ;

    virtual void		dump( ostream &strm ) const ;
} ;

#endif // I_WCSModule_H

