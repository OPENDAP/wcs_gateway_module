// WCSFile.h

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
#define WCS_CACHETIME "cachTime"
#define WCS_DESCRIPTION "request-description"

#define WCS_XML_NAME "name"

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

    static map<string,string>	type_list ;
    static string		convert_wcs_type( const string &ret_type ) ;
    static void			break_apart_types( const string &types ) ;
public:
    				WCSFile( const string &filename ) ;
    virtual		    	~WCSFile() ;

    virtual void		read( ) ;
    virtual string		get_property( const string &prop_name ) const ;
    virtual string		get_name() const { return _name ; }
    virtual string		get_url() const { return _url ; }
    virtual string		get_id() const { return _id ; }

    virtual void		dump( ostream &strm ) const ;
} ;

#endif // I_WCSFile_H

