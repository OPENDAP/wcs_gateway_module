// WCSFile.cc

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

#include "WCSFile.h"
#include "WCSUtils.h"
#include "BESDebug.h"
#include "BESHandlerException.h"

/** @brief instantiate the WCS file given the .wcs filename.
 *
 * The constructor does NOT read the .wcs file. To do that please call the
 * read method on this instance.
 *
 * @param filename name of the .wcs file
 */
WCSFile::WCSFile( const string &filename )
    : _filename( filename )
{
}

WCSFile::~WCSFile()
{
}

/** @brief read the .wcs file and access the information needed to make the
 * WCS request
 *
 * The WCS type from the .wcs file is converted to a type known by the BES
 * using the BES configuration parameter WCS.TypeList.
 *
 * @throws BESHandlerException if there is a problem reading the wcs file or
 * the target name, wcs url, wcs id, wcs request, return type, or cache time
 * are empty.
 */
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
    BESDEBUG( "wcs", "WCSFile::read - name = " << _name << endl )
    BESDEBUG( "wcs", "WCSFile::read - url = " << _url << endl )
    BESDEBUG( "wcs", "WCSFile::read - id = " << _id << endl )
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
    type = WCSUtils::convert_wcs_type( type ) ;
    _properties[WCS_RETURNTYPE] = type ;
    BESDEBUG( "wcs", "  " << WCS_REQUEST << ": " << wcs_request << endl )
    BESDEBUG( "wcs", "  " << WCS_RETURNTYPE << ": " << type << endl )
    BESDEBUG( "wcs", "  " << WCS_CACHETIME << ": " << cacheTime << endl )
    BESDEBUG( "wcs", "  " << WCS_DESCRIPTION << ": " << description << endl )

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();
}

/** @brief access a property from the property list
 *
 * properties include, with their macro preceeding the property:
 * WCS_REQUEST "wcs-request"
 * WCS_RETURNTYPE "returnType"
 * WCS_CACHETIME "cachTime"
 * WCS_DESCRIPTION "request-description"
 *
 * @param the property to retrieve
 * @return the value of the specified property name
 */
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

/** @brief dumps information about this object
 *
 * Displays the pointer value of this instance along with information about
 * the .wcs file. If the file has been read then the contents of the XML
 * document can be displayed. If not, then just the .wcs filename is
 * available.
 *
 * @param strm C++ i/o stream to dump the information to
 */
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

