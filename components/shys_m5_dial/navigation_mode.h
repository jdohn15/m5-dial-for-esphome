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

    void handle_rotary_knob(const char* direction, int& currentDevice) {
        if (is_navigation_mode_) {
            if (strcmp(direction, ROTARY_LEFT) == 0) {
                currentDevice--;
                if (currentDevice < 0) {
                    currentDevice = max_components_ - 1; // Wrap around
                }
            } else if (strcmp(direction, ROTARY_RIGHT) == 0) {
                currentDevice++;
                if (currentDevice >= max_components_) {
                    currentDevice = 0; // Wrap around
                }
            }
            ESP_LOGI("NAVIGATION_MODE", "Current device index: %d", currentDevice);
        }
    }

    void handle_button_press(const char* press_type) {
        if (strcmp(press_type, BUTTON_LONG) == 0) {
            if (is_navigation_mode_) {
                exit_navigation_mode();
            } else {
                enter_navigation_mode();
            }
        }
    }

    void update_display_for_selection(M5DialDisplay& display, bool forceRefresh, int currentDevice, HaDevice** devices) {
        // Example logic for refreshing the navigation mode display
        LovyanGFX* gfx = display.getGfx();

        if (forceRefresh) {
            gfx->fillScreen(TFT_BLACK); // Clear the screen
        }

        gfx->setTextColor(TFT_WHITE);
        gfx->setTextDatum(textdatum_t::middle_center);
        gfx->setTextSize(1);
        gfx->drawString(devices[currentDevice]->getName().c_str(), gfx->width() / 2, gfx->height() / 2);
    }

    bool is_navigation_mode() const { return is_navigation_mode_; }

private:
    bool is_navigation_mode_ = false;
    int max_components_;
};

} // namespace shys_m5_dial
} // namespace esphome
