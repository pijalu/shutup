#ifndef SETTINGS_PAGE_H
#define SETTINGS_PAGE_H

#include <TFT_eSPI.h>

#include "UIPage.h"
#include "config_manager.h"
#include "defs.h"
#include "gui_manager.h"
#include "togglebutton.h"
#include "valuebutton.h"

class SettingsPage : public UIPage {
private:
    ToggleButton* tb_sensor_level;
    ValueButton* s1s2_distance;
    ValueButton* s1s2_correction;

    ValueButton* s2s3_distance;
    ValueButton* s2s3_correction;

    ValueButton* max_measure_duration;

public:
    SettingsPage() : UIPage("Settings") {
        bool statusBlocked = ConfigManager::getInstance().getInt(
                                 SENSOR_BLOCKED_LEVEL, 0) == SENSOR_BLOCKED;
        tb_sensor_level = new ToggleButton("Blocked Sensor is 'on'",
                                           statusBlocked ? "On" : "Off", 0, 30,
                                           getWidth(), 20);
        tb_sensor_level->addHandler(this);
        add(tb_sensor_level);

        int s1s2_distance_value = ConfigManager::getInstance().getInt(
            S1S2_DISTANCE, 16);  // Default distance
        s1s2_distance =
            new ValueButton("S1-S2 Distance (mm)", s1s2_distance_value, "%02d",
                            20, 10, 1, 0, 60, getWidth(), 20);
        s1s2_distance->addHandler(this);
        add(s1s2_distance);

        int s1s2_correction_value = ConfigManager::getInstance().getInt(
            S1S2_CORRECTION, 0);  // Default distance
        s1s2_correction =
            new ValueButton("S1-S2 Correction (1/100mm)", s1s2_correction_value,
                            "%+04d", 999, -999, 1, 0, 90, getWidth(), 20);
        s1s2_correction->addHandler(this);
        add(s1s2_correction);

        int s2s3_distance_value = ConfigManager::getInstance().getInt(
            S2S3_DISTANCE, 16);  // Default distance
        s2s3_distance =
            new ValueButton("S2-S3 Distance (mm)", s2s3_distance_value, "%02d",
                            20, 10, 1, 0, 120, getWidth(), 20);
        s2s3_distance->addHandler(this);
        add(s2s3_distance);

        int s2s3_correction_value = ConfigManager::getInstance().getInt(
            S2S3_CORRECTION, 0);  // Default distance
        s2s3_correction =
            new ValueButton("S2-S3 Correction (1/100mm)", s2s3_correction_value,
                            "%+04d", 999, -999, 1, 0, 150, getWidth(), 20);
        s2s3_correction->addHandler(this);
        add(s2s3_correction);

        int max_measure_duration_value = ConfigManager::getInstance().getInt(
            MAX_MEASURE_DURATION, 2000);  // Default distance
        max_measure_duration = new ValueButton(
            "Max measure duration in MS", max_measure_duration_value, "%04d",
            9999, 0, 1, 0, 180, getWidth(), 20);
        max_measure_duration->addHandler(this);
        add(max_measure_duration);
    }

    void handle(Component* sender, int evtCode, u_int32_t hparam,
                u_int32_t lparam) override {
        if (sender == tb_sensor_level && evtCode == EVT_CLICK) {
            bool statusBlocked =
                ConfigManager::getInstance().getBool(SENSOR_BLOCKED_LEVEL, 0);
            ConfigManager::getInstance().setBool(SENSOR_BLOCKED_LEVEL,
                                                 !statusBlocked);
        }
        if (sender == s1s2_distance && evtCode == EVT_NOTIFY &&
            hparam == VB_VALUECHANGE) {
            int distance = lparam;
            log_d("S1-S2 Distance changed to: %d mm", distance);
            ConfigManager::getInstance().setInt(S1S2_DISTANCE, distance);
        }
        if (sender == s1s2_correction && evtCode == EVT_NOTIFY &&
            hparam == VB_VALUECHANGE) {
            int correction = lparam;
            log_d("S1-S2 correction changed to: %d mm", correction);
            ConfigManager::getInstance().setInt(S1S2_CORRECTION, correction);
        }
        if (sender == s2s3_distance && evtCode == EVT_NOTIFY &&
            hparam == VB_VALUECHANGE) {
            int distance = lparam;
            log_d("S2-S3 Distance changed to: %d mm", distance);
            ConfigManager::getInstance().setInt(S2S3_DISTANCE, distance);
        }
        if (sender == s2s3_correction && evtCode == EVT_NOTIFY &&
            hparam == VB_VALUECHANGE) {
            int correction = lparam;
            log_d("S2-S3 correction changed to: %d mm", correction);
            ConfigManager::getInstance().setInt(S2S3_CORRECTION, correction);
        }
        if (sender == max_measure_duration && evtCode == EVT_NOTIFY &&
            hparam == VB_VALUECHANGE) {
            int duration = lparam;
            log_d("Max measure duration: %d mm", duration);
            ConfigManager::getInstance().setInt(MAX_MEASURE_DURATION, duration);
        }
    }
};

#endif  // SETTINGS_PAGE_H