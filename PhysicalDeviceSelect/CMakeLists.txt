# CMakeList.txt: VulkanWindowCreate 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#


# 将源代码添加到此项目的可执行文件。
file(GLOB SOURCES *.cpp)
file(GLOB HEADERS *.h)
add_executable (PhysicalDeviceSelect ${SOURCES} ${HEADERS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET PhysicalDeviceSelect PROPERTY CXX_STANDARD 20)
endif()
#链接库依赖
target_link_libraries(PhysicalDeviceSelect Vulkan::Vulkan  glfw)
# TODO: 如有需要，请添加测试并安装目标。

