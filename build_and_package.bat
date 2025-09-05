@echo off
setlocal enabledelayedexpansion

REM 0. Ask for version
set /p VERSION=Enter version number (e.g. 0.3.0): 

REM 1. Define paths
set "ROOT_DIR=%~dp0"
set "OUTDIR=%ROOT_DIR%dist\SimsSwitcher-%VERSION%"
set "PKG_ID=com.wrenswift.simsswitcher"
set "PKG_DIR=%ROOT_DIR%installer\packages\%PKG_ID%"
set "REPO_DIR=%ROOT_DIR%repository\%PKG_ID%"
set "QTIFW_BIN=C:\Qt\Tools\QtInstallerFramework\4.10\bin"
set "DEPLOYQT=C:\Qt\6.8.3\msvc2022_64\bin\windeployqt.exe"

REM 2. Build and deploy files to OUTDIR
if exist "%OUTDIR%" rmdir /S /Q "%OUTDIR%"
mkdir "%OUTDIR%"

"%DEPLOYQT%" --release --dir "%OUTDIR%" "%ROOT_DIR%build\SimsSwitcher.exe"
copy /Y "%ROOT_DIR%build\SimsSwitcher.exe" "%OUTDIR%\"
xcopy /E /I /Y "%ROOT_DIR%build\inc"   "%OUTDIR%\inc\"
xcopy /E /I /Y "%ROOT_DIR%build\icons" "%OUTDIR%\icons\"

REM 3. Zip the distribution
powershell -Command ^
  "Compress-Archive -Path '%OUTDIR%\*' -DestinationPath '%OUTDIR%.zip' -Force"

REM 4. Prepare installer package directory
if exist "%PKG_DIR%\data" rmdir /S /Q "%PKG_DIR%\data"
mkdir "%PKG_DIR%\data"
xcopy /E /I /Y "%OUTDIR%\*" "%PKG_DIR%\data\"

REM 5. Update Version in package.xml
powershell -Command "(Get-Content '%PKG_DIR%\meta\package.xml') -replace '<Version>.*?</Version>', '<Version>%VERSION%</Version>' | Set-Content '%PKG_DIR%\meta\package.xml'"

REM 6. Build the offline installer
"%QTIFW_BIN%\binarycreator.exe" --config installer\config\config.xml --packages installer\packages SimsSwitcherInstaller-%VERSION%.exe

REM 7. Prepare repository for updates
if exist "%REPO_DIR%\%VERSION%" rmdir /S /Q "%REPO_DIR%\%VERSION%"
mkdir "%REPO_DIR%\%VERSION%\meta"
mkdir "%REPO_DIR%\%VERSION%\data"

xcopy /E /I /Y "%PKG_DIR%\meta\*" "%REPO_DIR%\%VERSION%\meta\"
xcopy /E /I /Y "%PKG_DIR%\data\*" "%REPO_DIR%\%VERSION%\data\"
xcopy /E /I /Y "repository\com.wrenswift.simsswitcher\%VERSION%\meta" "repository\com.wrenswift.simsswitcher\meta"
xcopy /E /I /Y "repository\com.wrenswift.simsswitcher\%VERSION%\data" "repository\com.wrenswift.simsswitcher\data"

REM 8. Generate Updates.xml
"%QTIFW_BIN%\repogen.exe" -v --update-new-components --packages "%ROOT_DIR%repository" "%ROOT_DIR%repository\Updates.xml"

echo.
echo ================================
echo Build, installer, and repo update complete for v%VERSION%.
echo Repository Updates.xml refreshed.
echo ================================
pause