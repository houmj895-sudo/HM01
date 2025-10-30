#include <rclcpp/rclcpp.hpp>
#include <soem/soem.h>
#include "l6n_soem_csp/logger.hpp"
#include "l6n_soem_csp/common_defs.hpp"

using namespace l6n;

/**
 * @file slave_scan_node.cpp
 * @brief EtherCAT 从站扫描节点（新版 SOEM 接口 + 自定义日志系统）。
 *        自动检测从站数量并输出基本信息。
 */

int main(int argc, char *argv[])
{
    // ------------------------------
    // 1️⃣ 初始化 ROS2 节点与日志
    // ------------------------------
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("slave_scan_node");
    Logger logger(node->get_logger());

    const char *ifname = "enp4s0";  // ⚙️ 实际网卡名
    logger.info("==========================================");
    logger.info("🧠 EtherCAT 从站扫描节点启动...");
    logger.info("使用网卡接口: " + std::string(ifname));

    // ------------------------------
    // 2️⃣ 创建 SOEM 上下文对象
    // ------------------------------
    ecx_contextt ecx_context;
    memset(&ecx_context, 0, sizeof(ecx_context));

    // ------------------------------
    // 3️⃣ 初始化 EtherCAT 主站
    // ------------------------------
    if (ecx_init(&ecx_context, ifname)) {
        logger.info("✅ 成功初始化 EtherCAT 接口。");
    } else {
        logger.error("❌ 无法初始化 EtherCAT 接口，请检查网卡或权限。");
        return -1;
    }

    // ------------------------------
    // 4️⃣ 扫描从站
    // ------------------------------
    int slave_cnt = ecx_config_init(&ecx_context);
    if (slave_cnt > 0) {
        logger.info("✅ 成功发现从站数量: " + std::to_string(slave_cnt));
        for (int i = 1; i <= slave_cnt; ++i) {
            const auto &s = ecx_context.slavelist[i];
            std::string info =
                "从站 [" + std::to_string(i) + "] 名称: " + std::string(s.name) +
                ", 产品码: 0x" + to_hex(static_cast<uint16_t>(s.eep_id)) +
                ", 修订号: " + std::to_string(s.eep_rev);
            logger.info(info);
        }
    } else {
        logger.error("❌ 未检测到任何 EtherCAT 从站，请检查线路或电源。");
        ecx_close(&ecx_context);
        return -2;
    }

    // ------------------------------
    // 5️⃣ DC 配置（可选）
    // ------------------------------
    ecx_configdc(&ecx_context);
    logger.info("🔧 分布式时钟配置完成。");

    // ------------------------------
    // 6️⃣ 切换到 PRE-OP 状态检查
    // ------------------------------
    ecx_statecheck(&ecx_context, 0, EC_STATE_PRE_OP, EC_TIMEOUTSTATE);
    logger.info("当前主站状态: PRE_OP");

    // ------------------------------
    // 7️⃣ 关闭通信
    // ------------------------------
    ecx_close(&ecx_context);
    logger.info("🔚 扫描完成，通信已关闭。");

    rclcpp::shutdown();
    return 0;
}
