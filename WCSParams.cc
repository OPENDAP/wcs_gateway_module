// WCSParams.cc

// -*- mode: c++; c-basic-offset:4 -*-

// This file is part of wcs_module, A C++ module that can be loaded in to
// the OPeNDAP Back-End Server (BES) and is able to handle wcs requests.

// Copyright (c) 2002,2003 OPeNDAP, Inc.
// Author: Patrick West <pwest@ucar.edu>
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
// You can contact OPeNDAP, Inc. at PO Box 112, Saunderstown, RI. 02874-0112.

// (c) COPYRIGHT URI/MIT 1994-1999
// Please read the full copyright statement in the file COPYRIGHT_URI.
//
// Authors:
//      pcw       Patrick West <pwest@ucar.edu>

#include "config.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cerrno>

using std::endl ;
using std::string ;
using std::ofstream ;
using std::istringstream ;

#include "WCSParams.h"
#include "BESDebug.h"
#include "TheBESKeys.h"
#include "BESUtil.h"
#include "BESInternalError.h"
#include "BESDebug.h"

bool WCSParams::_is_initialized = false ;
string WCSParams::_cache_dir = "" ;
int WCSParams::_cache_time = 0 ;
int WCSParams::_cache_size = 0 ;
int WCSParams::_entry_size = 0 ;

#define CACHE_SIZE_STR "20"
#define MIN_CACHE_SIZE 5
#define ENTRY_SIZE_STR "3"
#define MIN_ENTRY_SIZE 1

void
WCSParams::Initialize()
{
    // if already initialized, then return
    if( _is_initialized )
	return ;

    BESDEBUG( "wcs", "Initializing WCS Parameters" << endl )

    // get the amount of time that a cached entry can remain in the cache
    bool found = false ;
    string cache_time_str =
	TheBESKeys::TheKeys()->get_key( "WCS.CacheTime", found );
    if( !found || cache_time_str.empty() )
    {
	cache_time_str = "0" ;
    }
    istringstream ct( cache_time_str ) ;
    _cache_time = 0 ;
    ct >> _cache_time ;
    if( _cache_time < 0 ) _cache_time = 0 ;
    if( _cache_time == 0 && cache_time_str[0] != '0' )
    {
	string err = "WCS.CacheTime must be set to a valid number" ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }


    // get the cache directory. Must be able to write to it
    found = false ;
    _cache_dir = TheBESKeys::TheKeys()->get_key( "WCS.CacheDir", found ) ;
    if( !found || _cache_dir.empty() )
    {
	_cache_dir = "/tmp" ;
    }

    // make sure we can write to the cache directory
    char mypid[12] ;
    BESUtil::fastpidconverter( mypid, 10 ) ;
    string testfile = _cache_dir + "/.wcstestfile" + mypid ;
    ofstream strm( testfile.c_str() ) ;
    if( !strm )
    {
	string s = "Unable to write to the WCS Cache Directory" ;
	throw BESInternalError( s, __FILE__, __LINE__ ) ;
    }
    strm.close() ;

    // attempt to close the test file
    int ret = unlink( testfile.c_str() ) ;
    int myerrno = errno ;
    if( ret == -1 )
    {
	char *err_info = strerror( myerrno ) ;
	string err = "Unable to close the test file in WCS Cache Directory: " ;
	if( err_info )
	{
	    err += err_info ;
	}
	else
	{
	    err += "unknown error" ;
	}
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }

    // fine the cache size, must be at least MIN_CACHE_SIZE
    found = false ;
    string cache_size_str =
	TheBESKeys::TheKeys()->get_key( "WCS.CacheSize", found );
    if( !found || cache_size_str.empty() )
    {
	cache_size_str = CACHE_SIZE_STR ;
    }
    istringstream cacheSizeStream( cache_size_str ) ;
    cacheSizeStream >> _cache_size ;
    if( !_cache_size && cache_size_str[0] != '0' )
    {
	string err = "WCS.CacheSize must be set to a valid number" ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }
    if( _cache_size < MIN_CACHE_SIZE ) _cache_size = MIN_CACHE_SIZE ;

    found = false ;
    string entry_size_str =
	TheBESKeys::TheKeys()->get_key( "WCS.CacheEntrySize", found );
    if( !found || entry_size_str.empty() )
    {
	entry_size_str = ENTRY_SIZE_STR ;
    }
    istringstream entrySizeStream( entry_size_str ) ;
    entrySizeStream >> _entry_size ;
    if( !_entry_size && entry_size_str[0] != '0' )
    {
	string err = "WCS.CacheEntrySize must be set to a valid number" ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }
    if( _entry_size < MIN_ENTRY_SIZE ) _entry_size = MIN_ENTRY_SIZE ;

    if( _cache_size <= _entry_size )
    {
	string err = (string)"WCS Max Cache Size cannot be less than or equal "
		     + "to the max size of an entry in the cache" ;
	throw BESInternalError( err, __FILE__, __LINE__ ) ;
    }

    _is_initialized = true ;

    if( BESDebug::IsSet( "wcs" ) )
    {
	WCSParams::Dump( *(BESDebug::GetStrm()) ) ;
    }

    BESDEBUG( "wcs", "Done Initializing WCS Parameters" << endl )
}

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance along with information about
 * this module.
 *
 * @param strm C++ i/o stream to dump the information to
 */
void
WCSParams::Dump( ostream &strm )
{
    strm << BESIndent::LMarg << "WCSParams::Dump" << endl ;
    BESIndent::Indent() ;
    strm << BESIndent::LMarg << "cache directory: " << GetCacheDir() ;
    strm << BESIndent::LMarg << "cache time: " << GetCacheTime() ;
    strm << BESIndent::LMarg << "cache size: " << GetCacheSize() ;
    strm << BESIndent::LMarg << "entry size: " << GetEntrySize() ;
    BESIndent::UnIndent() ;
}

