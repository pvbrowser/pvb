@echo off
echo """""""""""""""""""""""""""""""""""""""""""
echo "update pvbrowser                         "
echo "Please close all pvbrowser applications  "
echo """""""""""""""""""""""""""""""""""""""""""

rem cd %PVBDIR%
cd ..
cd ..
rem from %PVBDIR% cd ..
set PVBDIR = "%CD%"
set PATH = "%PATH%;%CD%\win\bin"
cd ..
pause
wget http://pvbrowser.de/pvbrowser/tar/pvb.tar.gz
echo """""""""""""""""""""""""""""""""""""""""""
echo " now restoreing new version of pvbrowser "
echo " ignore the warnings during unzipping    "
echo """""""""""""""""""""""""""""""""""""""""""
gzip -d pvb.tar.gz
tar -xvf pvb.tar
del pvb.tar
echo """""""""""""""""""""""""""""""""""""""""""
echo "update done                              "
echo "Have a lot of fun                        "
echo "Yours pvbrowser community                "
echo """""""""""""""""""""""""""""""""""""""""""
pause
exit