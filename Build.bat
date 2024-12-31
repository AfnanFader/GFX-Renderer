@echo off
set CONFIG=%1
set BUILD_DIR=build

if "%CONFIG%"=="clean" if exist %BUILD_DIR% (
    echo [INFO] Cleaning previous build.
    rmdir /s /q %BUILD_DIR%
)

:: Configure project and currently by default it is set to Debug Mode
echo [INFO] Configuring Cmake Project ...
cmake -S . -B %BUILD_DIR% -DCMAKE_BUILD_TYPE=Debug

:: Build Project.
echo [INFO] Start Build ...
cmake --build %BUILD_DIR%
if %ERRORLEVEL%==0 (
    echo [INFO] Build Passed!
) else (
    echo [ERROR] Build Failed - Code: %ERRORLEVEL%
)

pause