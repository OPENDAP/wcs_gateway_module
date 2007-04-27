#include "sys/types.h"
#include "sys/stat.h"
#include "dirent.h"
#include "time.h"
#include "stdio.h"

#include "WCSCache.h"
#include "BESDebug.h"

WCSCache::WCSCache( const string &cacheDir,
		    const string &cacheTime,
		    const string &target )
    : _cacheDir( cacheDir ),
    _cacheTime( 0.0 ),
    _target( target )
{
    _cacheTime = atof( cacheTime.c_str() ) ;
    BESDEBUG( "WCSCache::WCSCache - cache time: " << _cacheTime << endl )
    // FIXME: What kind of ranges do we want for this? If it's greater than
    // some value then set it to the max cahce time? And what does a
    // cache time of zero represent ... unending? Or never cached?
    if( _cacheTime < 0.0 ) _cacheTime = 0.0 ;
}

WCSCache::~WCSCache()
{
}

bool
WCSCache::is_cached( )
{
    if( !_cacheTime )
    {
	// if cache time is 0 then refresh all the time
	BESDEBUG( "WCSCache::is_cached - cache time is 0" << endl )
	return false ;
    }

    struct stat cbuf ;
    if( stat( _target.c_str(), &cbuf )  == -1 )
    {
	// the file does not exist, so is not cached
	BESDEBUG( "WCSCache::is_cached - file does not exist" << endl )
	return false ;
    }

    time_t file_time = cbuf.st_mtime ;
    BESDEBUG( "WCSCache::is_cached - file_time: " << file_time << endl )
    time_t curr_time = time( NULL ) ;
    BESDEBUG( "WCSCache::is_cached - curr_time: " << curr_time << endl )
    double time_diff = difftime( curr_time, file_time ) ;
    BESDEBUG( "WCSCache::is_cached - time difference: " << time_diff << endl )
    if( time_diff > _cacheTime )
    {
	// time to refresh the file
	BESDEBUG( "WCSCache::is_cached - time difference > cache time" << endl )
	return false ;
    }

    // if the file exists and the cache time is greater than 0 and the
    // difference since created is less than the cache time, then it is
    // cached, don't need to make the wcs request.
    BESDEBUG( "WCSCache::is_cached - file is cached" << endl )
    return true ;
}

void
WCSCache::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "WCSCache::dump - ("
			     << (void *)this << ")" << endl ;
    BESIndent::Indent() ;
    strm << BESIndent::LMarg << "cacheDir:" << _cacheDir << endl ;
    strm << BESIndent::LMarg << "cacheTime:" << _cacheTime << endl ;
    strm << BESIndent::LMarg << "target:" << _target << endl ;
    BESIndent::UnIndent() ;
}

