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
	io.FontDefault = io.Fonts->AddFontFromFileTTF("res/fonts/Tillana-Regular.ttf", 21.0f);
#pragma endregion

#pragma region Geometry

	float vertices[] = {
		//coordinate			//colors				//texture coordinate
		-0.5f, 0.5f, 1.0f,		1.0f, 0.0f,	0.0f,		0.0f, 1.0f,
		 0.5f, 0.5f, 0.0f,		1.0f, 0.0f,	0.0f,		1.0f, 1.0f,
		 0.5f,-0.5f, 1.0f,		0.0f, 0.0f,	1.0f,		1.0f, 0.0f,
		-0.5f,-0.5f, 0.0f,		0.0f, 0.0f,	1.0f,		0.0f, 0.0f
	};

	int indices[] = {
		0,1,2,3
	};

	VertexBuffer vertexBufferObject();

	unsigned int vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 4, vertices, GL_STATIC_DRAW);

	unsigned int indexBuffer;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 4, indices, GL_STATIC_DRAW);

	//Attribute pointer
	//position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
	//Color Attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
	//texture coordinate Attribute
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));

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
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("res/images/pic1.png", &width, &height, &nChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Image not loaded" << std::endl;
	}
	stbi_image_free(data);

	ShaderSource shaderSource = ParseShader("src/shaders/shader.glsl");

	unsigned int program = CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
	glUseProgram(program);
#pragma endregion

#pragma region FrameBuffer
	/*
	unsigned int frameBuffer;
	glGenBuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	unsigned int frameBufferTexture;
	glGenBuffers(1, &frameBufferTexture);
	glBindTexture(GL_FRAMEBUFFER, frameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	unsigned int renderBuffer;
	glGenBuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	*/
#pragma endregion

#pragma region VariablesBeforeMainLoop
	bool draw = false, dockOverViewport = true;
#pragma endregion

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

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

			if (ImGui::BeginMenu("View")) {



				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help")) {
				ImGui::MenuItem("Documentation");
				ImGui::MenuItem("About");
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGui::Begin("Viewport");
		ImVec2 viewSize = ImGui::GetContentRegionAvail();
		ImGui::Text("X : %f\nY : %f", viewSize.x, viewSize.y);
		ImGui::End();

		ImGui::Begin("Add Menu");
		if (ImGui::BeginMenu("Mesh"))
		{
			ImGui::MenuItem("Plane");
			ImGui::MenuItem("Cube");
			ImGui::MenuItem("Cylinder");
			ImGui::MenuItem("Sphere");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Curve"))
		{
			ImGui::MenuItem("Bezier");
			ImGui::MenuItem("Circle");
			ImGui::MenuItem("Path");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Light"))
		{
			ImGui::MenuItem("Point");
			ImGui::MenuItem("Directional");
			ImGui::MenuItem("Spot");
			ImGui::MenuItem("Area");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Armature"))
		{
			ImGui::MenuItem("Human");
			ImGui::MenuItem("Single");
			ImGui::MenuItem("Basic");
			ImGui::MenuItem("Animals");
			ImGui::EndMenu();
		}
		ImGui::End();

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
		ImGui::Checkbox("Dockspace over viewport", &dockOverViewport);
		ImGui::End();

		if (draw)
			glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, nullptr);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwPollEvents();
		glfwSwapBuffers(window);

	}

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