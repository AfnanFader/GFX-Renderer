@echo off
set CONFIG=%1
set BUILD_DIR=build

if "%CONFIG%"=="clean_all" if exist %BUILD_DIR% (
    echo [INFO] Cleaning previous build.
    :: Some Windows bullshit, for CMD and Powershell compability ....
    powershell -Command "Remove-Item -Path "./build" -Recurse -Force -Verbose"
)

:: Configure project and currently by default it is set to Debug Mode
echo [INFO] Configuring Cmake Project ...
cmake -S . -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=Debug
if %ERRORLEVEL%==0 (
    echo [INFO] Setup was successful..!
) else (
    echo [ERROR] Setup Failed - Code: %ERRORLEVEL%
)

pause