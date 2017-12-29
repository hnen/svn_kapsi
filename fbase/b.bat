
@if "%FBASEDIR%" == "" (
	@echo Run f.bat first
	goto end
)

@mkdir %FBASEDIR%\build
@cd %FBASEDIR%\build

@cmake -G "NMake Makefiles" ../projects
@if errorlevel 1 goto end
@nmake
@cd fapp
@fapp

@cd %FBASEDIR%

:end
