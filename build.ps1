function check-dependency
{
    $r = new-object -TypeName System.IO.StreamReader((Get-Location).Path + '\' + $args[0])
    $firstLine = $r.ReadLine();
    $parts = $firstLine.Split(':');
    $objectDate = [System.IO.File]::GetLastWriteTimeUtc((Get-Location).Path + '\' + $parts[0].Replace("\ "," "))
    $sourceDate = [System.IO.File]::GetLastWriteTimeUtc((Get-Location).Path + '\' + $parts[1].Trim(' ','\',"`t").Replace("\ ", " "))
    if($objectDate -lt $sourceDate)
    {
        $r.Close();
        return 1
    }
    #Check all dependencies (until one has changed)
    while (($line = $r.ReadLine()) -ne $null)
    {
        $depDate = [System.IO.File]::GetLastWriteTimeUtc($line.Trim(' ', '\', "`t").Replace("\ ", " "))
        if ($objectDate -lt $depDate)
        {
            $r.Close();
            return 1
        }
    }
    $r.Close();
    return 0
}

$build = 'build'
$sources = 'src', 'src\io', 'src\core\os', 'src\terrain', 'src\terrain\track', 'src\terrain\scenery', 'src\terrain\managers', 'src\menu', 'src\engine', 'src\engine\controllers', 'src\engine\objects', 'src\engine\cameras', 'src\vehicles', 'src\ui', 'src\ui\layoutengine', 'src\ui\game', 'src\ui\components', 'src\inih', 'src\tools', 'src\videoplayer', 'src\mb'
$includes = 'src', ($env:CWFolder_NITRO + '\ARM_EABI_Support\msl\MSL_C\MSL_Common\Include'), ($env:CWFolder_NITRO + '\ARM_EABI_Support\msl\MSL_C++\MSL_Common\Include')
$static_excludes = @() #'DecodeARMv5Stride256VXS2.o', 'yuv2rgb_new.o'  
$arch = '-proc arm946e -little -interworking'
# we could use -d NNS_G3D_USE_FASTGXDMA probably
$cflags = '-MD -d SDK_ARM9 -d SDK_CODE_ARM -d NITRODWC_NOGS -g -fp soft -O4 -r -enum int  -inline all -ipa file ' + $arch;
$cxxflags = $cflags + ' -Cpp_exceptions off -RTTI off'
$asmflags = '-MD -gnu -proc arm5TE -g'
$ldflags = '-m _start -g -map closure, unused ' + $arch
$libdirs = '.', $env:NITROSDK_ROOT, $env:NITROSYSTEM_ROOT, $env:NITROWIFI_ROOT, $env:NITRODWC_ROOT, ($env:CWFolder_NITRO + '\ARM_EABI_Support\msl\MSL_C\MSL_ARM'), ($env:CWFolder_NITRO + '\ARM_EABI_Support\msl\MSL_Extras\MSL_ARM'), ($env:CWFolder_NITRO + '\ARM_EABI_Support\Mathlib'), ($env:CWFolder_NITRO + '\ARM_EABI_Support\Runtime\Runtime_ARM\Runtime_NITRO'), ($env:CWFolder_NITRO + '\ARM_EABI_Support\msl\MSL_C++\MSL_ARM\')
#'isdmainparm'
$libs = 'rtc', 'spi', 'stubscw', 'stubsisd', 'dgt', 'card', 'cp', 'ext', 'fs', 'fx', 'gx', 'mi', 'os', 'prc', 'pxi', 'snd', 'MSL_C_NITRO_Ai_LE.a', 'MSL_Extras_NITRO_Ai_LE.a', 'NITRO_Runtime_Ai_LE.a', 'MSL_CPP_NITRO_Ai_LE.a', 'wm', 'wbt', 'wfs', 'mb', 'syscall', 'ctrdg', 'math', 'cht', 'FP_fastI_v5t_LE.a', 'std', 'env', 'nnsfnd', 'nnsg3d', 'nnsgfd', 'nnsg2d', 'nnssnd', 'isdmainparm', 'cps', 'soc', 'ssl', 'wcm', 'dwcac', 'dwcaccount', 'dwcauth', 'dwcbase', 'dwcbm', 'dwcnd', 'dwcnhttp', 'dwcutil', 'stubsmd5'

$overlays = 'videoplayeritcm', 'renderingitcm'

if($Args.length -eq 0 -or $Args[0] -eq 'build')
{
    if(!(test-path $build))
    {
        new-item -name $build -type directory
    }
    $cfiles = ''
    $cppfiles = ''
    $sfiles = ''
    foreach($path in $sources)
    {
        $indir = get-item -path ($path + '\\*.c')
        if($indir.length -gt 0)
        {
            foreach($path2 in $indir)
            {
                $depname = 'build\' + [System.IO.Path]::GetFileNameWithoutExtension(($path2)) + '.d'
                if(test-path ($depname))
                {
                    if((check-dependency $depname) -eq 0)
                    {
                        continue
                    }
                }                
                $cfiles += '"' + $path2 + '" '
            }
        }
        $indir = get-item -path ($path + '\\*.cpp')
        if($indir.length -gt 0)
        {
            foreach($path2 in $indir)
            {
                $depname = 'build\' + [System.IO.Path]::GetFileNameWithoutExtension(($path2)) + '.d'
                if(test-path ($depname))
                {
                    if((check-dependency $depname) -eq 0)
                    {
                        continue
                    }
                }
                $cppfiles += '"' + $path2 + '" '
            }
        }
        $indir = get-item -path ($path + '\\*.s')
        if($indir.length -gt 0)
        {
            foreach($path2 in $indir)
            {
                $depname = 'build\' + [System.IO.Path]::GetFileNameWithoutExtension(($path2)) + '.d'
                if(test-path ($depname))
                {
                    if((check-dependency $depname) -eq 0)
                    {
                        continue
                    }
                }
                $sfiles += '"' + $path2 + '" '
            }
        }
    }
    $include_string = ''
    foreach($path in $includes)
    {
        $include_string += '"-I' + $path + '" ';
    }
    foreach($path in $libdirs)
    {
        if(test-path ($path + '\include'))
        {
            $include_string += '"-I' + $path + '\include" ';
        }
    }
    if($sfiles)
    {
        Start-Process ($env:CWFolder_NITRO + '\ARM_Tools\Command_Line_Tools\mwasmarm.exe') ('-nowraplines -nostdinc -msgstyle parseable -gccinc -c ' + $asmflags + ' ' + $include_string + ' -o ' + $build + ' ' + $sfiles) -NoNewWindow -Wait
    }
    if($cfiles)
    {
        Start-Process ($env:CWFolder_NITRO + '\ARM_Tools\Command_Line_Tools\mwccarm.exe') ('-nowraplines -nostdinc -msgstyle parseable -gccinc -c ' + $cflags + ' ' + $include_string + ' -o ' + $build + ' ' + $cfiles) -NoNewWindow -Wait
    }
    if($cppfiles)
    {
        Start-Process ($env:CWFolder_NITRO + '\ARM_Tools\Command_Line_Tools\mwccarm.exe') ('-nowraplines -nostdinc -msgstyle parseable -gccinc -c ' + $cxxflags + ' ' + $include_string + ' -o ' + $build + ' ' + $cppfiles) -NoNewWindow -Wait
    }
    $makelcf_lib_dirs = @()
    $libdirs_string = ''
    foreach($path in $libdirs)
    {
        if(test-path ($path + '\lib\ARM9-TS\Release'))
        {
            $libdirs_string += '-l "' + $path + '\lib\ARM9-TS\Release" '
            $makelcf_lib_dirs += ,($path + '\lib\ARM9-TS\Release')
            if(test-path ($path + '\lib\ARM9-TS\etc'))
            {
                $libdirs_string += '-l "' + $path + '\lib\ARM9-TS\etc" '
                $makelcf_lib_dirs += ,($path + '\lib\ARM9-TS\etc')
            }
        }
        else
        {
            $libdirs_string += '-l "' + $path + '\lib" '
            $makelcf_lib_dirs += ,($path + '\lib')
        }
    }
    $makelcf_lib = ''
    $lib_string = ''
    foreach($lib in $libs)
    {
        $lib_string += '"-l' + $lib + '" ';
        if($lib.EndsWith('.a'))
        {
            $makelcf_lib += '"' + $lib + '" '
        }
        else
        {
            $makelcf_lib += '"lib' + $lib + '.a" '
        }
    }
    $ofiles = ''
    $ofiles += '"' + $env:NITROSDK_ROOT + '\lib\ARM9-TS\Release\crt0.o" '
    #$static_ofiles = $ofiles
	$static_ofiles = ''
	$overlay_ofiles = ''
    $indir = get-item -path ($build + '\\*.o')
    foreach($path2 in $indir)
    {
        $ofiles += '"' + $path2 + '" '
        #if(!$static_excludes.Contains([System.IO.Path]::GetFileName($path2)))
        #{
       #     $static_ofiles += '"' + $path2 + '" '
       # }
	   $name = [System.IO.Path]::GetFileName($path2);
	   $placed = $false;
	   foreach($overlay in $overlays)
	   {
		   if($name.Contains("." + $overlay + "."))
		   {
				$placed = $true;
				break;
		   }
	   }
	   if(!$placed)
	   {
		$static_ofiles += '"' + $path2 + '" '
		}
    }
	$overlaystuff = '';
	foreach($overlay in $overlays)
	{
		$overlaystuff += "-DOVERLAYFILES_" + $overlay + '="'# + $overlay_ofiles[$overlay] + '" '
		foreach($path2 in $indir)
		{
		   $name = [System.IO.Path]::GetFileName($path2);
			if($name.Contains("." + $overlay + "."))
			{
				$overlaystuff += '"' + $path2 + '" '
			}
		}
		$overlaystuff += '" '
	}
    #Start-Process ($env:NITROSDK_ROOT + '\tools\bin\makelcf.exe') ('Nitro.lcf.lsf "' + $env:NITROSDK_ROOT + '\include\nitro\specfiles\ARM9-TS.lcf.template" ' + $build + '\Nitro.lcf') -NoNewWindow -Wait
    #Start-Process ($env:NITROSDK_ROOT + '\tools\bin\makelcf.exe') ('-DOBJDIR="' + $build + '" -DOBJS_STATIC="' + $static_ofiles + '" -DLLIBS="' + $makelcf_lib + '" Nitro.lcf.lsf "' + $env:NITROSDK_ROOT + '\include\nitro\specfiles\ARM9-TS.lcf.template" ' + $build + '\Nitro.lcf') -NoNewWindow -Wait
    #Start-Process ($env:NITROSDK_ROOT + '\tools\bin\makelcf.exe') ('-DOBJDIR="' + $build + '" Nitro.lcf.lsf "' + $env:NITROSDK_ROOT + '\include\nitro\specfiles\ARM9-TS.lcf.template" ' + $build + '\Nitro.lcf') -NoNewWindow -Wait
	#Start-Process ($env:NITROSDK_ROOT + '\tools\bin\makelcf.exe') ('-DOBJDIR="' + $build + '" Nitro.lcf.lsf "' + 'ARM9-TS.lcf.template" ' + $build + '\Nitro.lcf') -NoNewWindow -Wait
	Start-Process ($env:NITROSDK_ROOT + '\tools\bin\makelcf.exe') ('-DOBJDIR="' + $build + '" ' + $overlaystuff + ' -DOBJS_STATIC="' + $static_ofiles + '" -DLLIBS="' + $makelcf_lib + '" Nitro.lcf.lsf "' + $env:NITROSDK_ROOT + '\include\nitro\specfiles\ARM9-TS-cloneboot-C.lcf.template" ' + $build + '\Nitro.lcf') -NoNewWindow -Wait
	$name = [System.IO.Path]::GetFileName((get-location))
    Start-Process ($env:CWFolder_NITRO + '\ARM_Tools\Command_Line_Tools\mwldarm.exe') ($ldflags + ' -nowraplines -nostdlib -o ' + $build + '\main.nef ' + $libdirs_string + ' ' + $lib_string + ' ' + $ofiles + ' ' + $build + '\Nitro.lcf') -NoNewWindow -Wait
    start-process ($env:NITROSDK_ROOT + '\tools\bin\compstatic.exe') ('-a -9 -c -F ./' + $build + '/main.sbin ./' + $build + '/main_defs.sbin ./' + $build + '/main_table.sbin') -NoNewWindow -Wait 
    start-process ($env:NITROSDK_ROOT + '\tools\bin\makerom.exe') ('-DMAKEROM_ROMROOT=./files -DMAKEROM_ROMFILES=./ -DMAKEROM_ROMSPEED=MROM -DPROC=ARM9 -DPLATFORM=TS -DCODEGEN= -DBUILD=Rom -DMAKEROM_ARM9=./' + $build + '/main.nef -DMAKEROM_ARM7="' + $env:NITROSDK_ROOT + '/components/mongoose/ARM7-TS/Rom/mongoose_sub.nef" ROM-TS.rsf ' + $name + '.nds') -NoNewWindow -Wait 
}
elseif($Args[0] -eq 'clean')
{
    if(test-path $build)
    {
        remove-item -path $build -force -recurse
    }
    $name = [System.IO.Path]::GetFileName((get-location))
    if(test-path ($name + '.nds'))
    {
        remove-item -path ($name + '.nds') -force -recurse
    }
    if(test-path ($name + '.nlf'))
    {
        remove-item -path ($name + '.nlf') -force -recurse
    }
}
else
{
    write-host 'unknown command'
}