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

def extract_files(file_path, destiny_path):
    log.log(log.LogLabels.info, "Extracting: " + file_path)
    if file_path.endswith("zip"):
        with ZipFile(file_path) as zip_obj:
            members = zip_obj.namelist()
            total = len(members)
            for i, member in enumerate(members, 1):
                zip_obj.extract(member, destiny_path)
                log.print_progress(log.LogLabels.info, i, total)

    if file_path.endswith("tar.xz"):
        with tarfile.open(name=file_path, mode='r:xz') as tar_obj:
            members = tar_obj.getmembers()
            total = len(members)
            for i, member in enumerate(members, 1):
                tar_obj.extract(member, destiny_path)
                log.print_progress(log.LogLabels.info, i, total)

    if file_path.endswith("tar.gz"):
        with tarfile.open(name=file_path, mode='r:gz') as tar_obj:
            members = tar_obj.getmembers()
            total = len(members)
            for i, member in enumerate(members, 1):
                tar_obj.extract(member, destiny_path)
                log.print_progress(log.LogLabels.info, i, total)