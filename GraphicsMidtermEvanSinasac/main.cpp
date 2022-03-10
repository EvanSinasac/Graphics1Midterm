//Evan Sinasac - 1081418
//INFO6028 Graphics (Midterm)
//main.cpp description:
//					Graphics Midterm, making a space station scene consisting of two main parts, a hallway and the space port

#include "GLCommon.h"
#include "GLMCommon.h"
//Shader stuff in global things
#include "globalThings.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

//#include "lodepng.h"


//glm::vec3 cameraEye = glm::vec3(0.0f, 0.0f, 3.0f);     //default camera start position, just in case something goes wrong with loading the worldFile
//glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 1.0f);   //default camera Target position
//glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);       //default upVector

float lastX = 600.0f;
float lastY = 320.0f;
bool firstMouse = true;
float cameraYaw = 90.0f;
float cameraPitch = 0.0f;

float doorLightDirCurX = 1.0f;
float doorLightDirCurZ = 0.0f;
float doorLightRotateX = 1.0f;
float doorLightRotateZ = 1.0f;

double deltaTime = 0.0;

std::vector<std::string> modelLocations;


//Function signature for DrawObject()
void DrawObject(
	cMesh* pCurrentMesh,
	glm::mat4 matModel,
	GLint matModel_Location,
	GLint matModelInverseTranspose_Location,
	GLuint program,
	cVAOManager* pVAOManager);

//void DrawDebugObjects(
//	GLint matModel_Location,
//	GLint matModelInverseTranspose_Location,
//	GLuint program,
//	cVAOManager* pVAOManager);

void openBayDoors();
void closeBayDoors();

//callbacks
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

bool loadWorldFile();
bool loadModelsFromFile(GLuint& program);

int main(int argc, char** argv) 
{
	GLFWwindow* pWindow;

	GLuint program = 0;		//0 means no shader program

	GLint mvp_location = -1;
	std::stringstream ss;

	glfwSetErrorCallback(GLFW_error_callback);

	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	pWindow = glfwCreateWindow(1200, 640, "Graphics Midterm!", NULL, NULL);

	if (!pWindow)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(pWindow, GLFW_key_callback);

	glfwSetCursorEnterCallback(pWindow, GLFW_cursor_enter_callback);
	glfwSetCursorPosCallback(pWindow, GLFW_cursor_position_callback);
	glfwSetScrollCallback(pWindow, GLFW_scroll_callback);
	glfwSetMouseButtonCallback(pWindow, GLFW_mouse_button_callback);
	glfwSetWindowSizeCallback(pWindow, GLFW_window_size_callback);


	glfwSetCursorPosCallback(pWindow, mouse_callback);

	glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(pWindow);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);


	GLint max_uniform_location = 0;
	GLint* p_max_uniform_location = NULL;
	p_max_uniform_location = &max_uniform_location;
	glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, p_max_uniform_location);

	std::cout << "GL_MAX_UNIFORM_LOCATIONS: " << max_uniform_location << std::endl;

	// Create global things
	::g_StartUp(pWindow);

	/*::g_pFlyCamera->setEye(glm::vec3(0.0f, 0.0f, 20.0f));
	std::cout << "Fly Camera At: " << ::g_pFlyCamera->getAt().x << " " << ::g_pFlyCamera->getAt().y << " " << ::g_pFlyCamera->getAt().z << std::endl;*/

	// look this up
	/*glTransformFeedbackBufferBase;
	glTransformFeedbackBufferRange;
	glTransformFeedbackVaryings;*/


	cShaderManager::cShader vertShader;
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\shaders\\vertShader_01.glsl";
	vertShader.fileName = ss.str();

	cShaderManager::cShader fragShader;
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\shaders\\fragShader_01.glsl";
	fragShader.fileName = ss.str();

	if (::g_pShaderManager->createProgramFromFile("Shader#1", vertShader, fragShader))
	{
		std::cout << "Shader compiled OK" << std::endl;
		// 
		// Set the "program" variable to the one the Shader Manager used...
		program = ::g_pShaderManager->getIDFromFriendlyName("Shader#1");
	}
	else
	{
		std::cout << "Error making shader program: " << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;
	}


	// Select the shader program we want to use
	// (Note we only have one shader program at this point)
	glUseProgram(program);

	// *******************************************************
	// Now, I'm going to "load up" all the uniform locations
	// (This was to show how a map could be used)
	cShaderManager::cShaderProgram* pShaderProc = ::g_pShaderManager->pGetShaderProgramFromFriendlyName("Shader#1");
	int theUniformIDLoc = -1;
	theUniformIDLoc = glGetUniformLocation(program, "matModel");

	pShaderProc->mapUniformName_to_UniformLocation["matModel"] = theUniformIDLoc;

	// Or...
	pShaderProc->mapUniformName_to_UniformLocation["matModel"] = glGetUniformLocation(program, "matModel");

	pShaderProc->mapUniformName_to_UniformLocation["matView"] = glGetUniformLocation(program, "matView");
	pShaderProc->mapUniformName_to_UniformLocation["matProjection"] = glGetUniformLocation(program, "matProjection");
	pShaderProc->mapUniformName_to_UniformLocation["matModelInverseTranspose"] = glGetUniformLocation(program, "matModelInverseTranspose");

	// *******************************************************

	//GLint mvp_location = -1;
	mvp_location = glGetUniformLocation(program, "MVP");

	// Get "uniform locations" (aka the registers these are in)
	GLint matModel_Location = glGetUniformLocation(program, "matModel");
//	GLint matView_Location = glGetUniformLocation(program, "matView");
	GLint matProjection_Location = glGetUniformLocation(program, "matProjection");
	GLint matModelInverseTranspose_Location = glGetUniformLocation(program, "matModelInverseTranspose");


	//Lights stuff here
	
