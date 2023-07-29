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
absoluteBinariesDir= os.path.join(cwd, binariesDir)

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

if not os.path.isdir(buildDir):
      os.mkdir(buildDir)

os.chdir(buildDir)

system_name = platform.system()
system_info = str(platform.uname())

print(system_name)
print(system_info)

cmake_generator = ""
if system_name == "Linux" or system_name == "Linux2":
    # linux
    cmake_generator = '-G "Unix Makefiles"'
elif system_name == "Darwin":
    # OS X
    pass
elif system_name == "Windows":
    # Windows...
    cmake_generator = '-G "Visual Studio 17 2022"'
# elif ANDROID:
#     # Windows...
#     pass
# elif IOS:
#     # Windows...
#     pass

# -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++

buildCommandArgs = [
"-DCMAKE_BUILD_TYPE=" + str(buildType),
"-DBINARIES_OUTPUT_DIRECTORY=" + binariesDir,
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
  buildCommandArgsString = buildCommandArgsString,
)
print(buildCommand)
os.system(buildCommand)

if system_name == "Linux" or system_name == "Linux2":
    # linux
    os.system('make -j8')
elif system_name == "Darwin":
    # OS X
    pass
elif system_name == "Windows":
    # Windows...
    # Requires msbuild added to the PATH
    os.system('msbuild forest.sln')
# elif ANDROID:
#     # Windows...
#     pass
# elif IOS:
#     # Windows...
#     pass

# go back to root folder after build
os.chdir(cwd)

#####################
# POST BUILD
#####################

# clean not needed binaries
binaries_to_remove = [
   "example",
   "example64",
   "minigzip",
   "minigzip64"
]

for binary_name in binaries_to_remove:
    os.remove(os.path.join(binariesDir, binary_name))

# easy_profiler: create bin link
if enableProfiler:
    absolute_easy_profiler_bin_path = os.path.join(cwd, os.path.join(buildDir, os.path.join(dependenciesDir, "easy_profiler-2.1.0/bin")))
    os.symlink(os.path.join(absolute_easy_profiler_bin_path, "profiler_gui"), os.path.join(absoluteBinariesDir, "profiler_gui"))
