import os  
import shutil
import filecmp
import pathlib

from generateClassList import *

def searchGenerateHeadersMacro(text):
    return re.search(r'^\s*#ifndef\s*GENERATE_HEADERS\s*', text)
def searchEndIfMacro(text):
    return re.search(r'^\s*#endif\s*', text)

# HOW TO USE:
# # ifndef GENERATE_HEADERS
# ...
# header code
# ...
# # endif

def generateHeaders(projectDir, generated_code_dirname):

    # folders to find classes
    foldersMap = {}

    #add default code folder
    keyFolder_code = "code"
    foldersMap[keyFolder_code] = "code"

    # add folders to foldersMap
    foldersToExplore = ["tools", "games"]

    for folder in foldersToExplore:
        current_path = os.path.join(projectDir, folder)
        directory_contents = os.listdir(current_path)
        for item in directory_contents:
            if os.path.isdir(os.path.join(current_path, item)):
                foldersMap[item] = os.path.join(folder, item)

    for keyFolder, folder in foldersMap.items():
        path = os.path.join(projectDir, folder)
        for root,d_names,f_names in os.walk(path):
            for f in f_names:
                file_name = os.path.join(root, f)
                if ".cpp" in file_name:
                    with open(file_name, 'r') as file:
                        lines = file.readlines()
                        header_lines = []
                        generateHeadersMacroFound = False
                        for line in lines:
                            match = searchGenerateHeadersMacro(line)
                            if match:
                                generateHeadersMacroFound = True
                                continue #skip this line
                            
                            if generateHeadersMacroFound:
                                match = searchEndIfMacro(line)
                                if match:
                                    generateHeadersMacroFound = False
                                if generateHeadersMacroFound:    
                                    header_lines += line

                        if len(header_lines) > 0:
                            # remove extension
                            header_name_base_path = os.path.splitext(file_name)[0]
                            # remove root path
                            header_name_base_path = header_name_base_path.replace(projectDir+"/", "")
                            header_name_base_path = os.path.join(generated_code_dirname, header_name_base_path)

                            dirname = os.path.dirname(header_name_base_path)
                            pathlib.Path(dirname).mkdir(parents=True, exist_ok=True)
                            
                            header_name = header_name_base_path + ".hpp"
                            header_name_tmp = header_name_base_path + "_tmp" + ".hpp"

                            with open(header_name_tmp, 'w') as header_file_tmp:
                                header_file_tmp.write("#pragma once\n")

                                for line in header_lines:
                                    header_file_tmp.write(line)

                            overwrite = True
                            if os.path.isfile(header_name) and os.path.isfile(header_name_tmp):
                                if filecmp.cmp(header_name, header_name_tmp, shallow=False):
                                    print(header_name + " have no changes!")
                                    overwrite = False

                            if overwrite:
                                print(header_name + " have changes!")
                                shutil.copyfile(header_name_tmp, header_name)
                            
                            os.remove(header_name_tmp)