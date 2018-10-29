Name: acpitool
Version: 0.5
Release: 1
Source0: http://freeunix.dyndns.org:8000/ftp_site/pub/unix/acpitool/acpitool-0.5.tar.gz
Summary: A command line ACPI client for Linux.
Group: Applications/Internet
License: GPL
BuildRoot: %{_tmppath}/%{name}-root

%description
AcpiTool is a Linux ACPI client. It's a small command line application,
intended to be a replacement for the apm tool. The primary target
audience are laptop users, since these people are most interested in
things like battery status, thermal status and the ability to suspend
(sleep mode). The program simply accesses the /proc/acpi or /sysfs entries
to get/set acpi values. Hence, a computer running a Linux kernel from
the 2.4.x or 2.6.x series with ACPI enabled is needed.

%prep
%setup

%build
./configure

make

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -fr $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{_bindir}
mkdir -p $RPM_BUILD_ROOT%{_mandir}/man1
make install INSTALLPREFIX=$RPM_BUILD_ROOT/usr MANDIR=$RPM_BUILD_ROOT%{_mandir}/man1
mkdir -p $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}
cp -p INSTALL README TODO $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}

%files
%defattr(-,root,root)
%doc INSTALL README TODO
%{_bindir}/*
%{_mandir}/*/*

%changelog
* Tue Aug 24 2004 Robert Ambrose <rna@muttsoft.com>
- Created .spec file.


