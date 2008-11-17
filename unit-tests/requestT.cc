// requestT.cc

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <string>
#include <fstream>
#include <cerrno>

#include <Error.h>

#include <BESError.h>
#include <TheBESKeys.h>
#include <BESDebug.h>

#include "WCSContainer.h"
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
    } 

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE( requestT ) ;

    CPPUNIT_TEST( make_request ) ;

    CPPUNIT_TEST_SUITE_END() ;

    void make_request()
    {
	string url = "http://g0dup05u.ecs.nasa.gov/cgi-bin/ceopAIRX2RET?service=WCS&version=1.0.0&request=GetCoverage&coverage=totH2OStd&TIME=2003-03-08&crs=WGS84&bbox=-53.885000,-4.085000,-49.135000,0.665000&resx=0.25&resy=0.25&interpolationMethod=Nearest%20neighbor&format=netCDF";

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

	delete c1 ;
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

