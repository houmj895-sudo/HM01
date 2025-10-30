#pragma once
/**
 * @file cia402_fsm.hpp
 * @brief 封装 CiA402 状态机逻辑（标准 402 驱动状态流转）。
 *        包括故障复位、模式设置（CSP=8）、上电三步流程。
 */

#include "l6n_soem_csp/soem_iface.hpp"
#include "l6n_soem_csp/logger.hpp"
#include "l6n_soem_csp/common_defs.hpp"

namespace l6n {

class Cia402Fsm {
public:
    Cia402Fsm(SoemIface &iface, const Logger &logger, int slave);

    bool faultReset();        // 故障复位
    bool setModeCSP();        // 设置 CSP 模式
    bool enableOperation();   // 启用驱动（进入 OPERATION ENABLED）
    uint16_t readStatusword();// 读取状态字（含关键位解释）

private:
    SoemIface &iface_;
    Logger logger_;
    int slave_;

    bool writeControlword(uint16_t cw); // 写控制字模板
    template<typename Pred>
    bool waitUntil(Pred pred, int timeout_ms, int sleep_ms = 10);
};

} // namespace l6n
