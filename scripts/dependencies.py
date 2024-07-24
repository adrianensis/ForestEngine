import sys
import os  
import getopt
import shutil
import platform
import distro

import lib.cmake_build as cmake_build
from lib.build_global_data import BuildGlobalData
import lib.extract_files as extract_files
import lib.log as log

cwd = os.getcwd()
print(cwd)

##########################################
########## DATA ###########
##########################################

installSystemDepencencies=False

argv = []
if(len(sys.argv) > 1):
    argv = sys.argv[1:]

try:
  opts, args = getopt.getopt(argv, "s")
except:
  log.log(log.LogLabels.error, "Error parsing options!")
  exit(1)

for opt, arg in opts:
    arg_list = arg.split(",")

    if opt in ['-s']:
      installSystemDepencencies = True

# if os.path.isdir(BuildGlobalData.dependenciesDir):
#     shutil.rmtree(BuildGlobalData.dependenciesDir)

# os.mkdir(BuildGlobalData.dependenciesDir)

##########################################
########## INSTALL SYSTEM DEPENDENCIES ###########
##########################################

if installSystemDepencencies:
    log.log(log.LogLabels.build, "-----------------------------------")
    log.log(log.LogLabels.build, "INSTALLING SYSTEM DEPENDENCIES")
    system_name = platform.system()
    system_info = str(platform.uname())

    log.log(log.LogLabels.build, "System Name: " + system_name)
    log.log(log.LogLabels.build, "System Info: " + system_info)

    if system_name == "Linux" or system_name == "Linux2":
        # linux
        distro_id = distro.id()
        if distro_id == "ubuntu":
            os.system("sudo apt-get -y update")
            os.system("sudo apt-get -y install build-essential wget zlib1g-dev unzip cmake clang")
            os.system("sudo apt-get -y install mesa-common-dev")
            os.system("sudo apt-get -y install libtbb-dev") # needed by GDD in order to use c++ parallel for_each
            os.system("sudo apt-get -y install xorg-dev") # glfw3 dependency
            os.system("sudo apt-get -y install libharfbuzz-dev bzip2") # freetype dependency
            os.system("sudo apt-get -y install ccache") # compilation cache
            os.system("sudo apt-get -y install libdbus-glib-1-dev libcapstone-dev libtbb-dev libxkbcommon0 libwayland-dev wayland-protocols libglvnd0 libglfw3-dev libdbus-1-dev") # tracy dependencies
            os.system("sudo apt-get -y install doxygen graphviz")
        elif distro_id == "manjaro":
            # os.system("sudo pacman -Syy")
            # os.system("sudo pacman -Sy base-devel wget unzip cmake clang doxygen graphviz")
            pass
    elif system_name == "Darwin":
        # OS X
        pass
    elif system_name == "Win32":
        # Windows...
        pass
    # elif ANDROID:
    #     # Windows...
    #     pass
    # elif IOS:
    #     # Windows...
    #     pass

    log.log(log.LogLabels.build, "-----------------------------------")

##########################################
########## EXTRACT ###########
##########################################

log.log(log.LogLabels.build, "-----------------------------------")
log.log(log.LogLabels.build, "EXTRACTING FILES")
# ------------------------------------------------------------------------

# GLFW
extract_files.extract(os.path.join(BuildGlobalData.zipArchivesDir, "glfw-3.3.4.zip"), BuildGlobalData.dependenciesDir)

# ------------------------------------------------------------------------

# glew https://github.com/nigels-com/glew/releases/tag/glew-2.2.0
extract_files.extract(os.path.join(BuildGlobalData.zipArchivesDir, "glew-2.2.0.zip"), BuildGlobalData.dependenciesDir)

# ------------------------------------------------------------------------

# JSON https://github.com/nlohmann/json
extract_files.extract(os.path.join(BuildGlobalData.zipArchivesDir, "json-3.9.1.zip"), BuildGlobalData.dependenciesDir)

# ------------------------------------------------------------------------

# Tracy https://github.com/wolfpld/tracy
extract_files.extract(os.path.join(BuildGlobalData.zipArchivesDir, "tracy-0.11.0.zip"), BuildGlobalData.dependenciesDir)

# ------------------------------------------------------------------------

