#pragma once

#include "Globals.h" // For CUSTOM_PRODUCT_NAME, CUSTOM_MANUFACTURER, CUSTOM_SERIAL_NUMBER

extern "C" const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid);
