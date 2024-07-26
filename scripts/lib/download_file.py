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

    filePath = os.path.join(destinty_path, filename)

    user_agent = 'Mozilla/5.0 (Windows NT 6.1; Win64; x64)'
    headers = {'User-Agent': user_agent}
    req = urllib.request.Request(url, None, headers)
    with urllib.request.urlopen(req) as response:
        f = open(filePath,'wb')
        f.write(response.read())
        f.close()
        log.log(log.LogLabels.info, "Download Complete: " + filePath)
