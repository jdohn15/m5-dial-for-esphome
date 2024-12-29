#pragma once
#include "esphome.h"
#include "esp_log.h"

#include "globals.h"
#include "ha_api.h"
#include "ha_device.h"
#include "ha_device_light.h"
#include "ha_device_climate.h"
#include "ha_device_cover.h"
#include "ha_device_switch.h"
#include "ha_device_fan.h"
#include "ha_device_mediaplayer.h"
#include "ha_device_lock.h"
#include "navigation_mode.h"

#include "M5Dial.h"

#define MAX_DEVICE_COUNT 50

namespace esphome {
  namespace shys_m5_dial {
    class ShysM5Dial : public Component, public esphome::api::CustomAPIDevice {
    protected:
      int timeToScreenOff = 30000;
      int longPressMs = 1200;
      int rotaryStepWidth = 10;
      uint16_t displayRefeshPause = 700;

      int apiSendDelay = 1000; // Delay after value change to avoid frequent updates
      int apiSendLock = 3000;  // Lock time between API calls

      NavigationMode navigation_mode_{MAX_DEVICE_COUNT};

      HaDevice* devices[MAX_DEVICE_COUNT];
      int deviceAnzahl = 0;
      int currentDevice = 0;

      int lastDisplayDevice = -1;
      float lastDisplayValue = -1;
      int lastModeIndex = -1;

      unsigned long lastRotaryEvent = 0;
      unsigned long lastReceiveEvent = 0;
      unsigned long lastDisplayRefresh = 0;

      int lastLoop = 0;

      bool enableRFID = true;
      bool enableEncoder = true;

      M5DialDisplay* m5DialDisplay = new M5DialDisplay();
      M5DialRfid* m5DialRfid = new M5DialRfid();
      M5DialRotary* m5DialRotary = new M5DialRotary();
      M5DialTouch* m5DialTouch = new M5DialTouch();
      M5DialEEPROM* m5DialEEPROM = new M5DialEEPROM();

      bool startsWith(const char *pre, const char *str) {
        return strncmp(pre, str, strlen(pre)) == 0;
      }

      int getCurrentValue() {
        return devices[currentDevice]->getValue();
      }

      bool isDisplayRefreshNeeded() {
        if (getCurrentValue() != lastDisplayValue ||
            currentDevice != lastDisplayDevice ||
            devices[currentDevice]->getCurrentModeIndex() != lastModeIndex ||
            devices[currentDevice]->isDisplayRefreshNeeded()) {
          return esphome::millis() - lastDisplayRefresh > displayRefeshPause;
        }
        return false;
      }

      void refreshDisplay(bool forceRefresh) {
        if (forceRefresh || isDisplayRefreshNeeded()) {
          if (navigation_mode_.is_navigation_mode()) {
            navigation_mode_.update_display_for_selection(*m5DialDisplay);
          } else {
            devices[currentDevice]->refreshDisplay(*m5DialDisplay, lastDisplayDevice != currentDevice);
          }
          lastDisplayDevice = currentDevice;
          lastModeIndex = devices[currentDevice]->getCurrentModeIndex();
          lastDisplayValue = getCurrentValue();
        }
      }

      void nextDevice() {
        if (currentDevice >= deviceAnzahl - 1) {
          currentDevice = 0;
        } else {
          currentDevice++;
        }
      }

      void previousDevice() {
        if (currentDevice >= 1) {
          currentDevice--;
        } else {
          currentDevice = deviceAnzahl - 1;
        }
      }

      void addDevice(HaDevice* device) {
        if (device != nullptr) {
          if (this->deviceAnzahl >= MAX_DEVICE_COUNT - 1) {
            ESP_LOGE("DEVICE", "EXCEED DEVICE COUNT MAXIMUM: %s cannot be added!", device->getName().c_str());
            return;
          }

          ESP_LOGD("DEVICE", "New Device: %s", device->getName().c_str());

          devices[deviceAnzahl] = device;

          devices[deviceAnzahl]->setApiSendDelay(this->apiSendDelay);
          devices[deviceAnzahl]->setApiSendLock(this->apiSendLock);
          devices[deviceAnzahl]->setRotaryStepWidth(this->rotaryStepWidth);

          devices[deviceAnzahl]->init();

          deviceAnzahl++;
          ESP_LOGD("DEVICE", "Device added");
        }
      }

