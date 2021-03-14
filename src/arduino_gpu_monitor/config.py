from easydict import EasyDict as edict
config = edict()

# Configure these

config.debug_mode = False

# if None, the first available port is selected 
config.port_name = '/dev/ttyACM0' 

config.check_connection_interval = 5
config.read_timeout = 10

config.send_log_keys = [
    'name',
    'gpu_util_p',
    'mem_util_p',
    'curr_temp',
    'max_temp',
    'used_mem',
    'total_mem'
]