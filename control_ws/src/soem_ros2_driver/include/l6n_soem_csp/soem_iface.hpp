#pragma once
/**
 * @file soem_iface.hpp
 * @brief SOEM EtherCAT 接口封装类（适配新版 SOEM API）。
 *        提供主站初始化、PDO 映射、分布式时钟、状态切换、SDO 读写、
 *        过程数据收发与资源释放功能。
 *
 * 设计思路：
 *  - 使用 ecx_* 接口（基于上下文对象，更安全）
 *  - IOmap 采用 vector<uint8_t> 管理，避免裸指针
 *  - 支持多实例使用（不依赖全局变量）
 */

#include <string>
#include <vector>
#include <cstdint>
#include <soem/soem.h>
#include "l6n_soem_csp/logger.hpp"
#include "l6n_soem_csp/common_defs.hpp"

namespace l6n {

class SoemIface {
public:
    explicit SoemIface(const Logger &logger);

    bool init(const std::string &ifname);             // 初始化主站接口
    bool configureMapAndDC();                         // PDO + DC 配置
    bool requestState(uint16_t state, int timeout_us = EC_TIMEOUTSTATE); // 状态切换
    void writeState(uint16_t slave, uint16_t state);  // 写入状态字
    int processIO(int recv_timeout_us = EC_TIMEOUTRET); // 发送/接收 PDO
    void close();                                     // 关闭通信

    template<typename T>
    bool sdoRead(int slave, EC_Index idx, EC_SubIdx subidx, T &out_val); // 读取 SDO

    template<typename T>
    bool sdoWrite(int slave, EC_Index idx, EC_SubIdx subidx, const T &val); // 写入 SDO

    int slaveCount() const { return slave_cnt_; }
    const ec_slavet *slaveList() const { return ecx_context_.slavelist; }
    bool isInitialized() const { return initialized_; }
    int lastWkc() const { return last_wkc_; }

private:
    Logger logger_;
    ecx_contextt ecx_context_;      // SOEM 上下文（替代全局 ec_*）
    std::vector<uint8_t> IOmap_;    // PDO 映射内存
    int last_wkc_{0};               // 工作计数
    int slave_cnt_{0};              // 从站数量
    bool initialized_{false};       // 是否已初始化

    SoemIface(const SoemIface &) = delete;
    SoemIface &operator=(const SoemIface &) = delete;
};

} // namespace l6n
