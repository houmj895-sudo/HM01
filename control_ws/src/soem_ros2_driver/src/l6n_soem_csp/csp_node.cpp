#include <rclcpp/rclcpp.hpp>
#include "l6n_soem_csp/logger.hpp"
#include "l6n_soem_csp/soem_iface.hpp"
#include "l6n_soem_csp/cia402_fsm.hpp"
#include "l6n_soem_csp/config_loader.hpp"
#include "l6n_soem_csp/common_defs.hpp"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using namespace l6n;

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<rclcpp::Node>("csp_node");
    Logger log(node->get_logger());

    // ===================== 1️⃣ 加载配置 =====================
    auto ethercat_yaml = node->declare_parameter<std::string>("ethercat_config", "config/ethercat_config.yaml");
    auto motion_yaml   = node->declare_parameter<std::string>("motion_config",   "config/motion_config.yaml");
    auto log_yaml      = node->declare_parameter<std::string>("log_config",      "config/log_config.yaml");

    ConfigLoader cfg_loader(log);
    if (!cfg_loader.loadEthercatConfig(ethercat_yaml) ||
        !cfg_loader.loadMotionConfig(motion_yaml)     ||
        !cfg_loader.loadLogConfig(log_yaml)) {
        log.error("❌ 加载配置失败，退出。");
        return 1;
    }
    auto& ecfg = cfg_loader.ethercat();

    // ===================== 2️⃣ 初始化主站 =====================
    SoemIface iface(log);
    if (!iface.init(ecfg.iface)) {
        log.error("❌ SOEM 初始化失败。");
        return 2;
    }
    if (!iface.configureMapAndDC()) {
        log.error("❌ PDO/DC 配置失败。");
        return 3;
    }

    // ===================== 3️⃣ SAFE_OP =====================
    if (!iface.requestState(EC_STATE_SAFE_OP)) {
        log.error("❌ 切换到 SAFE_OP 失败。");
        return 4;
    }
    log.info("✅ 已到 SAFE_OP，准备进入 OPERATIONAL。");

    for (int i = 0; i < 50; ++i) {
        iface.processIO();
        std::this_thread::sleep_for(1ms);
    }

    // ===================== 4️⃣ 模式配置 =====================
    Cia402Fsm fsm(iface, log, 1);
    fsm.faultReset();
    fsm.setModeCSP();
    fsm.enableOperation();

    log.info("✅ 驱动器已准备好进入 OPERATIONAL。");

    // ===================== 5️⃣ OPERATIONAL =====================
// ...省略前面一致部分...

    if (!iface.requestState(EC_STATE_OPERATIONAL)) {
        log.error("❌ 切换到 OPERATIONAL 失败。");

        // ✅ 修正：新版 SOEM 仅保留 ALstatuscode
        for (int i = 1; i <= iface.slaveCount(); ++i) {
            auto& s = iface.slaveList()[i];
            log.error("从站[" + std::to_string(i) + "] ALstatuscode=0x" +
                      to_hex(s.ALstatuscode));
        }

        iface.close();
        return 5;
    }

    log.info("✅ 总线进入 OPERATIONAL！开始持续交换 PDO。");

    // ===================== 主循环 =====================
    const int loop_hz = 1000;
    const auto tick = 1ms;
    int print_divider = 0;

    for (;;)
    {
        int wkc = iface.processIO();

        if (++print_divider >= (loop_hz / 10)) {
            uint16_t sw = fsm.readStatusword();
            int32_t pos_actual = 0;

            // ✅ 确保名称匹配 common_defs.hpp
            iface.sdoRead<int32_t>(1, IDX_POSITION_ACTUAL, 0, pos_actual);

            log.info("WKC=" + std::to_string(wkc) +
                     " | StatusWord=0x" + to_hex(sw) +
                     " | PosActual=" + std::to_string(pos_actual));

            print_divider = 0;
        }

        std::this_thread::sleep_for(tick);
    }


    return 0;
}
