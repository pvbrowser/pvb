rem /*******************************************************************************
rem **    Script for compiling PvBrowser with MinGW
rem **    Version 0.1
rem **
rem **    Timo Pallach <timo.pallach@gmx.de>
rem **
rem **    This program is free software; you can redistribute it and/or
rem **    modify it under the terms of version 2 of the GNU General Public
rem **    License as published by the Free Software Foundation.
rem **
rem **    This program is distributed in the hope that it will be useful,
rem **    but WITHOUT ANY WARRANTY; without even the implied warranty of
rem **    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem **    GNU General Public License for more details.
rem **
rem **    You should have received a copy of the GNU General Public License
rem **    along with this program; if not, write to the Free Software
rem **    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
rem *******************************************************************************/

@ECHO OFF

rem  1.) Check if %QTDIR% was set.
IF NOT DEFINED QTDIR (GOTO :NOQTDIR)

rem  2.) Check if %MINGWDIR% was set.
IF NOT DEFINED MINGWDIR (GOTO :NOMINGWDIR)

rem  3.) Check if %PVBDIR% was set.
IF NOT DEFINED PVBDIR (GOTO :NOPVBDIR)

rem  4.) Set local variables.
SET MINGWMAKE=mingw32-make.exe
SET STARTDIR=%CD%
SET PVBWINDIR=%PVBDIR%\win
SET WINMINGWDIR=win-mingw
SET PVBMINGWDIR=%PVBDIR%\%WINMINGWDIR%
SET PVBINDIRNAME=bin
SET PVBINOLDDIRNAME=bin-original
SET BUILD_BAT_FILE=build.bat
SET BUILD_BAT=%PVBMINGWDIR%\%BUILD_BAT_FILE%
SET CLEAN_BAT_FILE=clean.bat
SET CLEAN_BAT=%PVBMINGWDIR%\%CLEAN_BAT_FILE%
SET PVBROWSER_PRO_FILE=pvbrowser.pro
SET PVBROWSER_PRO=%PVBMINGWDIR%\pvbrowser\%PVBROWSER_PRO_FILE%
SET PVPLUGINMAIN_PRO=%PVBMINGWDIR%\browserplugin\pvpluginmain.pro
SET PVDEVELOP_PRO_FILE=pvdevelop.pro
SET PVDEVELOP_PRO=%PVBMINGWDIR%\pvdevelop\%PVDEVELOP_PRO_FILE%
SET RLHISTORY_PRO_FILE=rlhistory.pro
SET RLHISTORY_PRO=%PVBMINGWDIR%\rlhistory\%RLHISTORY_PRO_FILE%
SET PVBMAKE_BAT_FILE=pvb_make.bat
SET PVBMAKE_BAT=%PVBMINGWDIR%\bin\%PVBMAKE_BAT_FILE%
SET PVBMAKEMODBUSDAEMON_BAT_FILE=pvb_make_modbusdaemon.bat
SET PVBMAKEMODBUSDAEMON_BAT=%PVBMINGWDIR%\bin\%PVBMAKEMODBUSDAEMON_BAT_FILE%
SET PVBMAKEPPIDAEMON_BAT_FILE=pvb_make_ppidaemon.bat
SET PVBMAKEPPIDAEMON_BAT=%PVBMINGWDIR%\bin\%PVBMAKEPPIDAEMON_BAT_FILE%
SET PVBMAKESIEMENSDAEMON_BAT_FILE=pvb_make_siemensdaemon.bat
SET PVBMAKESIEMENSDAEMON_BAT=%PVBMINGWDIR%\bin\%PVBMAKESIEMENSDAEMON_BAT_FILE%
SET PVBSERVER_BAT_FILE=pvb_server.bat
SET PVBSERVER_BAT=%PVBMINGWDIR%\bin\%PVBSERVER_BAT_FILE%
SET PVBROWSERBAT_BAT_FILE=pvbrowser_bat.bat
SET PVBROWSERBAT_BAT=%PVBMINGWDIR%\bin\%PVBROWSERBAT_BAT_FILE%
SET PVDEVELOPBAT_BAT_FILE=start_pvdevelop.bat
SET PVDEVELOPBAT_BAT=%PVBMINGWDIR%\bin\%PVDEVELOPBAT_BAT_FILE%
SET PVENV_BAT_FILE=pvenv.bat
SET PVENV_BAT=%PVBMINGWDIR%\bin\%PVENV_BAT_FILE%
SET PVMAKE_BAT_FILE=pvmake.bat
SET PVMAKE_BAT=%PVBMINGWDIR%\bin\%PVMAKE_BAT_FILE%

rem  5.) Copy win directory to win-mingw directory.
XCOPY /e /i /s /y /h %PVBWINDIR% %PVBMINGWDIR%
mkdir %PVBMINGWDIR%\browserplugin

rem  6.) Clean bin directory.
DEL /S /Q %PVBMINGWDIR%\%PVBINDIRNAME%

rem  7.) Copy all dlls to the new bin directory.
COPY %QTDIR%\bin\QtCore4.dll     %PVBMINGWDIR%\%PVBINDIRNAME%
COPY %QTDIR%\bin\QtGui4.dll      %PVBMINGWDIR%\%PVBINDIRNAME%
COPY %QTDIR%\bin\QtOpenGL4.dll   %PVBMINGWDIR%\%PVBINDIRNAME%
COPY %QTDIR%\bin\QtSql4.dll      %PVBMINGWDIR%\%PVBINDIRNAME%
COPY %QTDIR%\bin\QtSvg4.dll      %PVBMINGWDIR%\%PVBINDIRNAME%
COPY %QTDIR%\bin\QtXml4.dll      %PVBMINGWDIR%\%PVBINDIRNAME%
COPY %QTDIR%\bin\QtWebKit4.dll   %PVBMINGWDIR%\%PVBINDIRNAME%
COPY %QTDIR%\bin\QtNetwork4.dll  %PVBMINGWDIR%\%PVBINDIRNAME%
COPY %MINGWDIR%\bin\mingwm10.dll %PVBMINGWDIR%\%PVBINDIRNAME%

rem  7.a) Copy some bat files to the new bin directory.
COPY %PVBDIR%\win\bin\pvb_copy_python_template.bat %PVBDIR%\win-mingw\bin\
COPY %PVBDIR%\win\bin\pvb_import_ui.bat            %PVBDIR%\win-mingw\bin\

