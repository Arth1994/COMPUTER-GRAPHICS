// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

#include <iostream>
#include<stdio.h>
#include<string.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>


// Other includes
#include "nanogui/nanogui.h"
#include "../Assignment1/View.h"
#include "../Assignment1/CameraOrientation.h"
#include "../Assignment1/ShaderHelper.h"


/*****************

Assignment 1 - Computer Graphics CS 6366, Code Wriiten By Arth Shah axs175430

Sources referred - OpenGL Tutorials, Boiler Plate Code in Assignment 0, Tom Dalling blog's for Ambient Light, Directional Light
and Spot Lighting. Official Site for GLM Mathematics and boiler plate code given in Assignment 1.

The Works well as per specifications mentioned in the assignment document and satisfies all the requirements.

The Description :- 3 header files for camer position/orientation, ShaderHelper.h(For establishing basic.vrt and frag.vrt)

ShaderHelper.h was Shader.cpp in assignment 0.

int Main() has nano gui bar designed to the required specifications.

void Draw() :- mainly contains the output glDrawPloygonMode && glBindVertexArray() method and rendering in the form of solids points and traingles.

void loadObjectToDrawsInScene() :- Part of this method is taken from Code in Assignment 0 where we have glBufferData(GL_ARRAY_BUFFER, renderer.vao_vertices.size() * sizeof(Renderer::Vertex), &renderer.vao_vertices[0], GL_STATIC_DRAW);
where points are loaded in the scene.

void illuminate() - mainly to load camera position and for light amplification.

**********************/

using namespace nanogui;

enum RenderType {
	solid,
	points,
	lines
};
enum Culling {
	CW,
	CCW,
};

class Render {


	struct RenderPoint {
		bool status;

		glm::vec3 position;
		float constant;
		float linear;
		float quadratic;

		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;

	};

public:

	RenderPoint renderPoint;

	void initialize()
	{

		renderPoint.status = true;
		renderPoint.position = glm::vec3(1.2f, 1.0f, 2.0f);
		renderPoint.ambient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		renderPoint.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		renderPoint.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		renderPoint.constant = 1.0f;


	};
};

GLFWwindow* window;
Screen* screen = nullptr;
CameraOrientation* cameraOrientation = new CameraOrientation();
bool values[256];
std::vector<Renderer> objectToDrawArray;


std::string modelName = "./models/cyborg.obj";
RenderType renderType = lines;
Culling culling = CW;
bool reset = true;
GLfloat timeSlice = 0.0f;
GLfloat endFrame = 0.0f;
glm::vec4 backgroundColor = glm::vec4(0.1, 0.1, 0.1, 0.1);
bool enabled = true;
float X = 0, Y = 0;
GLfloat angle, radians;
double pie = 3.14159;
float floatPie = (float)pie;
Color colorValue(0.5f, 0.15f, 0.15f, 1.f);

Render* render = new Render();

void draw(Shader& shader, Renderer& objectToDraw)
{
	glBindVertexArray(objectToDraw.vao);

	glUniform3f(glGetUniformLocation(shader.program, "m_object.object_color"), objectToDraw.objectColor[0], objectToDraw.objectColor[1], objectToDraw.objectColor[2]);
	printf("%f, %f, %f\n", objectToDraw.objectColor[0], objectToDraw.objectColor[1], objectToDraw.objectColor[2]);
	glUniform1f(glGetUniformLocation(shader.program, "m_object.shininess"), objectToDraw.sharpness);
	glClear(GL_COLOR_BUFFER_BIT);
	if (renderType == solid) {
		glPolygonMode(GL_FRONT_AND_BACK, GLU_VERTEX);
		glDrawArrays(GL_TRIANGLES, 0, objectToDraw.vao_vertices.size());
	}
	else if (renderType == points) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_POINTS, 0, objectToDraw.vao_vertices.size());
	}
	else if (renderType == lines) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, objectToDraw.vao_vertices.size());
	}
	glBindVertexArray(0);
}

