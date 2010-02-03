Summary:         WCS Request module for OPeNDAP Back End Server
Name:            wcs_module
Version:         1.1.0
Release:         1
License:         LGPL
Group:           System Environment/Daemons 
Source0:         http://www.opendap.org/pub/source/%{name}-%{version}.tar.gz
URL:             http://www.opendap.org/

BuildRoot:       %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
BuildRequires:   libdap-devel >= 3.10.0
BuildRequires:   bes-devel >= 3.8.0

%description
This is the WCS request module that is to be loaded in to the OPeNDAP
Back-End Server (BES). It makes a WCS request, which returns a netcdf
or hdf file, then reads the reponse files and returns DAP responses
that are compatible with DAP2 and the dap-server software.

%prep 
%setup -q

%build
%configure --disable-static --disable-dependency-tracking
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install INSTALL="install -p"

rm $RPM_BUILD_ROOT%{_libdir}/*.la
rm $RPM_BUILD_ROOT%{_libdir}/*.so
rm $RPM_BUILD_ROOT%{_libdir}/bes/*.la

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%dir %{_sysconfdir}/bes/
%dir %{_sysconfdir}/bes/modules
%config(noreplace) %{_sysconfdir}/bes/modules/wcsg.conf
%{_libdir}/bes/libwcs_module.so
%doc COPYING COPYRIGHT NEWS README

%changelog
* Wed Feb  6 2008 Patrick West <pwest at ucar.edu> - 1.0.0-1
- Initial release

