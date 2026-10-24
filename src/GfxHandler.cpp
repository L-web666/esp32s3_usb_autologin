#ifdef ENABLE_GFX_HANDLER // Compile this file only if the feature flag is enabled

#include "TimeHandler.h"
#include "GfxHandler.h"
#include "ImageLockOpen.h"
#include "ImageLockClosed.h"
#include "ImageSettings.h"
#include "ImageWifi.h"
#include "ImageWifiOff.h"
#include "LedHandler.h"

// Initialize the static member
LGFX_LiLyGo_TDongleS3 GfxHandler::tft;
NonBlockingTimer GfxHandler::clockTimer(1000);
bool GfxHandler::showClock;

// Constructor implementation for LGFX_LiLyGo_TDongleS3
LGFX_LiLyGo_TDongleS3::LGFX_LiLyGo_TDongleS3()
{
    // SPI bus configuration
    auto cfg = _bus_instance.config();
    cfg.spi_host = SPI3_HOST;   // SPI3 for ESP32-S3
    cfg.spi_mode = 0;           // SPI mode 0
    cfg.freq_write = 27000000;  // Write frequency
    cfg.freq_read = 16000000;   // Read frequency
    cfg.spi_3wire = true;       // 3-wire SPI
    cfg.pin_sclk = TFT_SCL_PIN; // SCLK pin
    cfg.pin_mosi = TFT_SDA_PIN; // MOSI pin
    cfg.pin_dc = TFT_DC_PIN;    // DC pin
    cfg.pin_miso = -1;          // No MISO pin
    _bus_instance.config(cfg);
    _panel_instance.setBus(&_bus_instance);

    // Panel configuration
    auto panel_cfg = _panel_instance.config();
    panel_cfg.pin_cs = TFT_CS_PIN;   // CS pin
    panel_cfg.pin_rst = TFT_RES_PIN; // RST pin
    panel_cfg.panel_width = 80;      // Display width
    panel_cfg.panel_height = 160;    // Display height
    panel_cfg.offset_x = 26;         // X offset
    panel_cfg.offset_y = 1;          // Y offset
    panel_cfg.invert = true;         // Invert colors
    _panel_instance.config(panel_cfg);

    // Backlight configuration
    auto light_cfg = _light_instance.config();
    light_cfg.pin_bl = TFT_LEDA_PIN; // Backlight pin
    light_cfg.invert = true;         // Inverted brightness
    light_cfg.freq = 12000;          // PWM frequency
    light_cfg.pwm_channel = 7;       // PWM channel
    _light_instance.config(light_cfg);
    _panel_instance.setLight(&_light_instance);

    setPanel(&_panel_instance); // Attach the panel
}

void GfxHandler::toggleClock(bool state)
{
    showClock = state;

    if (!state)
    {
        tft.fillScreen(TFT_BLACK); // Clear the screen
    }
}

// Implementation for GfxHandler methods
void GfxHandler::init()
{
    tft.init();                             // Initialize the display
    tft.setBrightness(128);                 // Set the backlight brightness
    tft.setRotation(1);                     // Adjust rotation (modify as needed)
    tft.fillScreen(TFT_BLACK);              // Clear the screen
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // White text on black background
    tft.setTextSize(2);                     // Set the text size
    registerCommands();
}

void GfxHandler::loop()
{
    if (showClock && clockTimer.isReady())
    {
        printMessage(TimeHandler::formatDateTime("%I:%M:%S %p"));
    }
}

void GfxHandler::printMessage(const String &message)
{
    tft.fillScreen(TFT_BLACK); // Clear the screen
    tft.setCursor(5, 5);       // Position the cursor
    tft.println(message);      // Display the message
}

void GfxHandler::drawImage(int x, int y, int width, int height, const char *data)
{
    uint8_t pixel[3]; // RGB values
    uint16_t color;

    for (int j = 0; j < height; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            HEADER_PIXEL(data, pixel);                          // Decode pixel
            color = tft.color565(pixel[0], pixel[1], pixel[2]); // Convert RGB888 to RGB565
            tft.drawPixel(x + i, y + j, color);                 // Draw pixel on screen
        }
    }
}

void GfxHandler::registerCommands()
{
    CommandHandler::registerCommand("tft", [](const String &command){
        String cmd, args;
        CommandHandler::parseCommand(command, cmd, args);

        if (cmd == "print") {
            printMessage(args.c_str());
        }

        if (cmd == "identify") {
            // Print device name
            printMessage(settings.device.name.c_str());
            
            // Reboot due to heap stack overflow !!!!!!!!!!!!!
            LedHandler::setColorByName("blue");
            //delay(30000);
            //LedHandler::setColorByName("black");
        }

        else if (cmd == "clock") {
            bool state = args.equalsIgnoreCase("true");
            toggleClock(state);
        }

        else if (cmd == "draw"){
            args.trim(); // Remove any leading/trailing spaces

            // Find commas safely
            int firstComma = args.indexOf(',');
            int secondComma = args.indexOf(',', firstComma + 1);
            int thirdComma = args.indexOf(',', secondComma + 1);
            int fourthComma = args.indexOf(',', thirdComma + 1); // Might be -1

            // Ensure at least three commas exist (imageName, x, y, width, height)
            if (firstComma == -1 || secondComma == -1 || thirdComma == -1) {
                debugI("Error: Invalid arguments for draw command.");
                debugI("Received args: %s", args.c_str());
                return;
            }

            // Extract image name and numerical arguments
            String imageName = args.substring(0, firstComma);
            int x = args.substring(firstComma + 1, secondComma).toInt();
            int y = args.substring(secondComma + 1, thirdComma).toInt();
            int width = args.substring(thirdComma + 1, fourthComma != -1 ? fourthComma : args.length()).toInt();
            int height = (fourthComma != -1) ? args.substring(fourthComma + 1).toInt() : width; // Default height = width if not provided

            // Validate width and height
            if (width <= 0 || height <= 0) {
                debugI("Error: Invalid width or height.");
                return;
            }

            debugI("Drawing image: %s at (%d, %d), Size: (%d, %d)", imageName.c_str(), x, y, width, height);

            tft.fillScreen(TFT_BLACK); // Clear the screen

            if (imageName == "lock") {
                drawImage(x, y, width, height, image_lock_closed);
            } 
            else if (imageName == "unlock") {
                drawImage(x, y, width, height, image_lock_open);
            }
            else if (imageName == "settings") {
                drawImage(x, y, width, height, image_settings);
            }
            else if (imageName == "wifion") {
                drawImage(x, y, width, height, image_wifi);
            }
            else if (imageName == "wifioff") {
                drawImage(x, y, width, height, image_wifi_off);
            }
            else {
                debugI("Error: Unknown image name: %s", imageName.c_str());
                return;
            }
        }

        else {
            debugW("Unknown tft subcommand: %s", cmd.c_str());
        } }, "Handles tft commands. Usage: led <subcommand> [args]\n"
                                         "  Subcommands:\n"
                                         "  print <print> - Print a message to TFT screen\n"
                                         "  identify - Print device name\n"
                                         "  clock <true|false> - Show or hide current time on tft screen\n"
                                         "  demo - Show demo lock image on tft screen");
}

#endif // USE_GFX_HANDLER

//tft print test
//tft clock true
//tft clock false
//tft draw lock,45,5,60,60
//tft draw unlock,45,5,60,60
//tft draw settings,45,5,60,60
//tft draw wifion,45,5,60,60
//tft draw wifioff,45,5,60,60
