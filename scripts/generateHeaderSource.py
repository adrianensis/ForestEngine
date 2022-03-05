import os  
import re
import shutil

from generateClassList import *

cwd = os.path.dirname(os.path.realpath(__file__))
cwd = cwd + "/.."
os.chdir(cwd)
#print(cwd)

# folders to find classes
folders = ["code", "tools", "games"]

generated_code_dirname = "generated-code/final"

if os.path.isdir(generated_code_dirname):
    shutil.rmtree(generated_code_dirname)

os.mkdir(generated_code_dirname)

MACRO_FUNCTION = "CPP"
MACRO_PLAIN_TEXT = "CPP_INCLUDE"

cpp_plain_text_found = False
cpp_function_found = False
braces_count = 0
body_open = False
waiting_for_body = True
function_data = None
match_class = None

header_lines = []
source_lines = []

class FunctionData:
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

    def getDeclaration(self):
        return self.previous + "\n" + self.pre_return_type + " " + self.return_type + " " + self.name + self.params + ";"

    def getImplementation(self):
        return self.previous + "\n" + self.pre_return_type + " " + self.return_type + " " + self.declaration_class + "::" + self.name + self.params + self.body

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
    global match_class
    global header_lines
    global source_lines

    # detect class
    if not match_class:
        match_class = searchClassDefinition(line)

    # detect CPP macro
    if not cpp_plain_text_found:
        match_cpp = re.search(r''+MACRO_PLAIN_TEXT, line)
        if match_cpp:
            cpp_plain_text_found = True
            
            # ignore first CPP_... line
            return

        elif not cpp_function_found:
            match_cpp = re.search(r''+MACRO_FUNCTION, line)
            if match_cpp:
                cpp_function_found = True
                waiting_for_body = True

                # reset function_data
                function_data = FunctionData()
    
    # process each case
    if cpp_plain_text_found:
        # ignore last CPP_... line
        match_cpp = re.search(r''+MACRO_PLAIN_TEXT, line)
        if match_cpp:
            cpp_plain_text_found = False
            source_lines += "\n"
        else:
            source_lines += line

    elif cpp_function_found:
        
        countBraces(line)

        #print(line)
        if waiting_for_body:
            match_function = re.search(r'(.*)\s+([\w\d_<>:]+)\s+([\w\d_]+)(\(.*)', line)
            if match_function:
                if match_class:
                    function_data.declaration_class = match_class.group(1)

                function_data.pre_return_type = match_function.group(1)
                function_data.return_type = match_function.group(2)
                function_data.name = match_function.group(3)
                function_data.params = match_function.group(4)
                function_data.print()

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
            function_data.print()

            source_lines += function_data.getImplementation()
    else:
        header_lines += line

for folder in folders:
    path = cwd+"/"+folder
    for root,d_names,f_names in os.walk(path):

        file_folder = root.split("../")[1];
        generated_final_folder = os.path.join(generated_code_dirname,file_folder);

        if os.path.isdir(generated_final_folder):
            shutil.rmtree(generated_final_folder)

        os.makedirs(generated_final_folder)

        for f in f_names:
            file_name = os.path.join(root, f)
            if ".hpp" in file_name and "Macros" not in file_name:
                with open(file_name, 'r') as file:
                    lines = file.readlines()

                    header_lines = []
                    source_lines = []

                    file_name_we, _ = os.path.splitext(f)

                    if len(lines) != 0:

                        function_data = None
                        match_class = None

                        for line in lines:
                            process_line(line)

                    with open(os.path.join(generated_final_folder,file_name_we+".hpp"), "w") as file:
                        for line in header_lines:
                            file.write(line)

                    with open(os.path.join(generated_final_folder, file_name_we+".cpp"), "w") as file:
                        for line in source_lines:
                            file.write(line)