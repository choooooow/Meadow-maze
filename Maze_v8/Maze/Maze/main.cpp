#include "learnOpenGL\stdafx.h"
#include <iostream>
#include <cmath>
//#define GLEW_STATIC
//#include <GL/glew.h>
#include "glad/glad.h"
#include <GL/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "learnOpenGL\Camera.h"
#include "learnOpenGL\shader.h"
#include "learnOpenGL\stb_image.h"
#include "learnOpenGL\Maze.h"
#include "learnOpenGL\cloth.h"
#include "learnOpenGL\model.h"
#include "learnOpenGL\wave.h"


#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouseMove(GLFWwindow* window, double xpos, double ypos);
void mouseClick(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, Maze* & our_maze);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);
void RenderScene(Shader &shader, unsigned int & planeVAO, unsigned int & cubeVAO, Maze* & our_maze, bool useTexture, unsigned int cubeTexture = 0, unsigned int normalMap = 0);

void renderPool(Shader wallShader, float clipPlane[4]);
unsigned int initializeReflectionFBO();
unsigned int initializeRefractionFBO();

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
float cubeVertices[] = {
	// 顶点				  //法向量			  //纹理坐标  //切线			 //副切线
	//上
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
	//左
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	//右
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	//前
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	//后
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
};
float skyboxVertices[] = {
	// positions          
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
//地板平面
float planeVertices[]{
	 12.0f, -0.5f,  12.0f, 0.0f, 1.0f, 0.0f, 12.0f,  0.0f,
	-12.0f, -0.5f, -12.0f, 0.0f, 1.0f, 0.0f,  0.0f, 12.0f,
	-12.0f, -0.5f,  12.0f, 0.0f, 1.0f, 0.0f,  0.0f,  0.0f,

	 12.0f, -0.5f,  12.0f, 0.0f, 1.0f, 0.0f, 12.0f,  0.0f,
	 12.0f, -0.5f, -12.0f, 0.0f, 1.0f, 0.0f, 12.0f, 12.0f,
	-12.0f, -0.5f, -12.0f, 0.0f, 1.0f, 0.0f,  0.0f, 12.0f
};
float quadVertices[] = {   // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
						   // positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
};
// 水面

float poolWidth = 1;			// 池子原始的长宽高
float poolLength = 1;
float poolHeight = 0.4;		    // 水深
float poolHeightUp = 0.2;		// 池子比水面高出的高度
float water[] = { // all y set to 0

	-poolLength,  poolWidth,
	 poolLength,  poolWidth,
	 poolLength, -poolWidth,
	-poolLength,  poolWidth,
	 poolLength, -poolWidth,
	-poolLength, -poolWidth
};
// 池子的墙
float wall[] = {
	// position texture
	-poolLength,  poolHeightUp, -poolWidth,   0, 1,
	-poolLength, -poolHeight, -poolWidth,   0, 0,
	 poolLength, -poolHeight, -poolWidth,   1, 0,
	 poolLength, -poolHeight, -poolWidth,   1, 0,
	 poolLength,  poolHeightUp, -poolWidth,   1, 1,
	-poolLength,  poolHeightUp, -poolWidth,   0, 1,

	 poolLength,  poolHeightUp,  poolWidth,   0, 1,
	 poolLength, -poolHeight,  poolWidth,   0, 0,
	-poolLength, -poolHeight,  poolWidth,   1, 0,
	-poolLength, -poolHeight,  poolWidth,   1, 0,
	-poolLength,  poolHeightUp,  poolWidth,   1, 1,
	 poolLength,  poolHeightUp,  poolWidth,   0, 1,

	-poolLength,  poolHeightUp,  poolWidth,   0, 1,
	-poolLength, -poolHeight,  poolWidth,   0, 0,
	-poolLength, -poolHeight, -poolWidth,   1, 0,
	-poolLength, -poolHeight, -poolWidth,   1, 0,
	-poolLength,  poolHeightUp, -poolWidth,   1, 1,
	-poolLength,  poolHeightUp,  poolWidth,   0, 1,

	 poolLength,  poolHeightUp, -poolWidth,   0, 1,
	 poolLength, -poolHeight, -poolWidth,   0, 0,
	 poolLength, -poolHeight,  poolWidth,   1, 0,
	 poolLength, -poolHeight,  poolWidth,   1, 0,
	 poolLength,  poolHeightUp,  poolWidth,   1, 1,
	 poolLength,  poolHeightUp, -poolWidth,   0, 1,

};
// 池子的底
float floorVertex[] = {

	-poolLength, -poolHeight, -poolWidth,   0, 1,
	-poolLength, -poolHeight,  poolWidth,   0, 0,
	 poolLength, -poolHeight,  poolWidth,   1, 0,
	 poolLength, -poolHeight,  poolWidth,   1, 0,
	 poolLength, -poolHeight, -poolWidth,   1, 1,
	-poolLength, -poolHeight, -poolWidth,   0, 1,
};

//窗口大小
int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

//定义摄像机
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;

float objectSize = 0.04f;
bool firstMouse = true;

//定义光源
glm::vec3 lightPos(-21.0f, 35.0f, -35.0f);

//帧与帧时间差
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//用于记录鼠标按下并移动
//bool mouseMoveView = false;
bool mouseMoveView = true;
//控制迷宫小地图的显示
int mazeMap = 1;
bool M_KEY_ACT = true;


// Cloth
Cloth cloth;

// Pool
unsigned int reflectionColorBuffer;
unsigned int refractionColorBuffer;
float reflect_plane[4] = { 0, 1, 0, 0 };
float refract_plane[4] = { 0, -1, 0, 0 };
//float plane[4] = { 0, -1, 0, 100000 };
float plane[4] = { 0, -1, 0, 0 };
unsigned int wallVAO, floorVAO;
unsigned int texture1, texture2;
unsigned int DuDvTexture;
unsigned int normalTexture;

float wave_speed = 0.0001f;
//float wave_speed = 0.0001f;
float moveFactor = 0;
// 移动 & 缩放整个水池，直接在这里改位置和大小就好
glm::vec3 poolMove(0.0, -0.3, 1.0);
glm::vec3 poolScale(1.0, 1.0, 1.0);
int main()
{
	//初始化
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	//创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Maze", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//将此窗口的上下文设置为当前线程的主上下文
	glfwMakeContextCurrent(window);
	
	//GLAD加载
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	//回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouseMove);		//鼠标移动
	//glfwSetMouseButtonCallback(window, mouseClick);		//鼠标点击
	glfwSetScrollCallback(window, scroll_callback);		//鼠标滚轮
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	//隐藏鼠标

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);

	//着色器
	Shader cubeShader("shader/cube_shader.vs", "shader/cube_shader.fs");
	Shader depthShader("shader/depth_shader.vs", "shader/depth_shader.fs");
	Shader skyboxShader("shader/skybox.vs", "shader/skybox.fs");
	Shader screenShader("shader/screen_shader.vs", "shader/screen_shader.fs");
	Shader clothShader("shader/cloth_shader.vs", "shader/cloth_shader.fs");
	Shader postShader("shader/post_shader.vs", "shader/post_shader.fs");
	Shader lampShader("shader/lamp.vert", "shader/lamp.frag");
	Shader waterShader("waterShader/water.vs", "waterShader/water.frag");
	Shader waterWallShader("waterShader/wallShader.vs", "waterShader/wallShader.frag");

	// shader configuration
	// --------------------
	cubeShader.use();
	cubeShader.setInt("diffuseTexture", 0);
	cubeShader.setInt("shadowMap", 1);
	cubeShader.setInt("normalMap", 2);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	screenShader.use();
	screenShader.setInt("screenTexture", 0);

	clothShader.use();
	clothShader.setInt("shadowMap", 0);

	postShader.use();
	postShader.setInt("shadowMap", 2);

	waterWallShader.use();
	waterWallShader.setInt("texture1", 0);



	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);	//顶点位置
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));	//法向量
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));	//纹理
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));	//tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));	//bitangent
	glEnableVertexAttribArray(4);
	glBindVertexArray(0);
	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// plane VAO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);	//顶点位置
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));	//法向量
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));	//纹理
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// setup screen VAO
	unsigned int quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// water & wall & floor
	unsigned int waterVBO, waterVAO, wallVBO, floorVBO; // NOTE: wallVAO is declared as global var for convenience
	glGenVertexArrays(1, &waterVAO);
	glGenBuffers(1, &waterVBO);
	glGenVertexArrays(1, &wallVAO);
	glGenBuffers(1, &wallVBO);
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(waterVAO);
	glBindBuffer(GL_ARRAY_BUFFER, waterVBO);	// water
	glBufferData(GL_ARRAY_BUFFER, sizeof(water), water, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(wallVAO);
	glBindBuffer(GL_ARRAY_BUFFER, wallVBO);		// wall
	glBufferData(GL_ARRAY_BUFFER, sizeof(wall), wall, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);		// floor
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertex), floorVertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	// Load textures
	//地板纹理
	unsigned int planeTexture = loadTexture("./img/plane4.jpg");
	//迷宫墙
	unsigned int cubeTexture = loadTexture("./img/wall_test.jpg");
	unsigned int normalMap = loadTexture("./img/wall_normal.jpg");	//法线纹理
	// 水池
	texture1 = loadTexture("textures/marble.bmp");
	texture2 = loadTexture("textures/wood.bmp");
	// load DuDv texture 
	DuDvTexture = loadTexture("textures/dudvMap.png");
	// load normal texture 
	normalTexture = loadTexture("textures/normalMap.png");

	vector<std::string> faces
	{
		"skybox/waterMountain/right.jpg",
		"skybox/waterMountain/left.jpg",
		"skybox/waterMountain/top.jpg",
		"skybox/waterMountain/bottom.jpg",
		"skybox/waterMountain/back.jpg",
		"skybox/waterMountain/front.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// configure MSAA framebuffer
	// --------------------------
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a multisampled color attachment texture
	unsigned int textureColorBufferMultiSampled;
	glGenTextures(1, &textureColorBufferMultiSampled);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
	// create a (also multisampled) renderbuffer object for depth and stencil attachments
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// configure second post-processing framebuffer
	unsigned int intermediateFBO;
	glGenFramebuffers(1, &intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
	// create a color attachment texture
	unsigned int screenTexture;
	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);  // we only need a color buffer

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//深度贴图的帧缓冲
	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	//创建一个2D纹理
	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	//环绕方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	//过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);	//不进行颜色数据的渲染,我们只需要深度信息
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//利用Maze类生成迷宫
	Maze* our_maze = new Maze();
	our_maze->Init(8, 8);
	our_maze->autoGenerateMaze();
	glm::vec3 exitPos = our_maze->getExit();
	// load models
	// -----------
	Model post("./model/post/rv_lamp_post_2.obj");

	// Wave
	// Speed of wind, direction of wind
	/*
	float modelScale = 0.05;
	wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1);
	wave_model->setShader("shader/surface.vert", "shader/surface.frag");
	wave_model->initBufferObjects();

	glm::vec3 lampPos = sundir * 50.0f;
	glm::vec3 movePos(0, -4, 25);			// 控制wave位置
	*/
	//Render loop
	unsigned int reflectionFBO = initializeReflectionFBO();
	unsigned int refractionFBO = initializeRefractionFBO();

	float framX = 1600;

	while (!glfwWindowShouldClose(window))
	{
		// 帧的时间差
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		framX++;
		// Check and call events
		processInput(window, our_maze);

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Render depth of scene to texture (from light's perspective)
		// get light projection/view matrix.
		glm::mat4 lightProjection;
		GLfloat near_plane = 40.0f, far_plane = 80.0f;
		lightProjection = glm::perspective(glm::radians(50.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));	//从光源看向场景中央
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;	//变换矩阵T
		glm::mat4 model;
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		
		//1. 以光源视角渲染场景，得到深度纹理图（透视投影）
		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);	//渲染深度贴图
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		RenderScene(depthShader, planeVAO, cubeVAO, our_maze, false);
		model = glm::translate(glm::mat4(), glm::vec3(0.1f, 0.4f, 0.0f) + exitPos);
		model = glm::scale(model, glm::vec3(0.02f, 0.016f, 0.016f));
		depthShader.setMat4("model", model);
		cloth.update(deltaTime);
		cloth.draw();
		model = glm::translate(glm::mat4(), glm::vec3(0.0f, -0.5f, 0.0f) + exitPos);
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));
		depthShader.setMat4("model", model);
		post.Draw(depthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		// 水池		??? don't work
		glEnable(GL_CLIP_DISTANCE0); // enable clip distance
		glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
		float distance = 2 * (camera.Position.y - 0);
		camera.Pitch = -camera.Pitch; // invert camera pitch
		renderPool(waterWallShader, reflect_plane);
		// reset camera back to original position
		camera.Pitch = -camera.Pitch;
		// render refraction texture
		glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
		renderPool(waterWallShader, refract_plane);
		glDisable(GL_CLIP_DISTANCE0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//2. 正常渲染 （透视投影+Phong Shading）
		glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);	//重置视角
		// draw scene as normal in multisampled buffers
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cubeShader.use();
		cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", projection);
        cubeShader.setVec3("lightPos", lightPos);
        cubeShader.setVec3("viewPos", camera.Position);
        cubeShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		
		//绑定纹理 & 绘制地板
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		//迷宫 & 地板
		RenderScene(cubeShader, planeVAO, cubeVAO, our_maze, true, cubeTexture, normalMap);
		
		// Render flag 旗子
		clothShader.use();
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		model = glm::translate(glm::mat4(), glm::vec3(0.1f, 0.4f, 0.0f) + exitPos);
		model = glm::scale(model, glm::vec3(0.02f, 0.016f, 0.016f));
		clothShader.setMat4("view", view);
		clothShader.setMat4("projection", projection);
		clothShader.setMat4("model", model);
		clothShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		clothShader.setVec3("lightPos", lightPos);
		clothShader.setVec3("viewPos", camera.Position);
		cloth.draw();

		postShader.use();
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		model = glm::translate(glm::mat4(), glm::vec3(0.0f, -0.5f, 0.0f) + exitPos);
		model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));
		postShader.setMat4("projection", projection);
		postShader.setMat4("view", view);
		postShader.setMat4("model", model);
		postShader.setVec3("lightPos", lightPos);
		postShader.setVec3("viewPos", camera.Position);
		postShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		post.Draw(postShader);

		// Wave
		/*
		wave_model->buildTessendorfWaveMesh(deltaTime);
		wave_model->draw(camera, modelScale, lampPos, movePos,SCR_WIDTH, SCR_HEIGHT);
		*/
		// 画水池
		renderPool(waterWallShader, plane);
		// render water
		waterShader.use();
		waterShader.setInt("reflectionTexture", 0);
		waterShader.setInt("refractionTexture", 1);
		waterShader.setInt("dudvMap", 2);
		waterShader.setInt("normalMap", 3);

		// pass camera position 
		waterShader.setVec3("cameraPosition", camera.Position);
		glm::vec3 lightPosition = poolMove + glm::vec3(0.0, 3.0, 0.0);
		waterShader.setVec3("lightPosition", lightPosition);
		waterShader.setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));
		// wave 
		moveFactor += wave_speed * framX * deltaTime;
		if (moveFactor >= 1) {
			moveFactor -= 1;
			framX = 1600;
		}
		waterShader.setFloat("moveFactor", moveFactor);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, reflectionColorBuffer);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, refractionColorBuffer);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, DuDvTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalTexture);
		glBindVertexArray(waterVAO);
		// do transformations
		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		waterShader.setMat4("projection", projection);
		// camera/view transformation
		view = camera.GetViewMatrix();
		waterShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, poolMove);
		model = glm::scale(model, poolScale);
		waterShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// 天空盒
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		// now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
		glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		// now render quad with scene's visuals as its texture image
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// draw Screen quad
		screenShader.use();
		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screenTexture); // use the now resolved color attachment as the quad's texture
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &skyboxVBO);
	glDeleteBuffers(1, &planeVBO);
	glfwTerminate();

	delete our_maze;
	return 0;
}


