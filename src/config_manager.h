#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>  // For exceptions
#include <string>

#include "FS.h"
#include "defs.h"
#include "utils.h"

// Config file location
#define CONFIG_FILE "/config.txt"
// Defaut sensor level (0-1) when blocked
#define SENSOR_BLOCKED_LEVEL "sensor_blocked_level"
// Distance between S1 and S2 in mm
#define S1S2_DISTANCE "s1s2_distance"
// Distance between S2 and S3 in mm
#define S2S3_DISTANCE "s2s3_distance"

// Distance correction between s1s2 in tenth of mm
#define S1S2_CORRECTION "s1s2_correction"

// Distance correction between s1s2 in tenth of mm
#define S2S3_CORRECTION "s2s3_correction"

// Maximum duration between start of change (unblocked with all blocked on S1
// and blocked state on S3 in MS
#define MAX_MEASURE_DURATION "max_duration"

class ConfigManager {
private:
    std::map<std::string, std::string> config_;
    bool updated_;  // Flag to indicate if the configuration has been updated

    // Private constructor to prevent direct instantiation
    ConfigManager() : config_(), updated_(false) {
    }

public:
    // Static method to get the singleton instance
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }

    // Set default configuration values
    void setDefaultConfig(const std::map<std::string, std::string>& defaults) {
        for (const auto& pair : defaults) {
            config_[pair.first] = pair.second;
        }
        log_d("Default configuration set.");
    }

    // Read configuration from a text file
    bool load() {
#ifdef SIM
        return false;
#endif
        log_d("Loading configuration from file: %s\n", CONFIG_FILE);
        fs::File f = SPIFFS.open(CONFIG_FILE);
        if (!f.available()) {
            log_d("Error opening configurationfile: %s\n", CONFIG_FILE);
            return false;
        }

        std::stringstream ss(f.readString().c_str());
        f.close();

        std::string line;
        while (std::getline(ss, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#')
                continue;

            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                config_[key] = value;
            }
        }
        log_d("Configuration loaded from file: %s\n", CONFIG_FILE);
    }

    // Save configuration to a text file
    bool save() {
#ifdef SIM
        return false;
#endif
        fs::File f = SPIFFS.open(CONFIG_FILE, "w", true);
        if (!f.available()) {
            log_d("Error opening file for writing: %s\n", CONFIG_FILE);
            return false;
        }

        for (const auto& pair : config_) {
            f.printf("%s=%s\n", pair.first.c_str(), pair.second.c_str());
        }

        f.close();
        log_d("Configuration saved to file: %s\n", CONFIG_FILE);

        return true;
    }

    // Retrieve value as bool (with optional default)
    bool getBool(const std::string& key, bool defaultValue) {
        auto it = config_.find(key);
        if (it != config_.end()) {
            try {
                return std::stoi(it->second) !=
                       0;  // Convert to int and check for non-zero
            } catch (const std::invalid_argument& e) {
                log_d("Error parsing %s as bool. Returning default value.\n",
                      it->second.c_str());
                return defaultValue;
            } catch (const std::out_of_range& e) {
                log_d("Value %s out of range for bool conversion. Returning "
                      "default value.\n",
                      it->second.c_str());
                return defaultValue;
            }
        } else {
            log_d("Key %s not found. Returning default value.\n", key.c_str());
            return defaultValue;
        }
    }

    // Retrieve value as int (with optional default)
    int getInt(const std::string& key, int defaultValue) {
        auto it = config_.find(key);
        if (it != config_.end()) {
            log_d("Config: Key '%s' exists with value '%s'.\n", key.c_str(),
                  it->second.c_str());
            try {
                return std::stoi(it->second);
            } catch (const std::invalid_argument& e) {
                log_d("Error parsing %s as int. Returning default value.\n",
                      it->second.c_str());
                return defaultValue;
            } catch (const std::out_of_range& e) {
                log_d("Value %s out of range for int conversion. Returning "
                      "default value.\n",
                      it->second.c_str());
                return defaultValue;
            }
        } else {
            log_d("Key %s not found. Returning default value.\n", key.c_str());
            return defaultValue;
        }
    }

    // Retrieve value as float (with optional default)
    float getFloat(const std::string& key, float defaultValue) {
        auto it = config_.find(key);
        if (it != config_.end()) {
            try {
                return std::stof(it->second);
            } catch (const std::invalid_argument& e) {
                log_d("Error parsing %s as float. Returning default value.\n",
                      it->second.c_str());
                return defaultValue;
            } catch (const std::out_of_range& e) {
                log_d("Value %s out of range for float conversion. Returning "
                      "default value.\n",
                      it->second.c_str());
                return defaultValue;
            }
        } else {
            log_d("Key %s not found. Returning default value.\n", key.c_str());
            return defaultValue;
        }
    }

    // Retrieve value as string (with optional default)
    std::string getString(const std::string& key,
                          const std::string& defaultValue) {
        auto it = config_.find(key);
        if (it != config_.end()) {
            return it->second;
        } else {
            log_d("Key %s not found. Returning default value.\n", key.c_str());
            return defaultValue;
        }
    }

    // Set value as string and update the updated flag
    void setString(const std::string& key, const std::string& value) {
        log_d("Config: Setting %s to %s", key.c_str(), value.c_str());
        config_[key] = value;
        updated_ = true;
    }

    // Set value as int and update the updated flag
    void setInt(const std::string& key, int value) {
        std::stringstream ss;
        ss << value;
        setString(key, ss.str());
        updated_ = true;
    }

    // Set value as bool and update the updated flag
    void setBool(const std::string& key, bool value) {
        std::stringstream ss;
        ss << (value ? 1 : 0);
        setString(key, ss.str());
        updated_ = true;
    }

    // Set value as float and update the updated flag
    void setFloat(const std::string& key, float value) {
        std::stringstream ss;
        ss << value;
        setString(key, ss.str());
        updated_ = true;
    }

    // Check if the configuration has been updated
    bool isUpdated() const {
        return updated_;
    }

    // Reset the updated flag
    void resetUpdated() {
        updated_ = false;
    }
};

#endif  // CONFIG_MANAGER_H