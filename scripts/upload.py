import os, sys, time
from platform import system


if system() == 'Linux':
  current_path = str(os.path.abspath(os.getcwd())) + '/scripts'
  pico_dir = "/media/" + os.popen('whoami').read().replace('\n', '') + "/RPI-RP2"
  if os.path.exists(pico_dir):
    os.system("cp " + current_path +  "/../.pio/build/pico/firmware.uf2 " + pico_dir)
  else:
    sys.exit()

elif system() == 'Windows':
  # sleep for 1 second to get the 2040 to boot into uf2
  time.sleep(1)
  current_path = str(os.path.abspath(os.getcwd()))

  device_ids = os.popen('wmic logicaldisk get deviceid').read().replace(' ', '').strip().split('\n')
  volume_names = os.popen('wmic logicaldisk get volumename').read().replace(' ', '').strip().split('\n')
  
  for volume_name, device_id in zip(volume_names, device_ids):
    print(volume_name)
    if volume_name == 'RPI-RP2':
      pico_dir =  device_id + "\\"
      #print(pico_dir)
      break

  #print("copy " + current_path +  "\\.pio\\build\\pico\\firmware.uf2 " + pico_dir)
  os.system('copy "' + current_path +  '\\.pio\\build\\pico\\firmware.uf2" '  + pico_dir)
    
else:
  print("exit, failed")
  sys.exit()