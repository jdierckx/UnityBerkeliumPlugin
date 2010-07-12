@echo off

:start
echo Removing temporary files to prepare for packaging...

:: VS2008 files
del src\vs2008\Debug\*.* /Q
rmdir src\vs2008\Debug
del src\vs2008\Release\*.* /Q
rmdir src\vs2008\Release
del src\vs2008\*.ncb /Q
del src\vs2008\*.suo /Q
del src\vs2008\*.user /Q

:: Runtime files
del test-project\*.log /Q
del test-release\*.log /Q
del test-release\BerkeliumUnityTest_Data\output_log.txt /Q

:done
echo done!
pause

:exit
