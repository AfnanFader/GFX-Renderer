@echo off
set BUILD_DIR=build

:: Build Project.
if exist %BUILD_DIR% (

    echo [INFO] Start Build ...

    cmake --build %BUILD_DIR% --verbose
    if %ERRORLEVEL%==0 (
        echo [INFO] Build Passed!
    ) else (
        echo [ERROR] Build Failed - Code: %ERRORLEVEL%
    )
) else (
    echo [ERROR] Please run the setup script .. 
)

pause