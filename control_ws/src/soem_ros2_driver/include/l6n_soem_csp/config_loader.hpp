#pragma once
/**
 * @file config_loader.hpp
 * @brief 配置文件解析模块。
 *        负责加载 YAML 配置文件并解析为结构体：
 *        - EthercatConfig：主站参数
 *        - AxisConfig：轴参数与轨迹设置
 *        - LogConfig：日志控制
 *
 * 依赖：YAML-CPP 库（Ubuntu 默认可用：sudo apt install libyaml-cpp-dev）
 */

#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "l6n_soem_csp/logger.hpp"

namespace l6n {

// ======================================================
// 🧩 EtherCAT 通信配置结构体
// ======================================================
struct EthercatConfig {
    std::string iface;
    int expected_slaves;
    int timeout_us;
    int retry_on_fail;
    bool dc_enable;
    std::string sync_mode;
    int cycle_time_ns;
    int dc_shift_ns;
    int dc_sync_shift_ns;
    int watchdog_ms;
    bool auto_recover;
    int state_check_interval_ms;
    bool print_slave_info;
    bool print_wkc;
    bool log_cycle_stat;
    bool eeprom_access;
    bool safe_op_before_op;
    bool bus_reinit_on_error;
    std::string log_dir;
    std::string eni_file;
};

// ======================================================
// ⚙️ 单轴配置（Motion）
// ======================================================
struct AxisConfig {
    std::string control_mode;
    std::string feedback_source;
    int position_offset;
    int soft_limit_pos;
    int soft_limit_neg;
    int torque_limit;
    bool enable_on_start;

    struct Motor {
        std::string type;
        double rated_power_kw;
        double rated_voltage_v;
        double rated_speed_rpm;
        double max_speed_rpm;
        double rated_torque_nm;
        double max_torque_nm;
        double rated_current_a;
        double max_current_a;
        double torque_constant_nm_per_a;
        double rotor_inertia_kgm2e4;
        int encoder_resolution;
    } motor;

    struct Homing {
        bool enabled;
        int method;
        double speed_search;
        double speed_latch;
        int offset_pulse;
        int timeout_s;
    } homing;

    struct Trajectory {
        bool enabled;
        std::string type;
        double amplitude_deg;
        double frequency_hz;
        double duration_s;
    } trajectory;

    struct ControlLoop {
        int rate_hz;
        bool interpolation;
        int position_tolerance;
    } control_loop;
};

// ======================================================
// 🧠 日志配置结构体
// ======================================================
struct LogConfig {
    bool use_ros_log;
    std::string level;
    bool log_to_file;
    std::string log_dir;
    int max_file_size_mb;
    int max_backup_files;
    std::string filename_pattern;
    bool realtime_print;
    int log_frequency_hz;
    bool include_timestamp;
    bool color_output;
    bool log_pdo_data;
    bool log_statusword_changes;
    int flush_interval_ms;
};

// ======================================================
// 🧩 配置加载器类
// ======================================================
class ConfigLoader {
public:
    explicit ConfigLoader(const Logger &logger);

    // 加载各类配置文件
    bool loadEthercatConfig(const std::string &path);
    bool loadMotionConfig(const std::string &path);
    bool loadLogConfig(const std::string &path);

    // 获取解析结果
    EthercatConfig &ethercat() { return ethercat_cfg_; }
    AxisConfig &axis1() { return axis1_cfg_; }
    LogConfig &log() { return log_cfg_; }

private:
    Logger logger_;
    EthercatConfig ethercat_cfg_;
    AxisConfig axis1_cfg_;
    LogConfig log_cfg_;

    // 泛型安全读取：支持各种类型 + 默认值
    template<typename T>
    T getOrDefault(const YAML::Node &node, const std::string &key, const T &def);

    // 重载：解决 const char* 不能自动转 std::string 的 IntelliSense 红线
    template<typename T>
    T getOrDefault(const YAML::Node &node, const char *key, const T &def) {
        return getOrDefault(node, std::string(key), def);
    }
};

} // namespace l6n
