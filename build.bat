@echo off

set opts=-FC -GR- -EHa- -nologo -Zi
set code=%cd%
pushd .
cl %opts% %code%\main.cpp -Femain.exe
popd
