
@if "%FBASEDIR%" == "" (
	@echo Run f.bat first
	goto end
)

@tools\eclipse\eclipse

:end