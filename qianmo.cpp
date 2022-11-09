
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "model.h"
#include "camera.h"
#include "render.h"
#include "Fountain.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
bool display = false;

bool cast = false;
bool wireframe = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float handup = 40.0f;
float speed = 1.0f;
float dTime = 0.0f;
float lastF = 0.0f;

#define X .525731112119133606 
#define Z .850650808352039932


int winW = 512;
int winH = 512;

int material = 0;
// These are the 12 vertices for the icosahedron
static GLfloat vdata[] = {
   -X, 0.0, Z, X, 0.0, Z, -X, 0.0, -Z, X, 0.0, -Z,
   0.0, Z, X, 0.0, Z, -X, 0.0, -Z, X, 0.0, -Z, -X,
   Z, X, 0.0, -Z, X, 0.0, Z, -X, 0.0, -Z, -X, 0.0
};

// These are the 20 faces.  Each of the three entries for each 
// vertex gives the 3 vertices that make the face.
static GLint tindices[] = {
   0,4,1, 0,9,4, 9,5,4, 4,5,8, 4,8,1,
   8,10,1, 8,3,10, 5,3,8, 5,2,3, 2,7,3,
   7,10,3, 7,6,10, 7,11,6, 11,0,6, 0,1,6,
   6,1,10, 9,0,11, 9,11,2, 9,2,5, 7,2,11 };

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Qian Mo", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Fail to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	Fountain::Fountain fountain;
	
	glEnable(GL_DEPTH_TEST);

	Shader shader("res/Shaders/cube.vert", "res/Shaders/cube.frag");
	Shader skyboxShader("res/Shaders/skybox.vert", "res/Shaders/skybox.frag");
	Shader sphShader("res/Shaders/sp.vert", "res/Shaders/sp.frag");
	Shader n20Shader("res/Shaders/n20.vert", "res/Shaders/n20.frag");

	float cubeVertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	float bodyVertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f
	};

	float skyboxVertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	//n20
	GLuint n20VAO, n20VBO,n20EBO;	
	float color[3] = { 0.3f, 0.45f, 0.6f };
	glGenVertexArrays(1, &n20VAO);
	glGenBuffers(1, &n20VBO);
	glGenBuffers(1, &n20EBO);
	glBindVertexArray(n20VAO);
	glBindBuffer(GL_ARRAY_BUFFER, n20VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vdata), vdata, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, n20EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tindices), tindices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// body vao
	unsigned int bodyVAO, bodyVBO;
	glGenVertexArrays(1, &bodyVAO);
	glGenBuffers(1, &bodyVBO);
	glBindVertexArray(bodyVAO);
	glBindBuffer(GL_ARRAY_BUFFER, bodyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bodyVertices), &bodyVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(6 * 5 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(33 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	unsigned int cubeTexture = loadTexture("res/Textures/jin.png");
	unsigned int marc = loadTexture("res/Textures/Marc_Dekamps.jpg");
	unsigned int albedo = loadTexture("res/Textures/Mercator-projection.png");
	unsigned int normal = loadTexture("res/Textures/normal.png");
	unsigned int metallic = loadTexture("res/Textures/metallic.png");
	unsigned int roughness = loadTexture("res/Textures/roughness.png");
	unsigned int ao = loadTexture("res/Textures/ao.png");
	vector<std::string> faces
	{
		/*
		FileSystem::getPath("resources/textures/skybox/right.jpg"),
		FileSystem::getPath("resources/textures/skybox/left.jpg"),
		FileSystem::getPath("resources/textures/skybox/top.jpg"),
		FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
		FileSystem::getPath("resources/textures/skybox/front.jpg"),
		FileSystem::getPath("resources/textures/skybox/back.jpg")*/
		"res/Textures/right.jpg",
		"res/Textures/left.jpg",
		"res/Textures/top.jpg",
		"res/Textures/bottom.jpg",
		"res/Textures/front.jpg",
		"res/Textures/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	shader.use();
	shader.setInt("texture1", 0);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	sphShader.use();
	sphShader.setInt("albedoMap", 0);
	sphShader.setInt("normalMap", 1);
	sphShader.setInt("metallicMap", 2);
	sphShader.setInt("roughnessMap", 3);
	sphShader.setInt("aoMap", 4);

	while (!glfwWindowShouldClose(window))
	{	
		if (!display)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::vec3 lightPos(-8.0f, 1.5f, -10.0f);
		glm::vec3 lightColor(150.0f, 150.0f, 150.0f);

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		sphShader.use();
		sphShader.setVec3("camPos", camera.Position);
		sphShader.setVec3("lightPositions", lightPos);
		sphShader.setVec3("lightColors", lightColor);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 60.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		sphShader.setMat4("model", model);
		sphShader.setMat4("view", view);
		sphShader.setMat4("projection", projection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, albedo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, metallic);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, roughness);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, ao);
		renderSphere();


		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(lightPos) + glm::vec3(-0.5f, -0.5f, -0.5f));
		model = glm::rotate(model, glm::radians(120.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		sphShader.setMat4("model", model);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, marc);
		renderSphere();

		//n20
		n20Shader.use();
		n20Shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		n20Shader.setVec3("objectColor", color[0], color[1], color[2]);
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f * speed), glm::normalize(glm::vec3(0.0, 0.0, 1.0))); 
		model = glm::translate(model, glm::vec3(-1.8f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		n20Shader.setMat4("model", model);
		n20Shader.setMat4("view", view);
		n20Shader.setMat4("projection", projection);
		glBindVertexArray(n20VAO);
		glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		shader.use();		
		shader.setVec3("light.position", lightPos);
		shader.setVec3("viewPos", camera.Position);
		//light			
		shader.setVec3("light.ambient",1.2f, 1.2f, 1.2f);
		shader.setVec3("light.diffuse", 1.5f, 1.5f, 1.5f);
		shader.setVec3("light.specular", 2.0f, 2.0f, 2.0f);

		//material
		if (material == 1) {
			shader.setVec3("material.ambient", 0.0f, 0.1f, 0.06f);
			shader.setVec3("material.diffuse", 0.0f, 0.50980392f, 0.50980392f);
			shader.setVec3("material.specular", 0.50196078f, 0.50196078f, 0.50196078f);
			shader.setFloat("material.shininess", 32.0f);
		}
		else if (material == 0) {
			shader.setVec3("material.ambient", 0.0f,0.05f,0.05f);
			shader.setVec3("material.diffuse", 0.4f,0.5f,0.5f);
			shader.setVec3("material.specular", 0.07f,0.7f,0.7f);
			shader.setFloat("material.shininess", .078125f);
		}
		//body
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f * speed), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
		model = glm::translate(model, glm::vec3(-1.5f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.1f, 0.2f));
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//right arm
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f * speed), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
		model = glm::translate(model, glm::vec3(-1.7f, 0.0f, -0.1f));
		model = glm::rotate(model, glm::radians(handup), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		model = glm::translate(model, glm::vec3(0.2f, 0.0f, 0.05f));
		model = glm::scale(model, glm::vec3(0.3f, 0.1f, 0.07f));
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//left arm
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f * speed), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
		model = glm::translate(model, glm::vec3(-1.5f, 0.0f, 0.2f));
		model = glm::rotate(model, glm::radians(-40.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0))); // rotate the quad to show normal mapping from multiple directions
		model = glm::scale(model, glm::vec3(0.3f, 0.1f, 0.07f));
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//legs
		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f * speed), glm::normalize(glm::vec3(0.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
		model = glm::translate(model, glm::vec3(-1.15f, 0.0f, 0.05f));
		model = glm::scale(model, glm::vec3(0.3f, 0.1f, 0.07f));
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f * speed), glm::normalize(glm::vec3(0.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
		model = glm::translate(model, glm::vec3(-1.15f, 0.0f, -0.05f));
		model = glm::scale(model, glm::vec3(0.3f, 0.1f, 0.07f));
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		//particle
		if (cast) {
			model = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians((float)glfwGetTime() * 40.0f * speed), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
			model = glm::translate(model, glm::vec3(-1.7f, 0.0f, -0.1f));
			model = glm::rotate(model, glm::radians(handup), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
			model = glm::translate(model, glm::vec3(0.3f, 0.0f, 0.05f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
			view = camera.GetViewMatrix();
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			shader.setMat4("model", model);
			shader.setMat4("view", view);
			shader.setMat4("projection", projection);
			fountain.Render(deltaTime, model, view, projection);
		}

		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); 


		ImGui::Begin("UI"); 
		ImGui::Text("Welcome to the space");		
		ImGui::Checkbox("Wireframe", &wireframe);
		ImGui::SliderFloat("Speed", &speed, 0.5f, 1.5f);
		ImGui::ColorEdit3("Head Color", color);

		static const char* wizardMaterial[] = { "Rubber", "Plastic" };
		static const char* currentMaterial = "Rubber";
		if (ImGui::BeginCombo("Wizard Material", currentMaterial))
		{
			for (int n = 0; n < IM_ARRAYSIZE(wizardMaterial); n++)
			{
				bool isSelected = (currentMaterial == wizardMaterial[n]);
				if (ImGui::Selectable(wizardMaterial[n], isSelected)) {
					material = n;
					currentMaterial = wizardMaterial[n];
				}
			}
			ImGui::EndCombo();
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &skyboxVAO);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		display = true;
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
		display = false;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		cast = true;
		
		if (handup <= 130)
			handup += 1;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		cast = false;
		if (handup >= 40)
			handup -= 1;
	}
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}