//Code from Assignment 0
void loadObjectToDrawsInScene(std::string modelName) {
	objectToDrawArray.clear();
	Renderer renderer(modelName);
	renderer.objectColor = glm::vec4(colorValue.r(), colorValue.g(), colorValue.b(), colorValue.w());
	renderer.objectName = "mainObjectToDraw";
	glGenVertexArrays(1, &renderer.vao);
	glGenBuffers(1, &renderer.vbo);
	glBindVertexArray(renderer.vao);
	glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);
	glBufferData(GL_ARRAY_BUFFER, renderer.vao_vertices.size() * sizeof(Renderer::Vertex), &renderer.vao_vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Renderer::Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Renderer::Vertex), (GLvoid*)offsetof(Renderer::Vertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Renderer::Vertex), (GLvoid*)offsetof(Renderer::Vertex, TexCoords));
	glBindVertexArray(0);
	objectToDrawArray.push_back(renderer);
}


void drawEntireScene(Shader& shader)
{
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], backgroundColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);


	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	if (culling == CW) {
		glFrontFace(GL_CW);
	}
	else if (culling == CCW)
	{
		glFrontFace(GL_CCW);
	}

	int i = 0;

	while (i < objectToDrawArray.size())
	{
		if (objectToDrawArray[i].objectName == "mainObjectToDraw")
		{
			glm::mat4 mainObjectToDraw_model_mat = glm::mat4();
			glUniformMatrix4fv(glGetUniformLocation(shader.program, "model"), 1, GL_FALSE, glm::value_ptr(mainObjectToDraw_model_mat));
			glUniformMatrix4fv(glGetUniformLocation(shader.program, "view"), 1, GL_FALSE, glm::value_ptr(cameraOrientation->loadCameraViewMatrix()));
			objectToDrawArray[i].objectColor = glm::vec4(colorValue[0], colorValue[1], colorValue[2], colorValue[3]);
			//printf("%f, %f, %f, %f \n\n", colorValue[0], colorValue[1], colorValue[2], colorValue[3]);
			draw(shader, objectToDrawArray[i]);
		}
		else {
			std::cout << "The Entry is not valid" << std::endl;
		}
		i++;
	}
}


