@echo off
echo GPU Name Swap - Auto Inject
echo ================================
echo.

echo [1] Killing Task Manager...
taskkill /F /IM Taskmgr.exe 2>nul
timeout /t 3 /nobreak >nul

echo [2] Copying latest DLL...
copy /Y "C:\gpumod6\gpu_hook.dll" "C:\gpumod6\gpu_hook.dll" 2>nul
copy /Y "C:\gpumod6\inject.exe" "C:\gpumod6\inject.exe" 2>nul

echo [3] Starting Task Manager...
start "" "C:\Windows\System32\Taskmgr.exe" /3
timeout /t 3 /nobreak >nul

echo [4] Injecting DLL...
"C:\gpumod6\inject.exe" --dll "C:\gpumod6\gpu_hook.dll"

echo.
echo Done!