echo "You can ignore the following message."

rem  8.) Replace file build.bat.
CALL :ReplaceBuildFile

rem  9.) Replace file clean.bat.
CALL :ReplaceCleanFile

rem 10.) Replace file pvbrowser.pro.
CALL :ReplacePvbrowserProjectFile

rem 11.a) Replace file pvdevelop.pro.
CALL :ReplacePvdevelopProjectFile

rem 11.b) Replace file pvdevelop.pro.
CALL :ReplacePvpluginmainProjectFile

rem 12.) Replace file rlhistory.pro.
CALL :ReplaceRlhistoryProjectFile

rem 13.) Replace file pvb_make.bat.
CALL :ReplacePvbMakeBatFile

rem 14.) Replace file pvb_make_modbusdaemon.bat.
CALL :ReplacePvbMakeModbusdaemonBatFile

rem 15.) Replace file pvb_make_ppidaemon.bat.
CALL :ReplacePvbMakePpidaemonBatFile

rem 16.) Replace file pvb_make_siemensdaemon.bat.
CALL :ReplacePvbMakeSiemensdaemonBatFile

rem 17.) Replace file pvb_server.bat.
CALL :ReplacePvbServerBatFile

rem 18.) Replace file pvbrowser_bat.bat.
CALL :ReplacePvbrowserBatBatFile

rem 19.) Replace file pvdevelop_bat.bat.
CALL :ReplacePvdevelopBatBatFile

rem 20.) Replace file pvenv.bat.
CALL :ReplacePvenvBatFile

rem 21.) Replace file pvmake.bat.
CALL :ReplacePvmakeBatFile

rem 21a.) Copy pvb_import_ui.bat
copy %PVBDIR%\win\bin\pvb_import_ui.bat %PVBDIR%\win-mingw\bin\

rem 22.) Change to win-mingw directory and call build.bat.
CD %WINMINGWDIR%
copy %PVBDIR%\rllib\lib\nodave.h
copy %PVBDIR%\rllib\lib\nodave.c
copy %PVBDIR%\rllib\lib\log.h
gcc -c nodave.c -DBCCWIN -D_WIN32 -o nodave_mingw.o
CALL %BUILD_BAT_FILE%
CD %STARTDIR%

rem 23.) Exit the batchfile after a button was hit after unsetting the local used variables.
SET MINGWMAKE=
SET STARTDIR=
SET PVBWINDIR=
SET WINMINGWDIR=
SET PVBMINGWDIR=
SET PVBINDIRNAME=
SET PVBINOLDDIRNAME=
SET BUILD_BAT_FILE=
SET BUILD_BAT=
SET CLEAN_BAT_FILE=
SET CLEAN_BAT=
SET PVBROWSER_PRO_FILE=
SET PVBROWSER_PRO=
SET PVPLUGINMAIN_PRO=
SET PVDEVELOP_PRO_FILE=
SET PVDEVELOP_PRO=
SET RLHISTORY_PRO_FILE=
SET RLHISTORY_PRO=
SET PVBMAKE_BAT_FILE=
SET PVBMAKE_BAT=
SET PVBMAKEMODBUSDAEMON_BAT_FILE=
SET PVBMAKEMODBUSDAEMON_BAT=
SET PVBMAKEPPIDAEMON_BAT_FILE=
SET PVBMAKEPPIDAEMON_BAT=
SET PVBMAKESIEMENSDAEMON_BAT_FILE=
SET PVBMAKESIEMENSDAEMON_BAT=
SET PVBSERVER_BAT_FILE=
SET PVBSERVER_BAT=
SET PVBROWSERBAT_BAT_FILE=
SET PVBROWSERBAT_BAT=
SET PVDEVELOPBAT_BAT_FILE=
SET PVDEVELOPBAT_BAT=
SET PVENV_BAT_FILE=
SET PVENV_BAT=
SET PVMAKE_BAT_FILE=
SET PVMAKE_BAT=
PAUSE
GOTO :EOF

rem Tell user to set the QTDIR environment variable prior to restart this batch file.
:NOQTDIR
ECHO You have to set the environment variable QTDIR to the directory of your qt installation.
ECHO Please set the QTDIR environment variable and start this (%0) batchfile again.
PAUSE
GOTO :EOF

rem Tell user to set the MINGWDIR environment variable prior to restart this batch file.
:NOMINGWDIR
ECHO You have to set the environment variable MINGWDIR to the directory of your MinGW installation.
ECHO Please set the MINGWDIR environment variable and start this (%0) batchfile again.
PAUSE
GOTO :EOF

rem Tell user to set the PVBDIR environment variable prior to restart this batch file.
:NOPVBDIR
ECHO You have to set the environment variable PVBDIR to the directory where you extracted the PVB archive to.
ECHO Please set the PVBDIR environment variable and start this (%0) batchfile again.
PAUSE
GOTO :EOF

