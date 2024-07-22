import platform
from zipfile import ZipFile
import tarfile
import lib.log as log

if not platform.python_version().startswith('3'):
    log.log(log.LogLabels.error, "Please use python3 to run this script")
    exit()

##########################################
########## FUNCTIONS ###########
##########################################

def extract(file_path, destinty_path):
    log.log(log.LogLabels.info, "Extracting: " + file_path)
    if file_path.endswith("zip"):
        with ZipFile(file_path) as zipObj:
            zipObj.extractall(destinty_path)

    if file_path.endswith("tar.xz"):
        with tarfile.open(name=file_path, mode='r:xz') as tarObj:
            tarObj.extractall(destinty_path)

    if file_path.endswith("tar.gz"):
        with tarfile.open(name=file_path, mode='r:gz') as tarObj:
            tarObj.extractall(destinty_path)