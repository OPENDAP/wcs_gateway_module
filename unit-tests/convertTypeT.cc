// convertTypeT.cc

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <fstream>

#include "WCSGatewayUtils.h"
#include "BESError.h"
#include "BESUtil.h"
#include "TheBESKeys.h"
#include "test_config.h"

using namespace CppUnit ;
using std::ofstream ;
using std::endl ;
using std::ios ;

class convertTypeT: public TestFixture {
private:

public:
    convertTypeT() {}
    ~convertTypeT() {}

    void setUp()
    {
	string bes_conf = (string)TEST_SRC_DIR + "/bes.conf" ;
	TheBESKeys::ConfigFile = bes_conf ;
	putenv( (char *)bes_conf.c_str() ) ;
    } 

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE( convertTypeT ) ;

    CPPUNIT_TEST( do_conversion ) ;

    CPPUNIT_TEST_SUITE_END() ;

    void do_conversion()
    {
	ofstream dbg( "convertTypeT.dbg", ios::out ) ;

	try
	{
	    string newType = WCSGatewayUtils::convert_wcs_type( "netcdf" ) ;
	    CPPUNIT_ASSERT( newType == "nc" ) ;

	    newType = WCSGatewayUtils::convert_wcs_type( "netCDF" ) ;
	    CPPUNIT_ASSERT( newType == "nc" ) ;

	    newType = WCSGatewayUtils::convert_wcs_type( "NETCDF" ) ;
	    CPPUNIT_ASSERT( newType == "nc" ) ;

	    newType = WCSGatewayUtils::convert_wcs_type( "hdfeos" ) ;
	    CPPUNIT_ASSERT( newType == "h4" ) ;

	    newType = WCSGatewayUtils::convert_wcs_type( "HDFEOS" ) ;
	    CPPUNIT_ASSERT( newType == "h4" ) ;

	    newType = WCSGatewayUtils::convert_wcs_type( "HDFeos" ) ;
	    CPPUNIT_ASSERT( newType == "h4" ) ;

	    newType = WCSGatewayUtils::convert_wcs_type( "no_type" ) ;
	    CPPUNIT_ASSERT( newType == "no_type" ) ;
	}
	catch( BESError &e )
	{
	    dbg << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Caught BES exception" ) ;
	}
    }

} ;

CPPUNIT_TEST_SUITE_REGISTRATION( convertTypeT ) ;

int 
main( int, char** )
{
    CppUnit::TextTestRunner runner ;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() ) ;

    bool wasSuccessful = runner.run( "", false )  ;

    return wasSuccessful ? 0 : 1 ;
}