rem Echo the new content of the build.bat file into the original one.
:ReplaceBuildFile
ECHO cd ../qwt                                                                   > %BUILD_BAT%
ECHO cd src                                                                     >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake src.pro -o Makefile                                    >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO cd designer                                                                >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake designer.pro -o Makefile                               >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO cd textengines                                                             >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake textengines.pro -o Makefile                            >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake qwt.pro -o Makefile                                    >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ../%WINMINGWDIR%                                                        >> %BUILD_BAT%
ECHO rem pause                                                                  >> %BUILD_BAT%
ECHO.                                                                           >> %BUILD_BAT%
ECHO cd ..\designer                                                             >> %BUILD_BAT%
ECHO cd src                                                                     >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake  pvbdummy.pro -o Makefile                              >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake  pvbplugin.pro -o Makefile                             >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO cd win-mingw                                                               >> %BUILD_BAT%
ECHO rem pause                                                                  >> %BUILD_BAT%
ECHO                                                                            >> %BUILD_BAT%
ECHO cd pvbrowser                                                               >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake pvbrowser.pro -o Makefile                              >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO rem pause                                                                  >> %BUILD_BAT%
ECHO.                                                                           >> %BUILD_BAT%
ECHO cd browserplugin                                                           >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake pvpluginmain.pro -o Makefile                           >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO rem pause                                                                  >> %BUILD_BAT%
ECHO.                                                                           >> %BUILD_BAT%
ECHO cd pvdevelop                                                               >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake pvdevelop.pro -o Makefile                              >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO rem pause                                                                  >> %BUILD_BAT%
ECHO.                                                                           >> %BUILD_BAT%
ECHO cd pvserver                                                                >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake pvsmt.pro -o Makefile                                  >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO rem pause                                                                  >> %BUILD_BAT%
ECHO.                                                                           >> %BUILD_BAT%
ECHO cd rllib                                                                   >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake lib.pro -o Makefile                                    >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO copy rllib\librllib.a bin\librllib.a                                       >> %BUILD_BAT%
ECHO rem pause                                                                  >> %BUILD_BAT%
ECHO.                                                                           >> %BUILD_BAT%
ECHO cd rlsvg                                                                   >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake rlsvgcat.pro -o Makefile                               >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO rem pause                                                                  >> %BUILD_BAT%
ECHO.                                                                           >> %BUILD_BAT%
ECHO cd rlhistory                                                               >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake rlhistory.pro -o Makefile                              >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO rem pause                                                                  >> %BUILD_BAT%
ECHO.                                                                           >> %BUILD_BAT%
ECHO cd ..\start_pvbapp                                                         >> %BUILD_BAT%
ECHO %%QTDIR%%\bin\qmake start_pvbapp.pro -o Makefile                           >> %BUILD_BAT%
ECHO %MINGWMAKE%                                                                >> %BUILD_BAT%
ECHO cd ..\win-mingw                                                            >> %BUILD_BAT%
ECHO rem pause                                                                  >> %BUILD_BAT%
ECHO.                                                                           >> %BUILD_BAT%
ECHO cd fake_qmake                                                              >> %BUILD_BAT%
ECHO g++ fake_qmake.cpp rlcutil.cpp rlspreadsheet.cpp -o fake_qmake.exe         >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO cd setupregistry                                                           >> %BUILD_BAT%
ECHO g++ SetupRegistry.cpp -o SetupRegistry.exe                                 >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO.                                                                           >> %BUILD_BAT%
ECHO copy pvbrowser\release\pvbrowser.exe                             bin\pvbrowser.exe                               >> %BUILD_BAT%
ECHO copy browserplugin\release\nppvbrowser.dll                       bin\nppvbrowser.dll                             >> %BUILD_BAT%
ECHO copy pvdevelop\release\pvdevelop.exe                             bin\pvdevelop.exe                               >> %BUILD_BAT%
ECHO copy pvserver\libserverlib.a                                     bin\libserverlib.a                              >> %BUILD_BAT%
ECHO copy rlsvg\release\rlsvgcat.exe                                  bin\rlsvgcat.exe                                >> %BUILD_BAT%
ECHO copy rlhistory\release\rlhistory.exe                             bin\rlhistory.exe                               >> %BUILD_BAT%
ECHO copy ..\start_pvbapp\start_pvbapp.exe                            bin\start_pvbapp.exe                            >> %BUILD_BAT%
ECHO copy fake_qmake\fake_qmake.exe                                   bin\fake_qmake.exe                              >> %BUILD_BAT%
ECHO copy setupregistry\SetupRegistry.exe                             bin\SetupRegistry.exe                           >> %BUILD_BAT%
ECHO xcopy /e /y "%QTDIR%\plugins\imageformats"                       bin\plugins\imageformats                        >> %BUILD_BAT%
ECHO copy ..\qwt\designer\plugins\designer\qwt_designer_plugin5.dll   bin\plugins\designer\qwt_designer_plugin5.dll   >> %BUILD_BAT%
ECHO copy ..\qwt\designer\plugins\designer\libqwt_designer_plugin5.a  bin\plugins\designer\libqwt_designer_plugin5.a  >> %BUILD_BAT%
ECHO copy ..\designer\plugins\pvb_designer_plugin.dll                 bin\plugins\designer\pvb_designer_plugin.dll    >> %BUILD_BAT%
ECHO copy ..\designer\plugins\libpvb_designer_plugin.a                bin\plugins\designer\libpvb_designer_plugin.a   >> %BUILD_BAT%
ECHO copy bin\plugins\designer\*designer_plugin*                      %%QTDIR%%\plugins\designer\                     >> %BUILD_BAT%
ECHO.                                                                           >> %BUILD_BAT%
ECHO echo If you_want a Python binding then run: build.bat all                  >> %BUILD_BAT%
ECHO if x%%1==x GOTO SKIP_SCRIPT                                                >> %BUILD_BAT%
ECHO cd language_bindings\swig                                                  >> %BUILD_BAT%
ECHO call build.bat                                                             >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO cd ..                                                                      >> %BUILD_BAT%
ECHO :SKIP_SCRIPT                                                               >> %BUILD_BAT%

ECHO rem build python binding                                                                  >  language_bindings\swig\build.bat
ECHO %%swigdir%%\swig -c++ -python -D_WIN32 language_binding.i                                >>  language_bindings\swig\build.bat 
ECHO %%swigdir%%\swig -c++ -python -DRLWIN32 -D_WIN32 -DSWIG_SESSION language_binding_rllib.i >>  language_bindings\swig\build.bat
ECHO %%qtdir%%\bin\qmake pv.pro                                                               >>  language_bindings\swig\build.bat
ECHO mingw32-make.exe                                                                         >>  language_bindings\swig\build.bat
ECHO %%qtdir%%\bin\qmake rllib.pro                                                            >>  language_bindings\swig\build.bat
ECHO mingw32-make.exe                                                                         >>  language_bindings\swig\build.bat
ECHO echo "results are within debug. Ignore the error."                                       >>  language_bindings\swig\build.bat
ECHO copy *.py            ..\..\bin\                                                          >>  language_bindings\swig\build.bat
ECHO copy release\*.dll   ..\..\bin\                                                          >>  language_bindings\swig\build.bat
ECHO copy release\lib_*.a ..\..\bin\                                                          >>  language_bindings\swig\build.bat

GOTO :EOF

