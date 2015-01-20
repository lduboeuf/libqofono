Name:       libqofono

Summary:    A library of Qt bindings for ofono
Version:    0.59
Release:    1
Group:      System/Libraries
License:    LGPLv2.1
URL:        https://github.com/nemomobile/libqofono
Source0:    libqofono-%{version}.tar.bz2
Requires:   mobile-broadband-provider-info
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires:  pkgconfig(QtCore)
BuildRequires:  pkgconfig(QtDBus)
BuildRequires:  pkgconfig(QtXmlPatterns)

%description
This package contains Qt bindings for ofono cellular service
interfaces.

%package declarative
Summary:    Declarative plugin for libqofono
Group:      Development/Tools
Requires:   %{name} = %{version}-%{release}
Requires:   %{name} = %{version}

%description declarative
This package contains declarative plugin for libofono.

%package devel
Summary:    Development files for ofono Qt bindings
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}
Requires:   %{name} = %{version}

%description devel
This package contains the development header files for the ofono Qt bindings.

%package tests
Summary:    qml test app for the ofono Qt bindings
Group:      Development/Tools
Requires:   %{name} = %{version}-%{release}

%description tests
This package contains qml test for ofono Qt bindings.

%prep
%setup -q -n libqofono-%{version}

%build
%qmake
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%qmake_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/%{name}.so.*

%files declarative
%defattr(-,root,root,-)
%{_libdir}/qt4/imports/MeeGo/QOfono/*

%files devel
%defattr(-,root,root,-)
%{_libdir}/%{name}.prl
%{_libdir}/%{name}.so
%{_libdir}/pkgconfig/qofono.pc
%{_includedir}/qofono/*.h
%{_includedir}/qofono/dbus/ofono*.xml
%{_datadir}/qt4/mkspecs/features/qofono.prf

%files tests
%defattr(-,root,root,-)
/opt/ofonotest/bin/ofonotest
/opt/ofonotest/qml/ofonotest/*.qml
%{_libdir}/%{name}/tests/*
