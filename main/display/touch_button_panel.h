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
    void SetAbortButtonCallback(std::function<void()> callback);
    void SetBrightnessChangeCallback(std::function<void(int)> callback);
    void SetVolumeChangeCallback(std::function<void(int)> callback);
    void SetRotationChangeCallback(std::function<void()> callback);

    // Get/Set current values
    int GetBrightness() const;
    void SetBrightness(int value);
    int GetVolume() const;
    void SetVolume(int value);

private:
    lv_obj_t* parent_ = nullptr;
    
    // 设置按钮（右上角）
    lv_obj_t* settings_btn_ = nullptr;
    lv_obj_t* settings_icon_ = nullptr;
    
    // 设置弹出面板
    lv_obj_t* settings_popup_ = nullptr;
    lv_obj_t* brightness_container_ = nullptr;
    lv_obj_t* brightness_icon_ = nullptr;
    lv_obj_t* brightness_slider_ = nullptr;
    lv_obj_t* brightness_label_ = nullptr;
    lv_obj_t* volume_container_ = nullptr;
    lv_obj_t* volume_icon_ = nullptr;
    lv_obj_t* volume_slider_ = nullptr;
    lv_obj_t* volume_label_ = nullptr;
    
    // 旋转按钮
    lv_obj_t* rotation_btn_ = nullptr;
    lv_obj_t* rotation_icon_ = nullptr;
    
    // 唤醒/对话按钮（底部中间）
    lv_obj_t* speech_btn_ = nullptr;
    lv_obj_t* speech_icon_ = nullptr;
    
    // 打断按钮（底部，对话时显示）
    lv_obj_t* abort_btn_ = nullptr;
    lv_obj_t* abort_icon_ = nullptr;

    int width_ = 0;
    int height_ = 0;
    DeviceState current_state_ = kDeviceStateUnknown;
    bool visible_ = false;
    bool settings_popup_visible_ = false;

    std::function<void()> speech_callback_;
    std::function<void()> abort_callback_;
    std::function<void(int)> brightness_callback_;
    std::function<void(int)> volume_callback_;
    std::function<void()> rotation_callback_;

    void CreateSettingsButton();
    void CreateSettingsPopup();
    void CreateSpeechButton();
    void CreateAbortButton();
    void CreateBrightnessSlider();
    void CreateVolumeSlider();
    void CreateRotationButton();
    void UpdateSpeechButtonState();
    void UpdateVolumeIcon();
    void ToggleSettingsPopup();

    // Static event callbacks
    static void OnSettingsButtonClicked(lv_event_t* e);
    static void OnSpeechButtonClicked(lv_event_t* e);
    static void OnAbortButtonClicked(lv_event_t* e);
    static void OnBrightnessChanged(lv_event_t* e);
    static void OnBrightnessReleased(lv_event_t* e);
    static void OnVolumeChanged(lv_event_t* e);
    static void OnVolumeReleased(lv_event_t* e);
    static void OnRotationButtonClicked(lv_event_t* e);
};

#endif // TOUCH_BUTTON_PANEL_H
