for /r %%i in (*.bmp) do "%NITROSDK_ROOT%\tools\bin\ntexconv.exe" -b -f direct "%%i"
pause