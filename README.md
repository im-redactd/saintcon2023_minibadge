# Saintcon 2023 RP2040 Minibadge
 Minibadge Software

 # Flashing

 If you wish to flash your minibadge you'll need to use a Tag-Connect TC2050-IDC-NL cable or rig up your own SWD pins to attach to the footprint.

 I used a JLink EDU Mini.

# Branches

This is the main branch build on the Arduino-MBED core in platformio. The EarleH branch contains the changes needed to run that core if you're interested.

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