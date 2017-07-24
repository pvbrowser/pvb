echo "run this batch file after you have build qwt and pvbrowser using qtcreator msvc"

copy ..\build-pvbrowser-Desktop_Qt_5_7_1_MSVC2015_64bit-Debug\release\pvbrowser.exe bin\
copy c:\qt\qt5.7.1\5.7\msvc2015_64\bin\*.dll bin\
del bin\*d.dll

