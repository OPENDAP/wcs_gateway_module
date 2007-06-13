// WCSCache.cc

// This file is part of bes, A C++ back-end server implementation framework
// for the OPeNDAP Data Access Protocol.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
// Author: Patrick West <pwest@ucar.edu> and Jose Garcia <jgarcia@ucar.edu>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// You can contact University Corporation for Atmospheric Research at
// 3080 Center Green Drive, Boulder, CO 80301
 
// (c) COPYRIGHT University Corporation for Atmospheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>
//      jgarcia     Jose Garcia <jgarcia@ucar.edu>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <stdio.h>

#include <sstream>

#include "WCSCache.h"
#include "BESDebug.h"

WCSCache::WCSCache( const string &cacheDir,
		    const string &cacheTime,
		    const string &target )
    : _cacheDir( cacheDir ),
    _cacheTime( 0.0 ),
    _target( target )
{
    std::istringstream is( cacheTime ) ;
    is >> _cacheTime ;
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

