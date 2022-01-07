@echo off
if not exist "build" mkdir "build"
pushd "build"

set "files=../src/main.cpp ../src/loader.cpp ../src/func.cpp ../src/exporter.cpp ../tinyobjloader/tiny_obj_loader.cc"

@REM MSVC
@REM set "compiler_flags=-Oi -O2 -Zi -FC -WX -W4 -wd4201 -wd4189 -wd4100 -wd4146 -GR- -EHsc -MT -nologo -D_CRT_SECURE_NO_WARNINGS -I ../glm"
@REM cl %compiler_flags% %files% /link -opt:ref /out:mesh_simplify.exe

@REM Clang
set "compiler_flags=-O2 -g3 -Wall -Wextra -Werror -Wno-missing-braces -Wno-unused-variable -Wno-missing-field-initializers -D_CRT_SECURE_NO_WARNINGS -I ../glm"
clang++ %compiler_flags% %files% -o mesh_simplify.exe

popd

echo.
echo Build finished.