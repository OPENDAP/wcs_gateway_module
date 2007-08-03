// WCSFile.h

// This file is part of wcs_module, A C++ module that can be loaded in to
// the OPeNDAP Back-End Server (BES) and is able to handle wcs requests.

// Copyright (c) 2004,2005 University Corporation for Atmospheric Research
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
// You can contact University Corporation for Atmospheric Research at
// 3080 Center Green Drive, Boulder, CO 80301
 
// (c) COPYRIGHT University Corporation for Atmospheric Research 2004-2005
// Please read the full copyright statement in the file COPYRIGHT_UCAR.
//
// Authors:
//      pwest       Patrick West <pwest@ucar.edu>

#ifndef I_WCSFile_H
#define I_WCSFile_H 1

#include <map>
#include <string>

using std::map ;
using std::string ;

#include "BESObj.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

#define WCS_REQUEST "wcs-request"
#define WCS_RETURNTYPE "returnType"
#define WCS_CACHETIME "cacheTime"
#define WCS_DESCRIPTION "request-description"

#define WCS_XML_NAME "name"

/** @brief an XML document representing a WCS request.
 *
 * This file exists on the local file system and is served in the Hyrax
 * system just as any other data file. When a .wcs file is loaded an
 * instance of WCSFile is created to represent it and to load all of its
 * information, including the target name, the wcs URL (not the request url),
 * the wcs id, and other properties needed to make the request.
 *
 * The properties are the wcs request, returnType, cacheTime and
 * request-description (the latter is not used).
 *
 * The format of the wcs file is as follows:
 *
 * <pre>
 * <?xml version="1.0" encoding="UTF-8"?>
 * <dataset name="MYD11_L2.A2004210.0845.004.2004212100535"
 *          urlPath="wcs/GeoBrain/MODIS/2004/NetCDF/MYD11_L2.A2004210.0845.004.2004212100535"
 *          ID="wcs/GeoBrain/MODIS/2004/NetCDF/MYD11_L2.A2004210.0845.004.2004212100535" >
 *     <property name="wcs-request" value="http://geobrain.laits.gmu.edu/cgi-bin/ACCESS/MODWCS100?service=WCS&amp;version=1.0.0&amp;request=getCoverage&amp;coverage=/Volumes/RAIDL2/TEST_DATA/CEOPTEST/MOD11/MYD11_L2.A2004210.0845.004.2004212100535.hdf:Swath:MOD_Swath_LST:LST&amp;crs=EPSG:4326&amp;bbox=-109,25,-90,44&amp;format=netcdf&amp;resx=0.05&amp;resy=0.05" />
 *     <property name="returnType" value="netCDF" />
 *     <property name="cachTime" value="600" />
 *     <property name="request-description" value="cov: /Volumes/RAIDL2/TEST_DATA/CEOPTEST/MOD11/MYD11_L2.A2004210.0845.004.2004212100535.hdf:Swath:MOD_Swath_LST:LST pos: -116.5938,24.3448,-86.3859,45.5496" />
 * </dataset>
 * </pre>
 * 
 */
class WCSFile : public BESObj
{
private:
    string			_filename ;
    string			_name ;
    string			_url ;
    string			_id ;
    map<string,string>		_properties ;

    void			handle_dataset_properties( xmlNode *a_node ) ;
    void			handle_property_element( xmlNode *a_prop ) ;
public:
    				WCSFile( const string &filename ) ;
    virtual		    	~WCSFile() ;

    virtual void		read( ) ;
    virtual string		get_property( const string &prop_name ) const ;

    /** @brief return the target name of the WCS request
     * @return the target name of the WCS request
     */
    virtual string		get_name() const { return _name ; }

    /** @brief return the WCS request URL
     * @return the WCS request URL
     */
    virtual string		get_url() const { return _url ; }

    /** @brief return the WCS request id
     * @return the WCS request id
     */
    virtual string		get_id() const { return _id ; }

    virtual void		dump( ostream &strm ) const ;
} ;

#endif // I_WCSFile_H

