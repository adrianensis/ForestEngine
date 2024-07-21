import sys
import os  
import getopt
import shutil
import platform
import distro

from zipfile import ZipFile

import tarfile

if not platform.python_version().startswith('3'):
    print("Please use python3 to run this script")
    exit()

def extract(file_path, destinty_path):
    print("Extracting " + file_path)
    if file_path.endswith("zip"):
        with ZipFile(file_path) as zipObj:
            zipObj.extractall(destinty_path)

    if file_path.endswith("tar.xz"):
        with tarfile.open(name=file_path, mode='r:xz') as tarObj:
            tarObj.extractall(destinty_path)

    if file_path.endswith("tar.gz"):
        with tarfile.open(name=file_path, mode='r:gz') as tarObj:
            tarObj.extractall(destinty_path)

cwd = os.path.dirname(os.path.realpath(__file__))
cwd = cwd + "/.."
os.chdir(cwd)
#print(cwd)

destiny="dependencies"
zipArchivesDir="scripts/dependencies-zip"

installSystemDepencencies=False

argv = []
if(len(sys.argv) > 1):
    argv = sys.argv[1:]

try:
  opts, args = getopt.getopt(argv, "s")
except:
  print("Error parsing options!")
  exit(1)

for opt, arg in opts:
    arg_list = arg.split(",")

    if opt in ['-s']:
      installSystemDepencencies = True

if os.path.isdir(destiny):
    shutil.rmtree(destiny)

os.mkdir(destiny)

if installSystemDepencencies:
    system_name = platform.system()
    system_info = str(platform.uname())

    print(system_name)
    print(system_info)

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

# ------------------------------------------------------------------------

# GLFW
extract(os.path.join(zipArchivesDir, "glfw-3.3.4.zip"), destiny)

# ------------------------------------------------------------------------

# glew https://github.com/nigels-com/glew/releases/tag/glew-2.2.0
extract(os.path.join(zipArchivesDir, "glew-2.2.0.zip"), destiny)

# ------------------------------------------------------------------------

# JSON https://github.com/nlohmann/json
extract(os.path.join(zipArchivesDir, "json-3.9.1.zip"), destiny)

# ------------------------------------------------------------------------

# Tracy https://github.com/wolfpld/tracy
extract(os.path.join(zipArchivesDir, "tracy-0.11.0.zip"), destiny)

# ------------------------------------------------------------------------

# stb https://github.com/nothings/stb
extract(os.path.join(zipArchivesDir, "stb.zip"), destiny)

# ------------------------------------------------------------------------

# cgltf https://github.com/jkuhlmann/cgltf
extract(os.path.join(zipArchivesDir, "cgltf-1.13.zip"), destiny)

# ------------------------------------------------------------------------

# freetype https://download.savannah.gnu.org/releases/freetype/freetype-2.10.1.tar.gz
extract(os.path.join(zipArchivesDir, "freetype-2.10.1.tar.gz"), destiny)

# ------------------------------------------------------------------------