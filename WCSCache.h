// WCSCache.h

#ifndef I_WCSCache_H
#define I_WCSCache_H 1

#include <string>

using std::string ;

#include "BESObj.h"

class WCSCache : public BESObj
{
private:
    string			_cacheDir ;
    double			_cacheTime ;
    string			_target ;
public:
    				WCSCache( const string &cacheDir,
					  const string &cacheTime,
					  const string &target ) ;
    virtual		    	~WCSCache() ;

    virtual bool		is_cached( ) ;

    virtual void		dump( ostream &strm ) const ;
} ;

#endif // I_WCSCache_H

