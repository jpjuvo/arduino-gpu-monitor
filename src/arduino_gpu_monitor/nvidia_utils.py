# Modified from:
# Author: Danny Price
# https://gist.github.com/telegraphic/ecb8161aedb02d3a09e39f9585e91735

import subprocess
import pynvml

def read_gpu_log():
    out_dict = {}

    try:
        sp = subprocess.Popen(
            ['nvidia-smi', '-q'], 
            stdout=subprocess.PIPE, 
            stderr=subprocess.PIPE
            )

        out_str = sp.communicate()[0]
        if isinstance(out_str, bytes):
            out_str = out_str.decode('utf-8')
        out_list = out_str.split('\n')

        for item in out_list:
            try:
                key, val = item.split(':')
                key, val = key.strip(), val.strip()
                out_dict[key] = val
            except:
                pass
    except:
        pass
        
    return out_dict

def gpu_status(gpu_id=0):
    total_megs, used_megs = 100,0
    gpu_use_p, gpu_mem_p = 0,0
    try:
        pynvml.nvmlInit()
        handle = pynvml.nvmlDeviceGetHandleByIndex(gpu_id)
        meminfo = pynvml.nvmlDeviceGetMemoryInfo(handle)
        MB_SIZE = 1024*1024
        total_megs = int(meminfo.total/MB_SIZE)
        used_megs = int(meminfo.used/MB_SIZE)

        utilization_rates = pynvml.nvmlDeviceGetUtilizationRates(handle)
        gpu_use_p = utilization_rates.gpu
        gpu_mem_p = utilization_rates.memory

        pynvml.nvmlShutdown()
    except:
        pass
    return (used_megs, total_megs, gpu_use_p, gpu_mem_p)


def compact_gpu_stats():
    gpu_log = read_gpu_log()
    used_megs, total_megs, gpu_use_p, gpu_mem_p = gpu_status()
    current_temperature, max_temperature = 0,0

    if 'GPU Current Temp' in gpu_log and 'GPU Max Operating Temp' in gpu_log:
        current_temp_str = gpu_log['GPU Current Temp']
        max_temp_str = gpu_log['GPU Max Operating Temp']
        # trim
        try:
            current_temperature = int(current_temp_str.replace('C','').replace(' ',''))
            max_temperature = int(max_temp_str.replace('C','').replace(' ',''))
        except:
            pass

    product_name = gpu_log['Product Name'] if 'Product Name' in gpu_log else "-"

    gpu_dict = {
        'name':product_name,
        'gpu_util_p': gpu_use_p,
        'mem_util_p': gpu_mem_p,
        'curr_temp': current_temperature,
        'max_temp': max_temperature,
        'used_mem': used_megs,
        'total_mem': total_megs
    }

    return gpu_dict