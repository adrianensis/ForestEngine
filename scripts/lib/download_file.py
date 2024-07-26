import os
import lib.log as log
import urllib.request

##########################################
########## FUNCTIONS ###########
##########################################

def download_file(url, destinty_path, filename):
    log.log(log.LogLabels.info, "Downloading: " + url)
    log.log(log.LogLabels.info, "Download Location: " + destinty_path)
    
    if not os.path.isdir(destinty_path):
        os.mkdir(destinty_path)
        
    urllib.request.urlretrieve(url, os.path.join(destinty_path, filename))