++++++++++++++++++++
+ PVB-MinGW-Script +
++++++++++++++++++++
This manual/tutorial shows how to use PVB with MinGW and Qt4.
(Timo Pallach <timo.pallach@gmx.de>)

Version of this Script
======================
Version 0.2

Requirements
============
A.) You will need a internet connection to download the
    needed software and sourcecode archives.
B.) You will need at least 3.5 GB of free disk space on
    the harddrive for comiling the Qt executales and resources.

Files contained in this version
===============================
* CreatePvbWithMinGW.bat       Creates the PVB executables using the MinGW compiler.
* Readme.txt                   This manual and tutorial file.

Versions the have been tested
=============================
/--------------------------------------------------------------------------------------------\
| Windows-Version              | MinGW-Version | Qt-Version | PVB-Version | PVB-MinGW-Script |
|==============================|===============|============|=============|==================|
| Windows XP Professional SP2  | 3.4.5         | 4.3.1      | 4.1.9       | 0.1              |
|------------------------------|---------------|------------|-------------|------------------|
| Windows Server 2003          | 3.4.5         | 4.3.1      | 4.1.9       | 0.1              |
| Standard Edition SP1         |               |            |             |                  |
|------------------------------|---------------|------------|-------------|------------------|
| Windows XP Home Media Center | 3.4.5         | 4.3.1      | 4.1.9       | 0.1              |
| Edtion Version 2002 SP2      |               |            |             |                  |
|--------------------------------------------------------------------------------------------|
| Windows XP Professional SP2  | 3.4.5         | 4.3.1      | 4.2.0       | 0.2              |
|------------------------------|---------------|------------|-------------|------------------|
| Windows Server 2003          | 3.4.5         | 4.3.1      | 4.2.0       | 0.2              |
| Standard Edition SP1         |               |            |             |                  |
|------------------------------|---------------|------------|-------------|------------------|
| Windows XP Home Media Center | 3.4.5         | 4.3.1      | 4.2.0       | 0.2              |
| Edtion Version 2002 SP2      |               |            |             |                  |
\--------------------------------------------------------------------------------------------/

##############################################################################################

Installation with binary Qt4 library
====================================
1.) Installation of Qt4 and MinGW
---------------------------------
1.1.) Goto http://www.trolltech.com and download the following package:
      qt-win-opensource-<version>-mingw.exe
      During installation you will be asked if you also want to install MinGW
      Accept this or use your existing MinGW
1.2.) Now define some environment variables within windows control center->system
      MINGWDIR = path to mingw
      QTDIR    = path to qt library
      Add %MINGWDIR%\bin to Path environment variable
      Add %QTDIR%\bin    to Path environment variable
1.3.) Test the MinGW installation
      Open a command prompt and type the following commands
      to see if the system can find our installed MinGW binaries.
      >gcc  (Output must be: gcc: no input files)
      >g++  (Output must be: g++: no input files)
      >mingw32-make  (Output must be:
      mingw32-make: *** No targets specified and no makefile found.  Stop.)
      If you did get another output please
      check the points above.

2.) Install pvbrowser
---------------------
      Since you read this, you already have downloaded and extracted pvb.tar.gz
2.1.) From this directory run
      CreatePvbWithMinGW.bat
      This will build the MinGW version of pvbrowser
2.2.) Now define some environment variables within windows control center->system
      PVBDIR = path to pvb directory
      Add %PVBDIR%\win-mingw\bin to Path environment variable
2.2.) Create Links on your Desktop to
      win-mingw\bin\pvbrowser.exe
      win-mingw\bin\pvdevelop.exe       
      You may use win\logo1.ico as your icon

##############################################################################################

Installation from sourcecode
============================
1.) Installation MinGW
----------------------
1.1.) Download MinGW
      Goto http://www.mingw.org and download the following packages:
      binutils (latest availabel version)
      gdb (latest availabel version)
      gcc-core (Version 3 is recommanded)
      gcc-g++ (Version 3 is recommanded)
      mingw32-make (latest availabel version)
      mingw-runtime (latest availabel version)
      w32api (latest availabel version)
1.2.) Create a directory for the MinGW binaries
      The following installation instructions will use the
      directory C:\MinGW for the MinGW installation.
1.3.) Extract all downloaded MinGW archives
      Extract the above downloaded archives to C:\MinGW
      so that there will be a direcotry C:\MinGW\bin after
      the extraction.
1.4.) Create environment variables
      Set the environment variable MINGWDIR to C:\MinGW or
      the one you used for your MinGW installation.
      Add the path to the MinGW binaries to the PATH
      environment variable in our case C:\MinGW\bin.
1.5.) Test the MinGW installation
      Open a command prompt and type the following commands
      to see if the system can find our installed MinGW binaries.
      >gcc  (Output must be: gcc: no input files)
      >g++  (Output must be: g++: no input files)
      >mingw32-make  (Output must be:
      mingw32-make: *** No targets specified and no makefile found.  Stop.)
      If you did get another output please
      check the points above.
      
2.) Installation Qt
-------------------
2.1.) Download Qt
      Goto http://trolltech.com and download the latest
      Zip archive of the Qt/Windows Open Source Edition.
2.2.) Create a directory for the Qt installation
      The following installation instructions will use the
      directory C:\Qt for the Qt installation.
2.3.) Extract the downloaded Zip archive
      Extract the above downloaded archive to C:\Qt
      so that there will be a directory C:\Qt\bin after
      the extraction.
2.4.) Create environment variables
      Set the environment variable QTDIR to C:\Qt or
      the one you used for your Qt installation.
      Add the path to the Qt binaries to the PATH
      environment variable in our case C:\Qt\bin.
2.5.) Configuring Qt sources
      Open a command prompt and change to the directory C:\Qt:
      >cd c:\Qt
      The following command compiles the qmake tool and
      creates the Makefiles of the Qt executables and libraries:
      >configure.exe -platform win32-g++
2.6.) Compiling Qt sources
      Just type the following command in the command
      prompt to create the Qt executables and libraries:
      >mingw32-make.exe
2.7.) Cleanup Qt installation
      To save diskspace it is recommended to execute the
      following command to cleanup the Qt compilation:
      >mingw32-make.exe confclean
      
3.) Installation PVB
--------------------
3.1.) Download PVB
      Goto http://pvbrowser.de/ and download the pvb
      binary and source archive (pvb.tar.gz).
3.2.) Create a directory for the PVB installation
      The following installation instructions will use the
      directory C:\pvb for the PVB installation.
3.3.) Extract the downloaded archive
      Extract the above downloaded archive to C:\pvb
      so that there will be a directory C:\pvb\win after
      the extraction.
3.4.) Create environment variable
      Set the environment variable PVBDIR to C:\pvb or
      the one you used for your PVB installation.
3.5.) Compile PVB
      To start the compilation of PVB using MinGW just
      execute the CreatePvbWithMinGW.bat Batchfile.
3.6.) Use PVB
      After the compilation a directory C:\pvb\win-mingw\bin exists
      that contains the compiled executables of PVB that you can use
      like known from the MSVC edition.

##############################################################################################

Changelog
=========
Verison 0.2
02.10.2007
* Changes in cutil.cpp where integrated in the original cutil.cpp.
* UninstallPvbWithMinGW.bat became obsolete because the PVB_MinGW-Script
  was integrated into PVB.
* Some smaller changes were made CreatePvbWithMinGW.bat due to
  PVB-Integration.
* The chapter "Installation with binary Qt4 library" was added to this Readme.txt.

Verison 0.1
21.09.2007
* First release.
