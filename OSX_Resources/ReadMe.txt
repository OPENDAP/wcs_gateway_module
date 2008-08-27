

 $Id: README 16473 2007-05-14 22:03:25Z jimg $

See the INSTALL file for build instructions.

This is the WCS Gateway Module to be loaded by the OPeNDAP Back-End Server (BES). For more information on Hyrax and the BES, please visit our documentation wiki at docs.opendap.org. This will include the latest install and build instructions, the latest configuration inforamtion, tutorials, how to develop new modules for the BES, and more.

Contents  - What's here: What files are in the distribution

  - About the WCS Gateway Module: What exactly is this

  - Configuration: How to configure the WCS Gateway Module

  - Testing: Once built and configured, how do you know it works?

-----------------------------------------------------------------------

* What's here:

C++ source and header files - WCS Module codeunit-tests/ - directory containing unit tests. See below for testing.bes.conf - a sample BES configuration file, mainly for development purposes

* About the WCS Module

The WCS Gateway Module adds functionality to the OPeNDAP Back-End Server (BES) allowing it to retrieve data served by WCS requests. This is a gateway service and does not provide WCS functionality, simply returns OPeNDAP responses for WCS requests served by a remote WCS server.

* Configuration

Once the WCS Gateway Module has been installed, you will need to add the module to the local BES configuration file. This configuration file is located in the BES installation directory under etc/bes and is called bes.conf. You will also need at least the netcdf module and hdf4 module from OPeNDAP. Download these two modules and follow the instructions for configuring the BES to load these modules.

To load the WCS Module into the BES you will need to add wcs to the list of modules. Locate the paramter called BES.modules and add ",wcsg" to the end of that line. For example:

    BES.modules=dap,cmds,ascii,usage,www,nc,h4,wcsg

Next you will need to tell the BES where to find the WCS Gateway Module. Add thefollowing line after BES.modules:

    BES.module.wcsg=/path/to/libwcs_gateway_module.so

There is one WCS Gateway specific parameter that needs to be set in the BES configuraiton file. At the bottom of this file is a place where you can specify module-specific parameters. Add the following:

    WCS.TypeList=netcdf:nc;hdf:h4;

The WCS.TypeList paramter tells the WCS module how to translate WCS datatypes into BES data types. You shouldn't have to change this value.

* Testing

For testing, please refer to the INSTALL file. An internet connection is required for these tests to run successfully.

