#!/bin/bash
# ======================================================
# EtherCAT CSP 控制节点启动脚本（SOEM ROS2 驱动）
# 适配：Ubuntu 24.04 + ROS2 Jazzy
# ======================================================

set -e

# ----------------------------------------
# 🧩 1. 基础参数配置
# ----------------------------------------
IFACE="enp4s0"    # ⚠️ 根据实际网卡名修改
PROJECT_DIR="$HOME/桌面/HM01/control_ws"
NODE_PATH="$PROJECT_DIR/install/soem_ros2_driver/lib/soem_ros2_driver/csp_node"

# ----------------------------------------
# 🧹 2. Ctrl+C 清理函数
# ----------------------------------------
cleanup() {
  echo ""
  echo "---------------------------------------"
  echo "🌐 正在恢复网卡配置: $IFACE ..."
  sudo nmcli dev set $IFACE managed yes
  sudo ip link set $IFACE promisc off || true
  sudo nmcli device connect $IFACE || echo "⚠️ 自动重连失败，请手动重启网络。"
  echo "✅ 网络已恢复，可正常上网。"
  echo "---------------------------------------"
  exit 0
}
trap cleanup SIGINT

# ----------------------------------------
# ⚙️ 3. 配置网卡参数
# ----------------------------------------
echo "---------------------------------------"
echo "🔧 Step 1: 配置并优化网卡: $IFACE"

sudo nmcli dev set $IFACE managed no || true
sudo ip link set $IFACE down
sudo ip addr flush dev $IFACE
sudo ip link set $IFACE up

echo "⚙️ 锁定速率为 100Mbps 全双工..."
sudo ethtool -s $IFACE speed 100 duplex full autoneg off

echo "⚙️ 关闭中断延迟..."
sudo ethtool -C $IFACE rx-usecs 0 tx-usecs 0

echo "⚙️ 禁用节能..."
sudo ethtool -s $IFACE wol d
sudo ethtool --set-eee $IFACE eee off 2>/dev/null || true

echo "⚙️ 启用混杂模式..."
sudo ip link set $IFACE promisc on

echo "⏳ 等待 $IFACE 启动..."
sleep 2
while ! ip link show $IFACE | grep -q "LOWER_UP"; do
  echo "   ...waiting for link..."
  sleep 1
done

echo "✅ $IFACE 已准备好用于 EtherCAT 通信。"
sudo ethtool $IFACE | grep -E "Speed|Duplex|Auto-negotiation|Link detected"

# ----------------------------------------
# 🚀 4. 启动 ROS2 控制节点
# ----------------------------------------
echo "---------------------------------------"
read -p "按 Enter 启动 EtherCAT CSP 控制节点..."

echo "🚀 启动 csp_node ..."
sudo -E LD_LIBRARY_PATH=/opt/ros/jazzy/lib:$LD_LIBRARY_PATH \
  $NODE_PATH
