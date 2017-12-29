
@set FBASEDIR=%CD%

@set PATH=%FBASEDIR%\tools\vim;%PATH%
@set PATH=%FBASEDIR%\tools\SVN\bin;%PATH%
@set PATH=%FBASEDIR%\tools\eclipse;%PATH%
@set PATH=%FBASEDIR%\tools\cmake-2.8.2-win32-x86\bin;%PATH%
@set PATH=%FBASEDIR%\tools\UnxUtils\usr\local\wbin;%PATH%
@set PATH=%FBASEDIR%\SDK\DirectX\Utilities\bin\x86;%PATH%

@set PATH=%FBASEDIR%\tools\Microsoft Visual Studio 9.0\VC\redist\Debug_NonRedist\x86\Microsoft.VC90.DebugCRT;%PATH%
@set PATH=%FBASEDIR%\tools\Microsoft Visual Studio 9.0\VC\redist\Debug_NonRedist\x86;%PATH%
@set PATH=%FBASEDIR%\tools\Microsoft Visual Studio 9.0\VC\Debug_NonRedist\x86;%PATH%

@set LANG=en_US.utf8

@echo FBASEDIR=%FBASEDIR%
@cd tools\Microsoft Visual Studio 9.0\VC\bin
@call vcvars32 %FBASEDIR%
@cd %FBASEDIR%

:pois