//    	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
//	                // 0 = pointlight
//					// 1 = spot light
//					// 2 = directional light

	//Hallway Lights ******************************************************************************
	::g_pTheLights->theLights[1].position = glm::vec4(-1.0f, 4.0f, 5.0f, 1.0f);
	::g_pTheLights->theLights[1].diffuse = glm::vec4(1.0f, 1.0f, 0.4f, 1.0f);
	::g_pTheLights->theLights[1].specular = glm::vec4(1.0f, 1.0f, 0.4f, 1.0f);
	::g_pTheLights->theLights[1].param1.x = 0.0f;	// Pointlight
	//::g_pTheLights->theLights[1].param1.y = 360.0f;   // Inner
	//::g_pTheLights->theLights[1].param1.z = 360.0f;   // Outer
	//::g_pTheLights->theLights[1].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[1].atten.x = 1.0f;
	::g_pTheLights->theLights[1].atten.y = 0.0001f;
	::g_pTheLights->theLights[1].atten.z = 0.1f;
	::g_pTheLights->TurnOnLight(1);

	::g_pTheLights->theLights[2].position = glm::vec4(1.0f, 4.0f, 17.0f, 1.0f);
	::g_pTheLights->theLights[2].diffuse = glm::vec4(1.0f, 1.0f, 0.4f, 1.0f);
	::g_pTheLights->theLights[2].specular = glm::vec4(1.0f, 1.0f, 0.4f, 1.0f);
	::g_pTheLights->theLights[2].param1.x = 0.0f;	// Pointlight
	//::g_pTheLights->theLights[2].param1.y = 360.0f;   // Inner
	//::g_pTheLights->theLights[2].param1.z = 360.0f;   // Outer
	//::g_pTheLights->theLights[1].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[2].atten.x = 1.0f;
	::g_pTheLights->theLights[2].atten.y = 0.0001f;
	::g_pTheLights->theLights[2].atten.z = 0.1f;
	::g_pTheLights->TurnOnLight(2);

	//Hanger Spotlights ******************************************************************
	::g_pTheLights->theLights[3].position = glm::vec4(-2.5f, 24.9, 33.5f, 1.0f);
	::g_pTheLights->theLights[3].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//::g_pTheLights->theLights[3].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_pTheLights->theLights[3].param1.x = 1.0f;	// Spotlight
	::g_pTheLights->theLights[3].param1.y = 10.0f;   // Inner
	::g_pTheLights->theLights[3].param1.z = 25.0f;   // Outer
	::g_pTheLights->theLights[3].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[3].atten.x = 1.0f;
	::g_pTheLights->theLights[3].atten.y = 0.05f;
	::g_pTheLights->theLights[3].atten.z = 0.005f;
	::g_pTheLights->TurnOnLight(3);
	
	::g_pTheLights->theLights[4].position = glm::vec4(-2.5f, 24.9, 48.5f, 1.0f);
	::g_pTheLights->theLights[4].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//::g_pTheLights->theLights[4].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_pTheLights->theLights[4].param1.x = 1.0f;	// Spotlight
	::g_pTheLights->theLights[4].param1.y = 10.0f;   // Inner
	::g_pTheLights->theLights[4].param1.z = 25.0f;   // Outer
	::g_pTheLights->theLights[4].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[4].atten.x = 1.0f;
	::g_pTheLights->theLights[4].atten.y = 0.05f;
	::g_pTheLights->theLights[4].atten.z = 0.005f;
	::g_pTheLights->TurnOnLight(4);

	::g_pTheLights->theLights[5].position = glm::vec4(-22.5f, 24.9, 33.5f, 1.0f);
	::g_pTheLights->theLights[5].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//::g_pTheLights->theLights[5].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_pTheLights->theLights[5].param1.x = 1.0f;	// Spotlight
	::g_pTheLights->theLights[5].param1.y = 15.0f;   // Inner
	::g_pTheLights->theLights[5].param1.z = 15.0f;   // Outer
	::g_pTheLights->theLights[5].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[5].atten.x = 1.0f;
	::g_pTheLights->theLights[5].atten.y = 0.05f;
	::g_pTheLights->theLights[5].atten.z = 0.005f;
	::g_pTheLights->TurnOnLight(5);

	::g_pTheLights->theLights[6].position = glm::vec4(-22.5f, 24.9, 48.5f, 1.0f);
	::g_pTheLights->theLights[6].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//::g_pTheLights->theLights[6].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_pTheLights->theLights[6].param1.x = 1.0f;	// Spotlight
	::g_pTheLights->theLights[6].param1.y = 10.0f;   // Inner
	::g_pTheLights->theLights[6].param1.z = 25.0f;   // Outer
	::g_pTheLights->theLights[6].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[6].atten.x = 1.0f;
	::g_pTheLights->theLights[6].atten.y = 0.05f;
	::g_pTheLights->theLights[6].atten.z = 0.005f;
	::g_pTheLights->TurnOnLight(6);

	::g_pTheLights->theLights[7].position = glm::vec4(-42.5f, 24.9, 33.5f, 1.0f);
	::g_pTheLights->theLights[7].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//::g_pTheLights->theLights[7].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_pTheLights->theLights[7].param1.x = 1.0f;	// Spotlight
	::g_pTheLights->theLights[7].param1.y = 10.0f;   // Inner
	::g_pTheLights->theLights[7].param1.z = 25.0f;   // Outer
	::g_pTheLights->theLights[7].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[7].atten.x = 1.0f;
	::g_pTheLights->theLights[7].atten.y = 0.05f;
	::g_pTheLights->theLights[7].atten.z = 0.005f;
	::g_pTheLights->TurnOnLight(7);

	::g_pTheLights->theLights[8].position = glm::vec4(-42.5f, 24.9, 48.5f, 1.0f);
	::g_pTheLights->theLights[8].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//::g_pTheLights->theLights[8].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_pTheLights->theLights[8].param1.x = 1.0f;	// Spotlight
	::g_pTheLights->theLights[8].param1.y = 10.0f;   // Inner
	::g_pTheLights->theLights[8].param1.z = 25.0f;   // Outer
	::g_pTheLights->theLights[8].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[8].atten.x = 1.0f;
	::g_pTheLights->theLights[8].atten.y = 0.05f;
	::g_pTheLights->theLights[8].atten.z = 0.005f;
	::g_pTheLights->TurnOnLight(8);

	//Hanger point light ***********************************************************************
	::g_pTheLights->theLights[9].position = glm::vec4(-25.0f, 12.0, 45.0f, 1.0f);
	//::g_pTheLights->theLights[9].diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f);
	//::g_pTheLights->theLights[9].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	::g_pTheLights->theLights[9].param1.x = 0.0f;	 // Point
	//::g_pTheLights->theLights[9].param1.y = 90.0f;   // Inner
	//::g_pTheLights->theLights[9].param1.z = 90.0f;   // Outer
	//::g_pTheLights->theLights[9].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[9].atten.x = 1.0f;
	::g_pTheLights->theLights[9].atten.y = 0.00000055f;		//0.055f
	::g_pTheLights->theLights[9].atten.z = 0.001f;			//0.0000001f
	::g_pTheLights->TurnOnLight(9);

	//::g_pTheLights->theLights[9].position = glm::vec4(0.0f, 100.0, 0.0f, 1.0f);//glm::vec4(-25.0f, 12.0, 45.0f, 1.0f);
	//::g_pTheLights->theLights[9].diffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	////::g_pTheLights->theLights[9].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	//::g_pTheLights->theLights[9].param1.x = 0.0f;	 // Point
	//::g_pTheLights->theLights[9].param1.y = 5.0f;   // Inner
	//::g_pTheLights->theLights[9].param1.z = 5.0f;   // Outer
	//::g_pTheLights->theLights[9].direction = glm::vec4(glm::normalize(glm::vec3(100'000.0f, -250'000.0f, 100'000.0f)), 1.0f);
	//::g_pTheLights->theLights[9].atten.x = 1.0f;
	////::g_pTheLights->theLights[9].atten.y = 0.00000055f;		//0.055f
	////::g_pTheLights->theLights[9].atten.z = 0.001f;			//0.0000001f
	//::g_pTheLights->theLights[9].atten.y = 0.0001f;		//0.055f
	//::g_pTheLights->theLights[9].atten.z = 0.00001f;			//0.0000001f
	//::g_pTheLights->TurnOnLight(9);


	//Hanger opening door light *****************************************************************
	::g_pTheLights->theLights[0].position = glm::vec4(-22.5f, 24.5, 40.0f, 1.0f);
	::g_pTheLights->theLights[0].diffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[0].specular = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[0].param1.x = 1.0f;	// Spotlight
	::g_pTheLights->theLights[0].param1.y = 10.0f;   // Inner
	::g_pTheLights->theLights[0].param1.z = 12.0f;   // Outer
	::g_pTheLights->theLights[0].direction = glm::vec4(1.0f, -0.1f, 0.0f, 1.0f);
	::g_pTheLights->theLights[0].atten.x = 1.0f;
	::g_pTheLights->theLights[0].atten.y = 0.0000001f;
	::g_pTheLights->theLights[0].atten.z = 0.00005f;
	//::g_pTheLights->TurnOnLight(0);


	// New light 11 to light whole scene
	::g_pTheLights->theLights[10].position = glm::vec4(100.0f, 100.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[10].atten.y = 0.0001f;
	::g_pTheLights->theLights[10].atten.z = 0.00001f;
	::g_pTheLights->theLights[10].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	::g_pTheLights->theLights[10].param1.x = 2.0f;    // directional
	::g_pTheLights->theLights[10].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
	::g_pTheLights->theLights[10].diffuse = glm::vec4(0.9922f, 0.9843f, 0.8275f, 1.0f);
	//::g_pTheLights->theLights[0].diffuse *= 0.0001f;
	::g_pTheLights->TurnOnLight(10);  // Or this!
	//::g_pTheLights->TurnOffLight(10);



	// Get the uniform locations of the light shader values
	::g_pTheLights->SetUpUniformLocations(program);

	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\models\\";

	::g_pVAOManager->setFilePath(ss.str());

	//ALL the models (loading from a text file wasn't working so we're doing it the less efficient way)	
	//modelLocations.push_back("SM_Env_Ceiling_Light_01_xyz_n_rgba.ply");
	//modelLocations.push_back("SM_Env_Ceiling_Light_02_xyz_n_rgba.ply");
	//modelLocations.push_back("SM_Env_Construction_Wall_01_xyz_n_rgba.ply");
	//modelLocations.push_back("SM_Env_Door_01_xyz_n_rgba.ply");
	modelLocations.push_back("SM_Env_Ceiling_Light_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Env_Ceiling_Light_02_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Env_Construction_Wall_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Env_Door_01_xyz_n_rgba_uv.ply");

	modelLocations.push_back("SM_Env_Floor_01_xyz_n_rgba_uv.ply");
	//modelLocations.push_back("SM_ENV_Floor_01_xyz_n_rgba.ply");
	
	modelLocations.push_back("SM_Env_Floor_04_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Env_Transition_Door_Curved_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Env_Wall_Curved_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Env_Wall_Curved_02_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Env_Wall_Curved_03_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Env_Wall_Curved_04_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Env_Wall_Curved_05_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Beaker_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Desk_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Desk_02_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Desk_03_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Desk_04_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Desk_Lab_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Desk_Lab_02_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Lockers_03_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Lockers_05_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Monitor_03_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Plants_03_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Plants_04_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Server_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_Server_03_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_StepLadder_01_xyz_n_rgba_uv.ply");
	modelLocations.push_back("SM_Prop_SwivelChair_01_xyz_n_rgba_uv.ply");

	// Follow along models
	//modelLocations.push_back("ISO_Sphere_flat_4div_xyz_n_rgba_uv.ply");
	modelLocations.push_back("Isosphere_Smooth_Normals.ply");
	modelLocations.push_back("Isosphere_Smooth_Inverted_Normals_for_SkyBox.ply");
	//modelLocations.push_back("Isosphere_Smooth_Inverted_Normals_for_SkyBox - Copy.ply");
	modelLocations.push_back("Lisse_mobile_shipyard_LARGER_xyz_n_rgba_uv.ply");
	modelLocations.push_back("mod-bomber_xyz_n_rgba_uv.ply");
	modelLocations.push_back("FlatQuad_xyz_n_rgba_uv.ply");

	modelLocations.push_back("SillyTerrain_REALLY_BIG_xyz_n_rgba_uv.ply");
	modelLocations.push_back("FlatTerrain_xyz_n_rgba_uv.ply");
	

	unsigned int totalVerticesLoaded = 0;
	unsigned int totalTrianglesLoaded = 0;
	for (std::vector<std::string>::iterator itModel = modelLocations.begin(); itModel != modelLocations.end(); itModel++)
	{
		sModelDrawInfo theModel;
		std::string modelName = *itModel;
		std::cout << "Loading " << modelName << "...";
		if (!::g_pVAOManager->LoadModelIntoVAO(modelName, theModel, program))
		{
			std::cout << "didn't work because: " << std::endl;
			std::cout << ::g_pVAOManager->getLastError(true) << std::endl;
		}
		else
		{
			std::cout << "OK." << std::endl;
			std::cout << "\t" << theModel.numberOfVertices << " vertices and " << theModel.numberOfTriangles << " triangles loaded." << std::endl;
			totalTrianglesLoaded += theModel.numberOfTriangles;
			totalVerticesLoaded += theModel.numberOfVertices;
		}
	} //end of for (std::vector<std::string>::iterator itModel

	std::cout << "Done loading models." << std::endl;
	std::cout << "Total vertices loaded = " << totalVerticesLoaded << std::endl;
	std::cout << "Total triangles loaded = " << totalTrianglesLoaded << std::endl;



	/*cMesh* pTheFloor1 = new cMesh();
	pTheFloor1->meshName = "SM_Env_Floor_01_xyz_n_rgba.ply";
	pTheFloor1->positionXYZ.y = 180.0f;
	pTheFloor1->bUseWholeObjectDiffuseColour = true;
	pTheFloor1->wholeObjectDiffuseRGBA = glm::vec4(1.0f, 0.f, 0.f, 1.0f);*/

	//::g_vec_pMeshes.push_back(pTheFloor1);

	/*cMesh* pTempShip = new cMesh();
	pTempShip->meshName = "mod-gas harvester.ply";
	pTempShip->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	pTempShip->bUseWholeObjectDiffuseColour = true;
	pTempShip->wholeObjectDiffuseRGBA = glm::vec4(0.0f, 1.0f, 0.2f, 1.0f);

	::g_vec_pMeshes.push_back(pTempShip);*/

	//Load the wall I'm going to use as a door here before the rest of the models so I know what indexes will be used for the door
	//Furthest Right, Furthest Left, and top most row will not move, the group of panels inside that will
	cMesh* blCorner = new cMesh();
	blCorner->meshName = "SM_Env_Construction_Wall_01_xyz_n_rgba_uv.ply";
	blCorner->positionXYZ = glm::vec3(-52.5f, 0.0f, 53.5f);
	blCorner->orientationXYZ = glm::vec3(0.f, glm::radians(90.0f), 0.0f);
	blCorner->scale = 1.0f;
	blCorner->bUseWholeObjectDiffuseColour = true;
	blCorner->wholeObjectDiffuseRGBA = glm::vec4(224.0f / 225.0f, 224.0f / 225.0f, 224.0f / 255.0f, 1.0f);
	blCorner->clearTextureRatiosToZero();
	blCorner->textureRatios[1] = 1.0f;
	blCorner->textureNames[1] = "SpaceInteriors_Texture.bmp";
	::g_vec_pMeshes.push_back(blCorner);			//0	

	cMesh* blCornerUp1 = new cMesh();
	blCornerUp1->meshName = "SM_Env_Construction_Wall_01_xyz_n_rgba_uv.ply";
	blCornerUp1->positionXYZ = glm::vec3(-52.5f, 5.0f, 53.5f);
	blCornerUp1->orientationXYZ = glm::vec3(0.f, glm::radians(90.0f), 0.0f);
	blCornerUp1->scale = 1.0f;
	blCornerUp1->bUseWholeObjectDiffuseColour = true;
	blCornerUp1->wholeObjectDiffuseRGBA = glm::vec4(224.0f / 225.0f, 224.0f / 225.0f, 224.0f / 255.0f, 1.0f);
	blCornerUp1->clearTextureRatiosToZero();
	blCornerUp1->textureRatios[1] = 1.0f;
	blCornerUp1->textureNames[1] = "SpaceInteriors_Texture.bmp";
	::g_vec_pMeshes.push_back(blCornerUp1);			//1	

	cMesh* blCornerUp2 = new cMesh();
	blCornerUp2->meshName = "SM_Env_Construction_Wall_01_xyz_n_rgba_uv.ply";
	blCornerUp2->positionXYZ = glm::vec3(-52.5f, 10.0f, 53.5f);
	blCornerUp2->orientationXYZ = glm::vec3(0.f, glm::radians(90.0f), 0.0f);
	blCornerUp2->scale = 1.0f;
	blCornerUp2->bUseWholeObjectDiffuseColour = true;
	blCornerUp2->wholeObjectDiffuseRGBA = glm::vec4(224.0f / 225.0f, 224.0f / 225.0f, 224.0f / 255.0f, 1.0f);
	blCornerUp2->clearTextureRatiosToZero();
	blCornerUp2->textureRatios[1] = 1.0f;
	blCornerUp2->textureNames[1] = "SpaceInteriors_Texture.bmp";
	::g_vec_pMeshes.push_back(blCornerUp2);			//2

	cMesh* blCornerUp3 = new cMesh();
	blCornerUp3->meshName = "SM_Env_Construction_Wall_01_xyz_n_rgba_uv.ply";
	blCornerUp3->positionXYZ = glm::vec3(-52.5f, 15.0f, 53.5f);
	blCornerUp3->orientationXYZ = glm::vec3(0.f, glm::radians(90.0f), 0.0f);
	blCornerUp3->scale = 1.0f;
	blCornerUp3->bUseWholeObjectDiffuseColour = true;
	blCornerUp3->wholeObjectDiffuseRGBA = glm::vec4(224.0f / 225.0f, 224.0f / 225.0f, 224.0f / 255.0f, 1.0f);
	blCornerUp3->clearTextureRatiosToZero();
	blCornerUp3->textureRatios[1] = 1.0f;
	blCornerUp3->textureNames[1] = "SpaceInteriors_Texture.bmp";
	::g_vec_pMeshes.push_back(blCornerUp3);			//3	

	cMesh* brCorner = new cMesh();
	brCorner->meshName = "SM_Env_Construction_Wall_01_xyz_n_rgba_uv.ply";
	brCorner->positionXYZ = glm::vec3(-52.5f, 0.0f, 43.5f);
	brCorner->orientationXYZ = glm::vec3(0.f, glm::radians(90.0f), 0.0f);
	brCorner->scale = 1.0f;
	brCorner->bUseWholeObjectDiffuseColour = true;
	brCorner->wholeObjectDiffuseRGBA = glm::vec4(224.0f / 225.0f, 224.0f / 225.0f, 224.0f / 255.0f, 1.0f);
	brCorner->clearTextureRatiosToZero();
	brCorner->textureRatios[1] = 1.0f;
	brCorner->textureNames[1] = "SpaceInteriors_Texture.bmp";
	::g_vec_pMeshes.push_back(brCorner);			//4

	cMesh* brCornerUp1 = new cMesh();
	brCornerUp1->meshName = "SM_Env_Construction_Wall_01_xyz_n_rgba_uv.ply";
	brCornerUp1->positionXYZ = glm::vec3(-52.5f, 5.0f, 43.5f);
	brCornerUp1->orientationXYZ = glm::vec3(0.f, glm::radians(90.0f), 0.0f);
	brCornerUp1->scale = 1.0f;
	brCornerUp1->bUseWholeObjectDiffuseColour = true;
	brCornerUp1->wholeObjectDiffuseRGBA = glm::vec4(224.0f / 225.0f, 224.0f / 225.0f, 224.0f / 255.0f, 1.0f);
	brCornerUp1->clearTextureRatiosToZero();
	brCornerUp1->textureRatios[1] = 1.0f;
	brCornerUp1->textureNames[1] = "SpaceInteriors_Texture.bmp";
	::g_vec_pMeshes.push_back(brCornerUp1);			//5	

	cMesh* brCornerUp2 = new cMesh();
	brCornerUp2->meshName = "SM_Env_Construction_Wall_01_xyz_n_rgba_uv.ply";
	brCornerUp2->positionXYZ = glm::vec3(-52.5f, 10.0f, 43.5f);
	brCornerUp2->orientationXYZ = glm::vec3(0.f, glm::radians(90.0f), 0.0f);
	brCornerUp2->scale = 1.0f;
	brCornerUp2->bUseWholeObjectDiffuseColour = true;
	brCornerUp2->wholeObjectDiffuseRGBA = glm::vec4(224.0f / 225.0f, 224.0f / 225.0f, 224.0f / 255.0f, 1.0f);
	brCornerUp2->clearTextureRatiosToZero();
	brCornerUp2->textureRatios[1] = 1.0f;
	brCornerUp2->textureNames[1] = "SpaceInteriors_Texture.bmp";
	::g_vec_pMeshes.push_back(brCornerUp2);			//6

	cMesh* brCornerUp3 = new cMesh();
	brCornerUp3->meshName = "SM_Env_Construction_Wall_01_xyz_n_rgba_uv.ply";
	brCornerUp3->positionXYZ = glm::vec3(-52.5f, 15.0f, 43.5f);
	brCornerUp3->orientationXYZ = glm::vec3(0.f, glm::radians(90.0f), 0.0f);
	brCornerUp3->scale = 1.0f;
	brCornerUp3->bUseWholeObjectDiffuseColour = true;
	brCornerUp3->wholeObjectDiffuseRGBA = glm::vec4(224.0f / 225.0f, 224.0f / 225.0f, 224.0f / 255.0f, 1.0f);
	brCornerUp3->clearTextureRatiosToZero();
	brCornerUp3->textureRatios[1] = 1.0f;
	brCornerUp3->textureNames[1] = "SpaceInteriors_Texture.bmp";
	::g_vec_pMeshes.push_back(brCornerUp3);			//7	

	//indexes 0-7 for opening and closing doors

	

	////World file stuff here
	if (loadWorldFile())
	{
		std::cout << "loadWorldFile finished OK" << std::endl;
	}
	else
	{
		std::cout << "loadWorldFile did not finish OK, aborting" << std::endl;
		return -1;
	}


	//Michael Feeney's BMP texture mapping
	//// Load the textures
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\textures\\";
	::g_pTextureManager->SetBasePath(ss.str());

	if (::g_pTextureManager->Create2DTextureFromBMPFile("SpaceInteriors_Texture.bmp", true))
		//if (::g_pTextureManager->Create2DTextureFromBMPFile("Pebbleswithquarzite.bmp", true))
	{
		std::cout << "Loaded the texture" << std::endl;
	}
	else
	{
		std::cout << "DIDN'T load the texture" << std::endl;
	}

	::g_pTextureManager->Create2DTextureFromBMPFile("SpaceInteriors_Emmision.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("WorldMap.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("2k_jupiter.bmp", true);

	::g_pTextureManager->Create2DTextureFromBMPFile("Lisse_mobile_shipyard-mal1.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("Lisse_mobile_shipyard-mal1_Window_Mask.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("Ship_Pack_WIP-map-all.bmp", true);

	::g_pTextureManager->Create2DTextureFromBMPFile("Sand_Texture-1192.bmp", true);
	//::g_pTextureManager->Create2DTextureFromBMPFile("Banksy_Panda_Stencil.bmp", true);
	::g_pTextureManager->Create2DTextureFromBMPFile("Australia_height_map.bmp", true);

	// Default don't have a texture
	if (::g_pTextureManager->Create2DTextureFromBMPFile("BrightColouredUVMap.bmp", true))
		//if (::g_pTextureManager->Create2DTextureFromBMPFile("Pebbleswithquarzite.bmp", true))
	{
		std::cout << "Loaded the texture" << std::endl;
	}
	else
	{
		std::cout << "DIDN'T load the texture" << std::endl;
	}

	// Add a skybox texture
	std::string errorTextString;
	ss.str("");
	ss << SOLUTION_DIR << "common\\assets\\textures\\cubemaps\\";
	::g_pTextureManager->SetBasePath(ss.str());		// update base path to cube texture location

	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles("TropicalSunnyDay",
		"TropicalSunnyDayRight2048.bmp",	/* +X */	"TropicalSunnyDayLeft2048.bmp" /* -X */,
		"TropicalSunnyDayUp2048.bmp",		/* +Y */	"TropicalSunnyDayDown2048.bmp" /* -Y */,
		"TropicalSunnyDayFront2048.bmp",	/* +Z */	"TropicalSunnyDayBack2048.bmp" /* -Z */,
		true, errorTextString))
	{
		std::cout << "Didn't load because: " << errorTextString << std::endl;
	}

	
	if (!::g_pTextureManager->CreateCubeTextureFromBMPFiles("Space01",
		"SpaceBox_right1_posX.bmp",		/* +X */	"SpaceBox_left2_negX.bmp" /* -X */,
		"SpaceBox_top3_posY.bmp",		/* +Y */	"SpaceBox_bottom4_negY.bmp" /* -Y */,
		"SpaceBox_front5_posZ.bmp",		/* +Z */	"SpaceBox_back6_negZ.bmp" /* -Z */,
		true, errorTextString))
	{
		std::cout << "Didn't load because: " << errorTextString << std::endl;
	}

	// Make sphere for "skybox" before world file
	cMesh* sphereSky = new cMesh();
	//sphereSky->meshName = "Isosphere_Smooth_Normals.ply";
	// We are using a sphere with INWARD facing normals, so we see the "back" of the sphere
	sphereSky->meshName = "Isosphere_Smooth_Inverted_Normals_for_SkyBox.ply";
	// 2 main ways we can do a skybox:
	//
	// - Make a sphere really big, so everything fits inside
	// (be careful of the far clipping plane)
	//
	// - Typical way is:
	//	- Turn off the depth test
	//	- Turn off the depth function (i.e. writing to the depth buffer)
	//	- Draw the skybox object (which can be really small, since it's not interacting with the depth buffer)
	//	- Once drawn:
	//		- Turn on the depth function
	//		- Turn on the depth test

	sphereSky->positionXYZ = ::cameraEye;	//glm::vec3(100.0f, -250.0f, 100.0f);
	//sphereSky->orientationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	sphereSky->scale = 10000.0f;
	//sphereSky->bUseWholeObjectDiffuseColour = false;
	//sphereSky->wholeObjectDiffuseRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	sphereSky->bDontLight = true;
	sphereSky->clearTextureRatiosToZero();
	sphereSky->textureNames[1] = "2k_jupiter.bmp";
	sphereSky->textureRatios[1] = 1.0f;
	//Draw it separately so don't add it to the vector
	//::g_vec_pMeshes.push_back(sphereSky);


	// Added some ships for the discard transparency learning
	cMesh* pShipyard = new cMesh();
	pShipyard->meshName = "Lisse_mobile_shipyard_LARGER_xyz_n_rgba_uv.ply";
	pShipyard->positionXYZ = glm::vec3(0.0f, 0.0f, -15.0f);
	pShipyard->scale = 0.01f;
	pShipyard->bDontLight = true;
	pShipyard->clearTextureRatiosToZero();
	pShipyard->textureNames[1] = "Lisse_mobile_shipyard-mal1.bmp";
	pShipyard->textureRatios[1] = 1.0f;
	//::g_vec_pMeshes.push_back(pShipyard);

	cMesh* pWall = new cMesh();
	pWall->meshName = "FlatQuad_xyz_n_rgba_uv.ply";
	pWall->positionXYZ = glm::vec3(0.0f, 10.0f, -10.0f);
	pWall->scale = 5.0f;
	pWall->bDontLight = true;
	pWall->clearTextureRatiosToZero();
	pWall->textureNames[1] = "WorldMap.bmp";
	pWall->textureRatios[1] = 1.0f;
	pWall->textureNames[2] = "SpaceInteriors_Texture.bmp";
	pWall->textureRatios[2] = 1.0f;


	cMesh* pTheGround = new cMesh();
	//    pTheGround->meshName = "SillyTerrain_REALLY_BIG_xyz_n_rgba_uv.ply";
	//pTheGround->friendlyName = "Terrain";

	pTheGround->meshName = "FlatTerrain_xyz_n_rgba_uv.ply";
	pTheGround->friendlyName = "FlatTerrain";

	pTheGround->positionXYZ.y = -1000.0f;
	pTheGround->positionXYZ.z = 6000.0f;
	pTheGround->scale = 0.5f;
	pTheGround->textureNames[0] = "Sand_Texture-1192.bmp";
	//    pTheGround->textureNames[0] = "Australia_height_map.bmp";
	//    pTheGround->textureNames[0] = "Banksy_Panda_Stencil.bmp";
	pTheGround->textureRatios[0] = 1.0f;
	// Make it shiny?
	pTheGround->bUseWholeObjectDiffuseColour = true;
	pTheGround->wholeObjectDiffuseRGBA = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	//pTheGround->bIsWireframe = true;
	//pTheGround->bDontLight = true;
	::g_vec_pMeshes.push_back(pTheGround);



	//Load each texture file individually here
	//ss.str("");
	//ss << SOLUTION_DIR << "common\\assets\\models";
	//::g_pTextureManager->SetBasePath(ss.str());

	//if (::g_pTextureManager->Create2DTextureFromBMPFile("SpaceInteriors_Texture.bmp", true))
	//	//if (::g_pTextureManager->Create2DTextureFromBMPFile("Pebbleswithquarzite.bmp", true))
	//{
	//	std::cout << "Loaded the texture" << std::endl;
	//}
	//else
	//{
	//	std::cout << "DIDN'T load the texture" << std::endl;
	//}
	//etc...


	//Then set all these values in the SetupTextures function (TODO)	which got moved into DrawObject_function ...
	// FOR NOW, set ALL the texture to this texture
	//GLuint SapceInteriors_TextureNumber = ::g_pTextureManager->getTextureIDFromName("SpaceInteriors_Texture.bmp");
	////GLuint Fauci_TextureNumber = ::g_pTextureManager->getTextureIDFromName("Pebbleswithquarzite.bmp");

	//// Texture binding...
	//GLuint textureUnit = 14;			// Texture unit go from 0 to 79
	//glActiveTexture(textureUnit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984
	//glBindTexture(GL_TEXTURE_2D, SapceInteriors_TextureNumber);
	//// glBindTextureUnit( texture00Unit, texture00Number );	// OpenGL 4.5+ only
	//// Set texture unit in the shader, too

	//GLint texture_00_LocID = glGetUniformLocation(program, "texture_00");

	//glUniform1i(texture_00_LocID, textureUnit);


	if (::cameraEye.x > 0 && ::cameraEye.z > 0)
	{
		::cameraYaw = 180.f + (atan(::cameraEye.z / ::cameraEye.x) * 180.f / glm::pi<float>());
	}
	else if (::cameraEye.x > 0 && ::cameraEye.z < 0)
	{
		::cameraYaw = 90.f - (atan(::cameraEye.z / ::cameraEye.x) * 180.f / glm::pi<float>());
	}
	else if (::cameraEye.x < 0 && ::cameraEye.z > 0)
	{
		::cameraYaw = (atan(::cameraEye.z / ::cameraEye.x) * 180.f / glm::pi<float>());
	}
	else if (::cameraEye.x < 0 && ::cameraEye.z < 0)
	{
		::cameraYaw = (atan(::cameraEye.z / ::cameraEye.x) * 180.f / glm::pi<float>());
	}
	else if (::cameraEye.x == 0.f)
	{
		if (::cameraEye.z >= 0.f)
		{
			::cameraYaw = 270.f;
		}
		else
		{
			::cameraYaw = 90.f;
		}
	}
	else if (::cameraEye.z == 0.f)
	{
		if (::cameraEye.x <= 0)
		{
			::cameraYaw = 0.f;
		}
		else
		{
			::cameraYaw = 180.f;
		}
	}
	//anyways, after figuring out the yaw, we set the target at the negative of the xz of the ::camera position and y=0 (this faces the ::camera towards the origin)
	::cameraTarget = glm::vec3(-1.f * ::cameraEye.x, 0, -1.f * ::cameraEye.z);
	glm::normalize(::cameraTarget);



	const double MAX_DELTA_TIME = 0.1;	//100 ms
	double previousTime = glfwGetTime();

	while (!glfwWindowShouldClose(pWindow))
	{
		float ratio;
		int width, height;
		glm::mat4 matModel;				// used to be "m"; Sometimes it's called "world"
		glm::mat4 matProjection;        // used to be "p";
		glm::mat4 matView;              // used to be "v";

		double currentTime = glfwGetTime();
		deltaTime = currentTime - previousTime;
		deltaTime = (deltaTime > MAX_DELTA_TIME ? MAX_DELTA_TIME : deltaTime);
		previousTime = currentTime;

		glfwGetFramebufferSize(pWindow, &width, &height);
		ratio = width / (float)height;

		//Turn on the depth buffer
		glEnable(GL_DEPTH);         // Turns on the depth buffer
		glEnable(GL_DEPTH_TEST);    // Check if the pixel is already closer

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// *******************************************************
		// Screen is cleared and we are ready to draw the scene...
		// *******************************************************

		// Update the title text
		glfwSetWindowTitle(pWindow, ::g_TitleText.c_str());


		// Move the skybox "object" to match the camera
		//sphereSky->positionXYZ = ::cameraEye;

		// Copy the light information into the shader to draw the scene
		::g_pTheLights->CopyLightInfoToShader();

		/*matProjection = glm::perspective(0.6f,
			ratio,
			0.1f,
			1000.0f);*/

		matProjection = glm::perspective(0.6f,	// FOV variable later
			ratio,
			0.1f,								// Near plane
			1'000'000.0f);						// Far plane



		//matProjection = glm::perspective(
		//	::g_pFlyCamera->FOV,
		//	ratio,
		//	::g_pFlyCamera->nearPlane,      // Near plane (as large as possible)
		//	::g_pFlyCamera->farPlane);      // Far plane (as small as possible)


		//::g_pFlyCamera->Update(deltaTime);

		//glm::vec3 cameraEye = ::g_pFlyCamera->getEye();
		//glm::vec3 cameraAt = ::g_pFlyCamera->getAtInWorldSpace();
		//glm::vec3 cameraUp = ::g_pFlyCamera->getUpVector();


		matView = glm::mat4(1.0f);
		//matView = glm::lookAt(	cameraEye,   // "eye"
		//						cameraAt,    // "at"
		//						cameraUp);

		matView = glm::lookAt(::cameraEye,
			::cameraEye + ::cameraTarget,
			upVector);

	//	glUniformMatrix4fv(matView_Location, 1, GL_FALSE, glm::value_ptr(matView));
		
		glUniformMatrix4fv(pShaderProc->getUniformID_From_Name("matView"),
			1, GL_FALSE, glm::value_ptr(matView));


		glUniformMatrix4fv(matProjection_Location, 1, GL_FALSE, glm::value_ptr(matProjection));


		//Update the doors here
		if (::openDoors)
		{
			openBayDoors();
		}
		if (::closeDoors)
		{
			closeBayDoors();
		}


		// Since this is a space game (most of the screen is filled with "sky"),
		// I'll draw the skybox first
		
		GLint bIsSkyBox_LocID = glGetUniformLocation(program, "bIsSkyBox");
		glUniform1f(bIsSkyBox_LocID, (GLfloat)GL_TRUE);

		// Move the "skybox object" with the camera
		sphereSky->positionXYZ = ::cameraEye;
		DrawObject(sphereSky, glm::mat4(1.0f),
			matModel_Location, matModelInverseTranspose_Location,
			program, ::g_pVAOManager);

		glUniform1f(bIsSkyBox_LocID, (GLfloat)GL_FALSE);


		//For transparency for the beakers, they need to be drawn farthest from the camera first, so we need to sort the last 4 meshes
		for (unsigned int index = ::g_vec_pMeshes.size() - 4; index != g_vec_pMeshes.size()-1; index++)
		{
			//Distance is the difference in length between camera eye and mesh position
			float dist1, dist2;
			dist1 = glm::abs(glm::length(cameraEye - ::g_vec_pMeshes[index]->positionXYZ));
			dist2 = glm::abs(glm::length(cameraEye - ::g_vec_pMeshes[index + 1]->positionXYZ));
			//if the second object is farther, move it up
			if (dist2 > dist1)
			{
				cMesh* temp = ::g_vec_pMeshes[index + 1];
				::g_vec_pMeshes[index + 1] = ::g_vec_pMeshes[index];
				::g_vec_pMeshes[index] = temp;
			}
			
		}

		// Set up the discard texture, etc.
		// HACK: Set the transparency for just the mobile shipyard, nothing else will be affected by this

		GLuint discardTextureNumber = ::g_pTextureManager->getTextureIDFromName("Lisse_mobile_shipyard-mal1.bmp");
		// I'm picking texture unit 30 since it's not in use.
		GLuint textureUnit = 30;
		glActiveTexture(textureUnit + GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, discardTextureNumber);
		GLint discardTexture_LocID = glGetUniformLocation(program, "discardTexture");
		glUniform1i(discardTexture_LocID, textureUnit);

		GLint bDiscardTransparencyWindowsON_LocID = glGetUniformLocation(program, "bDiscardTransparencyWindowsOn");
		// Turn it on
		glUniform1f(bDiscardTransparencyWindowsON_LocID, (GLfloat)GL_TRUE);

		matModel = glm::mat4(1.0f);
		DrawObject(pShipyard,
			matModel,
			matModel_Location,
			matModelInverseTranspose_Location,
			program,
			::g_pVAOManager);

		DrawObject(pWall,
			matModel,
			matModel_Location,
			matModelInverseTranspose_Location,
			program,
			::g_pVAOManager);

		// Turn it off
		glUniform1f(bDiscardTransparencyWindowsON_LocID, (GLfloat)GL_FALSE);

		// **********************************************************************
		// Draw the "scene" of all objects.
		// i.e. go through the vector and draw each one...
		// **********************************************************************
		for (unsigned int index = 0; index != ::g_vec_pMeshes.size(); index++)
		{
			// So the code is a little easier...
			cMesh* pCurrentMesh = ::g_vec_pMeshes[index];

			matModel = glm::mat4(1.0f);  // "Identity" ("do nothing", like x1)
			//mat4x4_identity(m);

			// ***************************************************
// HACK: Set up a height map from a texture (used in the VERTEX shader)
			{
				GLint bUseHeightMap_LodID = glGetUniformLocation(program, "bUseHeightMap");
				glUniform1f(bUseHeightMap_LodID, (GLfloat)GL_FALSE);

				if (pCurrentMesh->friendlyName == "FlatTerrain")
				{

					// I'm picking texture unit 31 since it's not in use.
					GLuint heightMapTextureUnit = 31;			// Texture unit go from 0 to 79
					glActiveTexture(heightMapTextureUnit + GL_TEXTURE0);	// GL_TEXTURE0 = 33984

					GLuint heightMapTextureID = ::g_pTextureManager->getTextureIDFromName("Australia_height_map.bmp");
					glBindTexture(GL_TEXTURE_2D, heightMapTextureID);

					GLint heightMapTexture_LocID = glGetUniformLocation(program, "heightMapTexture");
					glUniform1i(heightMapTexture_LocID, heightMapTextureUnit);


					GLint heightMapScale_LocID = glGetUniformLocation(program, "heightMapScale");
					glUniform1f(heightMapScale_LocID, 1000.0f);

					GLint heightMapUVOffsetRotation_LocID = glGetUniformLocation(program, "heightMapUVOffsetRotation");
					// Move to a global variable so we can move them with the keyboard
					//glm::vec2 heightMapUVOffset = glm::vec2(0.0f, 0.0f);
					glUniform3f(heightMapUVOffsetRotation_LocID,
						::g_heightMapUVOffsetRotation.x, ::g_heightMapUVOffsetRotation.y, ::g_heightMapUVOffsetRotation.z);

					// Turn height map function on (will be turned off at next object - line 778)
					glUniform1f(bUseHeightMap_LodID, (GLfloat)GL_TRUE);


					//                    std::cout << "heightMapTextureID = " << heightMapTextureID << std::endl;
					//                    std::cout << "heightMapTextureUnit = " << heightMapTextureUnit << std::endl;
					//                    std::cout << "heightMapTextureUnit + GL_TEXTURE0 = " << heightMapTextureUnit + GL_TEXTURE0 << std::endl;
					//
					//                    // returns a single value indicating the active multitexture unit. The initial value is GL_TEXTURE0.
					//                    GLint glActiveTexture = 0;
					//                    glGetIntegerv(GL_ACTIVE_TEXTURE, &glActiveTexture);
					//                    std::cout << "GL_ACTIVE_TEXTURE = " << glActiveTexture << std::endl;
					//                    std::cout << "GL_ACTIVE_TEXTURE - GL_TEXTURE0 = " << glActiveTexture - GL_TEXTURE0 << std::endl;
					//
					//                    // returns a single value, the name of the texture currently bound to the target GL_TEXTURE_2D. The initial value is 0.
					//                    GLint glTextureBindings = 0;
					//                    glGetIntegerv(GL_TEXTURE_BINDING_2D, &glTextureBindings);
					//                    std::cout << "GL_TEXTURE_BINDING_2D = " << glTextureBindings << std::endl;

				}//if (pCurrentMesh->friendlyName == "FlatTerrain")
			}
			// ***************************************************

			DrawObject(pCurrentMesh,
				matModel,
				matModel_Location,
				matModelInverseTranspose_Location,
				program,
				::g_pVAOManager);


		}//for (unsigned int index

		
		// Scene is drawn
		// **********************************************************************

		// "Present" what we've drawn.
		glfwSwapBuffers(pWindow);        // Show what we've drawn

		// Process any events that have happened
		glfwPollEvents();

		// Handle OUR keyboard, mouse stuff
		handleAsyncKeyboard(pWindow, deltaTime);
		handleAsyncMouse(pWindow, deltaTime);

	}//while (!glfwWindowShouldClose(window))

	// All done, so delete things...
	::g_ShutDown(pWindow);


	glfwDestroyWindow(pWindow);

	glfwTerminate();
	exit(EXIT_SUCCESS);
} //end of main


bool loadModelsFromFile(GLuint& program)
{
	std::stringstream ss;
	
	ss << SOLUTION_DIR << "common\\assets\\models\\modelsToLoad.txt";

	std::ifstream theFile(ss.str());

	if (!theFile.is_open())
	{
		fprintf(stderr, "Could not open modelsToLoad.txt");
		return false;
	}

	std::string nextToken;
	ss.str("");

	while (theFile >> nextToken)
	{
		ss << SOLUTION_DIR << "common\\assets\\models\\" << nextToken.c_str();
		modelLocations.push_back(ss.str().c_str());
		ss.str("");
	}
	theFile.close();
	return true;
} //end of loadModelsFromFile


bool loadWorldFile()
{
	std::stringstream ss;
	std::stringstream sFile;
	

	ss << SOLUTION_DIR << "common\\assets\\worldFile.txt";

	std::ifstream theFile(ss.str());

	if (!theFile.is_open())
	{
		fprintf(stderr, "Could not open modelsToLoad.txr");
		return false;
	}

	std::string nextToken;
	ss.str("");

	//Throwaway text describing the format of the file
	theFile >> nextToken;       //ModelFileName(extension)
	theFile >> nextToken;       //Position(x,y,z)
	theFile >> nextToken;       //Orientation(x,y,z)
	theFile >> nextToken;       //Scale
	theFile >> nextToken;       //Colour(r,g,b)

	//From here modify based on worldFile format
	while (theFile >> nextToken)    //this should always be the name of the model to load or end.  Potential error check, add a check for "ply" in the mdoel name
	{
		cMesh* curMesh = new cMesh;
		if (nextToken == "end")
		{
			break;
		}
		std::cout << nextToken << std::endl;        //Printing model names to console, just making sure we're loading ok.  Can be commented out whenever
		//First is the file name of model
		//ss << SOLUTION_DIR << "common\\assets\\models\\" << nextToken;
		curMesh->meshName = nextToken;
		//Next 3 are the position of the model
		theFile >> nextToken;                                               //x position for the model
		curMesh->positionXYZ.x = std::stof(nextToken);
		theFile >> nextToken;                                               //y position for the model
		curMesh->positionXYZ.y = std::stof(nextToken);
		theFile >> nextToken;                                               //z position for the model
		curMesh->positionXYZ.z = std::stof(nextToken);
		//Next 3 are the orientation of the model
		theFile >> nextToken;                                               //x orientation value
		//curMesh.orientationXYZ.x = std::stof(nextToken);
		curMesh->orientationXYZ.x = glm::radians(std::stof(nextToken));
		theFile >> nextToken;                                               //y orientation value
		//curMesh.orientationXYZ.y = std::stof(nextToken);
		curMesh->orientationXYZ.y = glm::radians(std::stof(nextToken));
		theFile >> nextToken;                                               //z orientation value
		//curMesh.orientationXYZ.z = std::stof(nextToken);
		curMesh->orientationXYZ.z = glm::radians(std::stof(nextToken));
		//Next is the scale to multiply the model by
		theFile >> nextToken;                                               //scale multiplier
		curMesh->scale = std::stof(nextToken);
		//Next 3 are the r, g, b values for the model
		curMesh->bUseWholeObjectDiffuseColour = false;
		theFile >> nextToken;													//RGB red value
		curMesh->wholeObjectDiffuseRGBA.r = std::stof(nextToken) / 255.0f;		//convert to nice shader value (between 0 and 1)
		theFile >> nextToken;													//RGB green value
		curMesh->wholeObjectDiffuseRGBA.g = std::stof(nextToken) / 255.0f;		//convert to nice shader value (between 0 and 1)
		theFile >> nextToken;													//RGB blue value
		curMesh->wholeObjectDiffuseRGBA.b = std::stof(nextToken) / 255.0f;		//convert to nice shader value (between 0 and 1)
		theFile >> nextToken;
		curMesh->wholeObjectDiffuseRGBA.a = std::stof(nextToken);
		curMesh->alphaTransparency = std::stof(nextToken);

		curMesh->bDontLight = false;		// true to test texture stuff cause the lighting is still gawd aweful lmao

		curMesh->clearTextureRatiosToZero();
		curMesh->textureRatios[1] = 1.0f;
		curMesh->textureNames[1] = "SpaceInteriors_Texture.bmp";

		::g_vec_pMeshes.push_back(curMesh);     //push the model onto our vector of meshes
		ss.str("");                         //reset the stringstream
	} //end of while
	theFile.close();
	return true;
}	//end of load world file

//Figured out the math for how to do this from https://learnopengl.com/Getting-started/Camera and http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
//Using the mouse position we calculate the direction that the camera will be facing
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	//if it's the start of the program this smooths out a potentially glitchy jump
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	//find the offset of where the mouse positions have moved
	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;
	//multiply by sensitivity so that it's not potentially crazy fast
	float sensitivity = 0.1f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	::cameraYaw += xOffset;         // The yaw is the rotation around the ::camera's y-axis (so we want to add the xOffset to it)
	::cameraPitch += yOffset;       // The pitch is the rotation around the ::camera's x-axis (so we want to add the yOffset to it)
	//This limits the pitch so that we can't just spin the ::camera under/over itself
	if (::cameraPitch > 89.0f)
		::cameraPitch = 89.0f;
	if (::cameraPitch < -89.0f)
		::cameraPitch = -89.0f;
	//calculations for the new direction based on the mouse movements
	glm::vec3 direction;
	direction.x = cos(glm::radians(::cameraYaw)) * cos(glm::radians(::cameraPitch));
	direction.y = sin(glm::radians(::cameraPitch));
	direction.z = sin(glm::radians(::cameraYaw)) * cos(glm::radians(::cameraPitch));
	::cameraTarget = glm::normalize(direction);
} //fly ::camera

