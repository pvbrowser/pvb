cd ..\qwt
%QTDIR%\bin\qmake qwt.pro -o Makefile
nmake /f Makefile.Release
cd designer
%QTDIR%\bin\qmake qwtplugin.pro -o Makefile
nmake /f Makefile.Release
cd ..
cd ..\win
rem pause

cd ..\designer
cd src
%QTDIR%\bin\qmake  pvbdummy.pro -o Makefile
nmake /f Makefile.Release
%QTDIR%\bin\qmake  pvbplugin.pro -o Makefile
nmake /f Makefile.Release
cd ..
cd ..
cd win
rem pause

cd pvbrowser
%QTDIR%\bin\qmake pvbrowser.pro -o Makefile
nmake /f Makefile.Release
cd ..
rem pause

cd pvdevelop
%QTDIR%\bin\qmake pvdevelop.pro -o Makefile
nmake /f Makefile.Release
cd ..
rem pause

cd pvserver
%QTDIR%\bin\qmake pvsmt.pro -o Makefile
nmake /f Makefile
cd ..
rem pause

cd rllib
%QTDIR%\bin\qmake lib.pro -o Makefile
nmake /f Makefile
cd ..
copy rllib\rllib.lib                 bin\rllib.lib
rem pause

cd rlsvg
%QTDIR%\bin\qmake rlsvgcat.pro -o Makefile
nmake /f Makefile.Release
cd ..
rem pause

cd rlhistory
%QTDIR%\bin\qmake rlhistory.pro -o Makefile
nmake /f Makefile.Release
cd ..
rem pause

cd ..\start_pvbapp
%QTDIR%\bin\qmake start_pvbapp.pro -o Makefile
nmake /f Makefile.Release
cd ..\win
rem pause

cd fake_qmake
cl fake_qmake.cpp rlcutil.cpp rlspreadsheet.cpp
cd ..
cd setupregistry
cl SetupRegistry.cpp advapi32.lib
cd ..

if x%1==x GOTO SKIP_SCRIPT
cd language_bindings\swig
# you may comment this out, if you don't want python
call build.bat
cd ..
cd ..

:SKIP_SCRIPT
copy pvbrowser\release\pvbrowser.exe                          bin\pvbrowser.exe
copy pvdevelop\release\pvdevelop.exe                          bin\pvdevelop.exe
copy ..\start_pvbapp\release\start_pvbapp.exe                 bin\start_pvbapp.exe
copy pvserver\serverlib.lib                                   bin\serverlib.lib
copy rlsvg\release\rlsvgcat.exe                               bin\rlsvgcat.exe
copy rlhistory\release\rlhistory.exe                          bin\rlhistory.exe
copy fake_qmake\fake_qmake.exe                                bin\fake_qmake.exe
copy setupregistry\SetupRegistry.exe                          bin\SetupRegistry.exe
copy ..\qwt\designer\plugins\designer\qwt_designer_plugin.dll bin\plugins\designer\qwt_designer_plugin.dll
copy ..\qwt\designer\plugins\designer\qwt_designer_plugin.lib bin\plugins\designer\qwt_designer_plugin.lib
copy ..\designer\plugins\pvb_designer_plugin.dll              bin\plugins\designer\pvb_designer_plugin.dll
copy ..\designer\plugins\pvb_designer_plugin.lib              bin\plugins\designer\pvb_designer_plugin.lib
del  ..\designer\plugins\pvb_designer_plugin.dll
del  ..\designer\plugins\pvb_designer_plugin.lib
del  ..\designer\plugins\pvb_designer_plugin.exp

echo "If you want to build python support, call me again with: build all"
