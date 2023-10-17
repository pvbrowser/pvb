Summary:   Package pvbrowser + pvdevelop + rlsvgcat + libraries
Name:      pvbrowser
Version:   5.15.3
Release:   5.1
Group:     Productivity/Graphics/Visualization/Other
#Copyright: GPL with the possibility of commercial use
# for debian, ubuntu -> qt5-default qtdeclarative5-dev
License:   GPL
Packager:  Lehrig Software Engineering <lehrig@t-online.de>
Vendor:    pvb
URL:       http://pvbrowser.org
Source:    http://pvbrowser.de/pvbrowser/tar/pvb5/pvbrowser-5.tar.gz

#
AutoReqProv: no
Provides:  libpvsmt.a
Provides:  libpvsid.a
Provides:  librllib.a
Provides:  libpvsmt.so
Provides:  libpvsmt.so.1
Provides:  libpvsmt.so.1.0
Provides:  libpvsid.so
Provides:  libpvsid.so.1
Provides:  libpvsid.so.1.0
Provides:  librllib.so
Provides:  librllib.so.1
Provides:  librllib.so.1.0

%if 0%{?suse_version}
Requires:      libqt5 libpng
BuildRequires: libQt5Concurrent-devel libQt5Gui-devel libqt5-qtwebengine-devel libQt5OpenGL-devel libQt5Sql-devel libQt5PrintSupport-devel libqt5-qttools-devel libqt5-qtsvg-devel libqt5-qtmultimedia-devel gcc-c++ libreadline6 readline-devel fdupes 
%endif

%if 0%{?fedora_version}
# Requires:  ld-linux.so.2
Requires:  qt5-qtbase
BuildRequires: clang libstdc++-devel lcms-devel qt5-qtwayland-devel qt5-qtbase-devel qt5-qtwebengine-devel qt5-qtenginio-devel qt5-qtmultimedia-devel qt5-qtsvg-devel qt5-qttools-common qt5-qttools-static qt5-qttools qt5-qttools-devel readline-devel fdupes
# BuildRequires: gcc-c++ libstdc++-devel lcms-devel qt5-qtwayland-devel qt5-qtbase-devel qt5-qtwebengine-devel qt5-qtenginio-devel qt5-qtmultimedia-devel qt5-qtsvg-devel qt5-qttools-common qt5-qttools-static qt5-qttools qt5-qttools-devel readline-devel fdupes
%endif

%if 0%{?rhel_version}
# Requires:  ld-linux.so.2
Requires:  qt5-qtbase 
BuildRequires: gcc-c++ libstdc++-devel qt5-qtbase-devel qt5-qtwebengine-devel qt5-qtenginio-devel qt5-qtmultimedia-devel qt5-qtsvg-devel qt5-qttools qt5-qttools-devel readline-devel fdupes
%endif

%if 0%{?centos_version}
# Requires:  ld-linux.so.2
Requires:  qt5-qtbase
BuildRequires: pkgconfig
BuildRequires: gcc-c++ libstdc++-devel qt5-qtbase-devel qt5-qtwebengine-devel qt5-qtenginio-devel qt5-qtmultimedia-devel qt5-qtsvg-devel qt5-qttools qt5-qttools-devel readline-devel
%endif

%if 0%{?mageia}
# Requires:  ld-linux.so.2
Requires:  qt5-qtbase
BuildRequires: gcc-c++ libstdc++-devel lcms-devel qt5core-devel libqt5gui-devel libqt5multimedia-devel libqt5widgets-devel qtbase5-common-devel lib64qt5webengine-devel readline-devel fdupes
%endif

BuildRoot: %{_tmppath}/build-%{name}-%{version}

