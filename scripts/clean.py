import os  
import shutil
from lib.build_global_data import BuildGlobalData

cwd = os.path.dirname(os.path.realpath(__file__))
cwd = cwd + "/.."
os.chdir(cwd)
#print(cwd)

def remove_folder(folder):
    if os.path.isdir(folder):
        shutil.rmtree(folder)

remove_folder(BuildGlobalData.binariesDir)
remove_folder(BuildGlobalData.buildDir)
remove_folder(BuildGlobalData.outputDir)