rem Echo the new content of the clean.bat file into the original one.
:ReplaceCleanFile
ECHO cd ../qwt                                                                   > %CLEAN_BAT%
ECHO %MINGWMAKE% clean                                                          >> %CLEAN_BAT%
ECHO cd ../win                                                                  >> %CLEAN_BAT%
ECHO.                                                                           >> %CLEAN_BAT%
ECHO cd pvbrowser                                                               >> %CLEAN_BAT%
ECHO %MINGWMAKE% clean                                                          >> %CLEAN_BAT%
ECHO cd ..                                                                      >> %CLEAN_BAT%
ECHO.                                                                           >> %CLEAN_BAT%
ECHO cd pvdevelop                                                               >> %CLEAN_BAT%
ECHO %MINGWMAKE% clean                                                          >> %CLEAN_BAT%
ECHO cd ..                                                                      >> %CLEAN_BAT%
ECHO.                                                                           >> %CLEAN_BAT%
ECHO cd pvserver                                                                >> %CLEAN_BAT%
ECHO %MINGWMAKE% clean                                                          >> %CLEAN_BAT%
ECHO cd ..                                                                      >> %CLEAN_BAT%
ECHO.                                                                           >> %CLEAN_BAT%
ECHO cd rllib                                                                   >> %CLEAN_BAT%
ECHO %MINGWMAKE% clean                                                          >> %CLEAN_BAT%
ECHO cd ..                                                                      >> %CLEAN_BAT%
ECHO.                                                                           >> %CLEAN_BAT%
ECHO cd rlsvg                                                                   >> %CLEAN_BAT%
ECHO %MINGWMAKE% clean                                                          >> %CLEAN_BAT%
ECHO cd ..                                                                      >> %CLEAN_BAT%
ECHO.                                                                           >> %CLEAN_BAT%
ECHO cd rlhistory                                                               >> %CLEAN_BAT%
ECHO %MINGWMAKE% clean                                                          >> %CLEAN_BAT%
ECHO cd ..                                                                      >> %CLEAN_BAT%
ECHO.                                                                           >> %CLEAN_BAT%
ECHO cd fake_qmake                                                              >> %CLEAN_BAT%
ECHO del *.o                                                                    >> %CLEAN_BAT%
ECHO del *.exe                                                                  >> %CLEAN_BAT%
ECHO cd ..                                                                      >> %CLEAN_BAT%
ECHO.                                                                           >> %CLEAN_BAT%
ECHO cd setupregistry                                                           >> %CLEAN_BAT%
ECHO del *.o                                                                    >> %CLEAN_BAT%
ECHO del *.exe                                                                  >> %CLEAN_BAT%
ECHO cd                                                                         >> %CLEAN_BAT%
GOTO :EOF

rem Echo the new content of the pvbrowser.pro file into the original one.
:ReplacePvbrowserProjectFile
ECHO #######################################                                     > %PVBROWSER_PRO%
ECHO # project file for pvbrowser          #                                    >> %PVBROWSER_PRO%
ECHO # you can uncomment CONFIG += USE_VTK #                                    >> %PVBROWSER_PRO%
ECHO #######################################                                    >> %PVBROWSER_PRO%
ECHO #CONFIG       += USE_VTK                                                   >> %PVBROWSER_PRO%
ECHO #DEFINES     += NO_QWT                                                     >> %PVBROWSER_PRO%
ECHO DEFINES      -= UNICODE                                                    >> %PVBROWSER_PRO%
ECHO QT           += opengl svg webkit network                                  >> %PVBROWSER_PRO%
ECHO CONFIG       += uitools warn_on release                                    >> %PVBROWSER_PRO%
ECHO.                                                                           >> %PVBROWSER_PRO%
ECHO HEADERS       = ../../pvbrowser/mainwindow.h \                             >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/dlgopt.h \                                 >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/opt.h \                                    >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/tcputil.h \                                >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/interpreter.h \                            >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/pvserver.h \                               >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/MyWidgets.h \                              >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/qimagewidget.h \                           >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/qdrawwidget.h \                            >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/pvglwidget.h \                             >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/qwtwidgets.h \                             >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/qwtplotwidget.h \                          >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/dlgtextbrowser.h \                         >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/webkit_ui_dlgtextbrowser.h \               >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/dlgmybrowser.h                             >> %PVBROWSER_PRO%
ECHO.                                                                           >> %PVBROWSER_PRO%
ECHO SOURCES       = ../../pvbrowser/main.cpp \                                 >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/mainwindow.cpp \                           >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/dlgopt.cpp \                               >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/opt.cpp \                                  >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/tcputil.cpp \                              >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/interpreter.cpp \                          >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/MyWidgets.cpp \                            >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/QDrawWidget.cpp \                          >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/QImageWidget.cpp \                         >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/pvglwidget.cpp \                           >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/gldecode.cpp \                             >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/qwtplotwidget.cpp \                        >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/dlgtextbrowser.cpp \                       >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/dlgmybrowser.cpp                           >> %PVBROWSER_PRO%
ECHO.                                                                           >> %PVBROWSER_PRO%
REM ECHO FORMS         = ../../pvbrowser/dlgtextbrowser.ui                          >> %PVBROWSER_PRO%
ECHO.                                                                           >> %PVBROWSER_PRO%
ECHO #INCLUDEPATH  += ../../qwt/include                                         >> %PVBROWSER_PRO%
ECHO INCLUDEPATH  += ../../qwt/src                                              >> %PVBROWSER_PRO%
ECHO LIBS         += ../../qwt/lib/libqwt.a                                     >> %PVBROWSER_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libws2_32.a                                >> %PVBROWSER_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libimm32.a                                 >> %PVBROWSER_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libopengl32.a                              >> %PVBROWSER_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libglu32.a                                 >> %PVBROWSER_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libadvapi32.a                              >> %PVBROWSER_PRO%
ECHO.                                                                           >> %PVBROWSER_PRO%
ECHO ### begin USE_VTK #############################################            >> %PVBROWSER_PRO%
ECHO USE_VTK {                                                                  >> %PVBROWSER_PRO%
ECHO.                                                                           >> %PVBROWSER_PRO%
ECHO DEFINES      += USE_VTK                                                    >> %PVBROWSER_PRO%
ECHO.                                                                           >> %PVBROWSER_PRO%
ECHO HEADERS      += ../../pvbrowser/pvVtkTclWidget.h \                         >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/QVTKWidget.h                               >> %PVBROWSER_PRO%
ECHO.                                                                           >> %PVBROWSER_PRO%
ECHO SOURCES      += ../../pvbrowser/pvVtkTclWidget.cpp \                       >> %PVBROWSER_PRO%
ECHO                 ../../pvbrowser/QVTKWidget.cpp                             >> %PVBROWSER_PRO%
ECHO.                                                                           >> %PVBROWSER_PRO%
ECHO INCLUDEPATH  += $(HOME)\vtk5\VTK\Rendering                                 >> %PVBROWSER_PRO%
ECHO INCLUDEPATH  += $(HOME)\vtk5\VTK\Common                                    >> %PVBROWSER_PRO%
ECHO INCLUDEPATH  += $(HOME)\vtk5\VTK\Filtering                                 >> %PVBROWSER_PRO%
ECHO INCLUDEPATH  += $(HOME)\vtk5\VTK\Graphics                                  >> %PVBROWSER_PRO%
ECHO INCLUDEPATH  += $(VTKDIR)                                                  >> %PVBROWSER_PRO%
ECHO INCLUDEPATH  += $(TCLDIR)/include                                          >> %PVBROWSER_PRO%
ECHO.                                                                           >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkCommon.lib                                        >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkCommonTCL.lib                                     >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkDICOMParser.lib                                   >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkexoIIc.lib                                        >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkexpat.lib                                         >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkFiltering.lib                                     >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkFilteringTCL.lib                                  >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkfreetype.lib                                      >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkftgl.lib                                          >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkGenericFiltering.lib                              >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkGenericFilteringTCL.lib                           >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkGraphics.lib                                      >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkGraphicsTCL.lib                                   >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkHybrid.lib                                        >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkHybridTCL.lib                                     >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkImaging.lib                                       >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkImagingTCL.lib                                    >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkIO.lib                                            >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkIOTCL.lib                                         >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkjpeg.lib                                          >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkMPEG2Encode.lib                                   >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkNetCDF.lib                                        >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkpng.lib                                           >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkRendering.lib                                     >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkRenderingTCL.lib                                  >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtksys.lib                                           >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtktiff.lib                                          >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkVolumeRendering.lib                               >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkVolumeRenderingTCL.lib                            >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkWidgets.lib                                       >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkWidgetsTCL.lib                                    >> %PVBROWSER_PRO%
ECHO LIBS += $(VTKDIR)\bin\vtkzlib.lib                                          >> %PVBROWSER_PRO%
ECHO LIBS += $(TCLDIR)\lib\tcl84.lib                                            >> %PVBROWSER_PRO%
ECHO.                                                                           >> %PVBROWSER_PRO%
ECHO }                                                                          >> %PVBROWSER_PRO%
ECHO ### end USE_VTK ###############################################            >> %PVBROWSER_PRO%
ECHO.                                                                           >> %PVBROWSER_PRO%
ECHO RESOURCES     = ../../pvbrowser/pvbrowser.qrc                              >> %PVBROWSER_PRO%
ECHO TARGET        = pvbrowser                                                  >> %PVBROWSER_PRO%
GOTO :EOF

