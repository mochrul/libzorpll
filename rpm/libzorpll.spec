Summary:		Low level library functions for Zorp
Name:			libzorpll-6_0-9
Version:		6.0.9.0
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
BuildRequires:		boost-devel

BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
Low level library functions for Zorp

Zorp is a new generation firewall. It is essentially a transparent proxy
firewall, with strict protocol analyzing proxies, a modular architecture,
and fine-grained control over the mediated traffic. Configuration decisions
are scriptable with the Python based configuration language.

This package contains low level library functions needed by Zorp and
associated programs.

%prep
%setup -q -n libzorpll

%build
./autogen.sh
%configure --disable-werror --enable-debug
make %{?_smp_mflags}

%install
%make_install

%check
make check %{?_smp_mflags}

%post
ldconfig

%postun
ldconfig

%files
%defattr(-,root,root)
%{_libdir}/libzorpll*.so.*
%{_libexecdir}/failure_notify*

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

%files devel
%defattr(-,root,root)
%{_libdir}/pkgconfig/zorplibll.pc
%{_libdir}/libzorpll.a
%{_libdir}/libzorpll*.la
%{_libdir}/libzorpll*.so
%dir %{_includedir}/zorp
%{_includedir}/zorp/*.h

%changelog
* Sun Sep 25 2016 Balasys Development Team <devel@balasys.hu> - 6.0.9.0-1
- Version bump
* Sun Feb 21 2016 Balasys Development Team <devel@balasys.hu> - 6.0.8.0-1
- Version bump
* Sun Feb 15 2015 BalaBit Development Team <devel@balabit.hu> - 6.0.0.0-1
- Version bump