void openBayDoors()
{
	unsigned int index;

	for (index = 0; index < 4; index++)
	{
		g_vec_pMeshes[index]->positionXYZ.z -= 1.25 *deltaTime;
	}

	if (g_vec_pMeshes[0]->positionXYZ.z <= 44.5f)
	{
		for (index = 4; index < 8; index++)
		{
			g_vec_pMeshes[index]->positionXYZ.z -= 1.25 * deltaTime;
		}
	}

	doorLightDirCurX += doorLightRotateX * deltaTime;
	doorLightDirCurZ += doorLightRotateZ * deltaTime;
	
	if (doorLightDirCurX >= 1.0f)
	{
		doorLightDirCurX = 1.0f;
		doorLightRotateX *= -1.0f;
	}
	else if (doorLightDirCurX <= -1.0f)
	{
		doorLightDirCurX = -1.0f;
		doorLightRotateX *= -1.0f;
	}

	if (doorLightDirCurZ >= 1.0f)
	{
		doorLightDirCurZ = 1.0f;
		doorLightRotateZ *= -1.0f;
	}
	else if (doorLightDirCurZ <= -1.0f)
	{
		doorLightDirCurZ = -1.0f;
		doorLightRotateZ *= -1.0f;
	}
	
	glm::vec3 dir(doorLightDirCurX, -0.1f, doorLightDirCurZ);
	dir = glm::normalize(dir);
	::g_pTheLights->theLights[0].direction = glm::vec4(dir, 1.0f);

	if (g_vec_pMeshes[0]->positionXYZ.z <= 35.5f || g_vec_pMeshes[4]->positionXYZ.z <= 34.5f)
	{
		::g_pTheLights->TurnOffLight(0);
		::openDoors = false;
	}
}

