#include "VideoGlfw.h"

VideoGlfw::VideoGlfw()
{
	m_logger = spdlog::stdout_color_mt("VideoGlfw");
	m_logger->info("初始化！！！");
}

VideoGlfw::~VideoGlfw()
{
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glad_glViewport(0, 0, width, height);
}

int VideoGlfw::InitVideoGlfw(int width,int height)
{

	int state = glfwInit();

	//设置版本参数
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//初始化窗口
	m_glfw_window = glfwCreateWindow(864, 486, "VideoAngel", NULL, NULL);
	if (m_glfw_window == nullptr)
	{
		m_logger->error("GLFW的窗口句柄初始化失败！！！");
		glfwTerminate();
		return -1;
	}

	//为窗口绑定上下文
	glfwMakeContextCurrent(m_glfw_window);

	//初始化GLAD组件
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		m_logger->error("初始化GLAD组件失败！！！");
		return -2;
	}

	//设置窗口尺寸
	glad_glViewport(0, 0, 864, 486);


	//生成纹理对象，第一个参数为数量，第二个参数为纹理的对象数组，存放申请的纹理对象数组
	glad_glGenTextures(1, &m_texture_2D);

	//设定纹理对象类型
	glad_glBindTexture(GL_TEXTURE_2D, m_texture_2D);
	

	//设定纹理参数
	glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//创建顶点着色器
	m_vertex_shader = glad_glCreateShader(GL_VERTEX_SHADER);
	//绑定着色器源码 第一个参数着色器对象，第二参数指定了传递的源码字符串数量，第三个参数是顶点着色器真正的源码，第四个参数我们先设置为NULL
	glad_glShaderSource(m_vertex_shader, 1, &vertex_shader_source, nullptr);
	//编译顶点着色器
	glad_glCompileShader(m_vertex_shader);

	//检查是否编译成功
	int success;
	char compile_info[512];
	glad_glGetShaderiv(m_vertex_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		//编译失败
		glad_glGetShaderInfoLog(m_vertex_shader, sizeof(compile_info), nullptr, compile_info);
		m_logger->error(std::string() + "顶点着色器编译失败--" + compile_info);
		return -3;
	}

	//创建片段着色器
	m_fragment_shader = glad_glCreateShader(GL_FRAGMENT_SHADER);
	//绑定着色器源码
	glad_glShaderSource(m_fragment_shader, 1, &fragment_shader_source, nullptr);
	//编译着色器
	glad_glCompileShader(m_fragment_shader);

	//检查是否编译成功
	glad_glGetShaderiv(m_fragment_shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		//编译失败
		glad_glGetShaderInfoLog(m_fragment_shader, 512, nullptr, compile_info);
		m_logger->error(std::string() + "片段着色器编译失败-" + compile_info);
		return -3;
	}

	//创建程序对象
	m_shader_program = glad_glCreateProgram();
	//附加着色器
	glad_glAttachShader(m_shader_program, m_vertex_shader);
	glad_glAttachShader(m_shader_program, m_fragment_shader);
	//链接生成程序对象
	glad_glLinkProgram(m_shader_program);


	//清理着色器对象，
	glad_glDeleteShader(m_vertex_shader);
	glad_glDeleteShader(m_fragment_shader);

	//生成缓冲对象
	glad_glGenBuffers(1, &m_VBO);
	glad_glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glad_glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//顶点数组对象
	glad_glGenVertexArrays(1, &m_VAO);
	glad_glBindVertexArray(m_VAO);

	//绑定元素缓冲对象，绘制的区域
   //GL_ELEMENT_ARRAY_BUFFER类型会被glad_glDrawElements触发
	glad_glGenBuffers(1, &m_EBO);
	glad_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glad_glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//取出顶点坐标
	glad_glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glad_glEnableVertexAttribArray(0);

	//取出纹理坐标
	glad_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glad_glEnableVertexAttribArray(1);

	//注册窗口变动回调函数
	glfwSetFramebufferSizeCallback(m_glfw_window, &FramebufferSizeCallback);

	//初始化渲染数据
	m_width = width;
	m_height = height;

	return 0;
}

int VideoGlfw::ShowVideo(const uint64_t& width, const uint64_t& height, const uint8_t* data)
{

	if (m_glfw_window == nullptr)
	{
		return -1;
	}
	if (!glfwWindowShouldClose(m_glfw_window))
	{
		//处理数据输入
		ProcessInput(m_glfw_window);

		//生成纹理
		glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		//m_logger->
		glad_glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
		glad_glClear(GL_COLOR_BUFFER_BIT);

		glad_glActiveTexture(GL_TEXTURE0);
		glad_glBindTexture(GL_TEXTURE_2D, m_texture_2D);

		glad_glUseProgram(m_shader_program);
		glad_glBindVertexArray(m_VAO);

		//渲染多边形（矩形）区域
		//glad_glDrawArrays(GL_POLYGON, 0, 4);
		
		//渲染区域
		glad_glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(m_glfw_window);
		glfwPollEvents();
		return true;
	}
	else
	{
		ExitVideoAngel();
		return -2;
	}
	return -1;
}

void VideoGlfw::ProcessInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void VideoGlfw::ExitVideoAngel()
{
	glad_glDeleteVertexArrays(1, &m_VAO);
	glad_glDeleteBuffers(1, &m_VAO);
	glad_glDeleteBuffers(1, &m_VAO);

	//停止GLFW窗口
	glfwTerminate();

	m_glfw_window = nullptr;

	m_logger->info("退出视频渲染器");
}
