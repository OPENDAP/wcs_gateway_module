#include "WCSFile.h"
#include "BESDebug.h"
#include "BESHandlerException.h"
#include "TheBESKeys.h"

map<string,string> WCSFile::type_list ;

WCSFile::WCSFile( const string &filename )
    : _filename( filename )
{
}

WCSFile::~WCSFile()
{
}

void
WCSFile::read()
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile( _filename.c_str(), NULL, 0 ) ;

    if( doc == NULL )
    {
        string err = "could not parse wcs file " + _filename ;
	throw BESHandlerException( err, __FILE__, __LINE__ ) ;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement( doc ) ;
    if( root_element == NULL )
    {
	xmlFreeDoc( doc ) ;
	xmlCleanupParser() ;
	string err = "wcs file root element empty" ;
	throw BESHandlerException( err, __FILE__, __LINE__ ) ;
    }

    // The root element name should be dataset
    string root_name = (char *)root_element->name ;
    if( root_name != "dataset" )
    {
	xmlFreeDoc(doc);
	xmlCleanupParser() ;
	string err = "wcs file root document not called dataset" ;
	throw BESHandlerException( err, __FILE__, __LINE__ ) ;
    }

    // get the properties of the dataset root element
    handle_dataset_properties( root_element ) ;
    BESDEBUG( "WCSFile::read - name = " << _name << endl )
    BESDEBUG( "WCSFile::read - url = " << _url << endl )
    BESDEBUG( "WCSFile::read - id = " << _id << endl )
    if( _name.empty() )
    {
	xmlFreeDoc(doc);
	xmlCleanupParser() ;
	string err = "wcs file name attribute missing" ;
	throw BESHandlerException( err, __FILE__, __LINE__ ) ;
    }
    if( _url.empty() )
    {
	xmlFreeDoc(doc);
	xmlCleanupParser() ;
	string err = "wcs file url attribute missing" ;
	throw BESHandlerException( err, __FILE__, __LINE__ ) ;
    }
    if( _id.empty() )
    {
	xmlFreeDoc(doc);
	xmlCleanupParser() ;
	string err = "wcs file id attribute missing" ;
	throw BESHandlerException( err, __FILE__, __LINE__ ) ;
    }

    // all children of the root node should be called property, so deal with
    // each of the property elements
    handle_property_element( root_element->children ) ;
    string wcs_request = _properties[WCS_REQUEST] ;
    string type = _properties[WCS_RETURNTYPE] ;
    string cacheTime = _properties[WCS_CACHETIME] ;
    string description = _properties[WCS_DESCRIPTION] ;
    // only need the request, return type, and cacheTime
    if( wcs_request.empty() )
    {
	string err = string("wcs request url not specified") ;
	xmlFreeDoc(doc);
	xmlCleanupParser() ;
	throw BESHandlerException( err, __FILE__, __LINE__ ) ;
    }
    if( type.empty() )
    {
	string err = string("wcs return type not specified") ;
	xmlFreeDoc(doc);
	xmlCleanupParser() ;
	throw BESHandlerException( err, __FILE__, __LINE__ ) ;
    }
    if( cacheTime.empty() )
    {
	string err = string("wcs cache time not specified") ;
	xmlFreeDoc(doc);
	xmlCleanupParser() ;
	throw BESHandlerException( err, __FILE__, __LINE__ ) ;
    }
    // the type given is a wcs type, and not necessarily a BES type. Check
    // the configuration file to see if there is any conversion to be done.
    // Never will an empty string be returned.
    type = convert_wcs_type( type ) ;
    _properties[WCS_RETURNTYPE] = type ;
    BESDEBUG( "  " << WCS_REQUEST << ": " << wcs_request << endl )
    BESDEBUG( "  " << WCS_RETURNTYPE << ": " << type << endl )
    BESDEBUG( "  " << WCS_CACHETIME << ": " << cacheTime << endl )
    BESDEBUG( "  " << WCS_DESCRIPTION << ": " << description << endl )

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
}

