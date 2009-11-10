@echo off

rem we no longer use any registry settings
rem you can avoid the batch files by setting the following environment variables:
rem echo "Setting: PVBDIR=%CD% and PATH=%PATH%;%CD%\win-mingw\bin"
rem win-mingw\bin\SetupRegistry.exe

reg add "HKEY_LOCAL_MACHINE\SYSTEM\ControlSet001\Control\Session Manager\Environment" /v PVBDIR /t REG_SZ /d "%CD%" /f
echo "Please add PVBDIR=%CD%\bin to your PATH environment variable."

win\bin\shellink new pvbrowser.lnk -t "%CD%\win\bin\pvbrowser_bat.bat" -e pvbrowser -i "%CD%\win\logo1.ico" -d "%CD%"
win\bin\shellink new pvdevelop.lnk -t "%CD%\win\bin\pvdevelop_bat.bat" -e pvdevelop -i "%CD%\win\logo1.ico" -d "%CD%"

win\bin\shellink new "%USERPROFILE%\Desktop\pvbrowser.lnk" -t "%CD%\win\bin\pvbrowser_bat.bat" -e pvbrowser  -i "%CD%\win\logo1.ico" -d "%CD%"
win\bin\shellink new "%USERPROFILE%\Desktop\pvdevelop.lnk" -t "%CD%\win\bin\pvdevelop_bat.bat" -e pvdevelop  -i "%CD%\win\logo1.ico" -d "%CD%"

echo "##################################################################"
echo "# Now the complete pvbrowser development package is installed.   #"
echo "# For pvdevelop MinGW or Visual C++ must be installed !          #"
echo "# Have a lot of fun - Yours pvbrowser community                  #"
echo "##################################################################"
pause
