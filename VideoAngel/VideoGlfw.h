#ifndef _VIDEOGLFW_
#define _VIDEOGLFW_

#include <iostream>

// TODO: 在此处引用程序需要的其他标头。
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define STRING(x) #x
#define SHADER(x) "" STRING(x)

// 视频数据显示类，OpenGL渲染
class VideoGlfw
{
public:
	/// @brief 
	VideoGlfw();
	~VideoGlfw();

private:
    //uint8_t *m_data = nullptr;
    int m_width = 0;
    int m_height = 0;

private:
	/// @brief 快速日志句柄
	std::shared_ptr<spdlog::logger> m_logger;

	/// @brief GLFW窗口句柄
	GLFWwindow* m_glfw_window = nullptr;

	/// @brief 顶点渲染器
	GLuint m_vertex_shader;

	/// @brief 片段渲染器
	GLuint m_fragment_shader;

	/// @brief 渲染OpenGL的程序
	GLuint m_shader_program;

	/// @brief 顶点缓冲对象
	GLuint m_VBO;

	/// @brief 顶点数组对象
	GLuint m_VAO;

	///// @brief 元素缓冲对象
	GLuint m_EBO;

	/// @brief 2D纹理对象
	GLuint m_texture_2D;

public:
    /// @brief 初始化GLFW的各种参数
    /// @param width 视频数据宽度
    /// @param height 高度
    /// @return 0成功
    int InitVideoGlfw(int width, int height);

    /// @brief 显示数据
    /// @param width 宽度
    /// @param m_height 高度
    /// @param data 数据
    /// @return 
    int ShowVideo(const uint64_t& width, const uint64_t& height, const uint8_t* data);

private:
    void ProcessInput(GLFWwindow* window);

    void ExitVideoAngel();

private:
    /// @brief 顶点渲染器坐标设置
    const float vertices[4 * 5] = {
        //渲染的位置                   纹理坐标
        1.0f,1.0f,0.0f,             1.0f,0.0f,
        1.0f,-1.0f,0.0f,            1.0f,1.0f,
        -1.0f,-1.0f,0.0f,           0.0f,1.0f,
        -1.0f,1.0f,0.0f,            0.0f,0.0f,
    };
    /// @brief 绘制两个三角（合起来为矩形）的渲染区域
    const unsigned int indices[2 * 3] = {
        0,1,3,
        1,2,3
    };

    const char* vertex_shader_source = (char*)SHADER(\
        #version 330 core\n
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec2 aTexCoord;

        out vec2 TexCoord;

        void main()
        {
            gl_Position = vec4(aPos, 1.0);
            TexCoord = aTexCoord;
        }
    );
    /// @brief 片段着色器片段
    const char* fragment_shader_source = (char*)SHADER(\
        #version 330 core\n
        out vec4 FragColor;

        in vec2 TexCoord;

        uniform sampler2D ourTexture;

        void main()
        {
            FragColor = texture(ourTexture, TexCoord);
        }
    );

};

#endif // !_VEDIOGLFW_
