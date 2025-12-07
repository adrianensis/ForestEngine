import os
import platform
import lib.log as log

from enum import Enum, auto

##########################################
########## DATA ###########
##########################################

class CMakeGenerator(Enum):
    DEFAULT = auto()
    NINJA = auto()

class CMakeGeneratedData:
    def __init__(self):
        self.cmake_generator = ""
        self.systemBuildCommand = ""
        self.coresUsed = 0
        self.systemName = ""
        self.projectName = ""

##########################################
########## FUNCTIONS ###########
##########################################

# generate CMake data
def generate_cmake_data(projectName, cmakeGenerator=CMakeGenerator.DEFAULT):
    log.log(log.LogLabels.build, "-----------------------------------")
    log.log(log.LogLabels.build, "GENERATE CMAKE DATA")
    data = CMakeGeneratedData()
    system_name = platform.system()
    system_info = str(platform.uname())
    max_cpu_cores = os.cpu_count()
    system_reserved_cores = 4
    compilation_cores = max(max_cpu_cores - system_reserved_cores, 2)

    data.coresUsed = compilation_cores
    data.systemName = system_name
    data.projectName = projectName

    log.log(log.LogLabels.build, "System Info")
    log.log(log.LogLabels.build, system_name)
    log.log(log.LogLabels.build, data.systemName)
    log.log(log.LogLabels.build, "max_cpu_cores: " + str(max_cpu_cores))
    log.log(log.LogLabels.build, "system_reserved_cores: " + str(system_reserved_cores))
    log.log(log.LogLabels.build, "compilation_cores: " + str(data.coresUsed))

    if system_name == "Linux" or system_name == "Linux2":
        # linux
        data.cmake_generator = '-G "Unix Makefiles"'
        if cmakeGenerator == CMakeGenerator.NINJA:
            data.cmake_generator = '-G "Ninja"'

    elif system_name == "Darwin":
        # OS X
        pass
    elif system_name == "Windows":
        # Windows...
        data.cmake_generator = '-G "Visual Studio 17 2022"'
    # elif ANDROID:
    #     # Windows...
    #     pass
    # elif IOS:
    #     # Windows...
    #     pass

    log.log(log.LogLabels.build, "CMake data generated:")
    log.log(log.LogLabels.build, data.systemName)
    log.log(log.LogLabels.build, data.cmake_generator)
    log.log(log.LogLabels.build, data.systemBuildCommand)
    log.log(log.LogLabels.build, str(data.coresUsed))
    log.log(log.LogLabels.build, "-----------------------------------")

    return data

# build a CMake project
def build_cmake(projectDir, cmakeListFolder, buildDir, buildType, cmake_generated_data: CMakeGeneratedData, buildCommandArgs):
    log.log(log.LogLabels.build, "-----------------------------------")
    log.log(log.LogLabels.build, "BUILD CMAKE")
    log.log(log.LogLabels.build, "Project Dir: " + projectDir)
    log.log(log.LogLabels.build, "CMakeLists Folder: " + cmakeListFolder)
    log.log(log.LogLabels.build, "Build Dir: " + buildDir)
    log.log(log.LogLabels.build, "Build Type: " + buildType)
    buildCommandArgsString =" ".join(buildCommandArgs)
    log.log(log.LogLabels.build, "Build Command Args: " + buildCommandArgsString)
    cwd = os.getcwd()
    log.log(log.LogLabels.build, "Current Dir: " + cwd)
    os.chdir(projectDir)

    buildTargetDir=os.path.join(buildDir, buildType)
    if not os.path.isdir(buildDir):
        os.mkdir(buildDir)
    if not os.path.isdir(buildTargetDir):
        os.mkdir(buildTargetDir)

    configCommand = f'cmake -S{cmakeListFolder} -B{buildTargetDir} {cmake_generated_data.cmake_generator} {buildCommandArgsString}'
    buildCommand = f'cmake --build {buildTargetDir} --config {buildType} --parallel {cmake_generated_data.coresUsed}'
    log.log(log.LogLabels.build, "Build Command: " + configCommand)
    log.log(log.LogLabels.build, "Build Command: " + buildCommand)
    log.log(log.LogLabels.build, "Executing Config Command")
    os.system(configCommand)
    log.log(log.LogLabels.build, "Executing Build Command")
    os.system(buildCommand)

    # go back
    log.log(log.LogLabels.build, "Going back to: " + cwd)
    os.chdir(cwd)
    log.log(log.LogLabels.build, "-----------------------------------")
