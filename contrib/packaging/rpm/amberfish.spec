
%define version 1.5.9

#
#   Amberfish RPM Spec
#   Dimitri Tarassenko <mitka@mitka.us>
#
%define xerces_ver 2.5.0
%define release 1

# We'll check for presense of xerces-c-devel by this file:
%define m1_xercesc_file %{_includedir}/xercesc/util/XercesVersion.hpp

# Let's find out the flavor
%if %( [ ! "%{_vendor}" == "redhat" ]; echo $?; )
    %define m1_flavor RH
    %define m1_flavor_RH 1
%endif
%if %( [ ! "%{_vendor}" == "suse" ]; echo $?; )
    %define m1_flavor SuSE
    %define m1_flavor_SuSE 1
%endif
%if %( [ ! "%{_vendor}" == "MandrakeSoft" ]; echo $?; )
    %define m1_flavor MDK
    %define m1_flavor_MDK 1
%endif
%if %{?m1_flavor:0}%{!?m1_flavor:1}
    %define m1_flavor %_vendor
    %define m1_flavor_Other 1
%endif

Name: amberfish
Summary:  Text retrieval software
Version: %version
Release: %{release}%{m1_flavor}
Vendor: Etymon Systems Inc.
Packager: Dimitri Tarassenko <mitka@mitka.us>
License: GPL
Group: Applications/Text
URL: http://www.etymon.com/tr.html
Source0: %{name}-%{version}.tar.gz
Source1: pdftex.map
Buildroot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires: %{m1_xercesc_file}
BuildRequires: tetex texinfo sed autoconf gcc gcc-c++ make binutils libstdc++-devel
Requires: libstdc++
%{?m1_flavor_RH:BuildRequires: xerces-c-devel >= %xerces_ver }
%{?m1_flavor_RH:Requires: xerces-c >= %xerces_ver }
%{?m1_flavor_SuSE:BuildRequires: Xerces-c-devel >= %xerces_ver }
%{?m1_flavor_SuSE:Requires: Xerces-c >= %xerces_ver }
%{?m1_flavor_MDK:BuildRequires: xerces-c-devel >= %xerces_ver }
%{?m1_flavor_MDK:Requires: xerces-c >= %xerces_ver }

# Now, we're going to be relocateable!!!
Prefix: %{_bindir}
Prefix: %{_mandir}
Prefix: %{_defaultdocdir}

%description 
Amberfish is general purpose text retrieval and indexing software. 
Its distinguishing features are indexing/search of semi-structured 
text (i.e. both free text and multiply nested fields), built-in 
support for XML documents using the Xerces-C library, structured 
queries allowing generalized field/tag paths, hierarchical result 
sets (XML only), automatic searching across multiple databases 
(allowing modular indexing), efficient indexing, and relatively 
low memory requirements during indexing (and the ability to index 
documents larger than available memory). Other features include 
Boolean queries, right truncation, phrase searching, relevance 
ranking, support for multiple documents per file, incremental 
indexing, and easy integration with other UNIX tools. 

Please note that you need Xerces-c installed on your server for
Amberfish to work. On a RedHat/Fedora you can download the latest
source from http://xml.apache.org/xerces-c/download.cgi and follow
the steps at http://xml.apache.org/xerces-c/build-misc.html#RPMLinux

On SuSE, install Xerces-c (note capital X) package using yast or rpm.

%prep
%setup -q
cp %{_sourcedir}/pdftex.map ./doc/pdftex.map
#%patch0 -p1 -b .bindir

%build 
%configure 
make

%install 
rm -fr %{buildroot}
make DESTDIR=%{buildroot} install

%clean 
rm -fr %{buildroot}

#%post
#%preun
#%postun

%files 
%defattr(-,root,root) 
%{_bindir}/* 
%{_mandir}/man1/* 
%doc doc/html/* doc/amberfish.pdf doc/amberfish.texi doc/version.texi NOTES README COPYING INSTALL CREDITS
	
%changelog 
* Mon Jun 21 2004 Dimitri Tarassenko <mitka@mitka.us> 1.5.9-1
- pdftex.map added to resolve font mapping when pdf is built
- tetex is added as build requirement
- some cleanup done to prevent empty pre/postun scripts in RPM
* Wed Jun 16 2004 Joao Cruz <jalrnc@yahoo.com> 1.5.9-0
- upgrade to 1.5.9
- added mandrake flavor
* Tue Jun 15 2004 Dimitri Tarassenko <mitka@mitka.us>
- patch removed, added HTML docs and CREDITS file, install notes
* Mon Jun 14 2004 Dimitri Tarassenko <mitka@mitka.us>
- first 2 versions
