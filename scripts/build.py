import sys
import os
import getopt
import shutil
import lib.cmake_build as cmake_build
from lib.build_global_data import BuildGlobalData
import lib.log as log
from pathlib import Path

cwd = os.getcwd()
print(cwd)

def get_current_files(directory):
    path = Path(directory)
    return {str(f) for f in path.rglob('*') if f.is_file() and f.suffix in EXTENSIONS}

def load_previous_files(state_file):
    if not os.path.exists(state_file):
        return set()
    with open(state_file, 'r') as f:
        return {line.strip() for line in f if line.strip()}

def save_current_files(state_file, files):
    with open(state_file, 'w') as f:
        for file in sorted(files):
            f.write(f"{file}\n")

def to_cmake_bool(val):
    return "ON" if val else "OFF"

##########################################
########## DATA ###########
##########################################

TARGET_DIRS = ["./code", "./tools", "./test"]
STATE_FILE = os.path.join(BuildGlobalData.buildDir, "build_file_list.txt")
EXTENSIONS = {".cpp", ".h", ".hpp", ".c"}

buildUnitTests=False
buildIntegrationTests=False
enableLogs=False
enableProfiler=False
enableSanitizer=False
enableGPUDebug=False
enableNinja=False

appsToBuild = []
toolsToBuild = []

argv = []
if(len(sys.argv) > 1):
    argv = sys.argv[1:]

log.log(log.LogLabels.info, "Parsing options")
try:
  opts, args = getopt.getopt(argv, "uilcprd", ["app=", "tool=", "ninja", "sanitizer", "gpuDbg"])
except Exception as e:
  log.log(log.LogLabels.error, "Error parsing options! " + str(argv))
  log.log(log.LogLabels.error, e)
  exit(1)

buildType=BuildGlobalData.buildDebug

log.log(log.LogLabels.info, str(opts))

for opt, arg in opts:
    arg_list = arg.split(",")

    if opt in ['-c']:
      os.system('./scripts/clean.sh')
    elif opt in ['-r']:
      buildType=BuildGlobalData.buildRelease
    elif opt in ['-d']:
      buildType=BuildGlobalData.buildDebug
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
    elif opt in ['--sanitizer']:
      enableSanitizer=True
    elif opt in ['--gpuDbg']:
      enableGPUDebug=True
    elif opt in ['--ninja']:
      enableNinja=True
    else:
      log.log(log.LogLabels.error, "Unkown option! ->" + opt)
      exit(1)
      

buildTargetDir=os.path.join(BuildGlobalData.buildDir, buildType)

projectName = "ForestEngine"

cmake_generator = cmake_build.CMakeGenerator.DEFAULT
if enableNinja:
  cmake_generator = cmake_build.CMakeGenerator.NINJA

cmake_generated_data = cmake_build.generate_cmake_data(projectName, cmake_generator)

##########################################
########## PRE BUILD ###########
##########################################

##########################################
########## BUILD ###########
##########################################

buildCommandArgs = [
    "-DCMAKE_C_COMPILER=/usr/bin/clang",
    "-DCMAKE_CXX_COMPILER=/usr/bin/clang++",
    "-DPROJECTNAME=" + projectName,
    "-DCMAKE_BUILD_TYPE=" + buildType,
    "-DBUILD_UNIT_TESTS=" + str(buildUnitTests),
    "-DBUILD_INTEGRATION_TESTS=" + str(buildIntegrationTests),
    "-DTOOLS_TO_BUILD=" + str(";".join(toolsToBuild)),
    "-DAPPS_TO_BUILD=" + str(";".join(appsToBuild)),
    "-DENABLE_LOGS=" + to_cmake_bool(enableLogs),
    "-DENABLE_PROFILER=" + to_cmake_bool(enableProfiler),
    "-DENABLE_SANITIZER=" + to_cmake_bool(enableSanitizer),
    "-DENABLE_GPU_DEBUG=" + to_cmake_bool(enableGPUDebug),
    # "-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=mold",
    # "-DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=mold",
]

current_files_list = []
for target_dir in TARGET_DIRS:
  if os.path.exists(target_dir):
    current_files_list.extend(get_current_files(target_dir))

current_files = set(current_files_list)

previous_files = load_previous_files(STATE_FILE)

added = current_files - previous_files
removed = previous_files - current_files

runFullBuild = False

if not previous_files:
    runFullBuild = True
elif added or removed:
    runFullBuild = True

os.makedirs(BuildGlobalData.buildDir, exist_ok=True)
save_current_files(STATE_FILE, current_files)

cmake_build.build_cmake(cwd, ".", BuildGlobalData.buildDir, buildType, None, runFullBuild, cmake_generated_data, buildCommandArgs)

##########################################
########## POST BUILD ###########
##########################################

compileCommandsJson = "compile_commands.json"
shutil.copy(os.path.join(buildTargetDir, compileCommandsJson), compileCommandsJson)