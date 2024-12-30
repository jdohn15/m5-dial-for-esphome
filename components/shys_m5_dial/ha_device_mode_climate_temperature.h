#pragma once

namespace esphome
{
    namespace shys_m5_dial
    {
        class HaDeviceModeClimateTemperature : public esphome::shys_m5_dial::HaDeviceMode
        {
        protected:
            std::string hvac_mode = "none";
            int current_temperature_ = 0; // Variable to store the current temperature
            const std::string sensor_entity_id_ = "sensor.dining_room_temperature_2"; // Constant for sensor entity ID

            std::string getHvacMode()
            {
                return this->hvac_mode;
            }

            void setHvacMode(const std::string &newMode)
            {
                this->hvac_mode = newMode;
            }

            void sendValueToHomeAssistant(int value) override
            {
                haApi.setClimateTemperature(this->device.getEntityId(), value);
            }

            void showTemperatureMenu(M5DialDisplay &display)
            {
                LovyanGFX *gfx = display.getGfx();

                uint16_t setTemperature = getValue(); // The set temperature

                uint16_t height = gfx->height();
                uint16_t width = gfx->width();

                gfx->setTextColor(MAROON);
                gfx->setTextDatum(middle_center);

                gfx->startWrite(); // Secure SPI bus

                gfx->fillRect(0, 0, width, this->getDisplayPositionY(setTemperature), RED);
                gfx->fillRect(0, this->getDisplayPositionY(setTemperature), width, height, YELLOW);

                display.setFontsize(1);

                // Display the set temperature
                gfx->drawString(("Set: " + String(setTemperature) + "F").c_str(),
                                width / 2,
                                height / 2 - 30);

                // Display the current temperature from the sensor
                gfx->drawString(("Current: " + String(this->current_temperature_) + "F").c_str(),
                                width / 2,
                                height / 2 - 60);

                display.setFontsize(1);

                gfx->drawString(this->device.getName().c_str(),
                                width / 2,
                                height / 2 + 20);
                gfx->drawString("Temperature",
                                width / 2,
                                height / 2 + 50);

                gfx->endWrite(); // Release SPI bus
            }

        public:
            HaDeviceModeClimateTemperature(HaDevice &device) : HaDeviceMode(device)
            {
                this->maxValue = 80;
            }

            void refreshDisplay(M5DialDisplay &display, bool init) override
            {
                this->showTemperatureMenu(display);
                ESP_LOGD("DISPLAY", "Temperature-Modus");
            }

            void registerHAListener() override
            {
                // Subscribe to HVAC mode updates
                std::string attrNameState = "";
                api::global_api_server->subscribe_home_assistant_state(
                    this->device.getEntityId().c_str(),
                    esphome::optional<std::string>(attrNameState),
                    [this](const std::string &state)
                    {
                        if (this->isValueModified())
                        {
                            return;
                        }

                        this->setHvacMode(state.c_str());
                        ESP_LOGI("HA_API", "Got Mode %s for %s", state.c_str(), this->device.getEntityId().c_str());
                    });

                // Subscribe to the current temperature sensor
                api::global_api_server->subscribe_home_assistant_state(
                    this->sensor_entity_id_,
                    esphome::optional<std::string>(),
                    [this](const std::string &state)
                    {
                        auto val = parse_number<float>(state);

                        if (!val.has_value())
                        {
                            this->current_temperature_ = 0; // Reset if no valid value
                            ESP_LOGD("HA_API", "No valid temperature value in %s for %s", state.c_str(), this->sensor_entity_id_.c_str());
                        }
                        else
                        {
                            this->current_temperature_ = int(val.value());
                            ESP_LOGI("HA_API", "Got current temperature value %i from %s", int(val.value()), this->sensor_entity_id_.c_str());
                        }
                    });

                // Subscribe to the set temperature
                std::string attrNameTemp = "temperature";
                api::global_api_server->subscribe_home_assistant_state(
                    this->device.getEntityId().c_str(),
                    esphome::optional<std::string>(attrNameTemp),
                    [this](const std::string &state)
                    {
                        auto val = parse_number<float>(state);

                        if (!val.has_value())
                        {
                            this->setReceivedValue(0);
                            ESP_LOGD("HA_API", "No Temperature value in %s for %s", state.c_str(), this->device.getEntityId().c_str());
                        }
                        else
                        {
                            this->setReceivedValue(int(val.value()));
                            ESP_LOGI("HA_API", "Got Temperature value %i for %s", int(val.value()), this->device.getEntityId().c_str());
                        }
                    });
            }

            bool onTouch(M5DialDisplay &display, uint16_t x, uint16_t y) override
            {
                return defaultOnTouch(display, x, y);
            }

            bool onRotary(M5DialDisplay &display, const char *direction) override
            {
                return defaultOnRotary(display, direction);
            }

            bool onButton(M5DialDisplay &display, const char *clickType) override
            {
                if (strcmp(clickType, BUTTON_SHORT) == 0)
                {
                    if (strcmp(this->getHvacMode().c_str(), "off") == 0)
                    {
                        haApi.turnClimateOn(this->device.getEntityId(), "heat");
                    }
                    else
                    {
                        haApi.turnClimateOff(this->device.getEntityId());
                    }

                    return true;
                }
                return false;
            }
        };
    }
}
