#include "l6n_soem_csp/soem_iface.hpp"
#include <cstring>

namespace l6n {

SoemIface::SoemIface(const Logger &logger)
    : logger_(logger)
{
    std::memset(&ecx_context_, 0, sizeof(ecx_context_));
    IOmap_.resize(65536, 0); // 分配 64KB IOmap
}

bool SoemIface::init(const std::string &ifname)
{
    if (initialized_) {
        logger_.warn("⚠️ SOEM 已初始化，跳过重复初始化。");
        return true;
    }

    logger_.info("🔌 初始化 EtherCAT 主站接口，网卡：" + ifname);

    // 1️⃣ 初始化网卡接口
    if (!ecx_init(&ecx_context_, ifname.c_str())) {
        logger_.error("❌ ecx_init 失败，请检查网卡名或权限。");
        return false;
    }

    // 2️⃣ 扫描从站
    slave_cnt_ = ecx_config_init(&ecx_context_);
    if (slave_cnt_ <= 0) {
        logger_.error("❌ 未检测到任何从站，请检查接线、电源或网卡。");
        ecx_close(&ecx_context_);
        return false;
    }

    logger_.info("✅ 检测到从站数量：" + std::to_string(slave_cnt_));
    for (int i = 1; i <= slave_cnt_; ++i) {
        const auto &s = ecx_context_.slavelist[i];
        logger_.info(" - 从站[" + std::to_string(i) + "] 名称: " + std::string(s.name));
    }

    initialized_ = true;
    return true;
}

bool SoemIface::configureMapAndDC()
{
    if (!initialized_) {
        logger_.error("❌ configureMapAndDC 调用时 SOEM 尚未初始化。");
        return false;
    }

    logger_.info("⚙️ 开始进行 PDO 映射与分布式时钟配置。");

    // ✅ 使用新版接口 ecx_config_map_group()
    if (ecx_config_map_group(&ecx_context_, IOmap_.data(), 0) <= 0) {
        logger_.error("❌ ecx_config_map_group 执行失败。");
        return false;
    }

    // 配置分布式时钟（DC）
    ecx_configdc(&ecx_context_);
    logger_.info("✅ 分布式时钟配置完成。");
    return true;
}

bool SoemIface::requestState(uint16_t state, int timeout_us)
{
    if (!initialized_) return false;
    ecx_writestate(&ecx_context_, 0);
    int chk = ecx_statecheck(&ecx_context_, 0, state, timeout_us);
    bool ok = (chk == state);
    logger_.info("🧭 状态切换到 " + std::to_string(state) + (ok ? " 成功" : " 失败"));
    return ok;
}

void SoemIface::writeState(uint16_t slave, uint16_t state)
{
    if (!initialized_) return;
    ecx_context_.slavelist[slave].state = state;
    ecx_writestate(&ecx_context_, slave);
}

int SoemIface::processIO(int recv_timeout_us)
{
    if (!initialized_) return -1;
    ecx_send_processdata(&ecx_context_);
    last_wkc_ = ecx_receive_processdata(&ecx_context_, recv_timeout_us);
    return last_wkc_;
}

template<typename T>
bool SoemIface::sdoRead(int slave, EC_Index idx, EC_SubIdx subidx, T &out_val)
{
    if (!initialized_) return false;
    int size = sizeof(T);

    // ✅ 修正调用参数顺序（含 boolean CA 参数）
    int w = ecx_SDOread(&ecx_context_, slave, idx, subidx, FALSE, &size, &out_val, EC_TIMEOUTRXM);
    if (w <= 0) {
        logger_.error("❌ SDO 读取失败: slave=" + std::to_string(slave) +
                      " idx=0x" + std::to_string(idx) +
                      " sub=" + std::to_string(subidx));
        return false;
    }
    return true;
}

template<typename T>
bool SoemIface::sdoWrite(int slave, EC_Index idx, EC_SubIdx subidx, const T &val)
{
    if (!initialized_) return false;
    int size = sizeof(T);
    int w = ecx_SDOwrite(&ecx_context_, slave, idx, subidx, FALSE, size, &val, EC_TIMEOUTRXM);
    if (w <= 0) {
        logger_.error("❌ SDO 写入失败: slave=" + std::to_string(slave) +
                      " idx=0x" + std::to_string(idx) +
                      " sub=" + std::to_string(subidx));
        return false;
    }
    return true;
}

void SoemIface::close()
{
    if (!initialized_) return;
    logger_.info("🔚 关闭 EtherCAT 通信。");
    ecx_close(&ecx_context_);
    initialized_ = false;
}

// 模板实例化（确保编译器生成对应实现）
template bool SoemIface::sdoRead<uint16_t>(int, EC_Index, EC_SubIdx, uint16_t &);
template bool SoemIface::sdoWrite<uint16_t>(int, EC_Index, EC_SubIdx, const uint16_t &);
template bool SoemIface::sdoRead<int32_t>(int, EC_Index, EC_SubIdx, int32_t &);
template bool SoemIface::sdoWrite<int32_t>(int, EC_Index, EC_SubIdx, const int32_t &);
template bool SoemIface::sdoWrite<int8_t>(int, EC_Index, EC_SubIdx, const int8_t &);


} // namespace l6n
