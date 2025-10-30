#include <rclcpp/rclcpp.hpp>
#include <soem/soem.h>
#include "l6n_soem_csp/logger.hpp"

/**
 * @file slave_scan_node.cpp
 * @brief EtherCAT 从站扫描节点（适配新版本 SOEM 接口）。
 *        自动检测从站数量并输出基本信息。
 */

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("slave_scan_node");
    Logger logger(node->get_logger());

    const char *ifname = "enp4s0";  // 🧩 根据实际网卡名修改
    logger.info("==========================================");
    logger.info("🧠 EtherCAT 从站扫描节点启动...");
    logger.info("使用网卡接口: " + std::string(ifname));

    // 1️⃣ 创建上下文对象（新版 SOEM 需要）
    ecx_contextt ecx_context;

    // 2️⃣ 初始化 EtherCAT 主站
    if (ecx_init(&ecx_context, ifname)) {
        logger.info("✅ 成功初始化 EtherCAT 接口。");
    } else {
        logger.error("❌ 无法初始化 EtherCAT 接口，请检查网卡或权限。");
        return -1;
    }

    // 3️⃣ 扫描从站
    int slave_cnt = ecx_config_init(&ecx_context);
    if (slave_cnt > 0) {
        logger.info("✅ 成功发现从站数量: " + std::to_string(slave_cnt));
        for (int i = 1; i <= slave_cnt; ++i) {
            const auto &s = ecx_context.slavelist[i];
            std::string info = "从站 [" + std::to_string(i) + "] 名称: " +
                               std::string(s.name) +
                               ", 产品码: " + std::to_string(s.eep_id) +
                               ", 修订号: " + std::to_string(s.eep_rev);
            logger.info(info);
        }
    } else {
        logger.error("❌ 未检测到任何 EtherCAT 从站，请检查线路或电源。");
        ecx_close(&ecx_context);
        return -2;
    }

    // 4️⃣ DC 配置（可选）
    ecx_configdc(&ecx_context);
    logger.info("🔧 分布式时钟配置完成。");

    // 5️⃣ 切换到 PRE-OP 状态检查
    ecx_statecheck(&ecx_context, 0, EC_STATE_PRE_OP, EC_TIMEOUTSTATE);
    logger.info("当前主站状态: PRE_OP");

    // 6️⃣ 关闭通信
    ecx_close(&ecx_context);
    logger.info("🔚 扫描完成，通信已关闭。");

    rclcpp::shutdown();
    return 0;
}
