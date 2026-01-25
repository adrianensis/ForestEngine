import sys

def log(label, msg: str):
    print("["+label+"]"+"> "+msg)

def print_progress(label, iteration, total, length=30):
    percent = f"{100 * (iteration / float(total)):.1f}"
    filled_length = int(length * iteration // total)
    bar = '█' * filled_length + '-' * (length - filled_length)
    # \r returns the cursor to the start of the line so the next print overwrites it
    sys.stdout.write(f'\r[{label}] > Progress |{bar}| {percent}%')
    sys.stdout.flush()
    if iteration == total:
        print() # Move to a new line when finished

class LogLabels:
    build="BUILD"
    info="INFO"
    warning="WARNING"
    error="ERROR"