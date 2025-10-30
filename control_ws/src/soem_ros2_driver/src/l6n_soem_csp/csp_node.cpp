#include <rclcpp/rclcpp.hpp>
#include <iostream>

/**
 * @brief 临时占位程序，仅用于验证CMake能正常链接。
 *        后续会替换为完整CSP控制节点逻辑。
 */
int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    RCLCPP_INFO(rclcpp::get_logger("csp_node"), "✅ 构建测试成功：CSP 主节点框架OK！");
    rclcpp::shutdown();
    return 0;
}