rem Echo the new content of the pvpluginmain.pro file into the original one.
:ReplacePvpluginmainProjectFile
ECHO #######################################                                     > %PVPLUGINMAIN_PRO%
ECHO # project file for pvbrowser          #                                    >> %PVPLUGINMAIN_PRO%
ECHO # you can uncomment CONFIG += USE_VTK #                                    >> %PVPLUGINMAIN_PRO%
ECHO #######################################                                    >> %PVPLUGINMAIN_PRO%
ECHO #CONFIG       += USE_VTK                                                   >> %PVPLUGINMAIN_PRO%
ECHO #DEFINES     += NO_QWT                                                     >> %PVPLUGINMAIN_PRO%
ECHO TARGET        = pvbrowser                                                  >> %PVPLUGINMAIN_PRO%
ECHO.                                                                           >> %PVPLUGINMAIN_PRO%
ECHO QT           += opengl svg webkit network                                  >> %PVPLUGINMAIN_PRO%
ECHO INCLUDEPATH  += ../../pvbrowser                                            >> %PVPLUGINMAIN_PRO%
ECHO #INCLUDEPATH  += ../../qwt/include                                         >> %PVPLUGINMAIN_PRO%
ECHO INCLUDEPATH  += ../../qwt/src                                              >> %PVPLUGINMAIN_PRO%
ECHO DEFINES      += BROWSERPLUGIN                                              >> %PVPLUGINMAIN_PRO%
ECHO DEFINES      -= UNICODE                                                    >> %PVPLUGINMAIN_PRO%
ECHO QT           += opengl svg webkit network                                  >> %PVPLUGINMAIN_PRO%
ECHO CONFIG       += uitools warn_on release                                    >> %PVPLUGINMAIN_PRO%
ECHO.                                                                           >> %PVPLUGINMAIN_PRO%
ECHO HEADERS       = ../../pvbrowser/mainwindow.h \                             >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/dlgopt.h \                                 >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/opt.h \                                    >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/tcputil.h \                                >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/interpreter.h \                            >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/pvserver.h \                               >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/MyWidgets.h \                              >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/qimagewidget.h \                           >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/qdrawwidget.h \                            >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/pvglwidget.h \                             >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/qwtwidgets.h \                             >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/qwtplotwidget.h \                          >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/dlgtextbrowser.h \                         >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/webkit_ui_dlgtextbrowser.h \               >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/dlgmybrowser.h                             >> %PVPLUGINMAIN_PRO%
ECHO.                                                                           >> %PVPLUGINMAIN_PRO%
ECHO SOURCES       = ../../browserplugin/pvpluginmain.cpp \                     >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/main.cpp \                                 >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/mainwindow.cpp \                           >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/dlgopt.cpp \                               >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/opt.cpp \                                  >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/tcputil.cpp \                              >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/interpreter.cpp \                          >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/MyWidgets.cpp \                            >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/QDrawWidget.cpp \                          >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/QImageWidget.cpp \                         >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/pvglwidget.cpp \                           >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/gldecode.cpp \                             >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/qwtplotwidget.cpp \                        >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/dlgtextbrowser.cpp \                       >> %PVPLUGINMAIN_PRO%
ECHO                 ../../pvbrowser/dlgmybrowser.cpp                           >> %PVPLUGINMAIN_PRO%
ECHO.                                                                           >> %PVPLUGINMAIN_PRO%
REM ECHO FORMS         = ../../pvbrowser/dlgtextbrowser.ui                          >> %PVPLUGINMAIN_PRO%
ECHO.                                                                           >> %PVPLUGINMAIN_PRO%
ECHO RESOURCES     = ../../browserplugin/pvbrowser.qrc                          >> %PVPLUGINMAIN_PRO%
ECHO.                                                                           >> %PVPLUGINMAIN_PRO%
ECHO LIBS         += ../../qwt/lib/libqwt.a                                     >> %PVPLUGINMAIN_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libws2_32.a                                >> %PVPLUGINMAIN_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libimm32.a                                 >> %PVPLUGINMAIN_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libopengl32.a                              >> %PVPLUGINMAIN_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libglu32.a                                 >> %PVPLUGINMAIN_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libadvapi32.a                              >> %PVPLUGINMAIN_PRO%
ECHO.                                                                           >> %PVPLUGINMAIN_PRO%
ECHO win32 {                                                                    >> %PVPLUGINMAIN_PRO%
ECHO    RC_FILE    = ../../browserplugin/pvpluginmain.rc                        >> %PVPLUGINMAIN_PRO%
ECHO } else:mac {                                                               >> %PVPLUGINMAIN_PRO%
ECHO    QMAKE_INFO_PLIST = Info.plist                                           >> %PVPLUGINMAIN_PRO%
ECHO    REZ_FILES += pvpluginmain.r                                             >> %PVPLUGINMAIN_PRO%
ECHO    rsrc_files.files = pvpluginmain.rsrc                                    >> %PVPLUGINMAIN_PRO%
ECHO    rsrc_files.path = Contents/Resources                                    >> %PVPLUGINMAIN_PRO%
ECHO    QMAKE_BUNDLE_DATA += rsrc_files                                         >> %PVPLUGINMAIN_PRO%
ECHO }                                                                          >> %PVPLUGINMAIN_PRO%
ECHO.                                                                           >> %PVPLUGINMAIN_PRO%
ECHO include(../../browserplugin/qtbrowserplugin.pri)                           >> %PVPLUGINMAIN_PRO%
GOTO :EOF


