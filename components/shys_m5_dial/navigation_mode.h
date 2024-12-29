#pragma once

#include "esphome.h"
#include "globals.h"

namespace esphome {
namespace shys_m5_dial {

class NavigationMode {
public:
    NavigationMode(int max_components) : max_components_(max_components) {}

    void enter_navigation_mode() {
        is_navigation_mode_ = true;
        ESP_LOGI("NAVIGATION_MODE", "Entered navigation mode");
        update_display_for_selection();
    }

    void exit_navigation_mode() {
        is_navigation_mode_ = false;
        ESP_LOGI("NAVIGATION_MODE", "Exited navigation mode");
    }

    void handle_rotary_knob(const char *direction) {
        if (is_navigation_mode_) {
            if (strcmp(direction, ROTARY_LEFT) == 0) {
                selected_component_index_--;
                if (selected_component_index_ < 0) {
                    selected_component_index_ = max_components_ - 1; // Wrap around
                }
            } else if (strcmp(direction, ROTARY_RIGHT) == 0) {
                selected_component_index_++;
                if (selected_component_index_ >= max_components_) {
                    selected_component_index_ = 0; // Wrap around
                }
            }
            ESP_LOGI("NAVIGATION_MODE", "Selected component index: %d", selected_component_index_);
            update_display_for_selection();
        }
    }

    void handle_button_press(const char *press_type) {
        if (strcmp(press_type, BUTTON_LONG) == 0) {
            if (is_navigation_mode_) {
                exit_navigation_mode();
            } else {
                enter_navigation_mode();
            }
        } else if (strcmp(press_type, BUTTON_SHORT) == 0) {
            if (is_navigation_mode_) {
                select_component(selected_component_index_);
                exit_navigation_mode();
            }
        }
    }

    void update_display_for_selection() {
        // Add logic to highlight the selected component on the display
        ESP_LOGI("NAVIGATION_MODE", "Highlighting component index: %d", selected_component_index_);
    }

    bool is_navigation_mode() const { return is_navigation_mode_; }

private:
    bool is_navigation_mode_ = false;
    int selected_component_index_ = 0;
    int max_components_;

    void select_component(int index) {
        // Add logic to handle component selection
        ESP_LOGI("NAVIGATION_MODE", "Component selected: %d", index);
    }
};

} // namespace shys_m5_dial
} // namespace esphome
