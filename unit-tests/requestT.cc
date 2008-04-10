// requestT.cc

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <fstream>
#include <cerrno>

#include "HTTPCache.h"
#include "Error.h"

#include "WCSContainer.h"
#include "BESError.h"
#include "TheBESKeys.h"
#include "BESDebug.h"
#include "test_config.h"

using namespace CppUnit ;
using namespace libdap ;
using std::ofstream ;
using std::cerr ;
using std::endl ;
using std::ios ;

class requestT: public TestFixture {
private:
    string cacheDir ;
public:
    requestT() {}
    ~requestT() {}

    void setUp()
    {
	string bes_conf = (string)"BES_CONF=" + TEST_SRC_DIR + "/bes.conf" ;
	putenv( (char *)bes_conf.c_str() ) ;
	BESDebug::SetUp( "cerr,wcs" ) ;

	bool found = false ;
	cacheDir = TheBESKeys::TheKeys()->get_key( "WCS.CacheDir", found ) ;
	if( !found || cacheDir.empty() )
	{
	    cacheDir = "./wcs" ;
	}

	mode_t mode = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ;
	int ret = mkdir( cacheDir.c_str(), mode ) ;
	int myerrno = errno ;
	if( ret == -1 && myerrno != EEXIST )
	{
	    cerr << "Unable to create the wcs cache directory "
	         << cacheDir << " for this test" << endl ;
	    throw "failed" ;
	}
    } 

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE( requestT ) ;

    CPPUNIT_TEST( make_request ) ;

    CPPUNIT_TEST_SUITE_END() ;

    void make_request()
    {
	string url = "\"http://g0dup05u.ecs.nasa.gov/cgi-bin/ceopAIRX2RET?service=WCS&version=1.0.0&request=GetCoverage&coverage=totH2OStd&TIME=2003-03-08&crs=WGS84&bbox=-53.885000,-4.085000,-49.135000,0.665000&resx=0.25&resy=0.25&interpolationMethod=Nearest%20neighbor&format=netCDF\"";

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

