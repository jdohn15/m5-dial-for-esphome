#pragma once

#include "esphome.h"
#include "globals.h"
#include "M5Dial.h"

namespace esphome {
namespace shys_m5_dial {

class NavigationMode {
public:
    NavigationMode(int max_components) : max_components_(max_components) {}

    void enter_navigation_mode(M5DialDisplay &display) {
        is_navigation_mode_ = true;
        ESP_LOGI("NAVIGATION_MODE", "Entered navigation mode");
        refresh_display(display); // Ensure the display updates immediately
    }

    void exit_navigation_mode(M5DialDisplay &display) {
        is_navigation_mode_ = false;
        ESP_LOGI("NAVIGATION_MODE", "Exited navigation mode");
        refresh_display(display); // Refresh to return to normal behavior
    }

    void handle_rotary_knob(const char *direction, int &currentDevice, M5DialDisplay &display) {
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
            refresh_display(display); // Update the screen with the new selection
        }
    }

    void handle_button_press(const char *press_type, M5DialDisplay &display) {
        if (strcmp(press_type, BUTTON_LONG) == 0) {
            if (is_navigation_mode_) {
                exit_navigation_mode(display);
            } else {
                enter_navigation_mode(display);
            }
        }
    }

    void refresh_display(M5DialDisplay &display) {
        LovyanGFX *gfx = display.getGfx();

        const int circle_center_x = gfx->width() / 2;
        const int circle_center_y = gfx->height() / 2 + 100;
        const int outer_radius = 15;
        const int line_thickness = 2;
        const int inner_radius = outer_radius - line_thickness;

        // Draw the navigation mode indicator
        gfx->fillCircle(circle_center_x, circle_center_y, outer_radius, TFT_WHITE);
        gfx->fillCircle(circle_center_x, circle_center_y, inner_radius, TFT_BLACK);

        // Add "Navigation Mode" text
        gfx->setTextColor(TFT_BLACK);
        gfx->setTextSize(0.5);
        gfx->setTextDatum(textdatum_t::middle_center);
        gfx->drawString("Navigation mode", circle_center_x, circle_center_y - 23);
    }

    bool is_navigation_mode() const { return is_navigation_mode_; }

private:
    bool is_navigation_mode_ = false;
    int max_components_;
};

} // namespace shys_m5_dial
} // namespace esphome
