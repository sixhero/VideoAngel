cmake_minimum_required (VERSION 3.8)


# 引入spdlog库之后使用的fmt强制要求使用 /utf-8 下方的参数设置会出现冲突
# add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
# add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:gbk>")
# add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:char8_t->")

if(CMAKE_HOST_UNIX)
  set(VCPKG_ROOT "/root/Debian/vcpkg")
else(CMAKE_HOST_WIN32)
  set(VCPKG_ROOT "F:/vcpkg")
endif()

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

project ("VideoAngel")

add_subdirectory ("VideoAngel")