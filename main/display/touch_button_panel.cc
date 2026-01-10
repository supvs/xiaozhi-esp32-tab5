#include "touch_button_panel.h"
#include "touch_button_settings.h"
#include <esp_log.h>
#include <font_awesome.h>

#define TAG "TouchButtonPanel"

TouchButtonPanel::TouchButtonPanel(lv_obj_t* parent, int width, int height)
    : width_(width), height_(height) {
    CreatePanel(parent);
    CreateSpeechButton();
    CreateBrightnessSlider();
    CreateVolumeSlider();
    
    // Initially hidden
    Hide();
}

TouchButtonPanel::~TouchButtonPanel() {
    if (panel_ != nullptr) {
        lv_obj_del(panel_);
        panel_ = nullptr;
    }
}

void TouchButtonPanel::CreatePanel(lv_obj_t* parent) {
    panel_ = lv_obj_create(parent);
    
    // Panel size - bottom bar style
    int panel_height = height_ / 6;  // 1/6 of screen height
    lv_obj_set_size(panel_, width_, panel_height);
    lv_obj_align(panel_, LV_ALIGN_BOTTOM_MID, 0, 0);
    
    // Panel style
    lv_obj_set_style_bg_color(panel_, lv_color_hex(0x2D2D2D), 0);
    lv_obj_set_style_bg_opa(panel_, LV_OPA_90, 0);
    lv_obj_set_style_radius(panel_, 12, 0);
    lv_obj_set_style_border_width(panel_, 0, 0);
    lv_obj_set_style_pad_all(panel_, 8, 0);
    
    // Flex layout - horizontal
    lv_obj_set_flex_flow(panel_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(panel_, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(panel_, LV_SCROLLBAR_MODE_OFF);
}

void TouchButtonPanel::CreateSpeechButton() {
    speech_btn_ = lv_btn_create(panel_);
    lv_obj_set_size(speech_btn_, 50, 50);
    lv_obj_set_style_radius(speech_btn_, 25, 0);  // Circular button
    lv_obj_set_style_bg_color(speech_btn_, lv_color_hex(0x4CAF50), 0);  // Green
    lv_obj_set_style_bg_color(speech_btn_, lv_color_hex(0x388E3C), LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(speech_btn_, lv_color_hex(0x757575), LV_STATE_DISABLED);
    
    // Microphone icon
    speech_icon_ = lv_label_create(speech_btn_);
    lv_label_set_text(speech_icon_, FONT_AWESOME_MICROPHONE);
    lv_obj_set_style_text_color(speech_icon_, lv_color_white(), 0);
    lv_obj_center(speech_icon_);
    
    // Click event
    lv_obj_add_event_cb(speech_btn_, OnSpeechButtonClicked, LV_EVENT_CLICKED, this);
}


void TouchButtonPanel::CreateBrightnessSlider() {
    brightness_container_ = lv_obj_create(panel_);
    lv_obj_set_size(brightness_container_, (width_ - 100) / 2, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(brightness_container_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(brightness_container_, 0, 0);
    lv_obj_set_style_pad_all(brightness_container_, 4, 0);
    lv_obj_set_flex_flow(brightness_container_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(brightness_container_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(brightness_container_, LV_SCROLLBAR_MODE_OFF);
    
    // Sun icon
    brightness_icon_ = lv_label_create(brightness_container_);
    lv_label_set_text(brightness_icon_, FONT_AWESOME_SUN);
    lv_obj_set_style_text_color(brightness_icon_, lv_color_hex(0xFFD700), 0);  // Gold
    
    // Slider
    brightness_slider_ = lv_slider_create(brightness_container_);
    lv_obj_set_flex_grow(brightness_slider_, 1);
    lv_obj_set_height(brightness_slider_, 8);
    lv_slider_set_range(brightness_slider_, TouchButtonSettings::MIN_BRIGHTNESS, TouchButtonSettings::MAX_BRIGHTNESS);
    lv_slider_set_value(brightness_slider_, TouchButtonSettings::DEFAULT_BRIGHTNESS, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(brightness_slider_, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_bg_color(brightness_slider_, lv_color_hex(0xFFD700), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(brightness_slider_, lv_color_hex(0xFFD700), LV_PART_KNOB);
    lv_obj_set_style_pad_all(brightness_slider_, 3, LV_PART_KNOB);
    
    // Percentage label
    brightness_label_ = lv_label_create(brightness_container_);
    lv_label_set_text(brightness_label_, "80%");
    lv_obj_set_style_text_color(brightness_label_, lv_color_white(), 0);
    lv_obj_set_style_min_width(brightness_label_, 40, 0);
    lv_obj_set_style_text_align(brightness_label_, LV_TEXT_ALIGN_RIGHT, 0);
    
    // Events
    lv_obj_add_event_cb(brightness_slider_, OnBrightnessChanged, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_add_event_cb(brightness_slider_, OnBrightnessReleased, LV_EVENT_RELEASED, this);
}

void TouchButtonPanel::CreateVolumeSlider() {
    volume_container_ = lv_obj_create(panel_);
    lv_obj_set_size(volume_container_, (width_ - 100) / 2, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(volume_container_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(volume_container_, 0, 0);
    lv_obj_set_style_pad_all(volume_container_, 4, 0);
    lv_obj_set_flex_flow(volume_container_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(volume_container_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(volume_container_, LV_SCROLLBAR_MODE_OFF);
    
    // Speaker icon
    volume_icon_ = lv_label_create(volume_container_);
    lv_label_set_text(volume_icon_, FONT_AWESOME_VOLUME_HIGH);
    lv_obj_set_style_text_color(volume_icon_, lv_color_hex(0x2196F3), 0);  // Blue
    
    // Slider
    volume_slider_ = lv_slider_create(volume_container_);
    lv_obj_set_flex_grow(volume_slider_, 1);
    lv_obj_set_height(volume_slider_, 8);
    lv_slider_set_range(volume_slider_, TouchButtonSettings::MIN_VOLUME, TouchButtonSettings::MAX_VOLUME);
    lv_slider_set_value(volume_slider_, TouchButtonSettings::DEFAULT_VOLUME, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(volume_slider_, lv_color_hex(0x555555), LV_PART_MAIN);
    lv_obj_set_style_bg_color(volume_slider_, lv_color_hex(0x2196F3), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(volume_slider_, lv_color_hex(0x2196F3), LV_PART_KNOB);
    lv_obj_set_style_pad_all(volume_slider_, 3, LV_PART_KNOB);
    
    // Percentage label
    volume_label_ = lv_label_create(volume_container_);
    lv_label_set_text(volume_label_, "70%");
    lv_obj_set_style_text_color(volume_label_, lv_color_white(), 0);
    lv_obj_set_style_min_width(volume_label_, 40, 0);
    lv_obj_set_style_text_align(volume_label_, LV_TEXT_ALIGN_RIGHT, 0);
    
    // Events
    lv_obj_add_event_cb(volume_slider_, OnVolumeChanged, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_add_event_cb(volume_slider_, OnVolumeReleased, LV_EVENT_RELEASED, this);
}

void TouchButtonPanel::Show() {
    if (panel_ != nullptr) {
        lv_obj_clear_flag(panel_, LV_OBJ_FLAG_HIDDEN);
        visible_ = true;
        ESP_LOGI(TAG, "Panel shown");
    }
}

void TouchButtonPanel::Hide() {
    if (panel_ != nullptr) {
        lv_obj_add_flag(panel_, LV_OBJ_FLAG_HIDDEN);
        visible_ = false;
        ESP_LOGI(TAG, "Panel hidden");
    }
}

bool TouchButtonPanel::IsVisible() const {
    return visible_;
}

void TouchButtonPanel::UpdateDeviceState(DeviceState state) {
    current_state_ = state;
    UpdateSpeechButtonState();
    
    // Show/hide panel based on state
    if (state == kDeviceStateIdle || state == kDeviceStateConnecting) {
        Show();
    } else if (state == kDeviceStateListening || state == kDeviceStateSpeaking) {
        Hide();
    }
}

void TouchButtonPanel::UpdateSpeechButtonState() {
    if (speech_btn_ == nullptr || speech_icon_ == nullptr) {
        return;
    }
    
    switch (current_state_) {
        case kDeviceStateIdle:
            lv_obj_clear_state(speech_btn_, LV_STATE_DISABLED);
            lv_label_set_text(speech_icon_, FONT_AWESOME_MICROPHONE);
            lv_obj_set_style_bg_color(speech_btn_, lv_color_hex(0x4CAF50), 0);
            break;
        case kDeviceStateListening:
            lv_obj_clear_state(speech_btn_, LV_STATE_DISABLED);
            lv_label_set_text(speech_icon_, FONT_AWESOME_STOP);
            lv_obj_set_style_bg_color(speech_btn_, lv_color_hex(0xF44336), 0);  // Red
            break;
        case kDeviceStateSpeaking:
            lv_obj_add_state(speech_btn_, LV_STATE_DISABLED);
            lv_label_set_text(speech_icon_, FONT_AWESOME_COMMENT);
            break;
        case kDeviceStateConnecting:
            lv_obj_add_state(speech_btn_, LV_STATE_DISABLED);
            lv_label_set_text(speech_icon_, FONT_AWESOME_SPINNER);
            break;
        default:
            lv_obj_add_state(speech_btn_, LV_STATE_DISABLED);
            break;
    }
}


void TouchButtonPanel::UpdateVolumeIcon() {
    if (volume_icon_ == nullptr || volume_slider_ == nullptr) {
        return;
    }
    
    int volume = lv_slider_get_value(volume_slider_);
    if (volume == 0) {
        lv_label_set_text(volume_icon_, FONT_AWESOME_VOLUME_XMARK);
    } else if (volume < 33) {
        lv_label_set_text(volume_icon_, FONT_AWESOME_VOLUME_LOW);
    } else if (volume < 66) {
        lv_label_set_text(volume_icon_, FONT_AWESOME_VOLUME_LOW);
    } else {
        lv_label_set_text(volume_icon_, FONT_AWESOME_VOLUME_HIGH);
    }
}

void TouchButtonPanel::SetSpeechButtonCallback(std::function<void()> callback) {
    speech_callback_ = callback;
}

void TouchButtonPanel::SetBrightnessChangeCallback(std::function<void(int)> callback) {
    brightness_callback_ = callback;
}

void TouchButtonPanel::SetVolumeChangeCallback(std::function<void(int)> callback) {
    volume_callback_ = callback;
}

int TouchButtonPanel::GetBrightness() const {
    if (brightness_slider_ != nullptr) {
        return lv_slider_get_value(brightness_slider_);
    }
    return TouchButtonSettings::DEFAULT_BRIGHTNESS;
}

void TouchButtonPanel::SetBrightness(int value) {
    if (brightness_slider_ != nullptr) {
        int clamped = TouchButtonSettings::ClampBrightness(value);
        lv_slider_set_value(brightness_slider_, clamped, LV_ANIM_OFF);
        
        if (brightness_label_ != nullptr) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%d%%", clamped);
            lv_label_set_text(brightness_label_, buf);
        }
    }
}

int TouchButtonPanel::GetVolume() const {
    if (volume_slider_ != nullptr) {
        return lv_slider_get_value(volume_slider_);
    }
    return TouchButtonSettings::DEFAULT_VOLUME;
}

void TouchButtonPanel::SetVolume(int value) {
    if (volume_slider_ != nullptr) {
        int clamped = TouchButtonSettings::ClampVolume(value);
        lv_slider_set_value(volume_slider_, clamped, LV_ANIM_OFF);
        
        if (volume_label_ != nullptr) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%d%%", clamped);
            lv_label_set_text(volume_label_, buf);
        }
        
        UpdateVolumeIcon();
    }
}

// Static event callbacks
void TouchButtonPanel::OnSpeechButtonClicked(lv_event_t* e) {
    TouchButtonPanel* panel = static_cast<TouchButtonPanel*>(lv_event_get_user_data(e));
    if (panel != nullptr && panel->speech_callback_) {
        ESP_LOGI(TAG, "Speech button clicked");
        panel->speech_callback_();
    }
}

void TouchButtonPanel::OnBrightnessChanged(lv_event_t* e) {
    TouchButtonPanel* panel = static_cast<TouchButtonPanel*>(lv_event_get_user_data(e));
    if (panel != nullptr) {
        int value = lv_slider_get_value(panel->brightness_slider_);
        
        // Update label
        if (panel->brightness_label_ != nullptr) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%d%%", value);
            lv_label_set_text(panel->brightness_label_, buf);
        }
        
        // Call callback for real-time adjustment
        if (panel->brightness_callback_) {
            panel->brightness_callback_(value);
        }
    }
}

void TouchButtonPanel::OnBrightnessReleased(lv_event_t* e) {
    TouchButtonPanel* panel = static_cast<TouchButtonPanel*>(lv_event_get_user_data(e));
    if (panel != nullptr) {
        int value = lv_slider_get_value(panel->brightness_slider_);
        ESP_LOGI(TAG, "Brightness released at %d", value);
        
        // Save to settings
        TouchButtonSettings settings;
        settings.SetBrightness(value);
    }
}

void TouchButtonPanel::OnVolumeChanged(lv_event_t* e) {
    TouchButtonPanel* panel = static_cast<TouchButtonPanel*>(lv_event_get_user_data(e));
    if (panel != nullptr) {
        int value = lv_slider_get_value(panel->volume_slider_);
        
        // Update label
        if (panel->volume_label_ != nullptr) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%d%%", value);
            lv_label_set_text(panel->volume_label_, buf);
        }
        
        // Update icon
        panel->UpdateVolumeIcon();
        
        // Call callback for real-time adjustment
        if (panel->volume_callback_) {
            panel->volume_callback_(value);
        }
    }
}

void TouchButtonPanel::OnVolumeReleased(lv_event_t* e) {
    TouchButtonPanel* panel = static_cast<TouchButtonPanel*>(lv_event_get_user_data(e));
    if (panel != nullptr) {
        int value = lv_slider_get_value(panel->volume_slider_);
        ESP_LOGI(TAG, "Volume released at %d", value);
        
        // Save to settings
        TouchButtonSettings settings;
        settings.SetVolume(value);
    }
}
