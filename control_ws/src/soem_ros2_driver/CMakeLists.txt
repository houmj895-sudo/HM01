cmake_minimum_required(VERSION 3.16)
project(soem_ros2_driver)

# =========================================================
# 🧱 一、依赖声明
# =========================================================
find_package(ament_cmake REQUIRED)   # ROS2 构建系统
find_package(rclcpp REQUIRED)        # ROS2 C++ API
find_package(std_msgs REQUIRED)      # 常用消息类型
find_package(yaml-cpp REQUIRED)      # 🔥 YAML 配置解析库（需安装 libyaml-cpp-dev）

# =========================================================
# 🧰 二、查找 SOEM 库与头文件
# =========================================================
find_path(SOEM_INCLUDE_DIRS soem/soem.h
  PATHS
    ${CMAKE_SOURCE_DIR}/../SOEM/include
    /usr/local/include
    /usr/include
)

find_library(SOEM_LIB soem
  PATHS
    ${CMAKE_SOURCE_DIR}/../SOEM/build
    ${CMAKE_SOURCE_DIR}/../SOEM
    /usr/local/lib
    /usr/lib
  NAMES soem libsoem.a
)

if(NOT SOEM_INCLUDE_DIRS OR NOT SOEM_LIB)
  message(FATAL_ERROR "❌ 未找到 SOEM，请检查路径是否正确。")
endif()

message(STATUS "✅ 找到 SOEM include: ${SOEM_INCLUDE_DIRS}")
message(STATUS "✅ 找到 SOEM library: ${SOEM_LIB}")

# =========================================================
# 🧩 三、编译选项
# =========================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -O2)

# =========================================================
# 🧩 四、头文件路径
# =========================================================
include_directories(
  include
  ${SOEM_INCLUDE_DIRS}
)

# =========================================================
# 🧱 五、生成静态库（分层封装）
# =========================================================

# 🧱 EtherCAT 通信接口封装
add_library(soem_iface STATIC src/l6n_soem_csp/soem_iface.cpp)
ament_target_dependencies(soem_iface rclcpp std_msgs)

# ⚙️ CiA402 状态机封装
add_library(cia402_fsm STATIC src/l6n_soem_csp/cia402_fsm.cpp)
ament_target_dependencies(cia402_fsm rclcpp std_msgs)

# 🧭 单轴封装
add_library(axis STATIC src/l6n_soem_csp/axis.cpp)
ament_target_dependencies(axis rclcpp std_msgs)

# 🧩 多轴运动管理器
add_library(motion_manager STATIC src/l6n_soem_csp/motion_manager.cpp)
ament_target_dependencies(motion_manager rclcpp std_msgs)

# 🧾 日志系统封装
add_library(logger STATIC src/l6n_soem_csp/logger.cpp)
ament_target_dependencies(logger rclcpp std_msgs)

# ⚙️ YAML 配置解析模块（新）
add_library(config_loader STATIC src/l6n_soem_csp/config_loader.cpp)
ament_target_dependencies(config_loader rclcpp std_msgs)
target_link_libraries(config_loader yaml-cpp)

# =========================================================
# 🧩 六、构建节点（可执行文件）
# =========================================================

# 1️⃣ EtherCAT 从站扫描节点（验证用）
add_executable(slave_scan_node src/l6n_soem_csp/slave_scan_node.cpp)
target_link_libraries(slave_scan_node
  soem_iface
  ${SOEM_LIB}
)
ament_target_dependencies(slave_scan_node rclcpp std_msgs)

# 2️⃣ 主控制节点（多轴 CSP 控制）
add_executable(csp_node src/l6n_soem_csp/csp_node.cpp)
target_link_libraries(csp_node
  soem_iface
  cia402_fsm
  axis
  motion_manager
  logger
  config_loader      # ✅ 加入配置加载器
  yaml-cpp           # ✅ 链接 YAML 库
  ${SOEM_LIB}
)
ament_target_dependencies(csp_node rclcpp std_msgs)

# =========================================================
# 🧩 七、安装规则
# =========================================================
install(
  TARGETS
    slave_scan_node
    csp_node
    soem_iface
    cia402_fsm
    axis
    motion_manager
    logger
    config_loader
  DESTINATION lib/${PROJECT_NAME}
)

install(
  DIRECTORY include/
  DESTINATION include
)

# =========================================================
# ✅ 八、ament 打包导出
# =========================================================
ament_package()
