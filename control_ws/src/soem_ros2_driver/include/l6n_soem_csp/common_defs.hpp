#pragma once
#include <cstdint>

/**
 * @file common_defs.hpp
 * @brief 定义 EtherCAT 通信中常用的常量、索引号、掩码与类型别名。
 *        所有模块（soem_iface、cia402_fsm、axis、motion_manager）均可包含此文件。
 */

// ==========================
// EtherCAT 索引定义（标准 CiA402）
// ==========================
constexpr uint16_t IDX_CONTROLWORD        = 0x6040;  // 控制字
constexpr uint16_t IDX_STATUSWORD         = 0x6041;  // 状态字
constexpr uint16_t IDX_MODES_OF_OPERATION = 0x6060;  // 工作模式（8 = CSP）
constexpr uint16_t IDX_TARGET_POSITION    = 0x607A;  // 目标位置
constexpr uint16_t IDX_ACTUAL_POSITION    = 0x6064;  // 实际位置
constexpr uint16_t IDX_PROFILE_VELOCITY   = 0x60FF;  // 轮廓速度
constexpr uint16_t IDX_ERROR_CODE         = 0x603F;  // 错误码（用于报警诊断）

// ==========================
// CiA402 控制字位掩码
// ==========================
constexpr uint16_t CW_SHUTDOWN           = 0x0006;  // 关闭使能
constexpr uint16_t CW_SWITCH_ON          = 0x0007;  // 上电
constexpr uint16_t CW_ENABLE_OPERATION   = 0x000F;  // 使能操作
constexpr uint16_t CW_FAULT_RESET        = 0x0080;  // 故障复位

// ==========================
// 状态字关键位掩码
// ==========================
constexpr uint16_t SW_READY_TO_SWITCH_ON = 0x0001 << 0;
constexpr uint16_t SW_SWITCHED_ON        = 0x0001 << 1;
constexpr uint16_t SW_OPERATION_ENABLED  = 0x0001 << 2;
constexpr uint16_t SW_FAULT              = 0x0001 << 3;
constexpr uint16_t SW_TARGET_REACHED     = 0x0001 << 10;

// ==========================
// 类型别名与通用定义
// ==========================
using EC_Index = uint16_t;   // EtherCAT 索引
using EC_SubIdx = uint8_t;   // 子索引
using AxisID = int;          // 轴号标识