rem Echo the new content of the pvdevelop.pro file into the original one.
:ReplacePvdevelopProjectFile
ECHO CONFIG       += uitools warn_on release                                    > %PVDEVELOP_PRO%
ECHO DEFINES      += PVDEVELOP                                                  >> %PVDEVELOP_PRO%
ECHO #DEFINES     += NO_QWT                                                     >> %PVDEVELOP_PRO%
ECHO DEFINES      -= UNICODE                                                    >> %PVDEVELOP_PRO%
ECHO QT           += opengl svg webkit                                          >> %PVDEVELOP_PRO%
ECHO.                                                                           >> %PVDEVELOP_PRO%
ECHO HEADERS       = ../../pvbrowser/webkit_ui_dlgtextbrowser.h \               >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/MyWidgets.h \                              >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/qwtwidgets.h \                             >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/qwtplotwidget.h \                          >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/qdrawwidget.h \                            >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/qimagewidget.h \                           >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/pvglwidget.h \                             >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/interpreter.h \                            >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/pvserver.h \                               >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/dlgtextbrowser.h \                         >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/mainwindow.h \                             >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/opt.h \                                    >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/editor.h \                                 >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/designer.h \                               >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/syntax.h \                                 >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/cutil.h \                                  >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgopt.h \                                 >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgnewprj.h \                              >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgsearchreplace.h \                       >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlginsertFunction.h \                      >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgpastewidget.h \                         >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgdaemon.h \                              >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgproperty.h \                            >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlginsertwidget.h \                        >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgeditlayout.h \                          >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/widgetgenerator.h \                        >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/generatepython.h \                         >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/generatephp.h \                            >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/generateperl.h \                           >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/generatetcl.h \                            >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/ui_dlgnewprj.h \                           >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/ui_dlgsearchreplace.h \                    >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/ui_dlginsertfunction.h \                   >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/ui_dlgpastewidget.h \                      >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/ui_dlgdaemon.h \                           >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/ui_dlgproperty.h \                         >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/ui_dlginsertwidget.h \                     >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/ui_dlgeditlayout.h \                       >> %PVDEVELOP_PRO%
ECHO.                                                                           >> %PVDEVELOP_PRO%
ECHO SOURCES       = ../../pvbrowser/MyWidgets.cpp \                            >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/qwtplotwidget.cpp \                        >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/QDrawWidget.cpp \                          >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/QImageWidget.cpp \                         >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/pvglwidget.cpp \                           >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/tcputil.cpp \                              >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/main.cpp \                                 >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/mainwindow.cpp \                           >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/opt.cpp \                                  >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/editor.cpp \                               >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/designer.cpp \                             >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/syntax.cpp \                               >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/cutil.cpp \                                >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgopt.cpp \                               >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgnewprj.cpp \                            >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgsearchreplace.cpp \                     >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlginsertFunction.cpp \                    >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgpastewidget.cpp \                       >> %PVDEVELOP_PRO%
ECHO                 ../../pvbrowser/dlgtextbrowser.cpp \                       >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgdaemon.cpp \                            >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgproperty.cpp \                          >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlginsertwidget.cpp \                      >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgeditlayout.cpp \                        >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/gmodbus.cpp \                              >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/gppi.cpp \                                 >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/gsiemens.cpp \                             >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/widgetgenerator.cpp \                      >> %PVDEVELOP_PRO%
ECHO.                                                                           >> %PVDEVELOP_PRO%
REM ECHO FORMS         = ../../pvbrowser/dlgtextbrowser.ui \                        >> %PVDEVELOP_PRO%
ECHO FORMS         = ../../pvdevelop/dlgnewprj.ui \                             >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgsearchreplace.ui \                      >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlginsertfunction.ui \                     >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgpastewidget.ui \                        >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgdaemon.ui \                             >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgproperty.ui \                           >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlginsertwidget.ui \                       >> %PVDEVELOP_PRO%
ECHO                 ../../pvdevelop/dlgeditlayout.ui                           >> %PVDEVELOP_PRO%
ECHO.                                                                           >> %PVDEVELOP_PRO%
ECHO #INCLUDEPATH  += ../../qwt/include                                         >> %PVDEVELOP_PRO%
ECHO INCLUDEPATH  += ../../qwt/src                                              >> %PVDEVELOP_PRO%
ECHO INCLUDEPATH  += ../../pvbrowser                                            >> %PVDEVELOP_PRO%
ECHO INCLUDEPATH  += ../../pvdevelop                                            >> %PVDEVELOP_PRO%
ECHO.                                                                           >> %PVDEVELOP_PRO%
ECHO LIBS         += ../../qwt/lib/libqwt.a                                     >> %PVDEVELOP_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libws2_32.a                                >> %PVDEVELOP_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libimm32.a                                 >> %PVDEVELOP_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libopengl32.a                              >> %PVDEVELOP_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libglu32.a                                 >> %PVDEVELOP_PRO%
ECHO LIBS         += $(MINGWDIR)/lib/libadvapi32.a                              >> %PVDEVELOP_PRO%
ECHO.                                                                           >> %PVDEVELOP_PRO%
ECHO RESOURCES     = ../../pvdevelop/pvdevelop.qrc                              >> %PVDEVELOP_PRO%
ECHO TARGET = pvdevelop                                                         >> %PVDEVELOP_PRO%
GOTO :EOF

