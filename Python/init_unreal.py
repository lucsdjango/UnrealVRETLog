import unreal
import subprocess
import pkg_resources
import pathlib
import pandas as pd
import os
from IDT_alg_VR_centred import IDTVR
import unreal
import numpy as np


print(pathlib.Path().resolve())
print(unreal.Paths.project_content_dir())

PYTHON_INTERPRETER_PATH = unreal.get_interpreter_executable_path()
assert pathlib.Path(PYTHON_INTERPRETER_PATH).exists(), f"Python not found at '{PYTHON_INTERPRETER_PATH}'"

def pip_install(packages):
    # dont show window
    info = subprocess.STARTUPINFO()
    info.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    
    proc = subprocess.Popen(
        [
            PYTHON_INTERPRETER_PATH, 
            '-m', 'pip', 'install', 
            '--no-warn-script-location', 
            *packages
        ],
        startupinfo = info,
        stdout = subprocess.PIPE,
        stderr = subprocess.PIPE,
        encoding = "utf-8"
    )

    while proc.poll() is None:
        unreal.log(proc.stdout.readline().strip())
        unreal.log_warning(proc.stderr.readline().strip())

    return proc.poll()

# Put here your required python packages
required = {'numpy','pandas','matplotlib','notebook','numba'}
installed = {pkg.key for pkg in pkg_resources.working_set}
missing = required - installed

if len(missing) > 0:
    print(missing)
    #pip_install({'notebook'})
    #pip_install(missing)
else:
    unreal.log("All python requirements already satisfied")
    
