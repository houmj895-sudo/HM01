### 注意在（非venv环境里操作）

我们要安装 **ROS 2 Jazzy（Ubuntu 24.04 官方版）**。

请先执行这一步（添加 ROS 2 官方源）：

```bash
sudo apt update && sudo apt install -y curl gnupg2 lsb-release
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null
```

---

### ✅ 步骤 1：检查 ROS 版本

```bash
ros2 --version
```

如果已安装，会输出类似：

```
ros2 0.32.6 (Jazzy Jalisco)
```

---

### ✅ 步骤 2：验证基础命令是否可用

```bash
ros2 run demo_nodes_cpp talker
```

若成功，会看到类似输出：

```
[INFO] [talker]: Publishing: 'Hello World: 1'
```

这说明 ROS 2 的核心节点运行正常。

---

### ✅ 步骤 3（可选）：检查环境变量

```bash
printenv | grep ROS
```

确认是否加载了 ROS 2 的环境变量（例如 `/opt/ros/jazzy/setup.bash`）。

---

非常好，HMJ 👍

你这条输出说明 **ROS 2 的命令行工具已安装**，只是它不支持 `--version` 参数（ROS 官方工具确实没有这个参数）。
所以我们用其他方式验证它的版本。

---

### ✅ 现在执行这个命令来确认版本：

```bash
ros2 doctor --report
```

输出中请留意前几行，应该包含：

```
ROS Distro: jazzy
```

---

如果你看到 `jazzy`，那就说明你的系统确实安装的是 **ROS 2 Jazzy (Ubuntu 24.04 版)**。

