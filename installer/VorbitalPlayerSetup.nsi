;Include Modern UI

  !include "MUI2.nsh"
  !include "FileAssociation.nsh"

Name "Vorbital Player 4.0"
OutFile "VorbitalPlayer4.0Setup.exe"
InstallDir "$PROGRAMFILES\Zeta Centauri\Vorbital Player"
InstallDirRegKey HKLM "Software\VorbitalPlayer" "Install_Dir"
RequestExecutionLevel admin
!define MUI_ICON "vorbital.ico"
!define MUI_UNICON "vorbital.ico"

;Version Information

  VIProductVersion "4.0.0.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "Vorbital Player"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "Zeta Centauri"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright 2006-2013 Zeta Centauri"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "Vorbital Player Installer"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "4.0.0.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "4.0.0.0"

;Interface Settings

  !define MUI_ABORTWARNING

;Pages

  !insertmacro MUI_PAGE_LICENSE "License.txt"
;  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
      !define MUI_FINISHPAGE_NOAUTOCLOSE
      !define MUI_FINISHPAGE_RUN
      !define MUI_FINISHPAGE_RUN_CHECKED
      !define MUI_FINISHPAGE_RUN_TEXT "Launch Vorbital Player"
      !define MUI_FINISHPAGE_RUN_FUNCTION "LaunchProgram"
      !define MUI_FINISHPAGE_SHOWREADME ""
      !define MUI_FINISHPAGE_SHOWREADME_NOTCHECKED
      !define MUI_FINISHPAGE_SHOWREADME_TEXT "Create Desktop Shortcut"
      !define MUI_FINISHPAGE_SHOWREADME_FUNCTION finishpageaction
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;Languages
 
  !insertmacro MUI_LANGUAGE "English"


; The stuff to install
Section "VorbitalPlayer"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "vorbital.exe"
  File "License.txt"
  File "vorbital.ico"
  File "alut.dll"
  File "wavpackdll.dll"
  File "libsndfile-1.dll"
  File "libmpg123-0.dll.dll"
  File "oalinst.exe"

  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\VorbitalPlayer "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VorbitalPlayer" "DisplayName" "Vorbital Player"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VorbitalPlayer" "DisplayVersion" "4.0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VorbitalPlayer" "Publisher" "Zeta Centauri"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VorbitalPlayer" "DisplayIcon" "$INSTDIR\vorbital.ico"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VorbitalPlayer" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VorbitalPlayer" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VorbitalPlayer" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

  ${registerExtension} "$INSTDIR\vorbital.exe" ".ogg" "Ogg File"
  ${registerExtension} "$INSTDIR\vorbital.exe" ".wv" "Wavepack File"
  ${registerExtension} "$INSTDIR\vorbital.exe" ".wav" "Wave File"
  ${registerExtension} "$INSTDIR\vorbital.exe" ".mp3" "MP3 File"
  ${registerExtension} "$INSTDIR\vorbital.exe" ".aif" "AIFF File"
  ${registerExtension} "$INSTDIR\vorbital.exe" ".aiff" "AIFF File"
  ${registerExtension} "$INSTDIR\vorbital.exe" ".snd" "SND File"

  ExecWait '"$INSTDIR\oalinst.exe"' 

SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Zeta Centauri\Vorbital Player"
  CreateShortCut "$SMPROGRAMS\Zeta Centauri\Vorbital Player\Vorbital Player.lnk" "$INSTDIR\vorbital.exe" "" "" 0
  ;CreateShortCut "$SMPROGRAMS\Zeta Centauri\Vorbital Player\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  WriteINIStr "$SMPROGRAMS\Zeta Centauri\Vorbital Player\Vorbital Player Website.url" "InternetShortcut" "URL" "http://vorbitalplayer.com"
  
SectionEnd

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\VorbitalPlayer"
  DeleteRegKey HKLM SOFTWARE\VorbitalPlayer

  ; Remove files and uninstaller
  Delete $INSTDIR\vorbital.exe
  Delete $INSTDIR\License.txt
  Delete $INSTDIR\vorbital.ico
  Delete $INSTDIR\alut.dll
  Delete $INSTDIR\wavpackdll.dll
  Delete $INSTDIR\libsndfile-1.dll
  Delete $INSTDIR\uninstall.exe
  Delete $INSTDIR\libmpg123-0.dll.dll

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Zeta Centauri\Vorbital Player\*.*"
  Delete "$DESKTOP\Vorbital Player.lnk"
  Delete "$SMPROGRAMS\Zeta Centauri\VorbitalPlayer\Vorbital Player Website.url"
  ;DeleteINISec "$SMPROGRAMS\Zeta Centauri\Vorbital Player\Vorbital Player Website.url" "InternetShortcut"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Zeta Centauri\Vorbital Player"
  RMDir "$SMPROGRAMS\Zeta Centauri"
  RMDir "$INSTDIR"

  ${unregisterExtension} ".ogg" "Ogg File"
  ${unregisterExtension} ".wv" "Wavepack File"
  ${unregisterExtension} ".wav" "Wave File"
  ${unregisterExtension} ".mp3" "MP3 File"
  ${unregisterExtension} ".aif" "AIFF File"
  ${unregisterExtension} ".aiff" "AIFF File"
  ${unregisterExtension} ".snd" "SND File"

SectionEnd

; This function is automatically executed when the installer finishes
Function .onGUIEnd
	; Un-register the offer manager plugin with the operating system
        Delete $INSTDIR\oalinst.exe
FunctionEnd


Function LaunchProgram
  ExecShell "" "$SMPROGRAMS\Zeta Centauri\Vorbital Player\Vorbital Player.lnk"
FunctionEnd

Function finishpageaction
  CreateShortcut "$DESKTOP\Vorbital Player.lnk" "$INSTDIR\vorbital.exe"
FunctionEnd