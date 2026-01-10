#include "touch_button_settings.h"
#include <esp_log.h>

#define TAG "TouchButtonSettings"

TouchButtonSettings::TouchButtonSettings() {
}

int TouchButtonSettings::ClampBrightness(int value) {
    if (value < MIN_BRIGHTNESS) {
        return MIN_BRIGHTNESS;
    }
    if (value > MAX_BRIGHTNESS) {
        return MAX_BRIGHTNESS;
    }
    return value;
}

int TouchButtonSettings::ClampVolume(int value) {
    if (value < MIN_VOLUME) {
        return MIN_VOLUME;
    }
    if (value > MAX_VOLUME) {
        return MAX_VOLUME;
    }
    return value;
}

int TouchButtonSettings::GetBrightness() const {
    Settings settings(NAMESPACE, false);
    int value = settings.GetInt(KEY_BRIGHTNESS, DEFAULT_BRIGHTNESS);
    return ClampBrightness(value);
}

void TouchButtonSettings::SetBrightness(int value) {
    int clamped = ClampBrightness(value);
    Settings settings(NAMESPACE, true);
    settings.SetInt(KEY_BRIGHTNESS, clamped);
    ESP_LOGI(TAG, "Brightness set to %d", clamped);
}

int TouchButtonSettings::GetVolume() const {
    Settings settings(NAMESPACE, false);
    int value = settings.GetInt(KEY_VOLUME, DEFAULT_VOLUME);
    return ClampVolume(value);
}

void TouchButtonSettings::SetVolume(int value) {
    int clamped = ClampVolume(value);
    Settings settings(NAMESPACE, true);
    settings.SetInt(KEY_VOLUME, clamped);
    ESP_LOGI(TAG, "Volume set to %d", clamped);
}
