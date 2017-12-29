
@if "%FBASEDIR%" == "" (
	@echo Run f.bat first
	goto end
)

@mkdir %FBASEDIR%\build
@cd %FBASEDIR%\build

@cmake -G "Visual Studio 10" ../projects

@cd %FBASEDIR%

:end
