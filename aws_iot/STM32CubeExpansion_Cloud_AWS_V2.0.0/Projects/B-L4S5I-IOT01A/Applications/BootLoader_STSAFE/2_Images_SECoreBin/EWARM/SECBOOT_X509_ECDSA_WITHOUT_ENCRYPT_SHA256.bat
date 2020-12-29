::Post build for SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256
:: arg1 is the build directory
:: arg2 is the elf file path+name
:: arg3 is the bin file path+name
:: arg4 is the version
:: arg5 when present forces "bigelf" generation
@echo off
set "projectdir=%1"
set "execname=%~n3"
set "elf=%2"
set "bin=%3"
set "version=%4"

set "SBSFUBootLoader=%~d0%~p0\\..\\.."
::The default installation path of the Cube Programmer tool is: "C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
::If you installed it in another location, please update the %programmertool% variable below accordingly.
set "programmertool="C:\\Program Files\\STMicroelectronics\\STM32Cube\\STM32CubeProgrammer\\bin\\STM32_Programmer_CLI""
set "userAppBinary=%projectdir%\\PostBuild\\"

set "sfb=%userAppBinary%\\%execname%.sfb"
set "sign=%userAppBinary%\\%execname%.sign"
set "headerbin=%userAppBinary%\\%execname%sfuh.bin"
set "bigbinary=%userAppBinary%\\SBSFU_%execname%.bin"
set "elfbackup=%userAppBinary%\\SBSFU_%execname%.elf"
set "partialbin=%userAppBinary%\\Partial%execname%.bin"
set "partialsfb=%userAppBinary%\\Partial%execname%.sfb"
set "partialsign=%userAppBinary%\\Partial%execname%.sign"
set "partialoffset=%userAppBinary%\\Partial%execname%.offset"
set "ref_userapp=%projectdir%\\RefUserApp.bin"

set "fw_sign_ecckey=%SBSFUBootLoader%\\2_Images_SBSFU\\Certs\\sbsfu_fs_prv.pem"
set "sbsfuelf=%SBSFUBootLoader%\\2_Images_SBSFU\\EWARM\\B-L4S5I-IOT01\\Exe\\\project.out"
set "cert_fw_leaf=%SBSFUBootLoader%\\2_Images_SBSFU\\Certs\\sbsfu_fs_crt.der"
set "cert_fw_inter=%SBSFUBootLoader%\\2_Images_SBSFU\\Certs\\sbsfu_i2_crt.der"

::comment this line to force python
::python is used if windows executeable not found
pushd %projectdir%\..\..\..\..\..\..\Middlewares\ST\STM32_Secure_Engine\Utilities\KeysAndImages
set basedir=%cd%
popd
goto exe:
goto py:
:exe
::line for window executeable
echo Postbuild with windows executable
set "prepareimage=%basedir%\\win\\prepareimage\\prepareimage.exe"
set "python="
if exist %prepareimage% (
goto postbuild
)
:py
::line for python
echo Postbuild with python script
set "prepareimage=%basedir%\\prepareimage.py"
set "python=python "
:postbuild

::Make sure we have a Binary sub-folder in UserApp folder
if not exist "%userAppBinary%" (
mkdir "%userAppBinary%" > %projectdir%\output.txt 2>&1
IF %ERRORLEVEL% NEQ 0 goto :error
)

::PostBuild is fired if elf has been regenerated from last run, so elf is different from backup elf
if exist %elfbackup% (
fc %elfbackup% %elf% >NUL && (goto nothingtodo) || echo "elf has been modified, processing required"
)

:: no encryption
set "command=%python%%prepareimage% sha256 %bin% %sign% > %projectdir%\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

:: no encryption so pack the binary file
set "command=%python%%prepareimage% pack -k %fw_sign_ecckey% -p 1 -r 44 -v %version% --cert_fw_inter %cert_fw_inter% --cert_fw_leaf %cert_fw_leaf% -f %bin% -t %sign% %sfb% -o 2048 >> %projectdir%\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

set "command=%python%%prepareimage% header -k %fw_sign_ecckey% -p 1 -r 44 -v %version% -f %bin% -t %sign%  -o 2048 --cert_fw_inter %cert_fw_inter% --cert_fw_leaf %cert_fw_leaf% %headerbin% >> %projectdir%\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

set "command=%python%%prepareimage% merge -i %headerbin% -s %sbsfuelf% -u %elf% %bigbinary% >> %projectdir%\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

::Partial image generation if reference userapp exists
if not exist "%ref_userapp%" (
goto :bigelf
)
set "command=%python%%prepareimage% diff -1 %ref_userapp% -2 %bin% %partialbin% -a 16 --poffset %partialoffset% >> %projectdir%\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

set "command=%python%%prepareimage% sha256  %partialbin% %partialsign% >> %projectdir%\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

set "command=%python%%prepareimage% pack -k %fw_sign_ecckey% -p 1 -r 44 -v %version% --cert_fw_inter %cert_fw_inter% --cert_fw_leaf %cert_fw_leaf% -f %bin% -t %sign% -o 2048 --pfw %partialbin% --ptag %partialsign% --poffset  %partialoffset% %partialsfb%>> %projectdir%\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

:bigelf
IF  "%~5"=="" goto :finish
echo "Generating the global elf file (SBSFU and userApp)"
echo "Generating the global elf file (SBSFU and userApp)" >> %projectdir%\output.txt
set "command=%programmertool% -ms %elf% %headerbin% %sbsfuelf% >> %projectdir%\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error
set "command=copy %elf% %elfbackup%  >> %projectdir%\output.txt 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

:finish
::backup and clean up the intermediate file
del %sign%
del %headerbin%
if exist "%ref_userapp%" (
del %partialsign%
del %partialbin%
del %partialoffset%
)

exit 0

:error
echo "%command% : failed" >> %projectdir%\\output.txt
:: remove the elf to force the regeneration
if exist %elf%(
  del %elf%
)
if exist %elfbackup%(
  del %elfbackup%
)
echo %command% : failed

pause
exit 1

:nothingtodo
exit 0
