#include "l6n_soem_csp/cia402_fsm.hpp"
#include <thread>
#include <chrono>

namespace l6n {

Cia402Fsm::Cia402Fsm(SoemIface &iface, const Logger &logger, int slave)
    : iface_(iface), logger_(logger), slave_(slave)
{
}

bool Cia402Fsm::faultReset()
{
    logger_.info("🔄 执行 Fault Reset...");
    if (!writeControlword(CW_FAULT_RESET)) return false;

    bool ok = waitUntil([&]() {
        uint16_t sw = 0;
        iface_.sdoRead<uint16_t>(slave_, IDX_STATUSWORD, 0, sw);
        return (sw & SW_FAULT) == 0;
    }, 1000);

    logger_.info(ok ? "✅ Fault 清除成功" : "❌ Fault 清除超时");
    return ok;
}

bool Cia402Fsm::setModeCSP()
{
    logger_.info("⚙️ 设置驱动模式为 CSP (0x08)");
    int8_t mode = 8;
    if (!iface_.sdoWrite<int8_t>(slave_, IDX_MODES_OF_OPERATION, 0, mode)) {
        logger_.error("❌ 无法写入 0x6060 模式寄存器。");
        return false;
    }
    logger_.info("✅ 已写入 CSP 模式。");
    return true;
}

bool Cia402Fsm::enableOperation()
{
    // 1️⃣ Shutdown
    logger_.info("➡️ 执行 Shutdown (0x0006)");
    writeControlword(CW_SHUTDOWN);
    if (!waitUntil([&]() {
            uint16_t sw;
            iface_.sdoRead<uint16_t>(slave_, IDX_STATUSWORD, 0, sw);
            return (sw & SW_READY_TO_SWITCH_ON) != 0;
        }, 1000))
        return false;

    // 2️⃣ Switch On
    logger_.info("➡️ 执行 Switch On (0x0007)");
    writeControlword(CW_SWITCH_ON);
    if (!waitUntil([&]() {
            uint16_t sw;
            iface_.sdoRead<uint16_t>(slave_, IDX_STATUSWORD, 0, sw);
            return (sw & SW_SWITCHED_ON) != 0;
        }, 1000))
        return false;

    // 3️⃣ Enable Operation
    logger_.info("➡️ 执行 Enable Operation (0x000F)");
    writeControlword(CW_ENABLE_OPERATION);
    if (!waitUntil([&]() {
            uint16_t sw;
            iface_.sdoRead<uint16_t>(slave_, IDX_STATUSWORD, 0, sw);
            return (sw & SW_OPERATION_ENABLED) != 0;
        }, 1000))
        return false;

    logger_.info("✅ 驱动器进入 OPERATION ENABLED 状态。");
    return true;
}

uint16_t Cia402Fsm::readStatusword()
{
    uint16_t sw = 0;
    iface_.sdoRead<uint16_t>(slave_, IDX_STATUSWORD, 0, sw);
    logger_.info("📖 Statusword = 0x" + std::to_string(sw));
    return sw;
}

bool Cia402Fsm::writeControlword(uint16_t cw)
{
    return iface_.sdoWrite<uint16_t>(slave_, IDX_CONTROLWORD, 0, cw);
}

template<typename Pred>
bool Cia402Fsm::waitUntil(Pred pred, int timeout_ms, int sleep_ms)
{
    auto start = std::chrono::steady_clock::now();
    while (true) {
        if (pred()) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start).count() > timeout_ms)
            return false;
    }
}

} // namespace l6n