string
WCSFile::get_property( const string &prop_name ) const
{
    string prop ;
    map<string,string>::const_iterator iter = _properties.find( prop_name ) ;
    if( iter != _properties.end() )
	prop = iter->second ;
    return prop ;
}

void
WCSFile::handle_dataset_properties( xmlNode *a_node )
{
    xmlChar *x_name = xmlGetProp( a_node, (xmlChar *)"name" ) ;
    if( x_name )
	_name = (char *)x_name ;
    xmlChar *x_url = xmlGetProp( a_node, (xmlChar *)"urlPath" ) ;
    if( x_url )
	_url = (char *)x_url ;
    xmlChar *x_id = xmlGetProp( a_node, (xmlChar *)"ID" ) ;
    if( x_id )
	_id = (char *)x_id ;
}

void
WCSFile::handle_property_element( xmlNode *a_prop )
{
    xmlNode *cur_prop = NULL;

    for( cur_prop = a_prop; cur_prop; cur_prop = cur_prop->next)
    {
	string prop_name = (char *)cur_prop->name ;
	if( prop_name == "property" && cur_prop->type == 1 )
	{
	    string name ;
	    string value ;
	    xmlChar *x_name = xmlGetProp( cur_prop, (xmlChar *)"name" ) ;
	    if( x_name )
		name = (char *)x_name ;
	    xmlChar *x_value = xmlGetProp( cur_prop, (xmlChar *)"value" ) ;
	    if( x_value )
		value = (char *)x_value ;
	    _properties[name] = value ;
	}
    }
}

void
WCSFile::break_apart_types( const string &types )
{
    bool done = false ;
    string::size_type start = 0 ;
    while( !done )
    {
	string::size_type semi = types.find( ";", start ) ;
	if( semi == string::npos )
	{
	    done = true ;
	}
	else
	{
	    string::size_type colon = types.find( ":", start ) ;
	    if( colon == string::npos || colon > semi )
	    {
		string err = "Malformed wcs type list: " + types ;
		throw BESHandlerException( err, __FILE__, __LINE__ ) ;
	    }
	    else
	    {
		string wcs_type = types.substr( start, colon - start ) ;
		string bes_type = types.substr( colon+1, semi - colon - 1 ) ;
		type_list[wcs_type] = bes_type ;
	    }
	    start = semi + 1 ;
	    if( start == types.length() )
	    {
		done = true ;
	    }
	}
    }
}

string
WCSFile::convert_wcs_type( const string &ret_type )
{
    // Read from the BES configuration file to get the list of wcs->bes
    // types. If the list does not exist then assume that it is a direct
    // mapping and return the type passed.
    if( WCSFile::type_list.size() == 0 )
    {
	bool found = false ;
	string types = TheBESKeys::TheKeys()->get_key( "WCS.TypeList", found ) ;
	if( found && !types.empty() )
	{
	    break_apart_types( types ) ;
	}
    }
    string bes_type = type_list[ret_type] ;
    if( bes_type.empty() )
	bes_type = ret_type ;
    return bes_type ;
}

void
WCSFile::dump( ostream &strm ) const
{
    strm << BESIndent::LMarg << "WCSFile::dump - ("
			     << (void *)this << ")" << endl ;
    BESIndent::Indent() ;
    strm << BESIndent::LMarg << "filename:" << _filename << endl ;
    strm << BESIndent::LMarg << "name:" << _name << endl ;
    strm << BESIndent::LMarg << "url:" << _url << endl ;
    strm << BESIndent::LMarg << "id:" << _id << endl ;
    map<string,string>::const_iterator i = _properties.begin() ;
    map<string,string>::const_iterator ie = _properties.end() ;
    for( ; i != ie; i++ )
    {
	strm << BESIndent::LMarg << "property:"
	     << i->first << ":" << i->second << endl ;
    }
    BESIndent::UnIndent() ;
}

