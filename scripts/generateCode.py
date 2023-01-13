import pathlib

from generateIncludes import *

def generateCode(projectDir):
    generated_code_dirname = "generated-code"
    pathlib.Path(generated_code_dirname).mkdir(parents=True, exist_ok=True)

    generateIncludes(projectDir, generated_code_dirname)