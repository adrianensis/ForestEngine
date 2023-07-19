import os  
import shutil
import filecmp
import pathlib

from generateClassList import *
from overwriteFile import *

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
    foldersToExplore = ["tools", "apps"]

    for folder in foldersToExplore:
        current_path = os.path.join(projectDir, folder)
        directory_contents = os.listdir(current_path)
        for item in directory_contents:
            if os.path.isdir(os.path.join(current_path, item)):
                foldersMap[item] = os.path.join(folder, item)

    for keyFolder, folder in foldersMap.items():

        class_map = getClassList(projectDir, [folder])

        class_manager_generated = os.path.join(folder, "generated")
        class_manager_generated_class_header = class_manager_generated + ".hpp"
        class_manager_generated_class_header_tmp = class_manager_generated + "_tmp" + ".hpp"
        class_manager_generated_class_source = class_manager_generated + ".cpp"
        class_manager_generated_class_source_tmp = class_manager_generated + "_tmp" + ".cpp"

        register_macros = []
        for _, class_def in class_map.items():
            register_macros.append("REGISTER_CLASS_BY_NAME("+class_def.class_name+")\n")

        includes = []
        includes.append("#include \"code/Core/Module.hpp\"\n")
        for _, class_def in class_map.items():
            new_include = "#include \""+class_def.include+"\"\n"
            if not new_include in includes:
                includes.append(new_include)

        # if core code, keep data 
        if keyFolder == keyFolder_code:
            register_macros_code = register_macros
            includes_code = includes

        # only generate code for tools/apps, nor for core code
        if keyFolder != keyFolder_code:

            pathlib.Path(folder).mkdir(parents=True, exist_ok=True)

            classManagerName = "ModuleClassesRegister"

            with open(class_manager_generated_class_source_tmp, "w") as file:

                # code dependencies
                for line in includes_code:
                    file.write(line)
                for line in includes:
                    file.write(line)

                file.write("#include \"generated.hpp\"\n")
                
                file.write(classManagerName + "::" +classManagerName +"()\n")
                file.write("{\n")

                # code dependencies
                for line in register_macros_code:
                    file.write(line)
                for line in register_macros:
                    file.write(line)

                file.write("}\n")
                file.write("\n")

            with open(class_manager_generated_class_header_tmp, "w") as file:

                file.write("#pragma once\n")
                
                file.write("class " + classManagerName +"\n")
                file.write("{\n")
                file.write("public:\n")
                file.write(classManagerName +"();\n")
                file.write("};\n")
                file.write("\n")

            overwriteFileIfChanges(class_manager_generated_class_header_tmp, class_manager_generated_class_header)
            overwriteFileIfChanges(class_manager_generated_class_source_tmp, class_manager_generated_class_source)

            os.remove(class_manager_generated_class_header_tmp)
            os.remove(class_manager_generated_class_source_tmp)
    
    os.chdir(projectDir)