rem Echo the new content of the rlhistory.pro file into the original one.
:ReplaceRlhistoryProjectFile
ECHO ######################################################################      > %RLHISTORY_PRO%
ECHO # Automatically generated by qmake (1.07a) Sat Apr 8 14:34:18 2006         >> %RLHISTORY_PRO%
ECHO ######################################################################     >> %RLHISTORY_PRO%
ECHO.                                                                           >> %RLHISTORY_PRO%
ECHO TEMPLATE = app                                                             >> %RLHISTORY_PRO%
ECHO CONFIG  += console warn_on release                                         >> %RLHISTORY_PRO%
ECHO CONFIG  -= qt                                                              >> %RLHISTORY_PRO%
ECHO INCLUDEPATH += ../../rllib/rlhistory                                       >> %RLHISTORY_PRO%
ECHO.                                                                           >> %RLHISTORY_PRO%
ECHO # Input                                                                    >> %RLHISTORY_PRO%
ECHO # HEADERS +=                                                               >> %RLHISTORY_PRO%
ECHO SOURCES     += ../../rllib/rlhistory/rlhistory.cpp                         >> %RLHISTORY_PRO%
ECHO LIBS        += ../bin/librllib.a                                           >> %RLHISTORY_PRO%
ECHO INCLUDEPATH += ../../rllib/lib                                             >> %RLHISTORY_PRO%
ECHO TARGET = rlhistory                                                         >> %RLHISTORY_PRO%
GOTO :EOF

rem Echo the new content of the pvb_make.bat file into the original one.
:ReplacePvbMakeBatFile
ECHO @echo off                                                                   > %PVBMAKE_BAT%
ECHO rem system("pvb_make.bat + name");                                         >> %PVBMAKE_BAT%
ECHO CALL "%%PVBDIR%%\%WINMINGWDIR%\bin\pvenv.bat"                              >> %PVBMAKE_BAT%
ECHO qmake %%1.pro -o Makefile.win                                              >> %PVBMAKE_BAT%
ECHO %MINGWMAKE% -f Makefile.win                                                >> %PVBMAKE_BAT%
ECHO pause                                                                      >> %PVBMAKE_BAT%
ECHO exit                                                                       >> %PVBMAKE_BAT%
GOTO :EOF

rem Echo the new content of the pvb_make_modbusdaemon.bat file into the original one.
:ReplacePvbMakeModbusdaemonBatFile
ECHO g++ modbusdaemon.cpp "%%PVBDIR%%\%WINMINGWDIR%\bin\librllib.a" "%%MINGWDIR%%\lib\libws2_32.a" "-I%%PVBDIR%%\rllib\lib" "%%MINGWDIR%%\lib\libadvapi32.a" -o modbusdaemon.exe   >  %PVBMAKEMODBUSDAEMON_BAT%
ECHO pause                                                                                                                                                                  >> %PVBMAKEMODBUSDAEMON_BAT%
ECHO exit                                                                                                                                                                   >> %PVBMAKEMODBUSDAEMON_BAT%
GOTO :EOF

rem Echo the new content of the pvb_make_ppidaemon.bat file into the original one.
:ReplacePvbMakePpidaemonBatFile
ECHO g++ ppidaemon.cpp "%%PVBDIR%%\%WINMINGWDIR%\nodave_mingw.o" "%%PVBDIR%%\%WINMINGWDIR%\bin\librllib.a" "%MINGWDIR%\lib\libws2_32.a" "-I%%PVBDIR%%\rllib\lib" -o ppidaemon.exe                                                                      >  %PVBMAKEPPIDAEMON_BAT%
ECHO pause                                                                                                                                                                  >> %PVBMAKEPPIDAEMON_BAT%
ECHO exit                                                                                                                                                                   >> %PVBMAKEPPIDAEMON_BAT%
GOTO :EOF

rem Echo the new content of the pvb_make_siemensdaemon.bat file into the original one.
:ReplacePvbMakeSiemensdaemonBatFile
ECHO g++ siemensdaemon.cpp "%%PVBDIR%%\%WINMINGWDIR%\bin\librllib.a" "%%MINGWDIR%%\lib\libws2_32.a" "-I%%PVBDIR%%\rllib\lib" "%%MINGWDIR%%\lib\libadvapi32.a" -o siemensdaemon.exe >  %PVBMAKESIEMENSDAEMON_BAT%
ECHO pause                                                                                                                                                                  >> %PVBMAKESIEMENSDAEMON_BAT%
ECHO exit                                                                                                                                                                   >> %PVBMAKESIEMENSDAEMON_BAT%
GOTO :EOF

rem Echo the new content of the pvb_server.bat file into the original one.
:ReplacePvbServerBatFile
ECHO @echo off                                                                   > %PVBSERVER_BAT%
ECHO rem system("pvb_server.bat " + name);                                      >> %PVBSERVER_BAT%
ECHO CALL "%%PVBDIR%%\%WINMINGWDIR%\bin\pvenv.bat"                              >> %PVBSERVER_BAT%
ECHO qmake %%1.pro -o Makefile.win                                              >> %PVBSERVER_BAT%
ECHO %MINGWMAKE% -f Makefile.win                                                >> %PVBSERVER_BAT%
ECHO %%1                                                                        >> %PVBSERVER_BAT%
ECHO pause                                                                      >> %PVBSERVER_BAT%
ECHO exit                                                                       >> %PVBSERVER_BAT%
GOTO :EOF


