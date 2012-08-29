#!/usr/bin/makensis
# Use this in the source directory after the "make" command ends successfully.

!define name "Window Merge"
!define version 0.3
!define url "https://github.com/dm0-/window_merge"
!define rk 'HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${name}"'

Name "${name} ${version}"
OutFile "Install ${name} ${version}.exe"

RequestExecutionLevel admin
SetCompressor /solid lzma
XPStyle on

# Page configuration

Page license
LicenseData COPYING
LicenseText \
    "This is not an EULA.  It is a free software distribution license.  The \
     source code is available at <${url}>." \
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

Var install_size

Section "${name}" section_index_pwm
  SectionIn RO
  SetOutPath $INSTDIR\plugins
  File .libs/window_merge.dll

  SectionGetSize ${section_index_pwm} $0
  IntOp $install_size $install_size + $0
SectionEnd

Section -post section_index_post
  WriteUninstaller "$INSTDIR\Uninstall ${name}.exe"

  SectionGetSize ${section_index_post} $0
  IntOp $install_size $install_size + $0
  IntFmt $install_size 0x%08X $install_size

  WriteRegStr ${rk} DisplayName          "Pidgin - ${name}"
  WriteRegStr ${rk} DisplayVersion       "${version}"
  WriteRegStr ${rk} DisplayIcon          '"$INSTDIR\pidgin.exe"'
  WriteRegStr ${rk} UninstallString      '"$INSTDIR\Uninstall ${name}.exe"'
  WriteRegStr ${rk} QuietUninstallString '"$INSTDIR\Uninstall ${name}.exe" /S'
  WriteRegStr ${rk} URLInfoAbout         "${url}"
  WriteRegStr ${rk} URLUpdateInfo        "${url}/downloads"
  WriteRegStr ${rk} HelpLink             "${url}/wiki"
  WriteRegDWORD ${rk} EstimatedSize $install_size
  WriteRegDWORD ${rk} NoModify      0x00000001
  WriteRegDWORD ${rk} NoRepair      0x00000001
SectionEnd

# Uninstallation sections

Section Uninstall
  Delete /rebootok $INSTDIR\plugins\window_merge.dll
  Delete /rebootok "$INSTDIR\Uninstall ${name}.exe"

  DeleteRegKey ${rk}
SectionEnd
