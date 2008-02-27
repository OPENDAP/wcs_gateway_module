

 $Id: README 16473 2007-05-14 22:03:25Z jimg $

See the INSTALL file for build instructions.

This is the WCS Module to be loaded by the OPeNDAP Back-End Server (BES).For more information on Hyrax and the BES, please visit our documentationwiki at docs.opendap.org. This will include the latest install and buildinstructions, the latest configuration inforamtion, tutorials, how todevelop new modules for the BES, and more.

Contents  - What's here: What files are in the distribution

  - About the WCS Module: What exactly is this

  - Configuration: How to configure the WCS Module

  - Testing: Once built and configured, how do you know it works?

-----------------------------------------------------------------------

* What's here:

C++ source and header files - WCS Module codeunit-tests/ - directory containing unit tests. See below for testing.bes.conf - a sample BES configuration file, mainly for development purposes

* About the WCS Module

The WCS Module adds functionality to the OPeNDAP Back-End Server (BES)allowing it to retrieve data served by WCS requests.

* Configuration

Once the WCS Module has been installed, you will need to add the module tothe local BES configuration file. This configuration file is located in theBES installation directory under etc/bes and is called bes.conf. You willalso need at least the netcdf module and hdf4 module from OPeNDAP. Downloadthese two modules and follow the instructions for configuring the BES toload these modules.

To load the WCS Module into the BES you will need to add wcs to the list ofmodules. Locate the paramter called BES.modules and add ",wcs" to the end ofthat line. For example:

    BES.modules=dap,cmds,ascii,usage,www,nc,h4,wcs


Next you will need to tell the BES where to find the WCS Module. Add thefollowing line after BES.modules:

    BES.module.wcs=/path/to/libwcs_module.so


There are a few WCS specific parameters that need to be set in the BESconfiguraiton file. At the bottom of this file is a place where you canspecify module-specific parameters. Add the following:

    WCS.TypeList=netcdf:nc;hdf:h4;
    WCS.CacheDir=/tmp/wcs
    WCS.CacheTime=600
    WCS.CacheSize=20
    WCS.CacheEntrySize=3


The WCS.TypeList paramter tells the WCS module how to translate WCS datatypes into BES data types. You shouldn't have to change this value.

WCS.CacheDir tells the WCS Module where to put cached wcs request responses,which are the actual data files. /tmp/wcs is a default value and should bechanged. We suggest putting the cache directory under the BES rootdirectory, which is specified in the BES configuration file.

WCS.CacheTime specifies how long a cached object can remain in the cachebefore it is replaced by a subsequent wcs request to retrieve that object.

WCS.CacheSize specifies the maximum size of the WCS cache. Remember, thecached responses are netcdf and hdf files, so this value should besufficient to store many of these files without having to continuously purgethe cache.

WCS.CacheEntrySize specifies the maximum size of a single entry in the WCScache. Remember, the cached responses are netcdf and hdf files, so thisvalue should be set to hold your data. If an entry in the cache is too largethen an error will be thrown.

* Testing

For testing, please refer to the INSTALL file. An internet connection isrequired for these tests to run successfully.

