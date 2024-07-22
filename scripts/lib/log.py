def log(label, msg: str):
    print("["+label+"]"+"> "+msg)

class LogLabels:
    build="BUILD"
    info="INFO"
    warning="WARNING"
    error="ERROR"