import os  
import re
import shutil
import filecmp

from generateClassList import *

cwd = os.path.dirname(os.path.realpath(__file__))
cwd = cwd + "/.."
os.chdir(cwd)
#print(cwd)

# folders to find classes
folders = ["code", "tools", "games"]

generated_code_dirname = "generated-code/final"
generated_code_dirname_tmp = "generated-code/tmp/final"

if not os.path.isdir(generated_code_dirname):
    os.makedirs(generated_code_dirname)

if os.path.isdir(generated_code_dirname_tmp):
    shutil.rmtree(generated_code_dirname_tmp)
os.makedirs(generated_code_dirname_tmp)

MACRO_FUNCTION = "CPP"
MACRO_PLAIN_TEXT = "CPP_INCLUDE"
MACRO_IGNORE = "CPP_IGNORE"
MACRO_GENERATE_CPP = "CPP_GENERATE"
MACRO_ACCESS_MODIFIER = "(public|private|protected)\:"

cpp_plain_text_found = False
cpp_function_found = False
braces_count = 0
body_open = False
waiting_for_body = True
function_data = None
current_access_modifier = "private"
match_class = None
ignore_file = False

header_lines = []
source_lines = []

class FunctionData:
    access_modifier = ""
    declaration_class = ""
    previous = "" # everything that goes previous to the function declaration, like tamplates
    pre_return_type = "" # everything that goes previous to return type, like macros
    return_type = ""
    name = ""
    params = ""
    body = ""
    
    def print(self):
        print(self.previous)
        print(self.pre_return_type + " " + self.return_type + " " + self.name + self.params)
        print(self.body)

    def addBodyLine(self, newLine):
        self.body += newLine;

    def addPreviousLine(self, newLine):
        self.previous += newLine;

    def getAccessModifier(self):
        if self.access_modifier:
            return self.access_modifier + ": "
        else:
            return ""

    def getDeclaration(self):
        return self.getAccessModifier().strip() + self.previous.strip() + " " + self.pre_return_type.strip() + " " + self.return_type.strip() + " " + self.name.strip() + self.params.strip() + ";\n"

    def getImplementation(self):
        filteredParams = self.params.replace('override', '')

        default_parameters_list = re.findall(r'\s*(=\s*[\w\d_<>:]+)', filteredParams)
        for default_param in default_parameters_list:
            if default_param:
                filteredParams = filteredParams.replace(default_param, '')

        # filteredParams = filteredParams.replace('virtual', '')
        # filteredParams = filteredParams.replace('static', '')

        filteredPreReturnType = self.pre_return_type.replace('static', '')

        return self.previous + "\n" + filteredPreReturnType + " " + self.return_type + " " + self.declaration_class + "::" + self.name + filteredParams + self.body

def searchClassGenerateCPP(line):
    return re.search(r'^\s*GENERATE_CPP\s*\(\s*(\w+)\s*\)\s*', line)

def searchAccessModifier(line):
    match_access_modifier = re.search(r'\s*'+MACRO_ACCESS_MODIFIER, line)
    if match_access_modifier:
        return match_access_modifier.group(1)
    else:
        return current_access_modifier

def countBraces(line):
    global braces_count
    global body_open
    global waiting_for_body
    for c in line:
        if c == '{':
            if braces_count == 0:
                body_open = True
                waiting_for_body = False
            braces_count += 1
        elif c == '}':
            braces_count -= 1
            if braces_count == 0:
                body_open = False

def process_line(line):
    global cpp_function_found
    global cpp_plain_text_found
    global body_open
    global waiting_for_body
    global function_data
    global current_access_modifier
    global match_class
    global header_lines
    global source_lines

    match_pragma_once = re.search(r'#pragma\s+once', line)
    if match_pragma_once:
        # ignore pragma once line
        return

    current_access_modifier = searchAccessModifier(line)

    # detect class
    current_match_class = searchClassDefinition(line)

    if not current_match_class:
        current_match_class = searchClassGenerateCPP(line)

    if current_match_class:
        match_class = current_match_class

    # detect CPP macro
    if not cpp_plain_text_found:
        match_cpp = re.search(r'#\s*ifdef\s+'+MACRO_PLAIN_TEXT, line)
        if match_cpp:
            cpp_plain_text_found = True
            
            # ignore first CPP_... line
            return

        elif not cpp_function_found:
            if not MACRO_GENERATE_CPP in line:
                match_cpp = re.search(r''+MACRO_FUNCTION, line)
                if match_cpp:
                    cpp_function_found = True
                    waiting_for_body = True

                    # reset function_data
                    function_data = FunctionData()
    
    # process each case
    if cpp_plain_text_found:
        # ignore last CPP_... line
        match_cpp = re.search(r'#\s*endif', line)
        if match_cpp:
            cpp_plain_text_found = False
            source_lines += "\n"
        else:
            source_lines += line

    elif cpp_function_found:
        
        countBraces(line)

        #print(line)
        if waiting_for_body:
            match_function = re.search(r'([\w\d_\s]*)\s+([\w\d_\<\s\>&\*:]+)\s+([\w\d_]+)(\(.*)', line)
            if match_function:
                if match_class:
                    function_data.declaration_class = match_class.group(1)

                function_data.access_modifier = current_access_modifier
                function_data.pre_return_type = match_function.group(1)
                function_data.return_type = match_function.group(2)
                function_data.name = match_function.group(3)
                function_data.params = match_function.group(4)
                #function_data.print()

                header_lines += function_data.getDeclaration()
            else:
                match_cpp = re.search(r''+MACRO_FUNCTION, line)
                if not match_cpp:
                    function_data.addPreviousLine(line)

        if body_open:
            # save line
            function_data.addBodyLine(line)
        elif not waiting_for_body:
            cpp_function_found = False
            # print("function closed")
            function_data.addBodyLine(line)
            #function_data.print()

            source_lines += function_data.getImplementation()
    else:
        header_lines += line

