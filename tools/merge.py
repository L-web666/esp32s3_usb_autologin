import os
import subprocess
import sys
from os.path import join

Import("env")

def build_littlefs(env):
    print("Building LittleFS filesystem...")
    # 用 sys.executable -m platformio 而不是裸 `platformio` 命令：
    # 当 PlatformIO 是通过 `python -m platformio` 调用、且 PATH 里没有
    # pio.exe/platformio.exe 时，直接调 `platformio` 会报
    # "系统找不到指定的文件"，把本来已经成功的固件构建判成失败。
    result = subprocess.run(
        [sys.executable, "-m", "platformio", "run", "--target", "buildfs"],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        print("Error building LittleFS filesystem:")
        print(result.stderr)
        env.Exit(1)
    else:
        print("LittleFS filesystem built successfully.")
        print(result.stdout)

def merge_bin(source, target, env):
    # Ensure LittleFS is built first
    build_littlefs(env)

    # Define absolute paths for all binaries
    build_dir = env.subst("${BUILD_DIR}")
    merged_image = join(build_dir, "device.bin")
    bootloader_path = join(build_dir, "bootloader.bin")
    partition_table_path = join(build_dir, "partitions.bin")
    littlefs_path = join(build_dir, "littlefs.bin")

    # Stick with target[0].get_abspath() unless you are confident the firmware name is always firmware.bin. 
    # It ensures your script works seamlessly with PlatformIO's flexibility.
    firmware_path = target[0].get_abspath()
    #firmware_path = join(build_dir, "firmware.bin")

    # Check if required files exist
    for path in [bootloader_path, partition_table_path, firmware_path, littlefs_path]:
        if not os.path.exists(path):
            print(f"Error: {path} does not exist.")
            env.Exit(1)

    # The list contains all images to be merged
    flash_images = [
        f"0x0 {bootloader_path}",
        f"0x8000 {partition_table_path}",
        f"${{ESP32_APP_OFFSET}} {firmware_path}",
        f"0x670000 {littlefs_path}"  # Add LittleFS at its correct offset
    ]
    
    board_config = env.BoardConfig()

    # Figure out flash frequency and mode
    flash_freq = board_config.get("build.f_flash", "40000000L")
    flash_freq = str(flash_freq).replace("L", "")
    flash_freq = str(int(int(flash_freq) / 1000000)) + "m"
    flash_mode = board_config.get("build.flash_mode", "dio")
    memory_type = board_config.get("build.arduino.memory_type", "qio_qspi")

    if flash_mode == "qio" or flash_mode == "qout":
        flash_mode = "dio"
    if memory_type == "opi_opi" or memory_type == "opi_qspi":
        flash_mode = "dout"

    # Run esptool to merge images into a single binary
    env.Execute(
        f"{env.subst('${PYTHONEXE}')} {env.subst('${OBJCOPY}')} "
        f"--chip {board_config.get('build.mcu', 'esp32s3')} merge_bin "
        f"--flash_size {board_config.get('upload.flash_size', '16MB')} "
        f"--flash_mode {flash_mode} "
        f"--flash_freq {flash_freq} "
        f"-o {merged_image} "
        f"{' '.join(flash_images)}"
    )

# Add a post action that runs LittleFS build and then merges images
env.AddPostAction("${BUILD_DIR}/${PROGNAME}.bin", merge_bin)
