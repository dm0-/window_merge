#!/usr/bin/makensis

Name "Window Merge 0.2"
OutFile "Install Window Merge 0.2.exe"
InstallDir $ProgramFiles\Pidgin

RequestExecutionLevel admin
SetCompressor bzip2
ShowInstDetails show
ShowUninstDetails show

Page directory
Page instfiles

DirText "Please select the Pidgin installation folder." \
        "This folder must contain pidgin.exe."

Function .onInit
  Push $0

  ReadRegStr $0 HKLM SOFTWARE\pidgin ""
  StrCmp $0 "" usedef

  IfFileExists $0\pidgin.exe works
    StrCpy $0 ""
  works:

  StrCmp $0 "" usedef
  StrCpy $INSTDIR $0
  usedef:

  Pop $0
FunctionEnd

Function .onVerifyInstDir
  IfFileExists $INSTDIR\pidgin.exe good
    Abort
  good:
FunctionEnd

Section "Window Merge"
  SectionIn RO
  SetOutPath $INSTDIR\plugins
  File .libs/window_merge.dll
SectionEnd

Section Uninstall
  Delete /rebootok $INSTDIR\plugins\window_merge.dll
  Delete /rebootok "$INSTDIR\Uninstall Window Merge.exe"
SectionEnd

Section -post
  WriteUninstaller "$INSTDIR\Uninstall Window Merge.exe"
SectionEnd
