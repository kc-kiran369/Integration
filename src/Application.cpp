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

#include<glm/vec3.hpp>

#pragma region GlobalDeclarations
bool isConsoleEnabled = true;
#pragma endregion

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
	glfwInit();
	//const char* title = "Advance Graphics Engine %s", glGetString(GL_VERSION);
	GLFWwindow* window = glfwCreateWindow(500, 500, "Advance Graphics Engine v0.0.1a", NULL, NULL);
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

	float vertices[] = {
		//coordinate			//colors			//texture coordinate
		 0.0f, 0.5f, 0.0f,		1.0f, 0.0f,	0.0f,	  0.0f, 0.5f,
		 0.5f,-0.5f, 0.0f,		0.0f, 1.0f,	0.0f,	  0.5f,-0.5f,
		-0.5f,-0.5f, 0.0f,		0.0f, 0.0f,	1.0f,	 -0.5f,-0.5f,
	};

	int indices[] = {
		0,1,2
	};

	unsigned int vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, vertices, GL_STATIC_DRAW);

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

	unsigned int indexBuffer;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3, indices, GL_STATIC_DRAW);

	ShaderSource shaderSource = ParseShader("src/shaders/shader.shad");

	unsigned int program = CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);
	glUseProgram(program);

	int location = glGetUniformLocation(program, "u_Color");
	glUniform3f(location, 0.1f, 0.1f, 0.1f);

	bool draw = true;
	float triangleColor[3] = { 1.0f,1.0f, 1.0f };
	float backgroundColor[3] = { 25.0f / 255.0f,25.0f / 255.0f, 25.0f / 255.0f };

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::DockSpaceOverViewport();


		if (draw)
		{
			glUniform3f(location, triangleColor[0], triangleColor[1], triangleColor[2]);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
		}

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File")) {
				ImGui::MenuItem("New");
				ImGui::MenuItem("Save");
				ImGui::MenuItem("Save as");
				if (ImGui::MenuItem("Exit")) {
					exit(0);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window")) {

				/*if (ImGui::MenuItem("Toggle System Window"))
					ToggleConsole();*/
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
				if (ImGui::MenuItem("About"))
				{
					/*ImGui::OpenPopup("Helo");*/
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

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

		ImGui::Begin("Property");
		ImGui::ColorEdit3("Triangle Color", triangleColor, 1);
		ImGui::ColorEdit3("Background Color", backgroundColor);
		ImGui::End();

		ImGui::Begin("Statistics");
		ImGui::Text("Manufacturer : %s", glGetString(GL_VENDOR));
		ImGui::Text("Renderer : %s", glGetString(GL_RENDERER));
		ImGui::Text("Version : %s", glGetString(GL_VERSION));
		ImGui::Text("Time Elapsed : %f", glfwGetTime());
		//ImGui::Text("Delta Time : %f", io.DeltaTime);
		ImGui::Text("Frame Rate : %f", io.Framerate);
		ImGui::End();

		ImGui::Begin("Settings");
		ImGui::ShowStyleEditor();
		ImGui::End();

		ImGui::Begin("Scene");
		ImGui::Checkbox("Render", &draw);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}