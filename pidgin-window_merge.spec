Name:          pidgin-window_merge
%global pname  %(echo -n %{name} | sed s/^pidgin-//)
Version:       0.1
Release:       1%{?dist}
Summary:       Merges the Buddy List window with a conversation window

Group:         Applications/Internet
License:       GPLv3+
URL:           https://github.com/dm0-/window_merge
Source0:       https://sites.google.com/site/dm0uploads/window_merge-0.1.tar.gz

BuildRequires: pidgin-devel
Requires:      pidgin

%description
Enabling this plugin will allow conversations to be attached to the Buddy List
window.  Preferences are available to customize the plugin's panel layout.


%prep
%setup -qn %{pname}-%{version}

%build
%configure
make %{?_smp_mflags}

%install
install -Dpm 755 .libs/%{pname}.so %{buildroot}%{_libdir}/pidgin/%{pname}.so


%files
%{_libdir}/pidgin/%{pname}.so
%doc AUTHORS BUGS ChangeLog COPYING NEWS README TODO


%changelog
* Fri Mar 16 2012 David Michael <fedora.dm0@gmail.com> - 0.1-1
- Initial package
