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
!include ChangeEnv.nsh


!ifndef VERSION
  !define VERSION 1.0
!endif


;--------------------------------

; The name of the installer
Name "install-pvbrowser ${VERSION}"

; The file to write
OutFile "install-pvbrowser.exe"

; The default installation directory
InstallDir c:\pvb

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
    File /r pvb\win-mingw\bin\*
  
  SetOutPath $INSTDIR\win-mingw
    File pvb\win-mingw\Readme.txt
  
  SetOutPath $INSTDIR\rllib\lib
    File pvb\rllib\lib\*.h
    
  SetOutPath $INSTDIR\pvserver
    File pvb\pvserver\*.h
  
  SetOutPath $INSTDIR\doc
    File /r pvb\doc\*

  SetOutPath $INSTDIR\pvsexample  
    File pvb\pvsexample\*

  SetOutPath $INSTDIR\pvsexample\gl
    File pvb\pvsexample\gl\*
  
  SetOutPath $INSTDIR\win-mingw
    File pvb\win-mingw\logo1.ico
  
  SetOutPath $INSTDIR\win-mingw
    File pvb\win-mingw\nodave_mingw.o
  
  SetOutPath $INSTDIR\win-mingw\bin
    File pvb\browserplugin\pvbrowserplugin-example.html
  
; [Registry]
; Root: HKLM; Subkey: SYSTEM\CurrentControlSet\Control\Session Manager\Environment; ValueType: string; ValueName: PVBDIR; ValueData: {app}; Flags: uninsdeletevalue

  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser" "DisplayName" "pvbrowser"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser" "Publisher" "pvbrowser"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser" "URLInfoAbout" "http://pvbrowser.de"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
   
  ; Write environment 
  Push "PVBDIR"
  Push "$INSTDIR"
  Call AddToEnvVar
  
  Push $INSTDIR\win-mingw\bin
  Call AddToPath

  Exec 'notepad "$INSTDIR\win-mingw\Readme.txt"'

SectionEnd
  
Section "Start Menu Shortcuts"

  CreateDirectory $SMPROGRAMS\pvbrowser
  CreateShortCut "$SMPROGRAMS\pvbrowser\pvbrowser.lnk" "$INSTDIR\win-mingw\bin\pvbrowser.exe" "" "$INSTDIR\win-mingw\logo1.ico" 0
  CreateShortCut "$SMPROGRAMS\pvbrowser\start_pvdevelop.lnk" "$INSTDIR\win-mingw\bin\start_pvdevelop.bat" "" "$INSTDIR\win-mingw\logo1.ico" 0
  CreateShortCut "$SMPROGRAMS\pvbrowser\uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  
SectionEnd

Section "Desktop Menu Shortcuts"
  
  CreateShortCut "$DESKTOP\pvbrowser.lnk" "$INSTDIR\win-mingw\bin\pvbrowser.exe" "" "$INSTDIR\win-mingw\logo1.ico" 0
  CreateShortCut "$DESKTOP\start_pvdevelop.lnk" "$INSTDIR\win-mingw\bin\start_pvdevelop.bat" "" "$INSTDIR\win-mingw\logo1.ico" 0
  
SectionEnd  

; Uninstaller
Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\pvbrowser"
  
  ; Remove INSTDIR
  RMDir /r $INSTDIR

  ; Remove shortcuts, if any
  RMDir /r "$SMPROGRAMS\pvbrowser"
  Delete "$DESKTOP\pvbrowser.lnk"
  Delete "$DESKTOP\start_pvdevelop.lnk"
  
  ; Remove environment settings
  Push "PVBDIR"
  Push "$INSTDIR"
  Call un.RemoveFromEnvVar
  
  Push $INSTDIR\win-mingw\bin
  Call un.RemoveFromPath

SectionEnd

