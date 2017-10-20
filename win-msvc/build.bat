echo "run this batch file after you have build qwt and pvbrowser using qtcreator msvc"
echo First:
echo run qtcreator qwt/src/src.pro
echo run qtcreator pvbrowser/pvbrowser_msvc.pro
echo Then:
copy C:\rl\cc\pvb\build-pvbrowser_msvc-Desktop_Qt_5_9_1_MSVC2015_64bit-Debug\release\pvbrowser.exe bin\pvbrowser.exe
c:\qt\qt5.9.1\5.9.1\msvc2015_64\bin\windeployqt bin\pvbrowser.exe