def should_ignore(lines):
    global ignore_file
    ignore_file = True
    # detect IGNORE macro
    for line in lines:
        match_macro_generate = re.search(r''+MACRO_GENERATE_CPP, line)
        match_macro_ignore = re.search(r''+MACRO_IGNORE, line)
        if match_macro_ignore:
            ignore_file = True
            break
        elif match_macro_generate:
            ignore_file = False

def same_file(fileA, fileB):
    return filecmp.cmp(fileA, fileB, shallow=True)

def are_different_files(fileA, fileB):
    return not same_file(fileA, fileB)

def write_file(new_file_path, lines):
    with open(new_file_path, "w") as new_file:
        for line in lines:
            new_file.write(line)
            
def write_file_if_different(original_file_path, new_file_path, lines):
    should_write_file = False
    
    if os.path.isfile(new_file_path):
        should_write_file = not filecmp.cmp(original_file_path, new_file_path, shallow=True)
    else:
        should_write_file = True 

    if should_write_file:
        write_file(new_file_path, lines)

def write_file_if_new_changes(folder, tmp_folder, relative_file_path, lines):
    new_file_path = os.path.join(folder,relative_file_path)
    new_file_path_tmp = os.path.join(tmp_folder,relative_file_path)
    write_file(new_file_path_tmp, lines)
    write_file_if_different(new_file_path_tmp, new_file_path, lines)

for folder in folders:
    path = cwd+"/"+folder
    for root,d_names,f_names in os.walk(path):

        file_folder = root.split("../")[1]

        generated_final_folder = os.path.join(generated_code_dirname,file_folder)
        if not os.path.isdir(generated_final_folder):
            os.makedirs(generated_final_folder)

        generated_final_folder_tmp = os.path.join(generated_code_dirname_tmp,file_folder)
        if os.path.isdir(generated_final_folder_tmp):
            shutil.rmtree(generated_final_folder_tmp)
        os.makedirs(generated_final_folder_tmp)

        for f in f_names:
            
            ignore_file = False

            file_name = os.path.join(root, f)
            if f == "main.cpp":
                main_file_cpp_path = os.path.join(root,file_name)
                with open(main_file_cpp_path, 'r') as file_cpp:
                    lines_cpp = file_cpp.readlines()
                    write_file_if_new_changes(generated_final_folder, generated_final_folder_tmp, f, lines_cpp)
            elif ".hpp" in file_name:
                with open(file_name, 'r') as file:
                    lines = file.readlines()

                    header_lines = []
                    source_lines = []
                    function_data = None
                    match_class = None

                    file_name_we, _ = os.path.splitext(f)

                    if len(lines) != 0:

                        should_ignore(lines)

                        if "Macros" in file_name:
                            ignore_file = True
                        
                        if not ignore_file:
                            for line in lines:
                                process_line(line)

                            header_lines = ["#ifndef " + file_name_we.upper() + "\n"] + ["#define " + file_name_we.upper() + "\n"] + header_lines
                            header_lines = header_lines + ["\n\n#endif"]

                    if ignore_file:
                        write_file_if_new_changes(generated_final_folder, generated_final_folder_tmp, file_name_we+".hpp", lines)

                        file_cpp_path = os.path.join(root,file_name_we+".cpp")
                        if os.path.isfile(file_cpp_path):
                            with open(file_cpp_path, 'r') as file_cpp:
                                lines_cpp = file_cpp.readlines()
                                write_file_if_new_changes(generated_final_folder, generated_final_folder_tmp, file_name_we+".cpp", lines_cpp)


                    elif not ignore_file and match_class:
                        write_file_if_new_changes(generated_final_folder, generated_final_folder_tmp, file_name_we+".hpp", header_lines)
                        write_file_if_new_changes(generated_final_folder, generated_final_folder_tmp, file_name_we+".cpp", source_lines)
