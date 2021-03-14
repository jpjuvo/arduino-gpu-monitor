import serial
import glob
import sys
import time
import sys
sys.path.append('.')
from config import config

def get_port_name(preferred_name=None):
    """
    Returns the first serial port name. 
    If preferred_name is not None, this returns 
    the full port name of a first port that 
    includes the preferred_name
    """
    port_list = []

    if sys.platform.startswith ('linux'):
        port_list = glob.glob ('/dev/ttyACM[0-9]*')
    else:
        port_list = [p.name for p in list(serial.tools.list_ports.comports())]

    if len(port_list) > 0:
        if preferred_name is not None:
            for port in port_list:
                if preferred_name in port:
                    return port
        else:
            return port_list[0]
    
    return None

class ArduinoSerialConnection:

    def __init__(self):
        self.connection = None
        self.seek_connection()

    def seek_connection(self):
        """ Loop until connected """
        if config.debug_mode:
            print('seeking connection')
        
        self.connection = None

        while self.connection is None:
            port = get_port_name(config.port_name)
            
            if port is not None:
                try:
                    self.connection = serial.Serial(
                        port=port, 
                        baudrate=9600,
                        timeout=config.read_timeout
                        )
                    if config.debug_mode:
                        print(f'connected to {port}')
                except serial.serialutil.SerialException as e:
                    print(str(e))
                    self.connection = None
            
            time.sleep(config.check_connection_interval)

    def read(self):
        if config.debug_mode:
            print(f'serial read')
        if self.connection is None: return None
        try:
            msg = self.connection.read_until()
            return msg.decode('utf-8')
        except:
            return None

    def write(self, text):
        if config.debug_mode:
            print(f'serial write - {text}')
        try:
            self.connection.write((text + '\n').encode('utf-8'))
        except:
            return False
        return True
