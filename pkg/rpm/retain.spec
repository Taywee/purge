Name:           retain
Version:        1.0
Release:        0%{?dist}
Summary:        file retention calculator for backups

Group:          Applications/Productivity
License:        GPLv3+
URL:            https://github.com/Taywee/retain
Source:         %{name}-%{version}.tar.gz
BuildRequires:  gcc

%description
Small console program that specifically sorts and selects timestamped
file/directory sets based on a retention scheme

This assumes that daily backups (or some similar backup scheme) are already
taking place.  You feed this program the list of file or directory names (on
standard input, newline-separated), a time formatter, and a list of retention
rules, and it will output which ones should be deleted.

%prep
%setup -q -n %{name}

%build
make CC=gcc %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}/usr

%files
%doc README.md LICENSE
%{_bindir}/%{name}
%{_mandir}/man1/%{name}.1.gz

%changelog

