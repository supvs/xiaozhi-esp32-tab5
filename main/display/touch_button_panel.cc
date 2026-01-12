#include "touch_button_panel.h"
#include "touch_button_settings.h"
#include <esp_log.h>
#include <font_awesome.h>

#define TAG "TouchButtonPanel"

// Declare the Font Awesome font
LV_FONT_DECLARE(font_awesome_30_4);

TouchButtonPanel::TouchButtonPanel(lv_obj_t* parent, int width, int height)
    : parent_(parent), width_(width), height_(height) {
    CreateSettingsButton();
    CreateSettingsPopup();
    CreateSpeechButton();
    CreateAbortButton();
    
    // Initially hidden
    Hide();
}

TouchButtonPanel::~TouchButtonPanel() {
    if (settings_btn_ != nullptr) {
        lv_obj_del(settings_btn_);
        settings_btn_ = nullptr;
    }
    if (settings_popup_ != nullptr) {
        lv_obj_del(settings_popup_);
        settings_popup_ = nullptr;
    }
    if (speech_btn_ != nullptr) {
        lv_obj_del(speech_btn_);
        speech_btn_ = nullptr;
    }
    if (abort_btn_ != nullptr) {
        lv_obj_del(abort_btn_);
        abort_btn_ = nullptr;
    }
}

void TouchButtonPanel::CreateSettingsButton() {
    // 右上角设置按钮
    settings_btn_ = lv_btn_create(parent_);
    lv_obj_set_size(settings_btn_, 56, 56);
    lv_obj_set_style_radius(settings_btn_, 28, 0);
    lv_obj_align(settings_btn_, LV_ALIGN_TOP_RIGHT, -12, 40);
    
    // 半透明背景
    lv_obj_set_style_bg_color(settings_btn_, lv_color_hex(0x404040), 0);
    lv_obj_set_style_bg_opa(settings_btn_, LV_OPA_80, 0);
    lv_obj_set_style_bg_color(settings_btn_, lv_color_hex(0x606060), LV_STATE_PRESSED);
    lv_obj_set_style_border_width(settings_btn_, 0, 0);
    lv_obj_set_style_shadow_width(settings_btn_, 8, 0);
    lv_obj_set_style_shadow_opa(settings_btn_, LV_OPA_30, 0);
    
    // 齿轮图标 - 使用 Font Awesome 字体
    settings_icon_ = lv_label_create(settings_btn_);
    lv_label_set_text(settings_icon_, FONT_AWESOME_GEAR);
    lv_obj_set_style_text_color(settings_icon_, lv_color_white(), 0);
    lv_obj_set_style_text_font(settings_icon_, &font_awesome_30_4, 0);
    lv_obj_center(settings_icon_);
    
    lv_obj_add_event_cb(settings_btn_, OnSettingsButtonClicked, LV_EVENT_CLICKED, this);
}

