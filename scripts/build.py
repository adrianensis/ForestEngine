import sys
import os
import getopt
import platform

cwd = os.path.dirname(os.path.realpath(__file__))
cwd = os.path.join(cwd, "..")
os.chdir(cwd)
print(cwd)

##########################################
########## DATA ###########
##########################################

buildDir="build"
dependenciesDir="dependencies"
binariesDir="binaries"

profilerDir = "easy_profiler-2.1.0"
profilerDepencencyDir = os.path.join(dependenciesDir, profilerDir)
freetypeDir = "freetype-2.10.1"
freetypeDepencencyDir = os.path.join(dependenciesDir, freetypeDir)

buildType="Debug"

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
  opts, args = getopt.getopt(argv, "uilcpr", ["app=", "tool=", "asan", "gpuDbg"])
except:
  print("Error parsing options!")
  exit(1)

for opt, arg in opts:
    arg_list = arg.split(",")

    if opt in ['-c']:
      os.system('./scripts/clean.sh')
    elif opt in ['-r']:
      buildType="Release"
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
    elif opt in ['--asan']:
      enableAddressSanitizer=True
    elif opt in ['--gpuDbg']:
      enableGPUDebug=True

buildTargetDir=os.path.join(buildDir, buildType)

system_name = platform.system()
system_info = str(platform.uname())
max_cpu_cores = os.cpu_count()
system_reserved_cores = 4
compilation_cores = max_cpu_cores - system_reserved_cores

print(system_name)
print(system_info)
print("max_cpu_cores: " + str(max_cpu_cores))
print("system_reserved_cores: " + str(system_reserved_cores))
print("compilation_cores: " + str(compilation_cores))

cmake_generator = ""
systemBuildCommand=""
if system_name == "Linux" or system_name == "Linux2":
    # linux
    cmake_generator = '-G "Unix Makefiles"'
    systemBuildCommand = 'make -j{compilation_cores}'.format(compilation_cores = compilation_cores)
elif system_name == "Darwin":
    # OS X
    pass
elif system_name == "Windows":
    # Windows...
    cmake_generator = '-G "Visual Studio 17 2022"'
    # Requires msbuild added to the PATH
    systemBuildCommand = 'msbuild forest.sln'
# elif ANDROID:
#     # Windows...
#     pass
# elif IOS:
#     # Windows...
#     pass

##########################################
########## FUNCTIONS ###########
##########################################

# build a CMake project
def build_cmake(projectDir, buildCommandArgs):
    os.chdir(projectDir)

    if not os.path.isdir(buildDir):
        os.mkdir(buildDir)
    if not os.path.isdir(buildTargetDir):
        os.mkdir(buildTargetDir)

    os.chdir(buildTargetDir)

    buildCommandArgsString =" ".join(buildCommandArgs)

    buildCommand = 'cmake {cmake_generator} {buildCommandArgsString} ../..'.format(
    cmake_generator = cmake_generator,
    buildCommandArgsString = buildCommandArgsString)
    print(buildCommand)
    os.system(buildCommand)
    os.system(systemBuildCommand)
    # go back
    os.chdir(cwd)

##########################################
########## PRE BUILD ###########
##########################################

# easy_profiler GUI
if enableProfiler:
    buildCommandArgs = [
        "-DCMAKE_BUILD_TYPE=" + buildType
    ]

    build_cmake(profilerDepencencyDir, buildCommandArgs)


# freetype
buildCommandArgs = [
    "-DCMAKE_BUILD_TYPE=" + buildType
]

build_cmake(freetypeDepencencyDir, buildCommandArgs)

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
]

build_cmake(cwd, buildCommandArgs)

##########################################
########## POST BUILD ###########
##########################################

# easy_profiler: create bin link
if enableProfiler:
    easy_profiler_bin_path_source = os.path.join(cwd, os.path.join(os.path.join(profilerDepencencyDir, buildTargetDir), "bin"), "profiler_gui")
    bin_gui_path_destiny = os.path.join(cwd, os.path.join(binariesDir, os.path.join(buildType, "profiler_gui")))
    try:
        os.remove(bin_gui_path_destiny)
    except:
       pass   

    os.symlink(easy_profiler_bin_path_source, bin_gui_path_destiny)