void closeBayDoors()
{
	unsigned int index;

	for (index = 0; index < 4; index++)
	{
		g_vec_pMeshes[index]->positionXYZ.z += 1.25 * deltaTime;
	}

	if (g_vec_pMeshes[4]->positionXYZ.z < 43.5)
	{
		for (index = 4; index < 8; index++)
		{
			g_vec_pMeshes[index]->positionXYZ.z += 1.25 * deltaTime;
		}
	}

	doorLightDirCurX -= doorLightRotateX * deltaTime;
	doorLightDirCurZ -= doorLightRotateZ * deltaTime;

	if (doorLightDirCurX >= 1.0f)
	{
		doorLightDirCurX = 1.0f;
		doorLightRotateX *= -1.0f;
	}
	else if (doorLightDirCurX <= -1.0f)
	{
		doorLightDirCurX = -1.0f;
		doorLightRotateX *= -1.0f;
	}

	if (doorLightDirCurZ >= 1.0f)
	{
		doorLightDirCurZ = 1.0f;
		doorLightRotateZ *= -1.0f;
	}
	else if (doorLightDirCurZ <= -1.0f)
	{
		doorLightDirCurZ = -1.0f;
		doorLightRotateZ *= -1.0f;
	}

	glm::vec3 dir(doorLightDirCurX, -0.1f, doorLightDirCurZ);
	dir = glm::normalize(dir);
	::g_pTheLights->theLights[0].direction = glm::vec4(dir, 1.0f);

	if (g_vec_pMeshes[0]->positionXYZ.z >= 53.5)
	{
		::g_pTheLights->TurnOffLight(0);
		::closeDoors = false;
	}
}
