@set CVIDIR=c:\program files (x86)\national instruments\cvi2019
@set CVISHAREDDIR=C:\Program Files (x86)\National Instruments\Shared\CVI
@set CVIPUBDOCSDIR=C:\Users\Public\Documents\National Instruments\CVI
@set CVIPROJPATH=c:\work\elcom\valeo\UTSystem\src\modules\gui\rnr\rnr.prj
@set CVIPROJDIR=c:\work\elcom\valeo\UTSystem\src\modules\gui\rnr
@set CVIPROJNAME=rnr.prj
@set CVITARGETPATH=c:\Flexitester.OK\plugins\rnr.dll
@set CVITARGETDIR=c:\Flexitester.OK\plugins
@set CVITARGETNAME=rnr.dll
@set CVIBUILDCONFIG=Debug
@set CVITARGETFILEVER=1.0.0.0
@set CVITARGETPRODVER=1.0.0.0
@set CVIVXIPNPDIR=C:\Program Files (x86)\IVI Foundation\VISA\winnt
@set CVIIVIDIR=C:\Program Files (x86)\IVI Foundation\IVI
@set CVIWINDIR=C:\Windows
@set CVISYSDIR=C:\Windows\system32
@pushd "c:\work\elcom\valeo\UTSystem\src\modules\gui\rnr"
@setlocal
@set _some_build_step_failed=0
xcopy /y %CVIPROJDIR%\rnr.xml  c:\work\elcom\valeo\UTSystem\src\app\plugins\
@if %errorlevel% neq 0 (@set _some_build_step_failed=1)
@exit /b %_some_build_step_failed%
