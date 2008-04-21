// WCSError.cc

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

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <stdio.h>
#include <stdlib.h>

#include "WCSError.h"

/** @brief read the target response file that contains textual error
 * information
 *
 * The contents of the file are read directly into the error message.
 *
 * @param filename target response file name with the error information
 * @param err return the error message in this variable
 * @param url the WCS request URL
 */
void
WCSError::read_error( const string &filename,
			  string &err,
			  const string &url )
{
    err = "WCS Request failed for url: " + url + " with error: " ;

    // The temporary file will contain the error information that we need to
    // report.
    FILE *f = fopen( filename.c_str(), "r" ) ;
    if( !f )
    {
	err = err + "Could not open the error file " + filename ;
	return ;
    }

    // read from the file until there is no more to read
    bool done = false ;
    while( !done )
    {
	char buff[1025] ;
	size_t bytes_read = fread( buff, 1, 1024, f ) ;
	if( !bytes_read )
	{
	    done = true ;
	}
	else
	{
	    if( bytes_read >=0 && bytes_read <= 1024 )
		buff[bytes_read] = '\0' ;
	    err = err + buff ;
	}
    }

    fclose( f ) ;
}

/** @brief read the target response file that contains error information in
 * an XML document
 *
 * The XML document is parsed using the libxml2 library and the error
 * information is palced in the err parameter.
 *
 * The format of the XML document ExceptionReport is expected to be as
 * follows, where the report can have multiple Exception elements within it
 * and multiple ExceptionText elements.
 *
 * <pre>
 * <?xml version="1.0" encoding="UTF-8"?>
 * <ExceptionReport xmlns="http://www.opengis.net/ows" xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.opengis.net/ows/owsCommon.xsd" version="0.3.20" language="en">
 *   <Exception exceptionCode="-1">
 *     <ExceptionText>error message</ExceptionText>
 *   </Exception>
 * </ExceptionReport>
 * </pre>
 *
 * The format of the XML document ServiceExceptionReport is expected to be
 * as follows, where the report can have multiple ServiceException elements
 * within it and multiple messages.
 *
 * @param filename target response file name with the error information
 * @param err return the error message in this variable
 * @param url the WCS request URL
 */
void
WCSError::read_xml_error( const string &filename,
			      string &err,
			      const string &url )
{
    err = "WCS Request failed for url: " + url + " with error: " ;

    // it's an xml document, use libxml2 to read the file and then extract
    // the Exception tag which has as an attribute the exceptionCode and
    // then ExceptionText sub tag.
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    xmlNode *exception_element = NULL;
    xmlNode *msg_element = NULL;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile( filename.c_str(), NULL, 0 ) ;

    if( doc == NULL )
    {
        err = err + "could not parse wcs exception " + filename ;
	return ;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement( doc ) ;
    if( root_element == NULL )
    {
	xmlFreeDoc( doc ) ;
	xmlCleanupParser() ;
	err = err + "wcs exception root element empty" ;
	return ;
    }

    // The root element name should be ExceptionReport
    string root_name = (char *)root_element->name ;
    if( root_name == "ExceptionReport" )
    {
	// can be multiple exception elements within the report, each with
	// multiple messages.
	exception_element = root_element->children ;
	while( exception_element )
	{
	    string name = (char *)exception_element->name ;
	    if( name == "Exception" && exception_element->type == 1 )
	    {
		msg_element = exception_element->children ;
		while( msg_element )
		{
		    string name = (char *)msg_element->name ;
		    if( name == "ExceptionText" && msg_element->type == 1 )
		    {
			char *content =
			    (char *)xmlNodeGetContent( msg_element ) ;
			if( content )
			{
			    if( !err.empty() )
			    {
				err = err + "\n" ;
			    }
			    err = err + content ;
			}
		    }
		    msg_element = msg_element->next ;
		}
	    }
	    exception_element = exception_element->next ;
	}
	if( err.empty() )
	{
	    err = err + "No exception message is available" ;
	}
    }
    else if( root_name == "ServiceExceptionReport" )
    {
	// can be multiple exception elements within the report, each with
	// multiple messages.
	exception_element = root_element->children ;
	while( exception_element )
	{
	    string name = (char *)exception_element->name ;
	    if( name == "ServiceException" && exception_element->type == 1 )
	    {
		char *content = (char *)xmlNodeGetContent( exception_element ) ;
		if( content )
		{
		    if( !err.empty() )
		    {
			err = err + "\n" ;
		    }
		    err = err + content ;
		}
	    }
	    exception_element = exception_element->next ;
	}
	if( err.empty() )
	{
	    err = err + "No exception message is available" ;
	}
    }
    else
    {
	err = err + "wcs exception root document not ExceptionReport or ServiceExceptionReport" ;
    }

    /* free the document */
    xmlFreeDoc(doc);

    /*
     * Free the global variables that may
     * have been allocated by the parser.
     */
    xmlCleanupParser();
}

