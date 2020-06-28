; Script generated by the Inno Script Studio Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppVersion GetFileVersion("..\Deployment\WifiMouseServer.exe")

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{8DD0805A-BEE7-4C41-A13B-EA7DA72617F1}
AppName=WifiMouseServer
AppVersion={#MyAppVersion}
AppVerName=WifiMouseServer {#MyAppVersion}
AppPublisher=WifiMouseServer Authors
AppPublisherURL=https://github.com/michaelnpsp/WifiMouseServer
AppSupportURL=https://github.com/michaelnpsp/WifiMouseServer
AppUpdatesURL=https://github.com/michaelnpsp/WifiMouseServer
DefaultDirName={commonpf}\WifiMouseServer
DefaultGroupName=WifiMouseServer
OutputBaseFilename=WifiMouseServerInstall
Compression=lzma
SolidCompression=yes
OutputDir="..\Releases"
LicenseFile=LICENSE.txt

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\Deployment\WifiMouseServer.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\D3Dcompiler_47.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\libEGL.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\libGLESv2.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\opengl32sw.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\Qt5Bluetooth.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\Qt5Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\Qt5Svg.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\Deployment\WifiMouseServer.exe"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: "..\Deployment\bearer\*"; DestDir: "{app}"; Flags: ignoreversion createallsubdirs recursesubdirs
Source: "..\Deployment\iconengines\*"; DestDir: "{app}"; Flags: ignoreversion createallsubdirs recursesubdirs
Source: "..\Deployment\imageformats\*"; DestDir: "{app}"; Flags: ignoreversion createallsubdirs recursesubdirs
Source: "..\Deployment\platforms\*"; DestDir: "{app}"; Flags: ignoreversion createallsubdirs recursesubdirs
Source: "..\Deployment\styles\*"; DestDir: "{app}"; Flags: ignoreversion createallsubdirs recursesubdirs

[Icons]
Name: "{group}\WifiMouseServer"; Filename: "{app}\WifiMouseServer.exe"
Name: "{commondesktop}\WifiMouseServer"; Filename: "{app}\WifiMouseServer.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\WifiMouseServer.exe"; Description: "{cm:LaunchProgram,WifiMouseServer}"; Flags: nowait postinstall skipifsilent
