#pragma once

#include "esphome.h"
#include "globals.h"
#include "M5Dial.h"

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

    void handle_rotary_knob(const char* direction, int& currentDevice, M5DialDisplay& display, HaDevice** devices) {
        bool forceRefresh = false;

        if (is_navigation_mode_) {
            if (strcmp(direction, ROTARY_LEFT) == 0) {
                currentDevice--;
                if (currentDevice < 0) {
                    currentDevice = max_components_ - 1; // Wrap around
                }
                forceRefresh = true;
            } else if (strcmp(direction, ROTARY_RIGHT) == 0) {
                currentDevice++;
                if (currentDevice >= max_components_) {
                    currentDevice = 0; // Wrap around
                }
                forceRefresh = true;
            }

            if (forceRefresh) {
                update_display_for_selection(display, currentDevice, devices);
            }

            ESP_LOGI("NAVIGATION_MODE", "Current device index: %d", currentDevice);
        }
    }

    void update_display_for_selection(M5DialDisplay& display, int currentDevice, HaDevice** devices) {
        static int lastDisplayedDevice = -1;

        // Only refresh if the selected device has changed
        if (lastDisplayedDevice == currentDevice) {
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

        // Update the last displayed device
        lastDisplayedDevice = currentDevice;
    }

    bool is_navigation_mode() const { return is_navigation_mode_; }

private:
    bool is_navigation_mode_ = false;
    int max_components_;
};

} // namespace shys_m5_dial
} // namespace esphome
