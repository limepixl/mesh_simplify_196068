@echo off
if not exist "build" mkdir "build"
pushd "build"

set "files=../src/main.cpp ../src/loader.cpp ../tinyobjloader/tiny_obj_loader.cc"

@REM MSVC
set "compiler_flags=-Oi -O2 -Zi -FC -WX -W4 -wd4201 -wd4189 -wd4100 -wd4146 -GR- -EHsc -MT -nologo -D_CRT_SECURE_NO_WARNINGS -I ../glm"
cl %compiler_flags% %files% /link -opt:ref /out:mesh_simplify.exe

@REM Clang
@REM set "compiler_flags=-O2 -g3 -Wall -Wextra -Werror -Wno-missing-braces -Wno-unused-variable -Wno-missing-field-initializers -D_CRT_SECURE_NO_WARNINGS -I ../glm"
@REM clang++ %compiler_flags% %files% -o mesh_simplify.exe

popd

echo.
echo Build finished.