#ifndef TOUCH_BUTTON_SETTINGS_H
#define TOUCH_BUTTON_SETTINGS_H

#include "settings.h"

class TouchButtonSettings {
public:
    static constexpr int DEFAULT_BRIGHTNESS = 80;
    static constexpr int DEFAULT_VOLUME = 70;
    static constexpr int MIN_BRIGHTNESS = 10;
    static constexpr int MAX_BRIGHTNESS = 100;
    static constexpr int MIN_VOLUME = 0;
    static constexpr int MAX_VOLUME = 100;

    TouchButtonSettings();
    ~TouchButtonSettings() = default;

    int GetBrightness() const;
    void SetBrightness(int value);

    int GetVolume() const;
    void SetVolume(int value);

    // Clamp functions exposed for testing
    static int ClampBrightness(int value);
    static int ClampVolume(int value);

private:
    static constexpr const char* NAMESPACE = "touch_btn";
    static constexpr const char* KEY_BRIGHTNESS = "brightness";
    static constexpr const char* KEY_VOLUME = "volume";
};

#endif // TOUCH_BUTTON_SETTINGS_H
