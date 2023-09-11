# CMakeList.txt: VideoAngel 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 将源代码添加到此项目的可执行文件。
add_executable (VideoAngel	"VideoAngel.cpp" 
							"VideoAngel.h" 
							"main.cpp" 
							"VideoGlfw.h" 
							"VideoGlfw.cpp" 
							"Common.h" 
							"VideoDesc.h"
							"VideoDesc.cpp"
							"LowAudio.cpp" 
							"LowAudio.h" 
							"CircleBuffer.h"
							"CircleBuffer.cpp"
							"RingBuffer.h"
							"RingBuffer.cpp"
 "CvOperater.cpp" "CvOperater.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VideoAngel PROPERTY CXX_STANDARD 20)
endif()

find_path(MINIAUDIO_INCLUDE_DIRS "miniaudio.h")
target_include_directories(VideoAngel PRIVATE ${MINIAUDIO_INCLUDE_DIRS})

find_package(FFMPEG REQUIRED)
target_include_directories(VideoAngel PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_directories(VideoAngel PRIVATE ${FFMPEG_LIBRARY_DIRS})
target_link_libraries(VideoAngel PRIVATE ${FFMPEG_LIBRARIES})

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(VideoAngel PRIVATE glfw)

find_package(glad CONFIG REQUIRED)
target_link_libraries(VideoAngel PRIVATE glad::glad)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(VideoAngel PRIVATE spdlog::spdlog_header_only)

find_package(Stb REQUIRED)
target_include_directories(VideoAngel PRIVATE ${Stb_INCLUDE_DIR})

find_package(OpenCV CONFIG REQUIRED)
target_link_libraries(VideoAngel PRIVATE opencv_ml opencv_core opencv_highgui opencv_cudaimgproc)
# opencv_dnn  opencv_flann

# TODO: 如有需要，请添加测试并安装目标。
