import os  
import shutil
import filecmp
import pathlib

from generateClassList import *

def generateClassesRegister(projectDir, generated_code_dirname):

    os.chdir(os.path.join(projectDir,generated_code_dirname))

    # folders to find classes
    foldersMap = {}

    #add default code folder
    keyFolder_code = "code"
    foldersMap[keyFolder_code] = "code"
    register_macros_code = []
    includes_code = []

    # add folders to foldersMap
    foldersToExplore = ["tools", "games"]

    for folder in foldersToExplore:
        current_path = os.path.join(projectDir, folder)
        directory_contents = os.listdir(current_path)
        for item in directory_contents:
            if os.path.isdir(os.path.join(current_path, item)):
                foldersMap[item] = os.path.join(folder, item)

    for keyFolder, folder in foldersMap.items():

        class_map = getClassList(projectDir, [folder])

        class_manager_generated = os.path.join(folder, "generated")
        class_manager_generated_class = class_manager_generated + ".hpp"
        class_manager_generated_class_tmp = class_manager_generated + "_tmp" + ".hpp"

        register_macros = []
        for _, class_def in class_map.items():
            register_macros += "REGISTER_CLASS_BY_NAME("+class_def.class_name+")\n"

        includes = []
        for _, class_def in class_map.items():
            includes += "#include \""+class_def.include+"\"\n"

        # if core code, keep data 
        if keyFolder == keyFolder_code:
            register_macros_code = register_macros
            includes_code = includes

        # only generate code for tools/games, nor for core code
        if keyFolder != keyFolder_code:

            pathlib.Path(folder).mkdir(parents=True, exist_ok=True)

            with open(class_manager_generated_class_tmp, "w") as file:

                classManagerName = "ModuleClassesRegister"

                file.write("#pragma once\n")
                
                # code dependencies
                for line in includes_code:
                    file.write(line)
                for line in includes:
                    file.write(line)

                file.write("class " + classManagerName +"\n")
                file.write("{\n")
                file.write("public:\n")
                file.write(classManagerName +"()\n")
                file.write("{\n")

                # code dependencies
                for line in register_macros_code:
                    file.write(line)
                for line in register_macros:
                    file.write(line)

                file.write("}\n")
                file.write("};\n")
                file.write("\n")

            overwrite = True
            if os.path.isfile(class_manager_generated_class) and os.path.isfile(class_manager_generated_class_tmp):
                if filecmp.cmp(class_manager_generated_class, class_manager_generated_class_tmp, shallow=False):
                    print(class_manager_generated + " have no changes!")
                    overwrite = False

            if overwrite:
                print(class_manager_generated + " have changes!")
                if keyFolder != keyFolder_code:
                    shutil.copyfile(class_manager_generated_class_tmp, class_manager_generated_class)

            os.remove(class_manager_generated_class_tmp)
            
    
    os.chdir(projectDir)