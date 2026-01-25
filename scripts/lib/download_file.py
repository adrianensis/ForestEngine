import os
import sys
import lib.log as log
import urllib.request

def download_file(url, destinty_path, filename):
    log.log(log.LogLabels.info, "Downloading: " + url)
    log.log(log.LogLabels.info, "Download Location: " + destinty_path)
    
    if not os.path.isdir(destinty_path):
        os.makedirs(destinty_path) # Use makedirs to handle nested folders

    filePath = os.path.join(destinty_path, filename)

    user_agent = 'Mozilla/5.0 (Windows NT 6.1; Win64; x64)'
    headers = {'User-Agent': user_agent}
    req = urllib.request.Request(url, None, headers)

    with urllib.request.urlopen(req) as response:
        # Get total file size from headers
        total_size = int(response.headers.get('Content-Length', 0))
        downloaded = 0
        block_size = 8192 # 8kb chunks

        with open(filePath, 'wb') as f:
            while True:
                buffer = response.read(block_size)
                if not buffer:
                    break
                
                downloaded += len(buffer)
                f.write(buffer)
                
                if total_size > 0:                    
                    # Convert bytes to KB
                    downloaded_kb = downloaded / 1024
                    total_kb = total_size / 1024
                    
                    log.print_progress(log.LogLabels.info, downloaded_kb, total_kb)
        
        log.log(log.LogLabels.info, "Download Complete: " + filePath)