void RenderScene(Shader &shader, unsigned int & planeVAO, unsigned int & cubeVAO, Maze* & our_maze, bool useTexture, unsigned int cubeTexture, unsigned int normalMap) {
	

	//地板
	shader.setInt("object", 1);
	
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(10, 0, -10));

	shader.setMat4("model", model);
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	//迷宫
	if (useTexture) {
		shader.setInt("object", 2);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, normalMap);
	}
	our_maze->DrawMaze(shader, cubeVAO);
	if (mazeMap == 0) {		//请求帮助，显示小地图
		our_maze->DrawMap(camera.Position.x, camera.Position.z);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}
	
// draw everything aside from water
void renderPool(Shader wallShader, float clipPlane[4])
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture1); // marble texture

	wallShader.use();
	// pass clip plane
	wallShader.setVec4("plane", glm::vec4(clipPlane[0], clipPlane[1], clipPlane[2], clipPlane[3]));
	// do transformations
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	wallShader.setMat4("projection", projection);
	// camera/view transformation
	glm::mat4 view = camera.GetViewMatrix();
	wallShader.setMat4("view", view);

	glBindVertexArray(wallVAO);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, poolMove);
	model = glm::scale(model, poolScale);
	wallShader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, 24);

	// draw floor
	glBindVertexArray(floorVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture2); // floor texture
	glDrawArrays(GL_TRIANGLES, 0, 6);

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

