from genericpath import isdir
import sys
import os  
import getopt
import shutil
import platform
import distro
import time

import lib.cmake_build as cmake_build
from lib.build_global_data import BuildGlobalData
import lib.extract_files as extract_files
import lib.download_file as download_file
import lib.log as log

cwd = os.getcwd()
print(cwd)

##########################################
########## FUNCTIONS ###########
##########################################

def download_dependency(url, filename, extraDependencyFolder=""):
    download_file.download_file(url, BuildGlobalData.dependenciesDownloadDir, filename)
    extract_files.extract_files(os.path.join(BuildGlobalData.dependenciesDownloadDir, filename), os.path.join(BuildGlobalData.dependenciesDir, extraDependencyFolder))

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

if not os.path.isdir(BuildGlobalData.dependenciesDir):
    os.mkdir(BuildGlobalData.dependenciesDir)
if not os.path.isdir(BuildGlobalData.dependenciesDownloadDir):
    os.mkdir(BuildGlobalData.dependenciesDownloadDir)

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
########## DOWNLOAD ###########
##########################################

log.log(log.LogLabels.build, "-----------------------------------")
log.log(log.LogLabels.build, "EXTRACTING FILES")
download_dependency("https://github.com/glfw/glfw/archive/refs/tags/3.4.zip", "glfw-3.4.zip")
download_dependency("https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip", "glew-2.2.0.zip")
download_dependency("https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.zip", "json-3.11.3.zip")
download_dependency("https://github.com/wolfpld/tracy/archive/refs/tags/v0.11.0.zip", "tracy-0.11.0.zip")
download_dependency("https://github.com/nothings/stb/archive/refs/heads/master.zip", "stb.zip")
download_dependency("https://github.com/jkuhlmann/cgltf/archive/refs/tags/v1.14.zip", "cgltf-1.14.zip")
download_dependency("https://download.savannah.gnu.org/releases/freetype/freetype-2.13.2.tar.xz", "freetype-2.13.2.tar.xz")
download_dependency("https://sdk.lunarg.com/sdk/download/1.3.290.0/linux/vulkansdk-linux-x86_64-1.3.290.0.tar.xz", "vulkansdk-linux-x86_64-1.3.290.0.tar.xz", "vulkan")
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
freetypeDir = "freetype-2.13.2"
freetypeDepencencyDir = os.path.join(BuildGlobalData.dependenciesDir, freetypeDir)
glewDir = "glew-2.2.0"
glewDepencencyDir = os.path.join(BuildGlobalData.dependenciesDir, glewDir)

cmake_generated_data = cmake_build.generate_cmake_data()

# freetype
buildCommandArgs = [
    "-DCMAKE_BUILD_TYPE=" + buildType
]

cmake_build.build_cmake(freetypeDepencencyDir, ".", BuildGlobalData.buildDir, buildType, cmake_generated_data, buildCommandArgs)

# profiler GUI
buildCommandArgs = [
    "-DCMAKE_BUILD_TYPE=" + buildType,
    "-DLEGACY=ON",
    "-DDOWNLOAD_CAPSTONE=OFF",
    "-DDOWNLOAD_GLFW=OFF",
    "-DGLFW_BUILD_X11=ON",
    # "-DCMAKE_CXX_FLAGS_RELEASE=" + "-flto=auto"
]

cmake_build.build_cmake(tracyProfilerDepencencyDir, ".", BuildGlobalData.buildDir, buildType, cmake_generated_data, buildCommandArgs)

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
if os.path.isfile(bin_gui_path_destiny):
    os.remove(bin_gui_path_destiny)
os.symlink(tracy_profiler_bin_path_source, bin_gui_path_destiny)
log.log(log.LogLabels.build, "tracy profiler gui: " + bin_gui_path_destiny)
log.log(log.LogLabels.build, "-----------------------------------")