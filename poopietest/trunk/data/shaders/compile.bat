@echo off

@rem set params=/Zi /Od
@set params=/O3

@rem for /f "delims==" %%F in ('dir /b *.fx') do echo ----------------------------------------------------------- && echo PS: %%F && echo ----------------------------------------------------------- && fxc test.fx /Tps_5_0 %params% /EPS /Foo/%%~nF_PS.fxo || goto virhe
@rem for /f "delims==" %%F in ('dir /b *.fx') do echo ----------------------------------------------------------- && echo VS: %%F && echo ----------------------------------------------------------- && fxc test.fx /Tvs_5_0 %params% /EPS /Foo/%%~nF_VS.fxo || goto virhe

@for /f "delims==" %%F in ('dir /b *.fx') do echo ----------------------------------------------------------- && echo FX: %%F && echo ----------------------------------------------------------- && fxc test.fx /Tfx_5_0 %params% /EPS /Foo/%%~nF.fxo || goto virhe


@goto allok

:virhe
@echo -
@echo -----------------------------------------------------------
@echo Error compiling, see above for error
@echo -----------------------------------------------------------

:allok