%description
 This is pvbrowser Qt5-Series:

 pvbrowser and pvserver are
 a system for industrial process visualization.
 ProcessViewBrowser is based on Qt http://www.trolltech.com
 It is capable of displaying most Qt widgets and some more.
 Normally you will not have to modify pvbrowser.
 pvserver defines everything that is displayed within
 pvbrowser and how interaction is done.
 Your task will be to implement your pvserver.
 This is done by creating your pvserver with pvdevelop.
 pvprowser is a registrated Trademark of S. Lehrig 

 Commands:
   pvbrowser
   pvdevelop

%prep
%setup

%build

# within build.sh call qmake.sh
#!/bin/bash
###########################################################
rm -f ./pvsexample/*.o
rm -f ./pvsexample/pvsexample
./build.sh all buildservice
./build.sh all buildservice

%install

%__mkdir_p  %{buildroot}/opt/pvb
%__mkdir_p  %{buildroot}/opt/pvb/doc
%__mkdir_p  %{buildroot}/opt/pvb/pvsexample
%__mkdir_p  %{buildroot}/opt/pvb/pvserver
%__mkdir_p  %{buildroot}/opt/pvb/pvbrowser
%__mkdir_p  %{buildroot}/opt/pvb/pvbrowser/widget_plugins
%__mkdir_p  %{buildroot}/opt/pvb/pvdevelop
%__mkdir_p  %{buildroot}/opt/pvb/start_pvbapp
%__mkdir_p  %{buildroot}/opt/pvb/designer/plugins
%__mkdir_p  %{buildroot}/opt/pvb/rllib/lib
%__mkdir_p  %{buildroot}/opt/pvb/rllib/rlsvg
%__mkdir_p  %{buildroot}/opt/pvb/rllib/rlhistory
%__mkdir_p  %{buildroot}/opt/pvb/rllib/rlhtml2pdf
%__mkdir_p  %{buildroot}/opt/pvb/rllib/rlfind
%__mkdir_p  %{buildroot}/opt/pvb/fake_qmake
%__mkdir_p  %{buildroot}/opt/pvb/language_bindings/lua/pvslua
%__mkdir_p  %{buildroot}/opt/pvb/language_bindings/lua/pvapplua

cp -r doc                                                 %{buildroot}/opt/pvb/
cp -r pvsexample                                          %{buildroot}/opt/pvb/
cp    spec/5/pvbrowser.spec                               %{_sourcedir}/
cp    LICENSE.GPL                                         %{buildroot}/opt/pvb/
cp    logo1.png                                           %{buildroot}/opt/pvb/
cp    custom.bmp                                          %{buildroot}/opt/pvb/
cp    gamsleiten.png                                      %{buildroot}/opt/pvb/
cp    pvbrowser.desktop                                   %{buildroot}/opt/pvb/
cp    pvdevelop.desktop                                   %{buildroot}/opt/pvb/
cp    pvserver/libpvsid.a                                 %{buildroot}/opt/pvb/pvserver/
cp    pvserver/libpvsmt.a                                 %{buildroot}/opt/pvb/pvserver/
cp    pvserver/libpvsid.so                                %{buildroot}/opt/pvb/pvserver/
cp    pvserver/libpvsmt.so                                %{buildroot}/opt/pvb/pvserver/
cp    pvserver/wthread.h                                  %{buildroot}/opt/pvb/pvserver/
cp    pvserver/BMP.h                                      %{buildroot}/opt/pvb/pvserver/
cp    pvserver/vmsglext.h                                 %{buildroot}/opt/pvb/pvserver/
cp    pvserver/pvbImage.h                                 %{buildroot}/opt/pvb/pvserver/
cp    pvserver/vmsgl.h                                    %{buildroot}/opt/pvb/pvserver/
cp    pvserver/processviewserver.h                        %{buildroot}/opt/pvb/pvserver/
cp    pvbrowser/pvbrowser                                 %{buildroot}/opt/pvb/pvbrowser/
cp    pvdevelop/pvdevelop                                 %{buildroot}/opt/pvb/pvdevelop/
cp    pvdevelop/pvdevelop.sh                              %{buildroot}/opt/pvb/pvdevelop/
cp    start_pvbapp/start_pvbapp                           %{buildroot}/opt/pvb/start_pvbapp/
cp    start_pvbapp/README.txt                             %{buildroot}/opt/pvb/start_pvbapp/
cp    start_pvbapp/example.ini.linux                      %{buildroot}/opt/pvb/start_pvbapp/example.ini
cp    start_pvbapp/start_if_not_already_running.sh        %{buildroot}/opt/pvb/start_pvbapp/
cp    rllib/rlsvg/rlsvgcat                                %{buildroot}/opt/pvb/rllib/rlsvg/
cp    rllib/rlfind/rlfind                                 %{buildroot}/opt/pvb/rllib/rlfind/
cp    rllib/rlhistory/rlhistory                           %{buildroot}/opt/pvb/rllib/rlhistory/
cp    rllib/rlhtml2pdf/rlhtml2pdf                         %{buildroot}/opt/pvb/rllib/rlhtml2pdf/
cp    fake_qmake/fake_qmake                               %{buildroot}/opt/pvb/fake_qmake/
cp    rllib/lib/librllib.a                                %{buildroot}/opt/pvb/rllib/lib/
cp    rllib/lib/librllib.so                               %{buildroot}/opt/pvb/rllib/lib/
cp    rllib/lib/*.h                                       %{buildroot}/opt/pvb/rllib/lib/
cp    rllib/lib/nodave.o                                  %{buildroot}/opt/pvb/rllib/lib/
cp    rllib/lib/setport.o                                 %{buildroot}/opt/pvb/rllib/lib/
cp    language_bindings/lua/pvslua/pvslua                 %{buildroot}/opt/pvb/language_bindings/lua/pvslua/
cp    language_bindings/lua/pvapplua/pvapplua             %{buildroot}/opt/pvb/language_bindings/lua/pvapplua/
cp    designer/plugins/libpvb_designer_plugin.so          %{buildroot}/opt/pvb/designer/plugins/
cp    designer/plugins/libqwt_designer_plugin.so          %{buildroot}/opt/pvb/designer/plugins/
cp    designer/README.txt                                 %{buildroot}/opt/pvb/designer/

%__mkdir_p %{buildroot}/usr/lib
%__mkdir_p %{buildroot}%{_bindir}
%__mkdir_p %{buildroot}/usr/share/applications
%__mkdir_p %{buildroot}/usr/%{_lib}/browser-plugins

%__ln_s /opt/pvb/pvserver/libpvsid.a        %{buildroot}/usr/lib/libpvsid.a
%__ln_s /opt/pvb/pvserver/libpvsid.so       %{buildroot}/usr/lib/libpvsid.so
%__ln_s /opt/pvb/pvserver/libpvsid.so       %{buildroot}/usr/lib/libpvsid.so.1
%__ln_s /opt/pvb/pvserver/libpvsid.so       %{buildroot}/usr/lib/libpvsid.so.1.0
%__ln_s /opt/pvb/pvserver/libpvsid.so       %{buildroot}/usr/lib/libpvsid.so.1.0.0

%__ln_s /opt/pvb/pvserver/libpvsmt.a        %{buildroot}/usr/lib/libpvsmt.a
%__ln_s /opt/pvb/pvserver/libpvsmt.so       %{buildroot}/usr/lib/libpvsmt.so
%__ln_s /opt/pvb/pvserver/libpvsmt.so       %{buildroot}/usr/lib/libpvsmt.so.1
%__ln_s /opt/pvb/pvserver/libpvsmt.so       %{buildroot}/usr/lib/libpvsmt.so.1.0
%__ln_s /opt/pvb/pvserver/libpvsmt.so       %{buildroot}/usr/lib/libpvsmt.so.1.0.0

%__ln_s /opt/pvb/rllib/lib/librllib.a       %{buildroot}/usr/lib/librllib.a
%__ln_s /opt/pvb/rllib/lib/librllib.so      %{buildroot}/usr/lib/librllib.so
%__ln_s /opt/pvb/rllib/lib/librllib.so      %{buildroot}/usr/lib/librllib.so.1
%__ln_s /opt/pvb/rllib/lib/librllib.so      %{buildroot}/usr/lib/librllib.so.1.0
%__ln_s /opt/pvb/rllib/lib/librllib.so      %{buildroot}/usr/lib/librllib.so.1.0.0

%__ln_s /opt/pvb/pvbrowser/pvbrowser        %{buildroot}%{_bindir}/pvbrowser
cp pvdevelop/pvdevelop.sh                   %{buildroot}%{_bindir}/pvdevelop
%__ln_s /opt/pvb/start_pvbapp/start_pvbapp  %{buildroot}%{_bindir}/start_pvbapp
%__ln_s /opt/pvb/rllib/rlsvg/rlsvgcat       %{buildroot}%{_bindir}/rlsvgcat
%__ln_s /opt/pvb/rllib/rlfind/rlfind        %{buildroot}%{_bindir}/rlfind
%__ln_s /opt/pvb/rllib/rlhistory/rlhistory  %{buildroot}%{_bindir}/rlhistory
%__ln_s /opt/pvb/rllib/rlhtml2pdf/rlhtml2pdf              %{buildroot}%{_bindir}/rlhtml2pdf
%__ln_s /opt/pvb/fake_qmake/fake_qmake                    %{buildroot}%{_bindir}/fake_qmake
%__ln_s /opt/pvb/language_bindings/lua/pvslua/pvslua      %{buildroot}%{_bindir}/pvslua 
%__ln_s /opt/pvb/language_bindings/lua/pvapplua/pvapplua  %{buildroot}%{_bindir}/pvapplua 

%__mkdir_p %{buildroot}/srv/automation/shm
%__mkdir_p %{buildroot}/srv/automation/mbx
%__mkdir_p %{buildroot}/srv/automation/log

%fdupes -s %{buildroot}/opt/pvb/doc

%clean
rm -rf /opt/pvb

%package     devel
Summary:     Package pvbrowser + pvdevelop + rlsvgcat + libraries
Group:       Productivity/Graphics/Visualization/Other
License:     GPL

%description devel
 This is pvbrowser Qt5-Series:

 pvbrowser and pvserver are
 a system for industrial process visualization.
 ProcessViewBrowser is based on Qt http://www.trolltech.com
 It is capable of displaying most Qt widgets and some more.
 Normally you will not have to modify pvbrowser.
 pvserver defines everything that is displayed within
 pvbrowser and how interaction is done.
 Your task will be to implement your pvserver.
 This is done by creating your pvserver with pvdevelop.
 pvb is a registrated Trademark of S. Lehrig 

 Commands:
   pvbrowser
   pvdevelop

%post
/sbin/ldconfig
chmod ugoa+rw /srv/automation/mbx
chmod ugoa+rw /srv/automation/shm
chmod ugoa+rw /srv/automation/log

%postun
/sbin/ldconfig

%files devel
%defattr(-,root,root,755)
%dir /opt/pvb
%dir /opt/pvb/designer
%dir /opt/pvb/designer/plugins
%dir /opt/pvb/fake_qmake
%dir /opt/pvb/language_bindings
%dir /opt/pvb/language_bindings/lua
%dir /opt/pvb/language_bindings/lua/pvapplua
%dir /opt/pvb/language_bindings/lua/pvslua
%dir /opt/pvb/pvbrowser
%dir /opt/pvb/pvdevelop
%dir /opt/pvb/pvserver
%dir /opt/pvb/rllib
%dir /opt/pvb/rllib/lib
%dir /opt/pvb/rllib/rlhistory
%dir /opt/pvb/rllib/rlhtml2pdf
%dir /opt/pvb/rllib/rlfind
%dir /opt/pvb/rllib/rlsvg
%dir /opt/pvb/start_pvbapp
%dir /srv/automation
/usr/lib/libpvsid.a
/usr/lib/libpvsid.so
/usr/lib/libpvsid.so.1
/usr/lib/libpvsid.so.1.0
/usr/lib/libpvsid.so.1.0.0
/usr/lib/libpvsmt.a
/usr/lib/libpvsmt.so
/usr/lib/libpvsmt.so.1
/usr/lib/libpvsmt.so.1.0
/usr/lib/libpvsmt.so.1.0.0
%{_bindir}/pvbrowser
%{_bindir}/pvdevelop
%{_bindir}/rlsvgcat
%{_bindir}/rlfind
%{_bindir}/rlhistory
%{_bindir}/rlhtml2pdf
%{_bindir}/fake_qmake
%{_bindir}/start_pvbapp
%{_bindir}/pvslua
%{_bindir}/pvapplua
/usr/lib/librllib.a
/usr/lib/librllib.so
/usr/lib/librllib.so.1
/usr/lib/librllib.so.1.0
/usr/lib/librllib.so.1.0.0
/srv/automation/shm
/srv/automation/mbx
/srv/automation/log
/opt/pvb/LICENSE.GPL
/opt/pvb/logo1.png
/opt/pvb/custom.bmp
/opt/pvb/gamsleiten.png
/opt/pvb/pvbrowser.desktop
/opt/pvb/pvdevelop.desktop
/opt/pvb/doc
/opt/pvb/pvsexample
/opt/pvb/start_pvbapp/start_pvbapp
/opt/pvb/start_pvbapp/example.ini
/opt/pvb/start_pvbapp/README.txt
/opt/pvb/start_pvbapp/start_if_not_already_running.sh
/opt/pvb/pvserver/libpvsid.a
/opt/pvb/pvserver/libpvsmt.a
/opt/pvb/rllib/lib/librllib.a
/opt/pvb/pvserver/libpvsid.so
/opt/pvb/pvserver/libpvsmt.so
/opt/pvb/rllib/lib/librllib.so
%if 0%{?mandriva_version}
/opt/pvb/rllib/lib/librllib.so.1
%endif
/opt/pvb/pvbrowser/pvbrowser
/opt/pvb/pvdevelop/pvdevelop
/opt/pvb/pvdevelop/pvdevelop.sh
/opt/pvb/rllib/rlsvg/rlsvgcat
/opt/pvb/rllib/rlfind/rlfind
/opt/pvb/rllib/rlhistory/rlhistory
/opt/pvb/rllib/rlhtml2pdf/rlhtml2pdf
/opt/pvb/fake_qmake/fake_qmake
/opt/pvb/pvserver/wthread.h
/opt/pvb/pvserver/BMP.h
/opt/pvb/pvserver/vmsglext.h
/opt/pvb/pvserver/pvbImage.h
/opt/pvb/pvserver/vmsgl.h
/opt/pvb/pvserver/processviewserver.h
/opt/pvb/rllib/lib/rllib.h
/opt/pvb/rllib/lib/setport.h
/opt/pvb/rllib/lib/nodave.h
/opt/pvb/rllib/lib/setport.o
/opt/pvb/rllib/lib/nodave.o
/opt/pvb/rllib/lib/objdir.h
/opt/pvb/rllib/lib/log.h
/opt/pvb/rllib/lib/rlsvgcat.h
/opt/pvb/rllib/lib/rlcanopendaemon.h
/opt/pvb/rllib/lib/rlspreadsheet.h
/opt/pvb/rllib/lib/rlcanopenstructs.h
/opt/pvb/rllib/lib/rlcanopenclient.h
/opt/pvb/rllib/lib/rlsiemenstcpclient.h
/opt/pvb/rllib/lib/rlmodbus.h
/opt/pvb/rllib/lib/rltime.h
/opt/pvb/rllib/lib/rlmodbusclient.h
/opt/pvb/rllib/lib/rlsvganimator.h
/opt/pvb/rllib/lib/rleventlogserver.h
/opt/pvb/rllib/lib/rlcontroller.h
/opt/pvb/rllib/lib/rlbuffer.h
/opt/pvb/rllib/lib/rlbussignaldatabase.h
/opt/pvb/rllib/lib/rlmailbox.h
/opt/pvb/rllib/lib/rlfileload.h
/opt/pvb/rllib/lib/rlsharedmemory.h
/opt/pvb/rllib/lib/rldataprovider.h
/opt/pvb/rllib/lib/rldefine.h
/opt/pvb/rllib/lib/rlinterpreter.h
/opt/pvb/rllib/lib/rlcannode.h
/opt/pvb/rllib/lib/rlcanopen.h
/opt/pvb/rllib/lib/rlwthread.h
/opt/pvb/rllib/lib/rlcorba.h
/opt/pvb/rllib/lib/rlserial.h
/opt/pvb/rllib/lib/rlthread.h
/opt/pvb/rllib/lib/rlcutil.h
/opt/pvb/rllib/lib/rlcanopentypes.h
/opt/pvb/rllib/lib/rlpcontrol.h
/opt/pvb/rllib/lib/rlinifile.h
/opt/pvb/rllib/lib/rl3964r.h
/opt/pvb/rllib/lib/rlppiclient.h
/opt/pvb/rllib/lib/rlsocket.h
/opt/pvb/rllib/lib/rlsiemenstcp.h
/opt/pvb/rllib/lib/rlspawn.h
/opt/pvb/rllib/lib/rlevent.h
/opt/pvb/rllib/lib/rlfifo.h
/opt/pvb/rllib/lib/rlhistorylogger.h
/opt/pvb/rllib/lib/rlhistoryreader.h
/opt/pvb/rllib/lib/rlhilschercif.h
/opt/pvb/rllib/lib/rlstring.h
/opt/pvb/rllib/lib/rlplc.h
/opt/pvb/rllib/lib/rljson.h
/opt/pvb/rllib/lib/rlsubset.h
/opt/pvb/rllib/lib/asc_user.h
/opt/pvb/rllib/lib/cif_dev.h
/opt/pvb/rllib/lib/cif_dev_i.h
/opt/pvb/rllib/lib/cif_types.h
/opt/pvb/rllib/lib/cif_user.h
/opt/pvb/rllib/lib/defines.h
/opt/pvb/rllib/lib/nvr_user.h
/opt/pvb/rllib/lib/rcsdef.h
/opt/pvb/rllib/lib/rcs_user.h
/opt/pvb/rllib/lib/rludpsocket.h
/opt/pvb/rllib/lib/rleibnetip.h
/opt/pvb/rllib/lib/rlopcxmlda.h
/opt/pvb/rllib/lib/rldataaquisition.h
/opt/pvb/rllib/lib/rldataaquisitionprovider.h
/opt/pvb/rllib/lib/rldataacquisition.h
/opt/pvb/rllib/lib/rldataacquisitionprovider.h
/opt/pvb/rllib/lib/rlwebcam.h
/opt/pvb/rllib/lib/rlcommandlineinterface.h
/opt/pvb/rllib/lib/rlreport.h
/opt/pvb/rllib/lib/rlstate.h
/opt/pvb/rllib/lib/rldf1.h
/opt/pvb/rllib/lib/rlhtmldir.h  
/opt/pvb/rllib/lib/rlhtml.h
/opt/pvb/rllib/lib/rlsvgvdi.h
/opt/pvb/language_bindings/lua/pvslua/pvslua
/opt/pvb/language_bindings/lua/pvapplua/pvapplua
/opt/pvb/designer/plugins/libpvb_designer_plugin.so
/opt/pvb/designer/plugins/libqwt_designer_plugin.so
/opt/pvb/designer/README.txt

