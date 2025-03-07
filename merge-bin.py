#!/usr/bin/python3

# Adds PlatformIO post-processing to merge all the ESP flash images into a single image.

import os

Import ("env", "projenv") # type: ignore
from SCons.Script import AlwaysBuild # type: ignore

# Dump global construction environment (for debug purpose)
#print(env.Dump())

# Dump project construction environment (for debug purpose)
#print(projenv.Dump())

board_config = env.BoardConfig() #type: ignore
firmware_elf = "${BUILD_DIR}/${PROGNAME}.elf"
firmware_bin = "${BUILD_DIR}/${PROGNAME}.bin"
spiffs_bin =  "${BUILD_DIR}/spiffs.bin"
merged_bin = os.environ.get("MERGED_BIN_PATH", "${BUILD_DIR}/${PROGNAME}-merged.bin")


def merge_bin_action(source, target, env):
    flash_images = [
        *env.Flatten(env.get("FLASH_EXTRA_IMAGES", [])),
        "$ESP32_APP_OFFSET",
        firmware_bin,
        "0x3D0000",
        spiffs_bin
    ]
    merge_cmd = " ".join(
        [
            '"$PYTHONEXE"',
            '"$OBJCOPY"',
            "--chip",
            board_config.get("build.mcu", "esp32"),
            "merge_bin",
            "-o",
            merged_bin,
            "--flash_mode",
            board_config.get("build.flash_mode", "dio"),
            "--flash_freq",
            "${__get_board_f_flash(__env__)}",
            "--flash_size",
            board_config.get("upload.flash_size", "4MB"),
            *flash_images,
        ]
    )
    env.Execute(merge_cmd)


# Add a post action that runs esptoolpy to merge available flash images
env.AddPostAction(firmware_elf,  # type: ignore
                  env.Action("pio run --target buildfs","Building spiffs.bin" )) # type: ignore
env.AddPostAction(firmware_bin, merge_bin_action) # type: ignore

env.AddCustomTarget("merge-bin", # type: ignore
                    dependencies=[spiffs_bin, firmware_bin],
                    actions=[merge_bin_action])

# Patch the upload command to flash the merged binary at address 0x0
env.Replace( # type: ignore
    UPLOADERFLAGS=[
            f
            for f in env.get("UPLOADERFLAGS") # type: ignore
            if f not in env.Flatten(env.get("FLASH_EXTRA_IMAGES")) # type: ignore
        ]
        + ["0x0", merged_bin],
    UPLOADCMD='"$PYTHONEXE" "$UPLOADER" $UPLOADERFLAGS',
)