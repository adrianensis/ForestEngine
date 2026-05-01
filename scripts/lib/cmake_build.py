import os
import subprocess
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
        self.coresUsed = 0
        self.systemName = ""
        self.projectName = ""

##########################################
########## FUNCTIONS ###########
##########################################

def get_physical_cores():
    system = platform.system()
    
    try:
        if system == "Windows":
            cmd = "wmic cpu get NumberOfCores"
            output = subprocess.check_output(cmd, shell=True).decode().split()
            return sum(int(x) for x in output if x.isdigit())

        elif system == "Darwin":
            cmd = ["sysctl", "-n", "hw.physicalcpu"]
            return int(subprocess.check_output(cmd).decode().strip())

        elif system == "Linux":
            cmd = "grep -P '^core id' /proc/cpuinfo | sort -u | wc -l"
            output = subprocess.check_output(cmd, shell=True).decode().strip()
            return int(output)
            
    except Exception:
        return 0

# generate CMake data
def generate_cmake_data(projectName, cmakeGenerator=CMakeGenerator.DEFAULT):
    log.log(log.LogLabels.build, "-----------------------------------")
    log.log(log.LogLabels.build, "GENERATE CMAKE DATA")
    data = CMakeGeneratedData()
    system_name = platform.system()
    system_info = str(platform.uname())
    logical_cpu_cores = os.cpu_count()
    physical_cpu_cores = get_physical_cores()
    extra_logical_cores = 2
    compilation_cores = physical_cpu_cores + extra_logical_cores

    # fall back to logical cores
    system_reserved_logical_cores = 2
    if physical_cpu_cores == 0:
        log.log(log.LogLabels.build, "Couldn't find Physical cores count, fallback to Logical cores count.")
        compilation_cores = logical_cpu_cores - system_reserved_logical_cores

    data.coresUsed = compilation_cores
    data.systemName = system_name
    data.projectName = projectName

    log.log(log.LogLabels.build, "System Info")
    log.log(log.LogLabels.build, system_name)
    log.log(log.LogLabels.build, "physical_cpu_cores: " + str(physical_cpu_cores))
    log.log(log.LogLabels.build, "extra_logical_cores (physical only): " + str(extra_logical_cores))
    log.log(log.LogLabels.build, "logical_cpu_cores: " + str(logical_cpu_cores))
    log.log(log.LogLabels.build, "system_reserved_logical_cores (logical only): " + str(system_reserved_logical_cores))
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
    log.log(log.LogLabels.build, "cores used: " + str(data.coresUsed))
    log.log(log.LogLabels.build, "-----------------------------------")

    return data

# build a CMake project
def build_cmake(projectDir, cmakeListFolder, buildDir, buildType, target, runFullBuild, install, cmake_generated_data: CMakeGeneratedData, buildCommandArgs):
    log.log(log.LogLabels.build, "-----------------------------------")
    log.log(log.LogLabels.build, "BUILD CMAKE")
    log.log(log.LogLabels.build, "Project Dir: " + projectDir)
    log.log(log.LogLabels.build, "CMakeLists Folder: " + cmakeListFolder)
    log.log(log.LogLabels.build, "Build Dir: " + buildDir)
    log.log(log.LogLabels.build, "Build Type: " + buildType)
    log.log(log.LogLabels.build, "Full Build (Config + Build): " + str(runFullBuild))
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

    targetStr = ''
    if target:
        targetStr = f'--target {target}'

    buildCommand = f'cmake --build {buildTargetDir} --config {buildType} {targetStr} --parallel {cmake_generated_data.coresUsed}'
	
    log.log(log.LogLabels.build, "Config Command: " + configCommand)
    log.log(log.LogLabels.build, "Build Command: " + buildCommand)
    if runFullBuild:
        log.log(log.LogLabels.build, "Executing Config Command")
        os.system(configCommand)
    log.log(log.LogLabels.build, "Executing Build Command")
    os.system(buildCommand)

    if install == True:
        installCommand = f"cmake --install {buildTargetDir}"
        log.log(log.LogLabels.build, "Install Command: " + installCommand)
        log.log(log.LogLabels.build, "Executing Install Command")
        os.system(installCommand)

    # go back
    log.log(log.LogLabels.build, "Going back to: " + cwd)
    os.chdir(cwd)
    log.log(log.LogLabels.build, "-----------------------------------")
