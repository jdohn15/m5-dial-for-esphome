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

    void handle_rotary_knob(const char *direction, int &currentDevice) {
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
        }
    }

    void handle_button_press(const char *press_type) {
        if (strcmp(press_type, BUTTON_LONG) == 0) {
            if (is_navigation_mode_) {
                exit_navigation_mode();
            } else {
                enter_navigation_mode();
            }
        }
    }

    void update_display_for_selection(M5DialDisplay &display, int currentDevice, HaDevice **devices) {
        // Access the LovyanGFX instance from M5DialDisplay
        LovyanGFX* gfx = display.getGfx();
    
        static int last_selection = -1; // Track the last drawn selection to avoid redundant updates
    
        // Check if the selection has changed; skip unnecessary drawing
        if (last_selection == currentDevice) {
            return; // No changes, skip update
        }
    
        last_selection = currentDevice;
    
        // Clear the overlay area (specific to navigation mode)
        gfx->fillRect(0, gfx->height() - 50, gfx->width(), 50, TFT_BLACK);
    
        // Draw the "Navigation Mode" text
        gfx->setTextColor(TFT_WHITE);
        gfx->setTextDatum(middle_center);
        gfx->drawString("Navigation Mode", gfx->width() / 2, gfx->height() - 40);
    
        // Draw the name of the currently selected entity
        gfx->drawString(devices[currentDevice]->getName().c_str(), gfx->width() / 2, gfx->height() - 20);
    }




    bool is_navigation_mode() const { return is_navigation_mode_; }

private:
    bool is_navigation_mode_ = false;
    int max_components_;
};

} // namespace shys_m5_dial
} // namespace esphome