void TouchButtonPanel::CreateSettingsPopup() {
    // 设置弹出面板 - 自适应屏幕宽度
    settings_popup_ = lv_obj_create(parent_);
    
    int popup_width = width_ * 65 / 100;  // 65% 屏幕宽度
    int popup_height = height_ / 3;        // 1/3 屏幕高度 (increased for rotation button)
    lv_obj_set_size(settings_popup_, popup_width, popup_height);
    lv_obj_align(settings_popup_, LV_ALIGN_TOP_RIGHT, -8, 100);
    
    // 面板样式
    lv_obj_set_style_bg_color(settings_popup_, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_bg_opa(settings_popup_, LV_OPA_90, 0);
    lv_obj_set_style_radius(settings_popup_, 20, 0);
    lv_obj_set_style_border_width(settings_popup_, 1, 0);
    lv_obj_set_style_border_color(settings_popup_, lv_color_hex(0x444444), 0);
    lv_obj_set_style_pad_all(settings_popup_, 20, 0);
    lv_obj_set_style_shadow_width(settings_popup_, 20, 0);
    lv_obj_set_style_shadow_opa(settings_popup_, LV_OPA_50, 0);
    lv_obj_set_style_shadow_color(settings_popup_, lv_color_hex(0x000000), 0);
    
    // 垂直布局
    lv_obj_set_flex_flow(settings_popup_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(settings_popup_, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(settings_popup_, LV_SCROLLBAR_MODE_OFF);
    
    // 创建滑块和按钮
    CreateBrightnessSlider();
    CreateVolumeSlider();
    CreateRotationButton();
    
    // 默认隐藏
    lv_obj_add_flag(settings_popup_, LV_OBJ_FLAG_HIDDEN);
}

void TouchButtonPanel::CreateSpeechButton() {
    // 底部中间唤醒按钮
    speech_btn_ = lv_btn_create(parent_);
    lv_obj_set_size(speech_btn_, 90, 90);
    lv_obj_set_style_radius(speech_btn_, 45, 0);
    lv_obj_align(speech_btn_, LV_ALIGN_BOTTOM_MID, 0, -20);
    
    // 按钮样式 - 绿色
    lv_obj_set_style_bg_color(speech_btn_, lv_color_hex(0x4CAF50), 0);
    lv_obj_set_style_bg_color(speech_btn_, lv_color_hex(0x388E3C), LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(speech_btn_, lv_color_hex(0x666666), LV_STATE_DISABLED);
    lv_obj_set_style_border_width(speech_btn_, 0, 0);
    lv_obj_set_style_shadow_width(speech_btn_, 20, 0);
    lv_obj_set_style_shadow_color(speech_btn_, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(speech_btn_, LV_OPA_50, 0);
    
    // 麦克风图标 - 使用 Font Awesome 字体
    speech_icon_ = lv_label_create(speech_btn_);
    lv_label_set_text(speech_icon_, FONT_AWESOME_MICROPHONE);
    lv_obj_set_style_text_color(speech_icon_, lv_color_white(), 0);
    lv_obj_set_style_text_font(speech_icon_, &font_awesome_30_4, 0);
    lv_obj_center(speech_icon_);
    
    lv_obj_add_event_cb(speech_btn_, OnSpeechButtonClicked, LV_EVENT_CLICKED, this);
}

void TouchButtonPanel::CreateAbortButton() {
    // 打断按钮 - 底部中间，对话时显示
    abort_btn_ = lv_btn_create(parent_);
    lv_obj_set_size(abort_btn_, 90, 90);
    lv_obj_set_style_radius(abort_btn_, 45, 0);
    lv_obj_align(abort_btn_, LV_ALIGN_BOTTOM_MID, 0, -20);
    
    // 按钮样式 - 红色
    lv_obj_set_style_bg_color(abort_btn_, lv_color_hex(0xF44336), 0);
    lv_obj_set_style_bg_color(abort_btn_, lv_color_hex(0xD32F2F), LV_STATE_PRESSED);
    lv_obj_set_style_border_width(abort_btn_, 0, 0);
    lv_obj_set_style_shadow_width(abort_btn_, 20, 0);
    lv_obj_set_style_shadow_color(abort_btn_, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(abort_btn_, LV_OPA_50, 0);
    
    // 停止图标 - 使用 Font Awesome 字体
    abort_icon_ = lv_label_create(abort_btn_);
    lv_label_set_text(abort_icon_, FONT_AWESOME_STOP);
    lv_obj_set_style_text_color(abort_icon_, lv_color_white(), 0);
    lv_obj_set_style_text_font(abort_icon_, &font_awesome_30_4, 0);
    lv_obj_center(abort_icon_);
    
    lv_obj_add_event_cb(abort_btn_, OnAbortButtonClicked, LV_EVENT_CLICKED, this);
    
    // 默认隐藏
    lv_obj_add_flag(abort_btn_, LV_OBJ_FLAG_HIDDEN);
}

void TouchButtonPanel::CreateBrightnessSlider() {
    brightness_container_ = lv_obj_create(settings_popup_);
    lv_obj_set_size(brightness_container_, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(brightness_container_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(brightness_container_, 0, 0);
    lv_obj_set_style_pad_all(brightness_container_, 8, 0);
    lv_obj_set_flex_flow(brightness_container_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(brightness_container_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(brightness_container_, LV_SCROLLBAR_MODE_OFF);
    
    // 太阳图标 - 使用 Font Awesome 字体
    brightness_icon_ = lv_label_create(brightness_container_);
    lv_label_set_text(brightness_icon_, FONT_AWESOME_SUN);
    lv_obj_set_style_text_color(brightness_icon_, lv_color_hex(0xFFD700), 0);
    lv_obj_set_style_text_font(brightness_icon_, &font_awesome_30_4, 0);
    lv_obj_set_style_pad_right(brightness_icon_, 12, 0);
    
    // 滑块 - 更大更易操作
    brightness_slider_ = lv_slider_create(brightness_container_);
    lv_obj_set_flex_grow(brightness_slider_, 1);
    lv_obj_set_height(brightness_slider_, 20);
    lv_slider_set_range(brightness_slider_, TouchButtonSettings::MIN_BRIGHTNESS, TouchButtonSettings::MAX_BRIGHTNESS);
    lv_slider_set_value(brightness_slider_, TouchButtonSettings::DEFAULT_BRIGHTNESS, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(brightness_slider_, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_bg_color(brightness_slider_, lv_color_hex(0xFFD700), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(brightness_slider_, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
    lv_obj_set_style_pad_all(brightness_slider_, 8, LV_PART_KNOB);
    lv_obj_set_style_radius(brightness_slider_, 10, LV_PART_MAIN);
    lv_obj_set_style_radius(brightness_slider_, 10, LV_PART_INDICATOR);
    
    // 百分比标签
    brightness_label_ = lv_label_create(brightness_container_);
    lv_label_set_text(brightness_label_, "80%");
    lv_obj_set_style_text_color(brightness_label_, lv_color_white(), 0);
    lv_obj_set_style_min_width(brightness_label_, 60, 0);
    lv_obj_set_style_text_align(brightness_label_, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_pad_left(brightness_label_, 12, 0);
    
    lv_obj_add_event_cb(brightness_slider_, OnBrightnessChanged, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_add_event_cb(brightness_slider_, OnBrightnessReleased, LV_EVENT_RELEASED, this);
}

void TouchButtonPanel::CreateVolumeSlider() {
    volume_container_ = lv_obj_create(settings_popup_);
    lv_obj_set_size(volume_container_, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(volume_container_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(volume_container_, 0, 0);
    lv_obj_set_style_pad_all(volume_container_, 8, 0);
    lv_obj_set_flex_flow(volume_container_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(volume_container_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(volume_container_, LV_SCROLLBAR_MODE_OFF);
    
    // 喇叭图标 - 使用 Font Awesome 字体
    volume_icon_ = lv_label_create(volume_container_);
    lv_label_set_text(volume_icon_, FONT_AWESOME_VOLUME_HIGH);
    lv_obj_set_style_text_color(volume_icon_, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_text_font(volume_icon_, &font_awesome_30_4, 0);
    lv_obj_set_style_pad_right(volume_icon_, 12, 0);
    
    // 滑块 - 更大更易操作
    volume_slider_ = lv_slider_create(volume_container_);
    lv_obj_set_flex_grow(volume_slider_, 1);
    lv_obj_set_height(volume_slider_, 20);
    lv_slider_set_range(volume_slider_, TouchButtonSettings::MIN_VOLUME, TouchButtonSettings::MAX_VOLUME);
    lv_slider_set_value(volume_slider_, TouchButtonSettings::DEFAULT_VOLUME, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(volume_slider_, lv_color_hex(0x444444), LV_PART_MAIN);
    lv_obj_set_style_bg_color(volume_slider_, lv_color_hex(0x2196F3), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(volume_slider_, lv_color_hex(0xFFFFFF), LV_PART_KNOB);
    lv_obj_set_style_pad_all(volume_slider_, 8, LV_PART_KNOB);
    lv_obj_set_style_radius(volume_slider_, 10, LV_PART_MAIN);
    lv_obj_set_style_radius(volume_slider_, 10, LV_PART_INDICATOR);
    
    // 百分比标签
    volume_label_ = lv_label_create(volume_container_);
    lv_label_set_text(volume_label_, "70%");
    lv_obj_set_style_text_color(volume_label_, lv_color_white(), 0);
    lv_obj_set_style_min_width(volume_label_, 60, 0);
    lv_obj_set_style_text_align(volume_label_, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_pad_left(volume_label_, 12, 0);
    
    lv_obj_add_event_cb(volume_slider_, OnVolumeChanged, LV_EVENT_VALUE_CHANGED, this);
    lv_obj_add_event_cb(volume_slider_, OnVolumeReleased, LV_EVENT_RELEASED, this);
}

void TouchButtonPanel::CreateRotationButton() {
    // MIPI DSI displays don't support software rotation well
    // Rotation causes screen corruption due to memory/timing issues
    // This feature is disabled for now
    rotation_btn_ = nullptr;
    rotation_icon_ = nullptr;
}

void TouchButtonPanel::ToggleSettingsPopup() {
    if (settings_popup_ == nullptr) return;
    
    settings_popup_visible_ = !settings_popup_visible_;
    if (settings_popup_visible_) {
        lv_obj_clear_flag(settings_popup_, LV_OBJ_FLAG_HIDDEN);
        ESP_LOGI(TAG, "Settings popup shown");
    } else {
        lv_obj_add_flag(settings_popup_, LV_OBJ_FLAG_HIDDEN);
        ESP_LOGI(TAG, "Settings popup hidden");
    }
}

void TouchButtonPanel::Show() {
    if (settings_btn_ != nullptr) {
        lv_obj_clear_flag(settings_btn_, LV_OBJ_FLAG_HIDDEN);
    }
    if (speech_btn_ != nullptr) {
        lv_obj_clear_flag(speech_btn_, LV_OBJ_FLAG_HIDDEN);
    }
    visible_ = true;
    ESP_LOGI(TAG, "Panel shown");
}

void TouchButtonPanel::Hide() {
    if (settings_btn_ != nullptr) {
        lv_obj_add_flag(settings_btn_, LV_OBJ_FLAG_HIDDEN);
    }
    if (settings_popup_ != nullptr) {
        lv_obj_add_flag(settings_popup_, LV_OBJ_FLAG_HIDDEN);
        settings_popup_visible_ = false;
    }
    if (speech_btn_ != nullptr) {
        lv_obj_add_flag(speech_btn_, LV_OBJ_FLAG_HIDDEN);
    }
    if (abort_btn_ != nullptr) {
        lv_obj_add_flag(abort_btn_, LV_OBJ_FLAG_HIDDEN);
    }
    visible_ = false;
    ESP_LOGI(TAG, "Panel hidden");
}

bool TouchButtonPanel::IsVisible() const {
    return visible_;
}

void TouchButtonPanel::UpdateDeviceState(DeviceState state) {
    current_state_ = state;
    UpdateSpeechButtonState();
    
    // 根据状态显示/隐藏按钮，并更新按钮样式
    switch (state) {
        case kDeviceStateIdle:
            // 空闲状态：显示设置和对话按钮，隐藏打断按钮
            if (settings_btn_) lv_obj_clear_flag(settings_btn_, LV_OBJ_FLAG_HIDDEN);
            if (speech_btn_) lv_obj_clear_flag(speech_btn_, LV_OBJ_FLAG_HIDDEN);
            if (abort_btn_) lv_obj_add_flag(abort_btn_, LV_OBJ_FLAG_HIDDEN);
            visible_ = true;
            break;
            
        case kDeviceStateListening:
            // 聆听状态：显示"关闭对话"按钮（蓝色，X图标）
            if (settings_btn_) lv_obj_add_flag(settings_btn_, LV_OBJ_FLAG_HIDDEN);
            if (settings_popup_) {
                lv_obj_add_flag(settings_popup_, LV_OBJ_FLAG_HIDDEN);
                settings_popup_visible_ = false;
            }
            if (speech_btn_) lv_obj_add_flag(speech_btn_, LV_OBJ_FLAG_HIDDEN);
            if (abort_btn_) {
                lv_obj_clear_flag(abort_btn_, LV_OBJ_FLAG_HIDDEN);
                // 蓝色按钮 + X图标 = 关闭对话
                lv_obj_set_style_bg_color(abort_btn_, lv_color_hex(0x2196F3), 0);  // 蓝色
                lv_obj_set_style_bg_color(abort_btn_, lv_color_hex(0x1976D2), LV_STATE_PRESSED);
                if (abort_icon_) lv_label_set_text(abort_icon_, FONT_AWESOME_XMARK);
            }
            visible_ = true;
            break;
            
        case kDeviceStateSpeaking:
            // 说话状态：显示"打断对话"按钮（红色，停止图标）
            if (settings_btn_) lv_obj_add_flag(settings_btn_, LV_OBJ_FLAG_HIDDEN);
            if (settings_popup_) {
                lv_obj_add_flag(settings_popup_, LV_OBJ_FLAG_HIDDEN);
                settings_popup_visible_ = false;
            }
            if (speech_btn_) lv_obj_add_flag(speech_btn_, LV_OBJ_FLAG_HIDDEN);
            if (abort_btn_) {
                lv_obj_clear_flag(abort_btn_, LV_OBJ_FLAG_HIDDEN);
                // 红色按钮 + 停止图标 = 打断对话
                lv_obj_set_style_bg_color(abort_btn_, lv_color_hex(0xF44336), 0);  // 红色
                lv_obj_set_style_bg_color(abort_btn_, lv_color_hex(0xD32F2F), LV_STATE_PRESSED);
                if (abort_icon_) lv_label_set_text(abort_icon_, FONT_AWESOME_STOP);
            }
            visible_ = true;
            break;
            
        case kDeviceStateConnecting:
            // 连接状态：显示设置按钮，禁用对话按钮
            if (settings_btn_) lv_obj_clear_flag(settings_btn_, LV_OBJ_FLAG_HIDDEN);
            if (speech_btn_) lv_obj_clear_flag(speech_btn_, LV_OBJ_FLAG_HIDDEN);
            if (abort_btn_) lv_obj_add_flag(abort_btn_, LV_OBJ_FLAG_HIDDEN);
            visible_ = true;
            break;
            
        default:
            Hide();
            break;
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
            lv_label_set_text(speech_icon_, FONT_AWESOME_MICROPHONE);
            lv_obj_set_style_bg_color(speech_btn_, lv_color_hex(0x2196F3), 0);
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

void TouchButtonPanel::SetAbortButtonCallback(std::function<void()> callback) {
    abort_callback_ = callback;
}

void TouchButtonPanel::SetBrightnessChangeCallback(std::function<void(int)> callback) {
    brightness_callback_ = callback;
}

void TouchButtonPanel::SetVolumeChangeCallback(std::function<void(int)> callback) {
    volume_callback_ = callback;
}

void TouchButtonPanel::SetRotationChangeCallback(std::function<void()> callback) {
    rotation_callback_ = callback;
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
void TouchButtonPanel::OnSettingsButtonClicked(lv_event_t* e) {
    TouchButtonPanel* panel = static_cast<TouchButtonPanel*>(lv_event_get_user_data(e));
    if (panel != nullptr) {
        ESP_LOGI(TAG, "Settings button clicked");
        panel->ToggleSettingsPopup();
    }
}

void TouchButtonPanel::OnSpeechButtonClicked(lv_event_t* e) {
    TouchButtonPanel* panel = static_cast<TouchButtonPanel*>(lv_event_get_user_data(e));
    if (panel != nullptr && panel->speech_callback_) {
        ESP_LOGI(TAG, "Speech button clicked");
        panel->speech_callback_();
    }
}

void TouchButtonPanel::OnAbortButtonClicked(lv_event_t* e) {
    TouchButtonPanel* panel = static_cast<TouchButtonPanel*>(lv_event_get_user_data(e));
    if (panel != nullptr && panel->abort_callback_) {
        ESP_LOGI(TAG, "Abort button clicked");
        panel->abort_callback_();
    }
}

void TouchButtonPanel::OnBrightnessChanged(lv_event_t* e) {
    TouchButtonPanel* panel = static_cast<TouchButtonPanel*>(lv_event_get_user_data(e));
    if (panel != nullptr) {
        int value = lv_slider_get_value(panel->brightness_slider_);
        
        if (panel->brightness_label_ != nullptr) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%d%%", value);
            lv_label_set_text(panel->brightness_label_, buf);
        }
        
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
        
        TouchButtonSettings settings;
        settings.SetBrightness(value);
    }
}

void TouchButtonPanel::OnVolumeChanged(lv_event_t* e) {
    TouchButtonPanel* panel = static_cast<TouchButtonPanel*>(lv_event_get_user_data(e));
    if (panel != nullptr) {
        int value = lv_slider_get_value(panel->volume_slider_);
        
        if (panel->volume_label_ != nullptr) {
            char buf[8];
            snprintf(buf, sizeof(buf), "%d%%", value);
            lv_label_set_text(panel->volume_label_, buf);
        }
        
        panel->UpdateVolumeIcon();
        
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
        
        TouchButtonSettings settings;
        settings.SetVolume(value);
    }
}

void TouchButtonPanel::OnRotationButtonClicked(lv_event_t* e) {
    TouchButtonPanel* panel = static_cast<TouchButtonPanel*>(lv_event_get_user_data(e));
    if (panel != nullptr && panel->rotation_callback_) {
        ESP_LOGI(TAG, "Rotation button clicked");
        panel->rotation_callback_();
    }
}
