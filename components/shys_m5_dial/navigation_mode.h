#pragma once

#include "esphome.h"
#include "globals.h"
#include "M5Dial.h" // Include the M5Dial class for display operations

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

    void handle_rotary_knob(const char *direction) {
        if (is_navigation_mode_) {
            // Navigation logic here
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

    void update_display_for_selection(M5DialDisplay &display) {
        // Access the LovyanGFX instance from M5DialDisplay
        LovyanGFX* gfx = display.getGfx();

        // Screen dimensions
        const int circle_center_x = gfx->width() / 2;
        const int circle_center_y = gfx->height() / 2;
        const int circle_radius = 50;

        // Clear the display
        gfx->fillScreen(TFT_BLACK);

        // Draw a white circle
        gfx->drawCircle(circle_center_x, circle_center_y, circle_radius, TFT_WHITE);

        // Add "Navigation Mode" text
        gfx->setTextColor(TFT_WHITE);      // Set text color
        gfx->setTextSize(2);               // Set text size
        gfx->setTextDatum(textdatum_t::middle_center); // Center text alignment
        gfx->drawString("Navigation", circle_center_x, circle_center_y - 10);
        gfx->drawString("Mode", circle_center_x, circle_center_y + 10);
    }

    bool is_navigation_mode() const { return is_navigation_mode_; }

private:
    bool is_navigation_mode_ = false;
    int max_components_;
};

} // namespace shys_m5_dial
} // namespace esphome
