// validateUrlT.cc

#include <cppunit/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

#include <string>
#include <fstream>

#include "WCSUtils.h"
#include "BESException.h"
#include "BESUtil.h"

using namespace CppUnit ;
using std::ofstream ;
using std::endl ;
using std::ios ;

class validateUrlT: public TestFixture {
private:

public:
    validateUrlT() {}
    ~validateUrlT() {}

    void setUp()
    {
    } 

    void tearDown()
    {
    }

    CPPUNIT_TEST_SUITE( validateUrlT ) ;

    CPPUNIT_TEST( do_validation ) ;

    CPPUNIT_TEST_SUITE_END() ;

    void do_validation()
    {
	ofstream dbg( "validateUrlT.dbg", ios::out ) ;

	try
	{
	    string url = "\"http://data.laits.gmu.edu/cgi-bin/ACCESS/wcs300?service=WCS&version=1.0.0&request=getCoverage&coverage=/home/mmin/testdata/MOD021KM.A2002248.0140.003.2002248112526.hdf:Swath:MODIS_SWATH_Type_L1B:EV_1KM_Emissive&crs=EPSG:4326&bbox=-40,29,-39,30&format=netCDF&resx=0.01&resy=0.01&Band_1KM_Emissive=1\"" ;
	    string target ;
	    string e_target = BESUtil::lowercase( "MOD021KM.A2002248.0140.003.2002248112526.hdf" ) ;
	    string format ;
	    string e_format = "nc" ;
	    WCSUtils::validate_url( url, target, format ) ;
	    dbg << "url = " << url << endl ;
	    dbg << "target = " << target << endl ;
	    dbg << "expected target = " << e_target << endl ;
	    dbg << "format = " << format << endl ;
	    CPPUNIT_ASSERT( target == e_target ) ;
	    CPPUNIT_ASSERT( format == e_format ) ;

	    url = "\"http://webapps.datafed.net/ogc_NASA.wsfl?SERVICE=WCS&REQUEST=GetCoverage&VERSION=1.0.0&CRS=EPSG:4326&COVERAGE=GOCART_G_OL.BCEXTTAU&TIME=2007-06-24T23:00:00&BBOX=-180,-90,180,90,0,0&WIDTH=-1&HEIGHT=-1&DEPTH=-1&FORMAT=NetCDF\"" ;
	    target = "" ;
	    e_target = BESUtil::lowercase( "GOCART_G_OL.BCEXTTAU" ) ;
	    format = "" ;
	    e_format = "nc" ;
	    WCSUtils::validate_url( url, target, format ) ;
	    dbg << "url = " << url << endl ;
	    dbg << "target = " << target << endl ;
	    dbg << "expected target = " << e_target << endl ;
	    dbg << "format = " << format << endl ;
	    CPPUNIT_ASSERT( target == e_target ) ;
	    CPPUNIT_ASSERT( format == e_format ) ;
	}
	catch( BESException &e )
	{
	    dbg << e.get_message() << endl ;
	    CPPUNIT_ASSERT( !"Caught BES exception" ) ;
	}
    }

} ;

CPPUNIT_TEST_SUITE_REGISTRATION( validateUrlT ) ;

int 
main( int, char** )
{
    CppUnit::TextTestRunner runner ;
    runner.addTest( CppUnit::TestFactoryRegistry::getRegistry().makeTest() ) ;

    bool wasSuccessful = runner.run( "", false )  ;

    return wasSuccessful ? 0 : 1 ;
}

