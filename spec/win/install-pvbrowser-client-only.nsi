; ***** BEGIN LICENSE BLOCK *****
;
;##########################################################################
;   Copyright (C) 2008 Matthias Kretschmer                                #
;   64matze@gmx.de                                                        #
;                                                                         #
;   This program is free software; you can redistribute it and/or modify  #
;   it under the terms of the GNU General Public License as published by  #
;   the Free Software Foundation; either version 2 of the License, or     #
;   (at your option) any later version.                                   #
;                                                                         #
;   This program is distributed in the hope that it will be useful,       #
;   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
;   GNU General Public License for more details.                          #
;                                                                         #
;   You should have received a copy of the GNU General Public License     #
;   along with this program; if not, write to the                         #
;   Free Software Foundation, Inc.,                                       #
;   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
;##########################################################################
;
; ***** END LICENSE BLOCK *****

; install-pvbrowser.nsi
; Compile automatically:
; <Path of NSIS>\makensis.exe /DVERSION=<Version> <Path>\install-pvbrowser.nsi

!define ALL_USERS

!ifndef VERSION
  !define VERSION 1.0
!endif


;--------------------------------

; The name of the installer
Name "install-pvbrowser-${VERSION}-client ${VERSION}"

; The file to write
OutFile "install-pvbrowser-${VERSION}-client.exe"

; The default installation directory
InstallDir $PROGRAMFILES\pvb-client

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages
Page license
  LicenseData pvb\LICENSE.GPL
Page directory
Page components
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; Installer
Section "" ;No components page, name is not important

  SetOutPath $INSTDIR
    File pvb\LICENSE.GPL
    
  SetOutPath $INSTDIR
    File pvb\gamsleiten.png

  SetOutPath $INSTDIR\win-mingw\bin
    File /r pvb\win-mingw\bin\pvbrowser.exe
  
  SetOutPath $INSTDIR\win-mingw\bin
    File /r pvb\win-mingw\bin\*.dll
  
  SetOutPath $INSTDIR\win-mingw\bin\plugins
    File /r pvb\win-mingw\bin\plugins\*
  
  SetOutPath $INSTDIR\win-mingw\bin\imageformats
    File /r pvb\win-mingw\bin\imageformats\*

  SetOutPath $INSTDIR\win-mingw
    File pvb\win-mingw\logo1.ico

  SetOutPath $INSTDIR\win-mingw\bin
    File pvb\browserplugin\pvbrowserplugin-example.html

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client" "DisplayName" "pvbrowser-client"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client" "Publisher" "pvbrowser"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client" "URLInfoAbout" "http://pvbrowser.de"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowse-client" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
   
SectionEnd
  
Section "Start Menu Shortcuts"

  CreateDirectory $SMPROGRAMS\pvbrowser-client
  CreateShortCut "$SMPROGRAMS\pvbrowser-client\pvbrowser-client.lnk" "$INSTDIR\win-mingw\bin\pvbrowser.exe" "" "$INSTDIR\win-mingw\logo1.ico" 0
  CreateShortCut "$SMPROGRAMS\pvbrowser-client\uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

Section "Desktop Menu Shortcuts"
  
  CreateShortCut "$DESKTOP\pvbrowser-client.lnk" "$INSTDIR\win-mingw\bin\pvbrowser.exe" "" "$INSTDIR\win-mingw\logo1.ico" 0
  
SectionEnd  

; Uninstaller
Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client"
  
  ; Remove INSTDIR
  RMDir /r $INSTDIR

  ; Remove shortcuts, if any
  RMDir /r "$SMPROGRAMS\pvbrowser-client"
  Delete "$DESKTOP\pvbrowser-client.lnk"

SectionEnd

