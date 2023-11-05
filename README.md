# Saintcon 2023 RP2040 Minibadge
 Minibadge Software

# This Branch
A few of the minibadges would not boot after flashing. The assumption is a slight timing issue with the flash. This core which is more aligned with the Pico-SDK handles this skew better and boots the code. There needed to be some changes to handle the touch logic after replacing the arduino-mbed core.

# Debugging

launch.json entry
```
"configurations": [
        {
            "cwd": "${workspaceFolder}",
            "executable": "d:/GDrive/Projects/KiCad/Minibadge/saintcon-rp2040-platform-kicad7/software/saintcon2023_minibadge/.pio/build/pico/firmware.elf",
            "name": "Debug with JLink",
            "request": "launch",
            "type": "cortex-debug",
            "device": "RP2040_M0_0",
            "serverpath": "C:/.platformio/packages/tool-jlink/JLinkGDBServerCL.exe",
            "runToEntryPoint": "main",
            "showDevDebugOutput": "none",
            "servertype": "jlink"
        },
    ```