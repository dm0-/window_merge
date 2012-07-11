#!/usr/bin/makensis
# Use this in the source directory after the "make" command ends successfully.

Name "Window Merge 0.3"
OutFile "Install Window Merge 0.3.exe"

RequestExecutionLevel admin
SetCompressor /solid lzma
XPStyle on

# Page configuration

Page license
LicenseData COPYING
LicenseText \
    "This is not an EULA.  It is a free software distribution license.  The \
     source code is available at <https://github.com/dm0-/window_merge>." \
    "Continue"

Page directory
DirText \
    "Please select the Pidgin installation folder.  This folder must contain \
     the file pidgin.exe to proceed." \
    "Pidgin Installation Folder"
InstallDir $ProgramFiles\Pidgin
InstallDirRegKey HKLM SOFTWARE\pidgin ""

Page instfiles
ShowInstDetails show

UninstPage uninstConfirm

UninstPage instfiles
ShowUninstDetails show

# Callback functions

Function .onVerifyInstDir
  IfFileExists $INSTDIR\pidgin.exe good
    Abort
  good:
FunctionEnd

# Installation sections

Section "Window Merge"
  SectionIn RO
  SetOutPath $INSTDIR\plugins
  File .libs/window_merge.dll
SectionEnd

Section -post
  WriteUninstaller "$INSTDIR\Uninstall Window Merge.exe"
SectionEnd

# Uninstallation sections

Section Uninstall
  Delete /rebootok $INSTDIR\plugins\window_merge.dll
  Delete /rebootok "$INSTDIR\Uninstall Window Merge.exe"
SectionEnd
