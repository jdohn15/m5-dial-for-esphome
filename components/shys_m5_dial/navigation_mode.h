#pragma once

#include "esphome.h"
#include "globals.h"
#include "M5Dial.h"
#include "ha_device.h"

namespace esphome {
namespace shys_m5_dial {

class NavigationMode {
public:
    NavigationMode(int max_components) : max_components_(max_components) {}

    void enter_navigation_mode() {
        is_navigation_mode_ = true;
        ESP_LOGI("NAVIGATION_MODE", "Entered navigation mode");
    }

    void exit_navigation_mode() {
        is_navigation_mode_ = false;
        ESP_LOGI("NAVIGATION_MODE", "Exited navigation mode");
    }

    bool is_navigation_mode() const { return is_navigation_mode_; }

    int getCurrentDeviceValue(HaDevice** devices, int currentDevice) {
        return devices[currentDevice]->getValue();
    }

    bool isNavigationDisplayRefreshNeeded(HaDevice** devices, int currentDevice, int lastDisplayDevice, int lastModeIndex, int lastDisplayValue, unsigned long lastDisplayRefresh, int displayRefeshPause) {
        if (getCurrentDeviceValue(devices, currentDevice) != lastDisplayValue || 
            currentDevice != lastDisplayDevice ||
            devices[currentDevice]->getCurrentModeIndex() != lastModeIndex || 
            devices[currentDevice]->isDisplayRefreshNeeded()) {
            return esphome::millis() - lastDisplayRefresh > displayRefeshPause;
        }
        return false;
    }

    void update_display_for_selection(M5DialDisplay &display, bool forceRefresh, int currentDevice, HaDevice** devices) {
        static int lastDisplayedDevice = -1;  // Track the last displayed device
        static unsigned long lastRefreshTime = 0;

        if (!forceRefresh && !isNavigationDisplayRefreshNeeded(devices, currentDevice, lastDisplayedDevice, lastModeIndex_, lastDisplayValue_, lastRefreshTime, displayRefeshPause_)) {
            return;
        }

        LovyanGFX* gfx = display.getGfx();

        // Clear the overlay area
        gfx->fillRect(0, gfx->height() - 50, gfx->width(), 50, TFT_BLACK);

        // Draw the navigation mode overlay
        gfx->setTextColor(TFT_WHITE);
        gfx->setTextDatum(textdatum_t::middle_center);
        gfx->drawString("Navigation Mode", gfx->width() / 2, gfx->height() - 40);

        // Draw the name of the currently selected device
        gfx->drawString(devices[currentDevice]->getName().c_str(), gfx->width() / 2, gfx->height() - 20);

        // Update tracking variables
        lastDisplayedDevice = currentDevice;
        lastRefreshTime = esphome::millis();
        lastModeIndex_ = devices[currentDevice]->getCurrentModeIndex();
        lastDisplayValue_ = getCurrentDeviceValue(devices, currentDevice);
    }

private:
    bool is_navigation_mode_ = false;
    int max_components_;

    // Tracking variables for navigation mode
    int lastModeIndex_ = -1;
    int lastDisplayValue_ = -1;
    int displayRefeshPause_ = 700; // Default refresh pause
};

} // namespace shys_m5_dial
} // namespace esphome
