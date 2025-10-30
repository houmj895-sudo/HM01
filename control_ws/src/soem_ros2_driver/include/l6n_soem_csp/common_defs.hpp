#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

/**
 * @file common_defs.hpp
 * @brief EtherCAT 控制通用定义：
 *        - CiA402 寄存器索引
 *        - 控制字/状态字位掩码
 *        - 实用函数（十六进制打印）
 */

namespace l6n {

// ==========================
// ⚙️ EtherCAT 索引定义（标准 CiA402）
// ==========================
constexpr uint16_t IDX_CONTROLWORD        = 0x6040;
constexpr uint16_t IDX_STATUSWORD         = 0x6041;
constexpr uint16_t IDX_MODES_OF_OPERATION = 0x6060;
constexpr uint16_t IDX_MODE_DISPLAY       = 0x6061;
constexpr uint16_t IDX_TARGET_POSITION    = 0x607A;
constexpr uint16_t IDX_POSITION_ACTUAL    = 0x6064;
constexpr uint16_t IDX_PROFILE_VELOCITY   = 0x60FF;
constexpr uint16_t IDX_ERROR_CODE         = 0x603F;

// ==========================
// 🧩 CiA402 控制字掩码
// ==========================
constexpr uint16_t CW_SHUTDOWN           = 0x0006;
constexpr uint16_t CW_SWITCH_ON          = 0x0007;
constexpr uint16_t CW_ENABLE_OPERATION   = 0x000F;
constexpr uint16_t CW_FAULT_RESET        = 0x0080;

// ==========================
// 🧠 状态字关键位
// ==========================
constexpr uint16_t SW_READY_TO_SWITCH_ON = 0x0001 << 0;
constexpr uint16_t SW_SWITCHED_ON        = 0x0001 << 1;
constexpr uint16_t SW_OPERATION_ENABLED  = 0x0001 << 2;
constexpr uint16_t SW_FAULT              = 0x0001 << 3;
constexpr uint16_t SW_TARGET_REACHED     = 0x0001 << 10;

// ==========================
// 📦 通用类型别名
// ==========================
using EC_Index = uint16_t;
using EC_SubIdx = uint8_t;
using AxisID = int;

// ==========================
// 🔧 辅助函数：十六进制打印
// ==========================
inline std::string to_hex(uint16_t value)
{
    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << value;
    return ss.str();
}

} // namespace l6n
