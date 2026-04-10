; GMAT NSIS installer script. This file is run by the main Makefile.

; Includes
!include FileFunc.nsh
!include LogicLib.nsh
!include MUI2.nsh

; Defaults
!ifndef VERSION
    !define VERSION dev
!endif

; General configuration
Name "GMAT ${VERSION}"
!ifdef PUBLIC
    OutFile "gmat-winInstaller-${VERSION}.exe"
!else
    OutFile "gmat-internal-winInstaller-${VERSION}.exe"
!endif
SetCompressor /SOLID lzma

; Multi-user configuration
!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_INSTDIR "GMAT\${VERSION}"
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!include MultiUser.nsh

; User interface configuration
!ifdef PUBLIC
    !define MUI_ICON "gmat-public\GMAT\data\graphics\icons\GMAT.ico"
    !define MUI_UNICON "gmat-public\GMAT\data\graphics\icons\GMAT.ico"
!else
    !define MUI_ICON "gmat-internal\GMAT\data\graphics\icons\GMAT.ico"
    !define MUI_UNICON "gmat-internal\GMAT\data\graphics\icons\GMAT.ico"
!endif
!define MUI_DIRECTORYPAGE_VERIFYONLEAVE
!define MUI_WELCOMEFINISHPAGE_BITMAP "WelcomeImage.bmp"

; User interface pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MULTIUSER_PAGE_INSTALLMODE
!define MUI_PAGE_CUSTOMFUNCTION_LEAVE InstDirCheck
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro MUI_LANGUAGE "English"

; GMAT path settings
!define UNINST_KEY \
    "Software\Microsoft\Windows\CurrentVersion\Uninstall\Gmat${VERSION}"

; Make not show UAC on Win7
RequestExecutionLevel user

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Sections
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Installer
Section "!GMAT" SecGmat
    SectionIn 1 RO

    SetOutPath "$INSTDIR"
    !ifdef PUBLIC
        File /r "gmat-public\GMAT\*.*"
    !else
        File /r "gmat-internal\GMAT\*.*"
    !endif
    
    ; Start Menu shortcut
    SetOutPath "$INSTDIR\bin"
    !define SMDIR "$SMPROGRAMS\GMAT\GMAT ${VERSION}"
    CreateDirectory "${SMDIR}"
    CreateShortCut "${SMDIR}\GMAT ${VERSION}.lnk" "$INSTDIR\bin\GMAT.exe" "" "$INSTDIR\bin\GMAT.exe" 0
    CreateShortCut "${SMDIR}\Help.lnk" "$INSTDIR\docs\help\help.chm"
    CreateShortCut "${SMDIR}\Open GMAT folder.lnk" "$INSTDIR"
    
    ; Uninstaller and Add/Remove Programs registry key
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    WriteRegStr SHCTX "${UNINST_KEY}" "DisplayName" "GMAT ${VERSION}"
    WriteRegStr SHCTX "${UNINST_KEY}" "UninstallString" \
        "$\"$INSTDIR\Uninstall.exe$\" /$MultiUser.InstallMode"
    WriteRegStr SHCTX "${UNINST_KEY}" "QuietUninstallString" \
        "$\"$INSTDIR\Uninstall.exe$\" /$MultiUser.InstallMode /S"
    WriteRegStr SHCTX "${UNINST_KEY}" "DisplayVersion" "${VERSION}"
    WriteRegStr SHCTX "${UNINST_KEY}" "Publisher" "GMAT Development Team"
    WriteRegStr SHCTX "${UNINST_KEY}" "Version" "${VERSION}"
    WriteRegStr SHCTX "${UNINST_KEY}" "HelpLink" "http://gmat.gsfc.nasa.gov"
    WriteRegStr SHCTX "${UNINST_KEY}" "InstallLocation" "$\"$INSTDIR$\""
    WriteRegStr SHCTX "${UNINST_KEY}" "URLInfoAbout" "http://gmat.gsfc.nasa.gov"
    WriteRegStr SHCTX "${UNINST_KEY}" "URLUpdateInfo" "http://sourceforge.net/projects/gmat/"
    WriteRegStr SHCTX "${UNINST_KEY}" "Comments" "General Mission Analysis Tool (GMAT)"
    WriteRegDWORD SHCTX "${UNINST_KEY}" "Language" "1033"
    WriteRegDWORD SHCTX "${UNINST_KEY}" "NoModify" "1"
    WriteRegDWORD SHCTX "${UNINST_KEY}" "NoRepair" "1"
    WriteRegStr SHCTX "${UNINST_KEY}" "Readme" "$\"$INSTDIR\README.txt$\""
    WriteRegStr SHCTX "${UNINST_KEY}" "DisplayIcon" "$\"$INSTDIR\data\graphics\icons\GMAT.ico$\""
    
    ; Estimated size
    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    IntFmt $0 "0x%08X" $0
    WriteRegDWORD SHCTX "${UNINST_KEY}" "EstimatedSize" "$0"
    
