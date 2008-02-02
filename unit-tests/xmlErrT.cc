// xmlErrT.cc

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <fstream>

#include "WCSError.h"
#include "BESError.h"

using namespace CppUnit ;
using std::ofstream ;
using std::endl ;
using std::ios ;

class xmlErrT: public TestFixture {
private:

public:
    xmlErrT() {}
    ~xmlErrT() {}

    void setUp()
    {
	string bes_conf = "BES_CONF=./bes.conf" ;
	putenv( (char *)bes_conf.c_str() ) ;
    } 

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE( xmlErrT ) ;

    CPPUNIT_TEST( do_test ) ;

    CPPUNIT_TEST_SUITE_END() ;

    void do_test()
    {
	ofstream dbg( "xmlErrT.dbg", ios::out ) ;

	try
	{
	    string filename = "err.xml" ;
	    string err ;
	    string url = "http://www.testurl.org" ;
	    string expected = "WCS Request failed for url: " + url + " with error: Requested coverage layer(s)/measure(s): \"/Volumes/RAIDL2/TEST_DATA/CEOPTEST/testdata/MOD06_L2.A2002274.0115.005.2006333004108.hdf:Swath:mod06:Cloud_Effective_Emissivity\" not available in this server" ;
	    WCSError::read_xml_error( filename, err, url ) ;
	    dbg << "got error msg " << err << endl
	        << "should have gotten " << expected << endl ;
	    CPPUNIT_ASSERT( err == expected ) ;
	}
	catch( BESError &e )
	{
	    dbg << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Caught BES exception" ) ;
	}
    }

} ;

CPPUNIT_TEST_SUITE_REGISTRATION( xmlErrT ) ;

int 
main( int, char** )
{
    CppUnit::TextTestRunner runner ;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() ) ;

    bool wasSuccessful = runner.run( "", false )  ;

    return wasSuccessful ? 0 : 1 ;
}

