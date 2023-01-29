import os
import generateCode

# Pre build step
# This script only contains the pre build phase, useful to be called from IDEs like Clion...

cwd = os.path.dirname(os.path.realpath(__file__))
cwd = cwd + "/.."
os.chdir(cwd)
print(cwd) 

generateCode.generateCode(cwd)
