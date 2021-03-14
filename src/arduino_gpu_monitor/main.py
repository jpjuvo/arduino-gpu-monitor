import time
import sys
sys.path.append('.')
from config import config
from nvidia_utils import compact_gpu_stats
from serial_utils import ArduinoSerialConnection

arduino_connection = None

def loop_updates():
    if arduino_connection is None: return None
    while arduino_connection.connection is not None:
        # Wait for the send command
        msg = arduino_connection.read()
        print(f'msg: {msg}')
        if msg is None: return False

        if 'update' in msg:
            return True

def update():
    log_dict = compact_gpu_stats()
    if len(log_dict.keys()) > 0:
        msg = ''
        for key in config.send_log_keys:
            if key in log_dict:
                val = log_dict[key]
                msg += f'{key}:{val},'

        return arduino_connection.write(msg)
    return None

def main():
    global arduino_connection
    # init and seek connection
    arduino_connection = ArduinoSerialConnection()

    while True:
        success = loop_updates()
        if success is None:
            # reinit
            arduino_connection = ArduinoSerialConnection()
        elif not success:
            # reconnect
            arduino_connection.seek_connection()
        else:
            success = update()
            # if success is None, nvidia log returned empty dict 
            if success is not None:
                if not success:
                    arduino_connection.seek_connection()
    
        time.sleep(1)



if __name__ == "__main__":
    main()