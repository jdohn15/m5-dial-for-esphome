#include "navigation_mode.h"

namespace esphome {
namespace shys_m5_dial {

// Define the static bitmap
const uint8_t NavigationMode::circle_bitmap[8] = {
    0b00111100,
    0b01111110,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b01111110,
    0b00111100
};

} // namespace shys_m5_dial
} // namespace esphome
