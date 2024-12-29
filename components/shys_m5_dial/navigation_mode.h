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
            ESP_LOGI("NAVIGATION_MODE", "Selected device index: %d", currentDevice);
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
        ESP_LOGI("NAVIGATION_MODE", "Updating display in navigation mode.");
        // Access the LovyanGFX instance from M5DialDisplay
        LovyanGFX* gfx = display.getGfx();

        // Screen dimensions
        const int circle_center_x = gfx->width() / 2;
        const int circle_center_y = gfx->height() / 2 + 100; // Shift circle down by 100 pixels
        const int outer_radius = 15;
        const int line_thickness = 2; // Thickness of the circle line
        const int inner_radius = outer_radius - line_thickness;

        // Draw the outer filled circle
        gfx->fillCircle(circle_center_x, circle_center_y, outer_radius, TFT_WHITE);

        // Draw the inner circle in the background color to simulate a thicker line
        gfx->fillCircle(circle_center_x, circle_center_y, inner_radius, TFT_BLACK);

        // Add "Navigation Mode" text
        gfx->setTextColor(TFT_BLACK);      // Set text color
        gfx->setTextSize(.5);               // Set smaller text size
        gfx->setTextDatum(textdatum_t::middle_center); // Center text alignment
        gfx->drawString("Navigation mode", circle_center_x, circle_center_y - 23); // Adjust text positions
    }

    bool is_navigation_mode() const { return is_navigation_mode_; }

private:
    bool is_navigation_mode_ = false;
    int max_components_;
};

} // namespace shys_m5_dial
} // namespace esphome
