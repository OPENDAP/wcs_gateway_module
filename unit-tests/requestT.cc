// requestT.cc

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <fstream>

#include "HTTPCache.h"
#include "Error.h"

#include "WCSContainer.h"
#include "BESError.h"
#include "TheBESKeys.h"
#include "BESDebug.h"

using namespace CppUnit ;
using namespace libdap ;
using std::ofstream ;
using std::cerr ;
using std::endl ;
using std::ios ;

class requestT: public TestFixture {
private:

public:
    requestT() {}
    ~requestT() {}

    void setUp()
    {
	string bes_conf = "BES_CONF=./bes.conf" ;
	putenv( (char *)bes_conf.c_str() ) ;
	BESDebug::SetUp( "cerr,wcs" ) ;
    } 

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE( requestT ) ;

    CPPUNIT_TEST( make_request ) ;

    CPPUNIT_TEST_SUITE_END() ;

    void make_request()
    {
	bool found = false ;
	string cacheDir =
	    TheBESKeys::TheKeys()->get_key( "WCS.CacheDir", found ) ;
	if( !found || cacheDir.empty() )
	{
	    cacheDir = "./wcs" ;
	}

	string url = "\"http://webapps.datafed.net/ogc_NASA.wsfl?SERVICE=WCS&REQUEST=GetCoverage&VERSION=1.0.0&CRS=EPSG:4326&COVERAGE=GOCART_G_OL.BCEXTTAU&TIME=2007-06-24T23:00:00&BBOX=-180,-90,180,90,0,0&WIDTH=-1&HEIGHT=-1&DEPTH=-1&FORMAT=NetCDF\"" ;

	string qurl = url.substr( 1, url.length() - 2 ) ;
	cerr << qurl << endl ;

	// Creating the container validates the WCS request url
	WCSContainer *c1 = NULL ;
	try
	{
	    c1 = new WCSContainer( "c", url ) ;
	}
	catch( BESError &e )
	{
	    cerr << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to create container" ) ;
	}
	catch( Error &e )
	{
	    cerr << e.get_error_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to create container" ) ;
	}

	// Access the container, which makes the WCS request.
	string name1 ;
	try
	{
	    name1 = c1->access() ;
	}
	catch( BESError &e )
	{
	    cerr << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to access container" ) ;
	}
	catch( Error &e )
	{
	    cerr << e.get_error_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to access container" ) ;
	}

	// release the resource. This should just drop the lock and close
	// the file descriptor 
	try
	{
	    c1->release() ;
	}
	catch( BESError &e )
	{
	    cerr << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to release container" ) ;
	}
	catch( Error &e )
	{
	    cerr << e.get_error_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to release container" ) ;
	}

	// Grab the cache object for this cacheDir and make sure the url is
	// in the cache and is valid.
	HTTPCache *cache = 0 ;
	try
	{
	    cache = HTTPCache::instance( cacheDir, false ) ;
	    CPPUNIT_ASSERT( cache != 0 ) ;
	    CPPUNIT_ASSERT( cache->is_url_in_cache( qurl ) ) ;
	    CPPUNIT_ASSERT( cache->is_url_valid( qurl ) ) ;
	}
	catch( BESError &e )
	{
	    cerr << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed cache tests" ) ;
	}
	catch( Error &e )
	{
	    cerr << e.get_error_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed cache tests" ) ;
	}

	// cacheTime is set at 10 seconds, so sleep and run the tests again.
	cerr << "sleeping" << endl ;
	sleep( 15 ) ;
	cerr << "done sleeping" << endl ;

	// The url should be in the cache but it shouldn't be valid as the
	// time as expired.
	try
	{
	    CPPUNIT_ASSERT( cache != 0 ) ;
	    CPPUNIT_ASSERT( cache->is_url_in_cache( qurl ) ) ;
	    CPPUNIT_ASSERT( cache->is_url_valid( qurl ) == false ) ;
	}
	catch( BESError &e )
	{
	    cerr << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed cache tests" ) ;
	}
	catch( Error &e )
	{
	    cerr << e.get_error_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed cache tests" ) ;
	}

	// access the url again. Should make the WCS request again and
	// return the same name as before.
	string name2 ;
	WCSContainer *c2 = 0 ;
	try
	{
	    c2 = new WCSContainer( "c", url ) ;
	    name2 = c2->access() ;
	}
	catch( BESError &e )
	{
	    cerr << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to access container" ) ;
	}
	catch( Error &e )
	{
	    cerr << e.get_error_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to access container" ) ;
	}

	// release the resource. This should just drop the lock and close
	// the file descriptor 
	try
	{
	    c2->release() ;
	}
	catch( BESError &e )
	{
	    cerr << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to release container" ) ;
	}
	catch( Error &e )
	{
	    cerr << e.get_error_message() << endl ;
	    CPPUNIT_ASSERT( !"Failed to release container" ) ;
	}

	delete c1 ;
	delete c2 ;
    }

} ;

CPPUNIT_TEST_SUITE_REGISTRATION( requestT ) ;

int 
main( int, char** )
{
    CppUnit::TextTestRunner runner ;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() ) ;

    bool wasSuccessful = runner.run( "", false )  ;

    return wasSuccessful ? 0 : 1 ;
}

