import pathlib

from generateClassesRegister import *
from generateHeaders import *

def generateCode(projectDir):
    generated_code_dirname = "generated-code"
    pathlib.Path(generated_code_dirname).mkdir(parents=True, exist_ok=True)

    # generateHeaders(projectDir, generated_code_dirname)
    generateClassesRegister(projectDir, generated_code_dirname)