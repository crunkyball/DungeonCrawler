@echo off

REM Make sure the various variables that we need are set

IF EXIST %~dp0..\Externals\CMakeModules (
	set GW_DEPS_ROOT=%~dp0..\
)

IF NOT DEFINED GW_DEPS_ROOT GOTO GW_DEPS_ROOT_UNDEFINED

IF EXIST %GW_DEPS_ROOT%\Externals\CMakeModules (
	set CMAKE_MODULE_PATH=%GW_DEPS_ROOT%\Externals\CMakeModules
) ELSE (
	set CMAKE_MODULE_PATH=%GW_DEPS_ROOT%\sw\physx\tools\CMakeModules
)

set PX_OUTPUT_ROOT=%~dp0

REM Generate projects here

rmdir /s /q compiler\vc14xboxone-cmake\
mkdir compiler\vc14xboxone-cmake\
pushd compiler\vc14xboxone-cmake\
cmake ..\cmake\xboxone -G "Visual Studio 14 2015" -DTARGET_BUILD_PLATFORM=XboxOne -DPX_OUTPUT_LIB_DIR=%PX_OUTPUT_ROOT%\Lib\vc14xboxone-cmake -DCMAKE_TOOLCHAIN_FILE="%CMAKE_MODULE_PATH%\xboxone\XboxOneToolchain.txt" -DCMAKE_GENERATOR_PLATFORM=Durango

REM Because XboxOne build is a bit wonky, delete these meta projects.

del ZERO_CHECK.*
del ALL_BUILD.*

popd

GOTO :End

:GW_DEPS_ROOT_UNDEFINED
ECHO GW_DEPS_ROOT has to be defined, pointing to the root of the dependency tree.
PAUSE
GOTO END

:BOOST_ROOT_UNDEFINED
ECHO BOOST_ROOT has to be defined, pointing to the root of your local Boost install.
PAUSE
GOTO END

:CUDA_ROOT_UNDEFINED
ECHO CUDA_BIN_PATH has to be defined, pointing to the bin folder of your local CUDA install.
PAUSE

:End
