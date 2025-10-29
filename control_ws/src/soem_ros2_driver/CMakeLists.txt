cmake_minimum_required(VERSION 3.16)
project(soem_ros2_driver)

# ============================================================
# 📦 依赖声明
# ============================================================
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# ============================================================
# 🔍 查找 SOEM 头文件与静态库
# ============================================================
# 注意：如果你通过 apt 或源码安装了 SOEM，请确保路径正确
find_path(SOEM_INCLUDE_DIRS soem/soem.h
  PATHS /usr/local/include /usr/include
)

find_library(SOEM_LIB soem
  PATHS /usr/local/lib /usr/lib
  NAMES soem libsoem.a
)

if(NOT SOEM_INCLUDE_DIRS OR NOT SOEM_LIB)
  message(FATAL_ERROR
    "❌ SOEM library not found. Please install it under /usr/local or /usr.")
endif()

# ============================================================
# ⚙️ 构建节点：CSP 控制节点
# ============================================================
add_executable(csp_control_node src/csp_control_node.cpp)

# 包含 SOEM 头文件路径
target_include_directories(csp_control_node PRIVATE ${SOEM_INCLUDE_DIRS})

# 链接 SOEM 库
target_link_libraries(csp_control_node ${SOEM_LIB})

# 注册 ROS 依赖
ament_target_dependencies(csp_control_node
  rclcpp
  std_msgs
)

# ============================================================
# 🚀 安装规则
# ============================================================
install(
  TARGETS csp_control_node
  DESTINATION lib/${PROJECT_NAME}
)

# 若有 launch 或配置文件，可在此添加
# install(DIRECTORY launch/
#   DESTINATION share/${PROJECT_NAME}/launch
# )

# ============================================================
# 🧩 导出包信息
# ============================================================
ament_package()