void illuminateScene(Shader& shader)
{

	glUniform3f(glGetUniformLocation(shader.program, "camera_pos"), cameraOrientation->pos.x, cameraOrientation->pos.y, cameraOrientation->pos.z);
	glUniformMatrix4fv(glGetUniformLocation(shader.program, "projection"), 1, GL_FALSE, glm::value_ptr(cameraOrientation->loadCameraProjectionMatrix()));
	glUniform1i(glGetUniformLocation(shader.program, "point_light.status"), true);
	glUniform3f(glGetUniformLocation(shader.program, "point_light.position"), render->renderPoint.position[0], render->renderPoint.position[1], render->renderPoint.position[2]);
	glUniform3f(glGetUniformLocation(shader.program, "point_light.ambient"), render->renderPoint.ambient[0], render->renderPoint.ambient[1], render->renderPoint.ambient[2]);
	glUniform1f(glGetUniformLocation(shader.program, "point_light.constant"), render->renderPoint.constant);

}

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	cameraOrientation->init();
	render->initialize();

	window = glfwCreateWindow(cameraOrientation->width, cameraOrientation->height, "ASSIGNMENT1", nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	screen = new Screen();
	screen->initialize(window, true);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	int desiredHeight = cameraOrientation->height;
	int desiredWidth = cameraOrientation->width;
	glViewport(0, 0, desiredWidth, desiredHeight);
	glfwSwapInterval(0);
	glfwSwapBuffers(window);

	FormHelper *myGui = new FormHelper(screen);
	ref<nanogui::Window> nanoGuiWindow = myGui->addWindow(Eigen::Vector2i(0, 0), "NANO GUI CONTROL BAR");

	myGui->addGroup("COLOR");
	myGui->addVariable("COLOR", colorValue);

	myGui->addGroup("POSITION");
	static auto X = myGui->addVariable("X", cameraOrientation->pos.x);
	X->setSpinnable(true);
	static auto Y = myGui->addVariable("Y", cameraOrientation->pos.y);
	Y->setSpinnable(true);
	static auto Z = myGui->addVariable("Z", cameraOrientation->pos.z);
	Z->setSpinnable(true);

	myGui->addGroup("ROTATE");
	myGui->addVariable("ROTATE VALUE", radians)->setSpinnable(true);

	myGui->addButton("ROTATE RIGHT+", []()
	{
		cameraOrientation->rotX(glm::radians(radians));
	});
	myGui->addButton("ROTATE RIGHT-", []()
	{
		cameraOrientation->rotX(-glm::radians(radians));
	});
	myGui->addButton("ROTATE UP+", []()
	{
		cameraOrientation->rotY(-glm::radians(radians));
	});
	myGui->addButton("ROTATE UP-", []()
	{
		cameraOrientation->rotY(glm::radians(radians));
	});
	myGui->addButton("ROTATE FRONT+", []()
	{
		cameraOrientation->rotZ(-glm::radians(radians));
	});
	myGui->addButton("ROTATE FRONT-", []()
	{
		cameraOrientation->rotZ(glm::radians(radians));
	});

	myGui->addGroup("CONFIGURATION");
	myGui->addVariable("Z NEAR", cameraOrientation->zoomIn)->setSpinnable(true);
	myGui->addVariable("Z FAR", cameraOrientation->zoomOut)->setSpinnable(true);
	myGui->addVariable("RENDER TYPE", renderType, enabled)->setItems({ "SOLID", "POINTS", "LINES" });
	myGui->addVariable("CULLING TYPE", culling, enabled)->setItems({ "CW", "CCW" });
	myGui->addVariable("MODEL NAME", modelName);

	myGui->addButton("RELOAD MODEL", []()
	{
		loadObjectToDrawsInScene(modelName);
		cameraOrientation->reset();
	});
	myGui->addButton("RESET", []()
	{
		cameraOrientation->reset();
	});

	screen->setVisible(true);
	screen->performLayout();

	glfwSetCursorPosCallback(window,
		[](GLFWwindow *window, double x, double y) {
		screen->cursorPosCallbackEvent(x, y);
	}
	);

	glfwSetMouseButtonCallback(window,
		[](GLFWwindow *, int button, int action, int modifiers) {
		screen->mouseButtonCallbackEvent(button, action, modifiers);
		X->setValue(cameraOrientation->pos[0]);
		Y->setValue(cameraOrientation->pos[1]);
		Z->setValue(cameraOrientation->pos[2]);

	}
	);

	glfwSetKeyCallback(window,
		[](GLFWwindow *window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		if (key >= 0 && key < 256)
		{
			if (action == GLFW_PRESS)
			{
				values[key] = true;

			}
			else if (action == GLFW_RELEASE)
				values[key] = false;
		}
		X->setValue(cameraOrientation->pos[0]);
		Y->setValue(cameraOrientation->pos[1]);
		Z->setValue(cameraOrientation->pos[2]);
	}
	);

	glfwSetCharCallback(window,
		[](GLFWwindow *, unsigned int codepoint) {
		screen->charCallbackEvent(codepoint);
	}
	);

	glfwSetDropCallback(window,
		[](GLFWwindow *, int count, const char **filenames) {
		screen->dropCallbackEvent(count, filenames);
	}
	);

	glfwSetScrollCallback(window,
		[](GLFWwindow *, double x, double y) {
		screen->scrollCallbackEvent(x, y);

	}
	);

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow *, int width, int height) {
		screen->resizeCallbackEvent(width, height);
	}
	);


	Shader shader = Shader("./shader/basic.vert", "./shader/basic.frag");

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		loadObjectToDrawsInScene(modelName);
		shader.use();
		illuminateScene(shader);
		drawEntireScene(shader);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		screen->drawWidgets();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return EXIT_SUCCESS;
}
