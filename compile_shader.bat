@echo off

set CURRENT_DIR=%cd%
set SHADER_DIR=%CURRENT_DIR%\Assets\Shaders\
set OUT_DIR=%CURRENT_DIR%\Assets\Compiled_Shaders\

if exist "%OUT_DIR%" (
    echo Output directory exist: %OUT_DIR%
) else (
    mkdir ""%OUT_DIR%""
)

echo Compiling Vertex Shader ...
for %%f in (%SHADER_DIR%*.vert) do (
    glslc %%f -o %OUT_DIR%\%%~nf.vert.spv
    if errorlevel 1 (
        echo Failed to compile : %%f
        pause
        exit /b 1
    )
)

echo Compiling Fragment Shader ...
for %%f in (%SHADER_DIR%*.frag) do (
    glslc %%f -o %OUT_DIR%\%%~nf.frag.spv
    if errorlevel 1 (
        echo Failed to compile : %%f
        pause
        exit /b 1
    )
)

echo Shader Compilation Complete !!
pause