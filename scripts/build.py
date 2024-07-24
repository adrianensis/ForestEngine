import sys
import os
import getopt
import shutil
import lib.cmake_build as cmake_build
from lib.build_global_data import BuildGlobalData
import lib.log as log

cwd = os.getcwd()
print(cwd)

##########################################
########## DATA ###########
##########################################

buildUnitTests=False
buildIntegrationTests=False
enableLogs=False
enableProfiler=False
enableAddressSanitizer=False
enableGPUDebug=False

appsToBuild = []
toolsToBuild = []

argv = []
if(len(sys.argv) > 1):
    argv = sys.argv[1:]

try:
  opts, args = getopt.getopt(argv, "uilcprd", ["app=", "tool=", "asan", "gpuDbg"])
except:
  log.log(log.LogLabels.error, "Error parsing options!")
  exit(1)

buildType=BuildGlobalData.buildDebug

for opt, arg in opts:
    arg_list = arg.split(",")

    if opt in ['-c']:
      os.system('./scripts/clean.sh')
    elif opt in ['-r']:
      buildType=BuildGlobalData.buildRelease
    elif opt in ['-u']:
      buildUnitTests=True
    elif opt in ['-i']:
      buildIntegrationTests=True
    elif opt in ['--tool']:
      toolsToBuild = arg_list
    elif opt in ['--app']:
      appsToBuild = arg_list
    elif opt in ['-l']:
      enableLogs=True
    elif opt in ['-p']:
      enableProfiler=True
    elif opt in ['-d']:
      enableProfiler=True
    elif opt in ['--asan']:
      enableAddressSanitizer=True
    elif opt in ['--gpuDbg']:
      enableGPUDebug=True

buildTargetDir=os.path.join(BuildGlobalData.buildDir, buildType)

cmake_generated_data = cmake_build.generate_cmake_data()

##########################################
########## PRE BUILD ###########
##########################################

##########################################
########## BUILD ###########
##########################################

buildCommandArgs = [
    "-DCMAKE_BUILD_TYPE=" + buildType,
    "-DBUILD_UNIT_TESTS=" + str(buildUnitTests),
    "-DBUILD_INTEGRATION_TESTS=" + str(buildIntegrationTests),
    "-DTOOLS_TO_BUILD=" + str(";".join(toolsToBuild)),
    "-DAPPS_TO_BUILD=" + str(";".join(appsToBuild)),
    "-DENABLE_LOGS=" + str(enableLogs),
    "-DENABLE_PROFILER=" + str(enableProfiler),
    "-DENABLE_ADDRESS_SANITIZER=" + str(enableAddressSanitizer),
    "-DENABLE_GPU_DEBUG=" + str(enableGPUDebug),
    "-DCMAKE_CXX_FLAGS_DEBUG=" + "-flto=auto",
    "-DCMAKE_CXX_FLAGS_RELEASE=" + "-flto=auto",
    # "-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=mold",
    # "-DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=mold",
]

cmake_build.build_cmake(cwd, BuildGlobalData.buildDir, buildType, cmake_generated_data, buildCommandArgs)

##########################################
########## POST BUILD ###########
##########################################

compileCommandsJson = "compile_commands.json"
shutil.copy(os.path.join(buildTargetDir, compileCommandsJson), compileCommandsJson)