"%NITROSDK_ROOT%\tools\bin\makerom.exe" -DMAKEROM_ROMROOT=./files -DMAKEROM_ROMFILES=./data -DMAKEROM_ROMSPEED=UNDEFINED -DPROC=ARM9 -DPLATFORM=TS -DCODEGEN= -DBUILD=Rom -DMAKEROM_ARM9=./main.nef -DMAKEROM_ARM7="%NITROSDK_ROOT%/components/mongoose/ARM7-TS/Rom/mongoose_sub.nef" "%NITROSDK_ROOT%/include/nitro/specfiles/ROM-TS.rsf" trainds.nds
@if not errorlevel 1 goto end
@echo Binary could not be created.
@PAUSE
:end