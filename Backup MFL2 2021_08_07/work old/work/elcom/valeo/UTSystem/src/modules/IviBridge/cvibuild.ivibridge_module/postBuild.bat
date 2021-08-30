@set CVIDIR=c:\program files\national instruments\cvi2010
@set CVISHAREDDIR=C:\Program Files\National Instruments\Shared\CVI
@set CVIPUBDOCSDIR=C:\Users\Public\Documents\National Instruments\CVI
@set CVIPROJPATH=c:\work\elcom\valeo\UTSystem\src\modules\IviBridge\ivibridge_module.prj
@set CVIPROJDIR=c:\work\elcom\valeo\UTSystem\src\modules\IviBridge
@set CVIPROJNAME=ivibridge_module.prj
@set CVITARGETPATH=c:\FlexiTester.OK\modules\ivibridge.dll
@set CVITARGETDIR=c:\FlexiTester.OK\modules
@set CVITARGETNAME=ivibridge.dll
@set CVIBUILDCONFIG=Release
@set CVITARGETFILEVER=1.1.0.0
@set CVITARGETPRODVER=1.1.0.0
@set CVIVXIPNPDIR=C:\Program Files\IVI Foundation\VISA\winnt
@set CVIIVIDIR=C:\Program Files\IVI Foundation\IVI
@set CVIWINDIR=C:\Windows
@set CVISYSDIR=C:\Windows\system32
xcopy /y %CVIPROJDIR%\ivibridge.xml  c:\work\elcom\valeo\UTSystem\src\app\modules\