SectionEnd

; Optional MATLAB interface (disabled by default)
Section /o "Enable MATLAB interface" SecEnableMatlab
    Delete "$INSTDIR\bin\gmat_startup_file.txt"
    Rename "$INSTDIR\bin\gmat_startup_file.wmatlab.txt" \
        "$INSTDIR\bin\gmat_startup_file.txt"
SectionEnd

; Optional file type associations (disabled by default)
Section /o "Associate file types" SecFileAssoc
    ; Register application
    ; (This uses the Applications subkey instead of the App Paths subkey
    ; for compatibility with Windows XP. We can switch it when XP is no
    ; longer supported.)
    ;!define APP_KEY "Software\Classes\Applications\GMAT.exe"
    ;WriteRegStr SHCTX "${APP_KEY}" "FriendlyAppName" "GMAT"
    ;WriteRegStr SHCTX "${APP_KEY}\SupportedTypes" ".script" ""
    !define GENERIC_PROGID_KEY "Software\Classes\GMAT.Script"
    WriteRegStr SHCTX "${GENERIC_PROGID_KEY}" "" "GMAT"
    WriteRegStr SHCTX "${GENERIC_PROGID_KEY}\CurVer" "" "GMAT.Script.${VERSION}"
    !define PROGID_KEY "Software\Classes\GMAT.Script.${VERSION}"
    WriteRegStr SHCTX "${PROGID_KEY}" "" "GMAT ${VERSION}"
    WriteRegStr SHCTX "${PROGID_KEY}\shell\open\command" "" '"$INSTDIR\bin\GMAT.exe" "%1"'
    !define FILETYPE_KEY "Software\Classes\.script"
    WriteRegStr SHCTX "${FILETYPE_KEY}" "" "GMAT.Script"
    WriteRegStr SHCTX "${FILETYPE_KEY}" "Content Type" "text/plain"
    WriteRegStr SHCTX "${FILETYPE_KEY}\OpenWithProgIds" "GMAT.Script" ""
SectionEnd

; Uninstaller
Section "Uninstall" SecUninstall
    ; Uninstaller itself
    Delete "$INSTDIR\Uninstall.exe"
    DeleteRegKey SHCTX "${UNINST_KEY}"
    
    ; GMAT files
    !include .\uninstall.nsh
    
    ; Delete top-level GMAT folder if it exists
    IfFileExists "$INSTDIR\..\..\GMAT\*.*" +1 +2
        RMDir "$INSTDIR\..\..\GMAT"
    
    ; Start Menu shortcut
    Delete "${SMDIR}\GMAT ${VERSION}.lnk"
    Delete "${SMDIR}\Help.lnk"
    Delete "${SMDIR}\Open GMAT folder.lnk"
    RMDir "${SMDIR}"
    
    ; Registry keys
    ; don't remove ${APP_KEY} because user might have multiple GMAT versions
    ; installed
    DeleteRegKey SHCTX "${PROGID_KEY}"
SectionEnd

; Set descriptions
LangString DESC_SecGmat ${LANG_ENGLISH} "Core GMAT files"
LangString DESC_SecFileAssoc ${LANG_ENGLISH} "Associate *.script files with GMAT."
LangString DESC_SecEnableMatlab ${LANG_ENGLISH} "Enable the MATLAB interface and fmincon plugins. This can be enabled later by editing gmat_startup_file.txt."

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecGmat} $(DESC_SecGmat)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecFileAssoc} $(DESC_SecFileAssoc)
  !insertmacro MUI_DESCRIPTION_TEXT ${SecEnableMatlab} $(DESC_SecEnableMatlab)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

Function InstDirCheck
    GetInstDirError $0
    ${Switch} $0
        ${Case} 0
            IfFileExists "$INSTDIR\*.*" +1 +3
                MessageBox MB_YESNO|MB_ICONINFORMATION|MB_DEFBUTTON2 "The directory $INSTDIR already exists. Do you want to install there anyway?" IDNO +1 IDYES +2
                Abort
            ${Break}
        ${Case} 1
            MessageBox MB_OK|MB_ICONSTOP "The directory $INSTDIR is invalid."
            Abort
            ${Break}
        ${Case} 2
            MessageBox MB_OK|MB_ICONSTOP "There is not enough disk space available to install."
            Abort
            ${Break}
    ${EndSwitch}
FunctionEnd

; Callbacks
Function .onInit
    ; Required by MultiUser.nsh
    !insertmacro MULTIUSER_INIT
FunctionEnd

Function .onInstSuccess
    ; Clean up alternate startup file
    Delete "$INSTDIR\bin\gmat_startup_file.wmatlab.txt"
FunctionEnd

Function un.onInit
    ; Required by MultiUser.nsh
    !insertmacro MULTIUSER_UNINIT
FunctionEnd
