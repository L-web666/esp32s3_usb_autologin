
//NOW ITS WORKING NO ISSUES....................................
//Embedding binary files not working with current toolchain
//Not working - https://stackoverflow.com/questions/68924676/how-to-upload-binary-data-html-favicon-to-esp32-using-esp-idf
//https://community.platformio.org/t/add-t-none-or-t-binary/441/17
//https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html#embedding-binary-data
//https://community.platformio.org/t/how-to-embed-binary-files-into-esp32-firmware/3791/2
//https://community.platformio.org/t/how-to-embed-binary-files-into-esp32-firmware/3791/6
//https://docs.platformio.org/en/latest/platforms/espressif32.html#embedding-binary-data
//https://github.com/platformio/platform-espressif32/issues/245
//./xtensa-esp32s3-elf-nm /mnt/d1/home/mdelgert/source/ImprovWiFiWeb/.pio/build/esp32-s3-devkitc-1/firmware.elf | grep favicon

//extern const uint8_t favicon_ico_start[] asm("_binary_favicon_ico_start");
//extern const uint8_t favicon_ico_end[] asm("_binary_favicon_ico_end");
//serveEmbeddedFile(server, "/favicon.ico", favicon_ico_start, favicon_ico_end, "image/x-icon");