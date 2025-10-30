#pragma once
#include <rclcpp/rclcpp.hpp>
#include <string>
#include <iostream>

/**
 * @file logger.hpp
 * @brief 提供统一的日志接口。
 *        当节点以 ROS2 模式运行时使用 RCLCPP_LOG；
 *        否则回退为标准输出。
 */

class Logger {
public:
    explicit Logger(rclcpp::Logger ros_logger)
        : ros_logger_(ros_logger), use_ros_logger_(true) {}

    Logger()
        : ros_logger_(rclcpp::get_logger("default_logger")), use_ros_logger_(false) {}

    // 信息级别日志
    void info(const std::string &msg) const {
        if (use_ros_logger_)
            RCLCPP_INFO(ros_logger_, "%s", msg.c_str());
        else
            std::cout << "[INFO] " << msg << std::endl;
    }

    // 警告级别日志
    void warn(const std::string &msg) const {
        if (use_ros_logger_)
            RCLCPP_WARN(ros_logger_, "%s", msg.c_str());
        else
            std::cout << "[WARN] " << msg << std::endl;
    }

    // 错误级别日志
    void error(const std::string &msg) const {
        if (use_ros_logger_)
            RCLCPP_ERROR(ros_logger_, "%s", msg.c_str());
        else
            std::cerr << "[ERROR] " << msg << std::endl;
    }

private:
    rclcpp::Logger ros_logger_;
    bool use_ros_logger_;
};
