#ifndef TOUCH_BUTTON_PANEL_H
#define TOUCH_BUTTON_PANEL_H

#include "device_state.h"
#include <lvgl.h>
#include <functional>

class TouchButtonPanel {
public:
    TouchButtonPanel(lv_obj_t* parent, int width, int height);
    ~TouchButtonPanel();

    // Show/Hide panel
    void Show();
    void Hide();
    bool IsVisible() const;

    // Update device state (for button state updates)
    void UpdateDeviceState(DeviceState state);

    // Set callbacks
    void SetSpeechButtonCallback(std::function<void()> callback);
    void SetBrightnessChangeCallback(std::function<void(int)> callback);
    void SetVolumeChangeCallback(std::function<void(int)> callback);

    // Get/Set current values
    int GetBrightness() const;
    void SetBrightness(int value);
    int GetVolume() const;
    void SetVolume(int value);

private:
    lv_obj_t* panel_ = nullptr;
    lv_obj_t* speech_btn_ = nullptr;
    lv_obj_t* speech_icon_ = nullptr;
    lv_obj_t* brightness_container_ = nullptr;
    lv_obj_t* brightness_icon_ = nullptr;
    lv_obj_t* brightness_slider_ = nullptr;
    lv_obj_t* brightness_label_ = nullptr;
    lv_obj_t* volume_container_ = nullptr;
    lv_obj_t* volume_icon_ = nullptr;
    lv_obj_t* volume_slider_ = nullptr;
    lv_obj_t* volume_label_ = nullptr;

    int width_ = 0;
    int height_ = 0;
    DeviceState current_state_ = kDeviceStateUnknown;
    bool visible_ = false;

    std::function<void()> speech_callback_;
    std::function<void(int)> brightness_callback_;
    std::function<void(int)> volume_callback_;

    void CreatePanel(lv_obj_t* parent);
    void CreateSpeechButton();
    void CreateBrightnessSlider();
    void CreateVolumeSlider();
    void UpdateSpeechButtonState();
    void UpdateVolumeIcon();

    // Static event callbacks
    static void OnSpeechButtonClicked(lv_event_t* e);
    static void OnBrightnessChanged(lv_event_t* e);
    static void OnBrightnessReleased(lv_event_t* e);
    static void OnVolumeChanged(lv_event_t* e);
    static void OnVolumeReleased(lv_event_t* e);
};

#endif // TOUCH_BUTTON_PANEL_H
