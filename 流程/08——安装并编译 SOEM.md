## 安装 SOEM 源码版

# 1.在非venv环境里操作

# 2.
```bash
cd ~/桌面/HM01/control_ws/src
```

# 3.编译soem

cd ~/桌面/HM01/control_ws/src/SOEM
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install


# 4.验证安装结果

cd ~/桌面/HM01/control_ws/src/SOEM/build/install/bin

./slaveinfo
如果显示：
SOEM (Simple Open EtherCAT Master)
Slaveinfo
Usage: slaveinfo ifname [options]
ifname = eth0 for example
Options :
 -sdo : print SDO info
 -map : print mapping

Available adapters:
    - lo  (lo)
    - enp4s0  (enp4s0)
    - wlo1  (wlo1)
End program

说明：
✅ slaveinfo 可执行文件能正常运行（编译 & 链接正确）

✅ 本机网卡接口（enp4s0 / wlo1）被识别到

⚠️ 因为没有接入从站（EtherCAT 伺服或 I/O），程序在 usage 模式下退出，属于预期现象

# 5.删除SOEM自己的.git

cd 到SOEM，然后rm -rf .git

# 6.将SOEM安装到系统路径
请在 VS Code 系统终端（非 venv） 中执行以下命令：

cd ~/桌面/HM01/control_ws/src/SOEM/build

sudo make install

检查：
ls /usr/local/lib | grep soem
如果看到 libsoem.a 或 libsoem.so，代表安装成功。
