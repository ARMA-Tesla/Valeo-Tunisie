@set CVIDIR=c:\program files (x86)\national instruments\cvi2010
@set CVISHAREDDIR=C:\Program Files (x86)\National Instruments\Shared\CVI
@set CVIPUBDOCSDIR=C:\Users\Public\Documents\National Instruments\CVI
@set CVIPROJPATH=c:\work\elcom\valeo\UTSystem\src\modules\Calibration\calibration_module.prj
@set CVIPROJDIR=c:\work\elcom\valeo\UTSystem\src\modules\Calibration
@set CVIPROJNAME=calibration_module.prj
@set CVITARGETPATH=c:\work\elcom\valeo\UTSystem\src\app\modules\ivibridge.dll
@set CVITARGETDIR=c:\work\elcom\valeo\UTSystem\src\app\modules
@set CVITARGETNAME=ivibridge.dll
@set CVIBUILDCONFIG=Release
@set CVITARGETFILEVER=1.1.0.0
@set CVITARGETPRODVER=1.1.0.0
@set CVIVXIPNPDIR=C:\Program Files (x86)\IVI Foundation\VISA\winnt
@set CVIIVIDIR=C:\Program Files (x86)\IVI Foundation\IVI
@set CVIWINDIR=C:\Windows
@set CVISYSDIR=C:\Windows\system32
xcopy /y %CVIPROJDIR%\ivibridge.xml  c:\work\elcom\valeo\UTSystem\src\app\modules\
