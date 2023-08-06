import sys
import os
import getopt
import platform

cwd = os.path.dirname(os.path.realpath(__file__))
cwd = os.path.join(cwd, "..")
os.chdir(cwd)
print(cwd)
  
buildDir="build"
dependenciesDir="dependencies"
binariesDir="binaries"

profilerDir = os.path.join(dependenciesDir, "easy_profiler-2.1.0")

buildType="Debug"

buildUnitTests=False
buildIntegrationTests=False
enableLogs=False
enableProfiler=False

appsToBuild = []
toolsToBuild = []

argv = []
if(len(sys.argv) > 1):
    argv = sys.argv[1:]

try:
  opts, args = getopt.getopt(argv, "uilchp", ["app=", "tool="])
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

system_name = platform.system()
system_info = str(platform.uname())

print(system_name)
print(system_info)

cmake_generator = ""
systemBuildCommand=""
if system_name == "Linux" or system_name == "Linux2":
    # linux
    cmake_generator = '-G "Unix Makefiles"'
    systemBuildCommand = 'make -j8'
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

if not os.path.isdir(buildDir):
      os.mkdir(buildDir)

##########################################
########## PRE BUILD ###########
##########################################

# easy_profiler GUI
if enableProfiler:
    os.chdir(profilerDir)

    if not os.path.isdir(buildDir):
      os.mkdir(buildDir)

    os.chdir(buildDir)

    buildProfilerCommandArgs = [
        "-DCMAKE_BUILD_TYPE=Release",
    ]

    buildProfilerCommandArgsString =" ".join(buildProfilerCommandArgs)

    buildProfilerCommand = 'cmake {cmake_generator} {buildProfilerCommandArgsString} ..'.format(
    cmake_generator = cmake_generator,
    buildProfilerCommandArgsString = buildProfilerCommandArgsString)
    print(buildProfilerCommand)
    os.system(buildProfilerCommand)
    os.system(systemBuildCommand)

##########################################
########## BUILD ###########
##########################################
os.chdir(os.path.join(cwd, buildDir))

# -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++

buildCommandArgs = [
    "-DCMAKE_BUILD_TYPE=" + str(buildType),
    "-DBUILD_UNIT_TESTS=" + str(buildUnitTests),
    "-DBUILD_INTEGRATION_TESTS=" + str(buildIntegrationTests),
    "-DTOOLS_TO_BUILD=" + "\"" + str(";".join(toolsToBuild) + "\""),
    "-DAPPS_TO_BUILD=" + "\"" + str(";".join(appsToBuild) + "\""),
    "-DENABLE_LOGS=" + str(enableLogs),
    "-DENABLE_PROFILER=" + str(enableProfiler)
]

buildCommandArgsString =" ".join(buildCommandArgs)

buildCommand = 'cmake {cmake_generator} {buildCommandArgsString} ..'.format(
  cmake_generator = cmake_generator,
  buildCommandArgsString = buildCommandArgsString)
print(buildCommand)
os.system(buildCommand)

os.system(systemBuildCommand)

# go back to root folder after build
os.chdir(cwd)

##########################################
########## POST BUILD ###########
##########################################

# easy_profiler: create bin link
if enableProfiler:
    easy_profiler_bin_path_source = os.path.join(cwd, os.path.join(dependenciesDir, "easy_profiler-2.1.0/bin"), "profiler_gui")
    bin_gui_path_destiny = os.path.join(cwd, os.path.join(binariesDir, "profiler_gui"))
    try:
        os.remove(bin_gui_path_destiny)
    except:
       pass   

    os.symlink(easy_profiler_bin_path_source, bin_gui_path_destiny)
