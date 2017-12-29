
@if "%FBASEDIR%" == "" (
	@echo Run f.bat first
	goto end
)

@mkdir %FBASEDIR%\build
@cd %FBASEDIR%\build

@cmake -G "Eclipse CDT4 - NMake Makefiles" ..\projects

@cd %FBASEDIR%
rem @tools\eclipse\eclipse

:end