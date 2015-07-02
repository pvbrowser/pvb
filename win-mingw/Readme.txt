pvbrowser.exe does not need any registry settings,
and can be started directly.

pvdevelop.exe needs some environment variables and
is currently started using start_pvdevelop.bat

Within start_pvdevelop.bat the environment variables are set.
You have to adjust these to your configuration.
You can alternatively define these environment variables within
system control and start pvdevelop.exe directly.

For running your servers as a standalone program
%PVDIR%\win-mingw\bin
must be included within the PATH environment variable.

For pvdevelop you need to install the following additional software.
- Qt
- MinGW

Qt can be installed from
https://www.qt.io/download
During the installation of Qt you will be asked,
if you want to install MinGW also.

----------------------------------------------------------------

For compiling pvbrowser from sourcecode adjust login.bat and run

login.bat
build5.bat
build_language_bindings.bat

----------------------------------------------------------------