rem Echo the new content of the pvbrowser_bat.bat file into the original one.
:ReplacePvbrowserBatBatFile
rem ECHO rem ###############################                > %PVBROWSERBAT_BAT%
rem ECHO rem # start pvbrowser             #               >> %PVBROWSERBAT_BAT%
rem ECHO rem # no registry settings needed #               >> %PVBROWSERBAT_BAT%
rem ECHO rem ###############################               >> %PVBROWSERBAT_BAT%
rem ECHO @echo off                                         >> %PVBROWSERBAT_BAT%
rem ECHO set PATH=%%PVBDIR%%\%WINMINGWDIR%\bin;%PATH%;     >> %PVBROWSERBAT_BAT%
rem ECHO echo %%PATH%%                                     >> %PVBROWSERBAT_BAT%
rem ECHO echo %%PVBDIR%%                                   >> %PVBROWSERBAT_BAT%
rem ECHO start pvbrowser                                   >> %PVBROWSERBAT_BAT%
GOTO :EOF

rem Echo the new content of the pvdevelop_bat.bat file into the original one.
:ReplacePvdevelopBatBatFile
ECHO  @echo off                                                                            > %PVDEVELOPBAT_BAT%
ECHO  rem ###############################                                                 >> %PVDEVELOPBAT_BAT%
ECHO  rem # start pvdevelop             #                                                 >> %PVDEVELOPBAT_BAT%
ECHO  rem # no registry settings needed #                                                 >> %PVDEVELOPBAT_BAT%
ECHO  rem ###############################                                                 >> %PVDEVELOPBAT_BAT%
ECHO  if    [%%QTDIR%%]    == [] goto environment_not_set                                 >> %PVDEVELOPBAT_BAT%
ECHO  if    [%%MINGWDIR%%] == [] goto environment_not_set                                 >> %PVDEVELOPBAT_BAT%
ECHO  goto  start_pvdevelop                                                               >> %PVDEVELOPBAT_BAT%
ECHO  :environment_not_set                                                                >> %PVDEVELOPBAT_BAT%
ECHO  echo  ############################################################################# >> %PVDEVELOPBAT_BAT%
ECHO  echo  Please adjust environment variables within this file:                         >> %PVDEVELOPBAT_BAT%
ECHO  echo  %%PVBDIR%%\win-mingw\bin\start_pvdevelop.bat                                  >> %PVDEVELOPBAT_BAT%
ECHO  set   QTDIR=z:\win\qt>> %PVDEVELOPBAT_BAT%
ECHO  set   MINGWDIR=z:\win\mingw>> %PVDEVELOPBAT_BAT%
ECHO  rem   if you want to use python>> %PVDEVELOPBAT_BAT%
ECHO  set   PYDIR=c:\Python25>> %PVDEVELOPBAT_BAT%
ECHO  start notepad "%%PVBDIR%%\win-mingw\bin\start_pvdevelop.bat"                        >> %PVDEVELOPBAT_BAT%
ECHO  pause                                                                               >> %PVDEVELOPBAT_BAT%
ECHO  exit                                                                                >> %PVDEVELOPBAT_BAT%
ECHO  echo  ############################################################################# >> %PVDEVELOPBAT_BAT%
ECHO  rem   remove start notepad, pause and exit above                                    >> %PVDEVELOPBAT_BAT%
ECHO  rem   instead of setting QTDIR and MINGWDIR here you could also set them            >> %PVDEVELOPBAT_BAT%
ECHO  rem   within the registry using the system control                                  >> %PVDEVELOPBAT_BAT%
ECHO  :start_pvdevelop                                                                    >> %PVDEVELOPBAT_BAT%
ECHO  set   PATH=%%PVBDIR%%\win-mingw\bin;%%QTDIR%%\bin;%%MINGWDIR%%\bin;c:\windows;>> %PVDEVELOPBAT_BAT%
ECHO  set   PYTHONPATH=%%PVBDIR%%\win-mingw\bin;%%PYTHONPATH%%>> %PVDEVELOPBAT_BAT%
ECHO  echo  PVBDIR     = %%PVBDIR%%                                                       >> %PVDEVELOPBAT_BAT%
ECHO  echo  QTDIR      = %%QTDIR%%                                                        >> %PVDEVELOPBAT_BAT%
ECHO  echo  MINGWDIR   = %%MINGWDIR%%                                                     >> %PVDEVELOPBAT_BAT%
ECHO  echo  PYDIR      = %%PYDIR%%                                                        >> %PVDEVELOPBAT_BAT%
ECHO  echo  PATH       = %%PATH%%                                                         >> %PVDEVELOPBAT_BAT%
ECHO  echo  PYTHONPATH = %%PYTHONPATH%%                                                   >> %PVDEVELOPBAT_BAT%
ECHO  echo  starting pvdevelop                                                            >> %PVDEVELOPBAT_BAT%
ECHO  echo  you can set the above environment variables also within the system control    >> %PVDEVELOPBAT_BAT%
ECHO  echo  and start pvdevelop.exe directly                                              >> %PVDEVELOPBAT_BAT%
ECHO  start pvdevelop                                                                     >> %PVDEVELOPBAT_BAT%
ECHO  :end                                                                                >> %PVDEVELOPBAT_BAT%
ECHO  pause                                                                               >> %PVDEVELOPBAT_BAT%
GOTO :EOF

rem Echo the new content of the pvenv.bat file into the original one.
:ReplacePvenvBatFile
ECHO @echo off                                                                   > %PVENV_BAT%
ECHO ECHO.                                                                       >> %PVENV_BAT%
GOTO :EOF

rem Echo the new content of the pvmake_bat.bat file into the original one.
:ReplacePvmakeBatFile
ECHO @echo off                                          > %PVMAKE_BAT%
ECHO.                                                  >> %PVMAKE_BAT%
ECHO CALL "%PVBDIR%\%WINMINGWDIR%\bin\pvenv.bat"       >> %PVMAKE_BAT%
ECHO.                                                  >> %PVMAKE_BAT%
ECHO IF [%%1] == [] GOTO MAKE_ONLY                     >> %PVMAKE_BAT%
ECHO.                                                  >> %PVMAKE_BAT%
ECHO echo generating Makefile.win ...                  >> %PVMAKE_BAT%
ECHO qmake %%1.pro -o Makefile.win                     >> %PVMAKE_BAT%
ECHO.                                                  >> %PVMAKE_BAT%
ECHO :MAKE_ONLY                                        >> %PVMAKE_BAT%
ECHO %MINGWMAKE% -f Makefile.win                       >> %PVMAKE_BAT%
ECHO pause                                             >> %PVMAKE_BAT%
ECHO exit                                              >> %PVMAKE_BAT%
GOTO :EOF
