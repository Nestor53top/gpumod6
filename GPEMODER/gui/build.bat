@echo off
call "D:\VisualStudio\VC\Auxiliary\Build\vcvarsall.bat" x64
cd /d "C:\gpumod6\gui\build"
cmake -G Ninja -DCMAKE_PREFIX_PATH="C:\Qt5\install" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