    public:
      void dump_config() override;
      void setup() override;
      void loop() override;

      ShysM5Dial() : Component() {}

      void setScreenOffTime(int value) {
        ESP_LOGI("DEVICE", "setScreenOffTime %i", value);
        this->timeToScreenOff = value;
        m5DialDisplay->setTimeToScreenOff(value);
      }

      void setLongPressDuration(int value) {
        ESP_LOGI("DEVICE", "setLongPressDuration %i", value);
        this->longPressMs = value;
        m5DialRotary->setLongPressDuration(value);
      }

      void setApiSendDelay(int delayInMs) {
        ESP_LOGI("DEVICE", "setApiSendDelay %i", delayInMs);
        this->apiSendDelay = delayInMs;
      }

      void setApiSendLock(int delayInMs) {
        ESP_LOGI("DEVICE", "setApiSendLock %i", delayInMs);
        this->apiSendLock = delayInMs;
      }

      void setRotaryStepWidth(int value) {
        ESP_LOGI("DEVICE", "setRotaryStepWidth %i", value);
        this->rotaryStepWidth = value;
      }

      void setFontName(std::string value) {
        ESP_LOGI("DEVICE", "setFontName %s", value);
        m5DialDisplay->setFontName(value);
      }

      void setFontFactor(int value) {
        ESP_LOGI("DEVICE", "setFontFactor %i", value);
        m5DialDisplay->setFontFactor(value);
      }

      void turnRotaryLeft() {
        m5DialDisplay->resetLastEventTimer();
        M5Dial.Speaker.tone(5000, 20);

        if (navigation_mode_.is_navigation_mode()) {
          navigation_mode_.handle_rotary_knob(ROTARY_LEFT, currentDevice);
          refreshDisplay(true);
        } else if (m5DialDisplay->isDisplayOn()) {
          devices[currentDevice]->onRotary(*m5DialDisplay, ROTARY_LEFT);
        }

        lastRotaryEvent = esphome::millis();
      }

      void turnRotaryRight() {
        m5DialDisplay->resetLastEventTimer();
        M5Dial.Speaker.tone(5000, 20);

        if (navigation_mode_.is_navigation_mode()) {
          navigation_mode_.handle_rotary_knob(ROTARY_RIGHT, currentDevice);
          refreshDisplay(true);
        } else if (m5DialDisplay->isDisplayOn()) {
          devices[currentDevice]->onRotary(*m5DialDisplay, ROTARY_RIGHT);
        }

        lastRotaryEvent = esphome::millis();
      }

      void shortButtonPress() {
        m5DialDisplay->resetLastEventTimer();
        M5Dial.Speaker.tone(4000, 20);

        if (navigation_mode_.is_navigation_mode()) {
          navigation_mode_.handle_button_press(BUTTON_SHORT);
        } else if (m5DialDisplay->isDisplayOn()) {
          devices[currentDevice]->onButton(*m5DialDisplay, BUTTON_SHORT);
        }
      }

      void longButtonPress() {
        m5DialDisplay->resetLastEventTimer();

        if (navigation_mode_.is_navigation_mode()) {
          navigation_mode_.exit_navigation_mode();
        } else {
          navigation_mode_.enter_navigation_mode();
        }

        refreshDisplay(true);
      }

      void touchInput(uint16_t x, uint16_t y) {
        m5DialDisplay->resetLastEventTimer();
        if (m5DialDisplay->isDisplayOn()) {
          devices[currentDevice]->onTouch(*m5DialDisplay, x, y);
        }
      }

      void touchSwipe(const char* direction) {
        m5DialDisplay->resetLastEventTimer();
        if (!navigation_mode_.is_navigation_mode() && m5DialDisplay->isDisplayOn()) {
          if (!devices[currentDevice]->onSwipe(*m5DialDisplay, direction)) {
            if (strcmp(direction, TOUCH_SWIPE_LEFT) == 0) {
              this->previousDevice();
            } else if (strcmp(direction, TOUCH_SWIPE_RIGHT) == 0) {
              this->nextDevice();
            } else if (strcmp(direction, TOUCH_SWIPE_UP) == 0) {
              devices[currentDevice]->previousMode();
            } else if (strcmp(direction, TOUCH_SWIPE_DOWN) == 0) {
              devices[currentDevice]->nextMode();
            }
          }
        }
      }
    };
  }
}
