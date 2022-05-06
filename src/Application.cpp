#include<iostream>
#include<sstream>
#include<string>
#include<fstream>

#include<GLEW/glew.h>
#include<GLFW/glfw3.h>

#include<ImGUI/imgui.h>
#include<ImGUI/imgui_impl_glfw.h>
#include<ImGUI/imgui_impl_opengl3.h>

#include<stb_image/stb_image.h>

#include"VertexBuffer.h"
#include"IndexBuffer.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

struct ShaderSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderSource ParseShader(const std::string& filePath) {

	std::ifstream stream(filePath);
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};
	std::string line;
	std::stringstream ss[2];
	ShaderType shaderType = ShaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos)
			{
				shaderType = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				shaderType = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)shaderType] << line << '\n';
		}
	}
	stream.close();
	return { ss[0].str() ,ss[1].str() };
}

unsigned int CompileShader(std::string& _shader_, unsigned int type)
{
	unsigned int _shader_id_ = glCreateShader(type);
	const char* shaderSrc = _shader_.c_str();
	glShaderSource(_shader_id_, 1, &shaderSrc, 0);
	glCompileShader(_shader_id_);
	int result;
	glGetShaderiv(_shader_id_, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		std::cout << "Error in " << (type == GL_VERTEX_SHADER ? "VertexShader" : "FragmentShader") << std::endl;
		int length;
		glGetShaderiv(_shader_id_, GL_INFO_LOG_LENGTH, &length);
		char* msg = (char*)malloc(sizeof(char) * length);
		glGetShaderInfoLog(_shader_id_, length, &length, msg);
		std::cout << msg << std::endl;
		free(msg);
		glDeleteShader(_shader_id_);
		return 0;
	}
	else
	{
		std::cout << (type == GL_VERTEX_SHADER ? "VertexShader" : "FragmentShader") << " Compiled...." << std::endl;
		return _shader_id_;
	}
}

unsigned int CreateShader(std::string& _vertex_shader_, std::string& _fragment_shader_)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(_vertex_shader_, GL_VERTEX_SHADER);
	unsigned int fs = CompileShader(_fragment_shader_, GL_FRAGMENT_SHADER);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}

int main()
{
#pragma region Libraries Initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Advance Graphics Engine v0.0.1", NULL, NULL);
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	glewInit();

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/Mohave-Regular.ttf", 21.0f);
#pragma endregion

#pragma region Geometry
	float vertices[] = {
		//coordinate			//colors				//texture coordinate
	   -0.5f, 0.5f, 1.0f,		1.0f, 0.0f,	0.0f,		0.0f, 1.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 0.0f,	0.0f,		1.0f, 1.0f,
		0.5f,-0.5f, 1.0f,		0.0f, 0.0f,	1.0f,		1.0f, 0.0f,
	   -0.5f,-0.5f, 0.0f,		0.0f, 0.0f,	1.0f,		0.0f, 0.0f
	};

	unsigned int indices[] = {
		0,1,2,3
	};

	VertexBuffer VBO(vertices, sizeof(float) * 4 * 8);

	IndexBuffer IBO(indices, 4);

	//position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	//Color Attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
	//texture coordinate Attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));
#pragma endregion

#pragma region Texture
	//texture
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Load texture
	int width, height, nChannels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load("res/images/pic2.png", &width, &height, &nChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Image not loaded" << std::endl;
	}
	stbi_image_free(data);

	glActiveTexture(GL_TEXTURE0);
#pragma endregion

	ShaderSource shaderSource = ParseShader("src/shaders/shader.glsl");
	unsigned int program = CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
	glUseProgram(program);

#pragma region RBO
	/*unsigned int frameBuffer;
	glGenFramebuffers(1, &frameBuffer);

	unsigned int colorBufferTexture;
	glGenTextures(1, &colorBufferTexture);
	glBindTexture(GL_TEXTURE_2D, colorBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	unsigned int renderBuffer;
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 800);

	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);*/
#pragma endregion

#pragma region VariablesBeforeMainLoop
	bool draw = false, dockOverViewport = true;
#pragma endregion

#pragma region MainLoop
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (dockOverViewport)
			ImGui::DockSpaceOverViewport();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File")) {
				ImGui::MenuItem("Open");
				ImGui::MenuItem("New");
				ImGui::MenuItem("Save");
				ImGui::MenuItem("Save as");
				if (ImGui::MenuItem("Exit")) {
					exit(0);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Demo Window"));
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Theme")) {
				if (ImGui::MenuItem("Light"))
					ImGui::StyleColorsLight();
				if (ImGui::MenuItem("Dark"))
					ImGui::StyleColorsDark();
				if (ImGui::MenuItem("Classic"))
					ImGui::StyleColorsClassic();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help")) {
				ImGui::MenuItem("Documentation");
				ImGui::MenuItem("About");
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

#pragma region ViewPort
		ImGui::Begin("Viewport");
		ImVec2 viewSize = ImGui::GetContentRegionAvail();
		ImGui::Image((void*)texture1, ImVec2{ viewSize.x ,viewSize.y });
		ImGui::End();
#pragma endregion

		ImGui::Begin("Statistics");
		ImGui::Text("Manufacturer : %s", glGetString(GL_VENDOR));
		ImGui::Text("Renderer : %s", glGetString(GL_RENDERER));
		ImGui::Text("Version : %s", glGetString(GL_VERSION));
		ImGui::Text("Time Elapsed : %f", glfwGetTime());
		ImGui::Text("Frame Rate : %f", io.Framerate);
		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::ShowStyleEditor();
		ImGui::End();

		ImGui::Begin("Scene");
		ImGui::Checkbox("Render", &draw);
		ImGui::Checkbox("Dock space over viewport", &dockOverViewport);
		ImGui::End();

		if (draw)
			glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, nullptr);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwPollEvents();
		glfwSwapBuffers(window);

	}
#pragma endregion

#pragma region CleaningUp
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
#pragma endregion

	return 0;
}



/*
Not Open Console Window
ProjectProperty>Linker>system>subsystem set to Windows (/SUBSYSTEM:WINDOWS)
ProjectProperty>Linker>Advance>mainCRTStartup
*/