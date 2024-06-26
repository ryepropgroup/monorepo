@echo off

git submodule update --init --recursive
cd vendor/vcpkg && bootstrap-vcpkg.bat
