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
Name "install-pvbrowser-${VERSION}-client-msvc ${VERSION}"

; The file to write
OutFile "install-pvbrowser-${VERSION}-client-msvc.exe"

; The default installation directory
InstallDir $PROGRAMFILES\pvb-client-msvc

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

  SetOutPath $INSTDIR\win-msvc\bin
    File /r pvb\win-msvc\bin\pvbrowser.exe
  
  SetOutPath $INSTDIR\win-msvc\bin
    File /r pvb\win-msvc\bin\*.*

  SetOutPath $INSTDIR\win-msvc\bin\audio
    File /r pvb\win-msvc\bin\audio\*.*

  SetOutPath $INSTDIR\win-msvc\bin\bearer
    File /r pvb\win-msvc\bin\bearer\*.*

  SetOutPath $INSTDIR\win-msvc\bin\iconengines
    File /r pvb\win-msvc\bin\iconengines\*.*

  SetOutPath $INSTDIR\win-msvc\bin\imageformats
    File /r pvb\win-msvc\bin\imageformats\*.*

  SetOutPath $INSTDIR\win-msvc\bin\mediaservice
    File /r pvb\win-msvc\bin\mediaservice\*.*

  SetOutPath $INSTDIR\win-msvc\bin\platforms
    File /r pvb\win-msvc\bin\platforms\*.*

  SetOutPath $INSTDIR\win-msvc\bin\playlistformats
    File /r pvb\win-msvc\bin\playlistformats\*.*

  SetOutPath $INSTDIR\win-msvc\bin\position
    File /r pvb\win-msvc\bin\position\*.*

  SetOutPath $INSTDIR\win-msvc\bin\printsupport
    File /r pvb\win-msvc\bin\printsupport\*.*

  SetOutPath $INSTDIR\win-msvc\bin\resources
    File /r pvb\win-msvc\bin\resources\*.*

  SetOutPath $INSTDIR\win-msvc\bin\translations
    File /r pvb\win-msvc\bin\translations\*.*

  SetOutPath $INSTDIR\win-msvc\bin\translations\qtwebengine_locales
    File /r pvb\win-msvc\bin\translations\qtwebengine_locales\*.*
  
  SetOutPath $INSTDIR\win-msvc
    File pvb\win-mingw\logo1.ico

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client-msvc" "DisplayName" "pvbrowser-client-msvc"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client-msvc" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client-msvc" "Publisher" "pvbrowser"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client-msvc" "URLInfoAbout" "http://pvbrowser.de"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client-msvc" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowse-client" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client-msvc" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
   
SectionEnd
  
Section "Start Menu Shortcuts"

  CreateDirectory $SMPROGRAMS\pvbrowser-client-msvc
  CreateShortCut "$SMPROGRAMS\pvbrowser-client-msvc\pvbrowser-client-msvc.lnk" "$INSTDIR\win-msvc\bin\pvbrowser.exe" "" "$INSTDIR\win-msvc\logo1.ico" 0
  CreateShortCut "$SMPROGRAMS\pvbrowser-client-msvc\uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

Section "Desktop Menu Shortcuts"
  
  CreateShortCut "$DESKTOP\pvbrowser-client-msvc.lnk" "$INSTDIR\win-msvc\bin\pvbrowser.exe" "" "$INSTDIR\win-msvc\logo1.ico" 0
  
SectionEnd  

; Uninstaller
Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser-client-msvc"
  
  ; Remove INSTDIR
  RMDir /r $INSTDIR

  ; Remove shortcuts, if any
  RMDir /r "$SMPROGRAMS\pvbrowser-client-msvc"
  Delete "$DESKTOP\pvbrowser-client-msvc.lnk"

SectionEnd

