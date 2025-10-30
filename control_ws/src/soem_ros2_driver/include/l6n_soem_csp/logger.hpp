#pragma once
#include <rclcpp/rclcpp.hpp>
#include <string>
#include <iostream>

/**
 * @file logger.hpp
 * @brief 统一日志封装类。
 *        - 当在 ROS2 节点内运行时使用 rclcpp::Logger。
 *        - 若在独立测试中运行则退化为控制台输出。
 */

namespace l6n {

class Logger {
public:
    explicit Logger(rclcpp::Logger ros_logger)
        : ros_logger_(ros_logger), use_ros_logger_(true) {}

    Logger()
        : ros_logger_(rclcpp::get_logger("default_logger")), use_ros_logger_(false) {}

    // ------------------------------
    // 信息级别日志
    // ------------------------------
    void info(const std::string &msg) const {
        if (use_ros_logger_)
            RCLCPP_INFO(ros_logger_, "%s", msg.c_str());
        else
            std::cout << "[INFO] " << msg << std::endl;
    }

    // ------------------------------
    // 警告级别日志
    // ------------------------------
    void warn(const std::string &msg) const {
        if (use_ros_logger_)
            RCLCPP_WARN(ros_logger_, "%s", msg.c_str());
        else
            std::cout << "[WARN] " << msg << std::endl;
    }

    // ------------------------------
    // 错误级别日志
    // ------------------------------
    void error(const std::string &msg) const {
        if (use_ros_logger_)
            RCLCPP_ERROR(ros_logger_, "%s", msg.c_str());
        else
            std::cerr << "[ERROR] " << msg << std::endl;
    }

    // ------------------------------
    // 致命级别日志（新增）
    // ------------------------------
    void fatal(const std::string &msg) const {
        if (use_ros_logger_)
            RCLCPP_FATAL(ros_logger_, "%s", msg.c_str());
        else
            std::cerr << "\033[1;31m[FATAL]\033[0m " << msg << std::endl;
    }

private:
    rclcpp::Logger ros_logger_;
    bool use_ros_logger_;
};

} // namespace l6n
