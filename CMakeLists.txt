﻿# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/source-charset:utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/execution-charset:gbk>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:char8_t->")


#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
project ("VideoAngel")
set(CMAKE_CXX_STANDARD 11)

#add_definitions("-DUNICODE" "-D_UNICODE")

# 包含子项目。
add_subdirectory ("VideoAngel")
