%define rname postgres
%define name ruby-%{rname}
%define version 0.7.1
%define release 1ts

%define _unpackaged_files_terminate_build 0

%define ruby_libdir %(ruby -rrbconfig -e 'puts Config::CONFIG["sitelibdir"]')
%define ruby_archdir %(ruby -rrbconfig -e 'puts Config::CONFIG["sitearchdir"]')

Summary: Ruby bindings for PostgreSQL
Name: %{name}
Version: %{version}
Release: %{release}
URL: http://www.postgresql.jp/interfaces/ruby/index.html
Source0: http://www.postgresql.jp/interfaces/ruby/archive/ruby-%{rname}-%{version}.tar.gz
License: GPL
Group: Development/Ruby
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Requires: ruby >= 1.8
Requires: ruby(abi)
BuildRequires: ruby ruby-devel postgresql-devel

%description
This is the PostgreSQL API module for Ruby.
It provides the same functions for Ruby programs that the PostgreSQL C API
provides for C programs.

%prep
%setup -q -n ruby-%{rname}-%{version} 

%build
ruby extconf.rb
make

%install
rm -rf %buildroot
make DESTDIR=%{buildroot} install

%clean
rm -rf %buildroot

%files
%defattr(-,root,root)
%doc README README.ja ChangeLog doc sample
%{ruby_archdir}/*.so

%changelog
* Mon Oct 11 2004 Omar Kilani <omar@tinysofa.org> 0.7.1-1ts
- Initial tinysofa package.

* Wed Jan 11 2007 Damian Krzeminski <damian@pingtel.com> 0.7.1-1ts
- Added dependency on ruby(abi) since ruby-libs package available on RHEL does no provide this by default