/*
 * WSADQE键盘控制相机漫游
 * W & S：前进&后退
 * A & D：左右横移
 * Q & E：左右调整面向
 * 空格：跳起，可以根据按下的时间长度控制跳起的高度
 * M: 显示/隐藏小地图
*/


void processInput(GLFWwindow *window, Maze* & our_maze)
{
	float keyViewMoveSpeed = 2;
	Camera old_camera = camera;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
		our_maze->doCollisions(camera, objectSize, FORWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		our_maze->doCollisions(camera, objectSize, BACKWARD, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
		our_maze->doCollisions(camera, objectSize, LEFT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
		our_maze->doCollisions(camera, objectSize, RIGHT, deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessMouseMovement(-keyViewMoveSpeed, 0);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessMouseMovement(keyViewMoveSpeed, 0);

	// jump
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.ProcessKeyboard(JUMP, deltaTime);
	}
	// 显示or隐藏小地图
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
		if (M_KEY_ACT) {
			mazeMap = 1 - mazeMap;
			M_KEY_ACT = false;
		} 
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) {
		M_KEY_ACT = true;
	}
	/*
	// 调整海面振幅
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) {
		A *= 1.5;
		wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1);
		wave_model->setShader("shader/surface.vert", "shader/surface.frag");
		wave_model->initBufferObjects();
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE) {
		A /= 1.5;
		wave_model = new Wave(N, M, L_x, L_z, omega, V, A, 1);
		wave_model->setShader("shader/surface.vert", "shader/surface.frag");
		wave_model->initBufferObjects();
	}
	*/
}

/*
 * 鼠标按下左键并移动：视角随着鼠标而移动
 * 鼠标松开：不移动
*/

void mouseMove(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}
void mouseClick(GLFWwindow* window, int button, int action, int mods) {
	/*
	//左键按下
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		mouseMoveView = true;	//可以进行视角旋转了
		firstMouse = true;
	}
	//松开
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		mouseMoveView = false;	//不可旋转
	}
	*/
}

//鼠标滚轮监控
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

//普通2D纹理
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	//过滤方式
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

//天空盒纹理
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
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

// reflectionColorBuffer set as global var for convenience
unsigned int initializeReflectionFBO()
{
	// reflection frame buffer
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// create a color attachment texture
	unsigned int depthRenderBuffer;
	glGenTextures(1, &reflectionColorBuffer);
	glBindTexture(GL_TEXTURE_2D, reflectionColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionColorBuffer, 0);
	// create a depth render buffer attachment
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer); // now actually attach it
																										// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Reflection framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fbo;
}

unsigned int initializeRefractionFBO()
{
	// refraction frame buffer
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// create a color attachment texture
	unsigned int depthBuffer;
	glGenTextures(1, &refractionColorBuffer);
	glBindTexture(GL_TEXTURE_2D, refractionColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionColorBuffer, 0);
	// create a depth attachment texture
	glGenTextures(1, &depthBuffer);
	glBindTexture(GL_TEXTURE_2D, depthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Refraction framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fbo;
}
