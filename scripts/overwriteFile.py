import os
import filecmp
import shutil

# Overwrite file if changes

def overwriteFileIfChanges(tmp_filepath, filepath):
    overwrite = True
    if os.path.isfile(filepath) and os.path.isfile(tmp_filepath):
        if filecmp.cmp(filepath, tmp_filepath, shallow=False):
            print(filepath + " have no changes!")
            overwrite = False

    if overwrite:
        print(filepath + " have changes!")
        shutil.copyfile(tmp_filepath, filepath)
