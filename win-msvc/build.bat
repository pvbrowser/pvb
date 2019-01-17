echo "run this batch file after you have build qwt and pvbrowser using qtcreator msvc"
echo First:
echo run qtcreator qwt/src/src.pro
echo run qtcreator pvbrowser/pvbrowser_msvc.pro
echo Then:
copy C:\rl\cc\pvb\build-pvbrowser_msvc-Desktop_Qt_5_12_0_MSVC2017_64bit-Release\release\pvbrowser.exe bin\pvbrowser.exe
c:\qt\qt5.12.0\5.12.0\msvc2017_64\bin\windeployqt bin\pvbrowser.exe

