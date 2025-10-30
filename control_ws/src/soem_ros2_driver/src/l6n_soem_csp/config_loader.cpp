#include "l6n_soem_csp/config_loader.hpp"
#include <fstream>

namespace l6n {

// ======================================================
// 构造函数
// ======================================================
ConfigLoader::ConfigLoader(const Logger &logger)
    : logger_(logger)
{
}

// ======================================================
// 泛型辅助函数：带默认值的安全读取
// ======================================================
template<typename T>
T ConfigLoader::getOrDefault(const YAML::Node &node, const std::string &key, const T &def)
{
    // 检查 key 是否存在并可读，否则返回默认值
    if (node[key])
        return node[key].as<T>();
    else
        return def;
}

// ======================================================
// 加载 EtherCAT 主站配置
// ======================================================
bool ConfigLoader::loadEthercatConfig(const std::string &path)
{
    try {
        YAML::Node root = YAML::LoadFile(path);
        auto node = root["ethercat"];
        if (!node) {
            logger_.error("❌ 无法找到 'ethercat' 根节点: " + path);
            return false;
        }

        ethercat_cfg_.iface = getOrDefault(node, "iface", std::string("enp4s0"));
        ethercat_cfg_.expected_slaves = getOrDefault(node, "expected_slaves", 1);
        ethercat_cfg_.timeout_us = getOrDefault(node, "timeout_us", 5000);
        ethercat_cfg_.retry_on_fail = getOrDefault(node, "retry_on_fail", 3);
        ethercat_cfg_.dc_enable = getOrDefault(node, "dc_enable", true);
        ethercat_cfg_.sync_mode = getOrDefault(node, "sync_mode", std::string("DC"));
        ethercat_cfg_.cycle_time_ns = getOrDefault(node, "cycle_time_ns", 1000000);
        ethercat_cfg_.safe_op_before_op = getOrDefault(node, "safe_op_before_op", true);
        ethercat_cfg_.log_dir = getOrDefault(node, "log_dir", std::string("~/logs"));

        logger_.info("✅ 已加载 EtherCAT 配置：" + ethercat_cfg_.iface);
        return true;
    } catch (const std::exception &e) {
        logger_.error("❌ 加载 EtherCAT 配置失败: " + std::string(e.what()));
        return false;
    }
}

// ======================================================
// 加载单轴运动配置
// ======================================================
bool ConfigLoader::loadMotionConfig(const std::string &path)
{
    try {
        YAML::Node root = YAML::LoadFile(path);
        auto node = root["motion"]["axis_1"];
        if (!node) {
            logger_.error("❌ 无法找到 'motion.axis_1' 节点: " + path);
            return false;
        }

        auto &cfg = axis1_cfg_;
        cfg.control_mode = getOrDefault(node, "control_mode", std::string("CSP"));
        cfg.position_offset = getOrDefault(node, "position_offset", 0);
        cfg.soft_limit_pos = getOrDefault(node, "soft_limit_pos", 7000000);
        cfg.soft_limit_neg = getOrDefault(node, "soft_limit_neg", -7000000);
        cfg.torque_limit = getOrDefault(node, "torque_limit", 1000);

        // Motor 参数
        auto motor = node["motor"];
        if (motor) {
            cfg.motor.type = getOrDefault(motor, "type", std::string("标准"));
            cfg.motor.encoder_resolution = getOrDefault(motor, "encoder_resolution", 8388608);
        }

        logger_.info("✅ 已加载运动配置（Axis 1，模式：" + cfg.control_mode + "）");
        return true;
    } catch (const std::exception &e) {
        logger_.error("❌ 加载 Motion 配置失败: " + std::string(e.what()));
        return false;
    }
}

// ======================================================
// 加载日志配置
// ======================================================
bool ConfigLoader::loadLogConfig(const std::string &path)
{
    try {
        YAML::Node root = YAML::LoadFile(path);
        auto node = root["logger"];
        if (!node) {
            logger_.error("❌ 无法找到 'logger' 根节点: " + path);
            return false;
        }

        log_cfg_.use_ros_log = getOrDefault(node, "use_ros_log", true);
        log_cfg_.level = getOrDefault(node, "level", std::string("info"));
        log_cfg_.log_to_file = getOrDefault(node, "log_to_file", true);
        log_cfg_.log_dir = getOrDefault(node, "log_dir", std::string("~/logs"));
        log_cfg_.log_frequency_hz = getOrDefault(node, "log_frequency_hz", 10);
        log_cfg_.color_output = getOrDefault(node, "color_output", true);

        logger_.info("✅ 已加载日志配置：" + log_cfg_.level);
        return true;
    } catch (const std::exception &e) {
        logger_.error("❌ 加载日志配置失败: " + std::string(e.what()));
        return false;
    }
}

} // namespace l6n