# stb https://github.com/nothings/stb
extract_files.extract(os.path.join(BuildGlobalData.zipArchivesDir, "stb.zip"), BuildGlobalData.dependenciesDir)

# ------------------------------------------------------------------------

# cgltf https://github.com/jkuhlmann/cgltf
extract_files.extract(os.path.join(BuildGlobalData.zipArchivesDir, "cgltf-1.13.zip"), BuildGlobalData.dependenciesDir)

# ------------------------------------------------------------------------

# freetype https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.gz
extract_files.extract(os.path.join(BuildGlobalData.zipArchivesDir, "freetype-2.10.1.tar.gz"), BuildGlobalData.dependenciesDir)

# ------------------------------------------------------------------------
log.log(log.LogLabels.build, "-----------------------------------")

##########################################
########## BUILD ###########
##########################################

log.log(log.LogLabels.build, "-----------------------------------")
log.log(log.LogLabels.build, "BUILDING DEPENDENCIES")

buildType=BuildGlobalData.buildRelease
buildTargetDir=os.path.join(BuildGlobalData.buildDir, buildType)

tracyProfiler = "tracy-0.11.0/profiler"
tracyProfilerDepencencyDir = os.path.join(BuildGlobalData.dependenciesDir, tracyProfiler)
freetypeDir = "freetype-2.10.1"
freetypeDepencencyDir = os.path.join(BuildGlobalData.dependenciesDir, freetypeDir)
glewDir = "glew-2.2.0/build/cmake"
glewDepencencyDir = os.path.join(BuildGlobalData.dependenciesDir, glewDir)

cmake_generated_data = cmake_build.generate_cmake_data()

# glew
buildCommandArgs = [
    "-DCMAKE_BUILD_TYPE=" + buildType
]

cmake_build.build_cmake(glewDepencencyDir, BuildGlobalData.buildDir, buildType, cmake_generated_data, buildCommandArgs)

# freetype
buildCommandArgs = [
    "-DCMAKE_BUILD_TYPE=" + buildType
]

cmake_build.build_cmake(freetypeDepencencyDir, BuildGlobalData.buildDir, buildType, cmake_generated_data, buildCommandArgs)

# profiler GUI
buildCommandArgs = [
    "-DCMAKE_BUILD_TYPE=" + buildType,
    "-DLEGACY=ON",
    "-DDOWNLOAD_CAPSTONE=OFF",
    "-DDOWNLOAD_GLFW=OFF",
    "-DGLFW_BUILD_X11=ON",
    "-DCMAKE_CXX_FLAGS_DEBUG=" + "-flto=auto",
    "-DCMAKE_CXX_FLAGS_RELEASE=" + "-flto=auto"
]

cmake_build.build_cmake(tracyProfilerDepencencyDir, BuildGlobalData.buildDir, buildType, cmake_generated_data, buildCommandArgs)

log.log(log.LogLabels.build, "-----------------------------------")

##########################################
########## POST BUILD ###########
##########################################

if not os.path.isdir(os.path.join(BuildGlobalData.dependenciesDir, BuildGlobalData.binariesDir)):
    os.mkdir(os.path.join(BuildGlobalData.dependenciesDir, BuildGlobalData.binariesDir))
bin_dependencies_path_destiny = os.path.join(cwd, os.path.join(os.path.join(BuildGlobalData.dependenciesDir, BuildGlobalData.binariesDir), buildType))
if not os.path.isdir(bin_dependencies_path_destiny):
    os.mkdir(bin_dependencies_path_destiny)

log.log(log.LogLabels.build, "-----------------------------------")
log.log(log.LogLabels.build, "INSTALLING DEPENDENCIES BINARIES")
# profiler: create gui executable link
bin_gui_path_destiny = os.path.join(bin_dependencies_path_destiny, "tracy-profiler")
tracy_profiler_bin_path_source = os.path.join(cwd, os.path.join(tracyProfilerDepencencyDir, buildTargetDir), "tracy-profiler")
os.remove(bin_gui_path_destiny)
os.symlink(tracy_profiler_bin_path_source, bin_gui_path_destiny)
log.log(log.LogLabels.build, "tracy profiler gui: " + bin_gui_path_destiny)
log.log(log.LogLabels.build, "-----------------------------------")