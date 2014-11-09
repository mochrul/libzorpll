Summary:		Low level library functions for Zorp
Name:			libzorpll-5_0-0
Version:		5.0.0.0
Release:		1%{?dist}
License:		LGPL-2.0
Group:			System/Libraries
Source:			libzorpll_%{version}.tar.gz
URL:			https://www.balabit.com/network-security/zorp-gpl
%if 0%{?fedora}
%else
Vendor:			BalaBit IT Security
Packager:		BalaBit Development Team <devel@balabit.hu>
%endif

%if 0%{?fedora} || 0%{?rhel} || 0%{?centos}
BuildRequires:		openssl-devel
%else
BuildRequires:		libopenssl-devel
%endif
BuildRequires:		glib2-devel
BuildRequires:		libcap-devel
BuildRequires:		zlib-devel
BuildRequires:		binutils-devel
BuildRequires:		automake
BuildRequires:		autoconf
BuildRequires:		libtool
BuildRequires:		gcc-c++

BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
Low level library functions for Zorp

Zorp is a new generation firewall. It is essentially a transparent proxy
firewall, with strict protocol analyzing proxies, a modular architecture,
and fine-grained control over the mediated traffic. Configuration decisions
are scriptable with the Python based configuration language.

This package contains low level library functions needed by Zorp and
associated programs.

%package devel
Summary:		Headers for libzorpll
Group:			Development/Libraries
Requires:		zlib-devel
Requires:		glibc-devel
Requires:		libcap-devel
Requires:		glib2-devel
Requires:		binutils-devel
Requires:		%{name} = %{version}

%if 0%{?fedora} || 0%{?rhel} || 0%{?centos}
Requires:		openssl-devel
%else
Requires:		libopenssl-devel
%endif


%description devel
Low level library functions for Zorp, development files

Zorp is a new generation firewall. It is essentially a transparent proxy
firewall, with strict protocol analyzing proxies, a modular architecture,
and fine-grained control over the mediated traffic. Configuration decisions
are scriptable with the Python based configuration language.

This package contains the development files necessary to create programs
based on libzorpll.


%prep
%setup -q -n libzorpll

%build
./autogen.sh
%configure --disable-werror --enable-debug

%install
%make_install

%post
ldconfig

%postun
ldconfig

%files
%defattr(-,root,root)
%attr(-,root,root) %{_libdir}/libzorpll*.so.*
%attr(755,root,root) %{_libexecdir}/failure_notify*

%files devel
%defattr(-,root,root)
%attr(664,root,root) %{_libdir}/pkgconfig/zorplibll.pc
%attr(664,root,root) %{_libdir}/libzorpll.a
%attr(664,root,root) %{_libdir}/libzorpll*.la
%attr(-,root,root) %{_libdir}/libzorpll*.so
%dir %attr(755,root,root) %{_includedir}/zorp
%attr(664,root,root) %{_includedir}/zorp/*.h

%changelog
* Sun Feb 15 2015 BalaBit Development Team <devel@balabit.hu> - 5.0.0.0-1
- Fixed several packaging warings and errors

