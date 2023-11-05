# Saintcon 2023 RP2040 Minibadge
 Minibadge Software



# Debugging
launch.json entry

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