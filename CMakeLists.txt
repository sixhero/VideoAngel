cmake_minimum_required (VERSION 3.8)

if(CMAKE_HOST_UNIX)
  set(CMAKE_TOOLCHAIN_FILE "/root/Debian/vcpkg/scripts/buildsystems/vcpkg.cmake")
else()

endif()

project ("VideoAngel")

add_subdirectory ("VideoAngel")