#include "globalStuff.h"
// #include <glad/glad.h>		// in globalStuff.h
// #include <GLFW/glfw3.h>		// in globalStuff.h

//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "cShaderManager.h"

struct sMyVertex
{
    float x, y;				// vPos
    float r, g, b;			// vCol
};

sMyVertex vertices[6] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {  0.0f,  0.6f, 0.f, 0.f, 1.f },

    { -2.6f, -0.4f, 0.f, 1.f, 0.f },
    { -1.4f, -0.4f, 0.f, 1.f, 0.f },
    { -2.0f,  0.6f, 0.f, 1.f, 0.f }
};

// Camera 
glm::vec3 g_cameraEye = glm::vec3( 0.0, 0.0, +10.0f );

// Step 2: Make a "local" variable that matches the shader variable
// Triangle colour
// 	"uniform vec3 triColour;\n"	
glm::vec3 g_triangleColour = glm::vec3( 1.0f, 0.0f, 0.0f );


//cShaderManager myShaderManager;			// Stack
cShaderManager* g_pMyShaderManager = NULL;	//  = new cShaderManager();	// Heap


//static const char* vertex_shader_text =
//"#version 110\n"
//"uniform mat4 MVP;\n"
//"attribute vec3 vCol;\n"	
//"attribute vec2 vPos;\n"
//"varying vec3 color;\n"			
//"void main()\n"
//"{\n"
//"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
//"    color = vCol;\n"
//"}\n";
//
//static const char* fragment_shader_text =
//"#version 110\n"
//"varying vec3 color;\n"		
//"uniform vec3 triColour;\n"			
//"void main()\n"
//"{\n"
//"    gl_FragColor = vec4(triColour, 1.0);\n"  
//"}\n";


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

	// Use the keys to move the "Camera" around
	const float CAMERASPEED = 0.1f;

	// A + D are "left and right"
	if ( key == GLFW_KEY_A ) { ::g_cameraEye.x -= CAMERASPEED; }	// Left
	if ( key == GLFW_KEY_D ) { ::g_cameraEye.x += CAMERASPEED; }	// Left

	// W + S are "forward and backward"
	if ( key == GLFW_KEY_W ) { ::g_cameraEye.z += CAMERASPEED; }	// "forward"?
	if ( key == GLFW_KEY_S ) { ::g_cameraEye.z -= CAMERASPEED; }	// "backward"?

	// Q + E are "up and down"
	if ( key == GLFW_KEY_Q ) { ::g_cameraEye.y -= CAMERASPEED; }	// "down"?
	if ( key == GLFW_KEY_E ) { ::g_cameraEye.y += CAMERASPEED; }	// "up"?


	// Step 3: Change the LOCAL triangle colour
	if ( key == GLFW_KEY_1 ) { ::g_triangleColour.r += 0.01f; }
	if ( key == GLFW_KEY_2 ) { ::g_triangleColour.r -= 0.01f; }


	if ( key == GLFW_KEY_3 ) { ::g_triangleColour.g += 0.01f; }
	if ( key == GLFW_KEY_4 ) { ::g_triangleColour.g -= 0.01f; }

	if ( key == GLFW_KEY_5 ) { ::g_triangleColour.b += 0.01f; }
	if ( key == GLFW_KEY_6 ) { ::g_triangleColour.b -= 0.01f; }


	std::cout << "Tricolour ("
		<< ::g_triangleColour.r << ", "
		<< ::g_triangleColour.g << ", " 
		<< ::g_triangleColour.b << ")" << std::endl;
}

int main(void)
{
    GLFWwindow* window;
	GLuint vertex_buffer;
//	GLuint vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
	{
        exit(EXIT_FAILURE);
	}

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader( (GLADloadproc)glfwGetProcAddress );
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity
	//sMyVertex vertices[6]
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create a shader manager
	::g_pMyShaderManager = new cShaderManager();

	cShaderManager::cShader vertShad;
	vertShad.fileName = "vertexShader.glsl";

	cShaderManager::cShader fragShad; 
	fragShad.fileName = "fragmentShader.glsl";

	if (!::g_pMyShaderManager->createProgramFromFile(
					"basicshader",
					vertShad, 
					fragShad))
	{
		std::cout << "ERROR: didn't compile" << std::endl;
		std::cout << ::g_pMyShaderManager->getLastError() << std::endl;
		return -1;
	}
	else
	{
		std::cout << "Shaders compiled OK" << std::endl;
	}


//    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
//   glCompileShader(vertex_shader);
//
//    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
//   glCompileShader(fragment_shader);
//
//    program = glCreateProgram();
//
//    glAttachShader(program, vertex_shader);
//    glAttachShader(program, fragment_shader);
//    glLinkProgram(program);

	GLint shadProgID = ::g_pMyShaderManager->getIDFromFriendlyName("basicshader");


	//"uniform mat4 MVP;\n"
    mvp_location = glGetUniformLocation(shadProgID, "MVP");
    vpos_location = glGetAttribLocation(shadProgID, "vPos");
    vcol_location = glGetAttribLocation(shadProgID, "vCol");

	// Step 1: get the "location of the uniform variable"
	//"uniform vec3 triColour;\n"	
	GLint triColour_Loc = glGetUniformLocation(shadProgID, "triColour");


    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(vertices[0]), (void*) (sizeof(float) * 2));

	while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;

 //       mat4x4 m, p, mvp;
 //       glm::mat4 m, p, v, mvp;
		glm::mat4 matModel;		// "m"
		glm::mat4 matView;		// "v"
		glm::mat4 matProj;		// "p"


		// Separate the 3 matrix values


        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);

		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );	// White
        glClear(GL_COLOR_BUFFER_BIT);

        //         mat4x4_identity(m);
		//m = glm::mat4(1.0f);	
		matModel = glm::mat4(1.0f);

		//mat4x4_rotate_Z(m, m, (float) glfwGetTime());
		glm::mat4 rotateZ = glm::rotate( glm::mat4(1.0f), 
									     0.0f,	// (float) glfwGetTime(), 
									     glm::vec3( 0.0f, 0.0, 1.0f ) );

//		m = m * rotateZ;
		matModel = matModel * rotateZ;

        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		//p = glm::perspective( 0.6f, 
		//					  ratio, 
		//					  0.1f, 
		//					  1000.0f );
		matProj = glm::perspective( 0.6f, 
							        ratio, 
							        0.1f, 
							        1000.0f );

		//v = glm::mat4(1.0f);
		matView = glm::mat4(1.0f);

//  		glm::vec3 cameraEye = glm::vec3( 0.0, 0.0, -4.0f ); 
		glm::vec3 cameraTarget = glm::vec3( 0.0f, 0.0f, 0.0f ); 
		glm::vec3 upVector = glm::vec3( 0.0f, 1.0f, 0.0f );

		//v = glm::lookAt( ::g_cameraEye,			// You don't really need the "::", it's just cool and sexy
		//				 cameraTarget,
		//			 	 upVector );
		matView = glm::lookAt( ::g_cameraEye,			// You don't really need the "::", it's just cool and sexy
						 cameraTarget,
					 	 upVector );
     		
		//mat4x4_mul(mvp, p, m);
//		mvp = p * v * m; 




        glUseProgram(shadProgID);
		//::g_pMyShaderManager->useShaderProgram("basicshader");


		//uniform float newRed;
		//uniform float newGreen;
		//uniform float newBlue;
		GLint newRed_loc = glGetUniformLocation(shadProgID, "newRed");
		GLint newGreen_loc = glGetUniformLocation(shadProgID, "newGreen");
		GLint newBlue_loc = glGetUniformLocation(shadProgID, "newBlue");

		// Set them in the shader by calling glUniformX();
		glUniform1f(newRed_loc, 0.5f);
		glUniform1f(newGreen_loc, 0.25f);
		glUniform1f(newBlue_loc, 0.75f);



        //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
		//glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

		// Am getting the unifrom locations here, 
		// which is slow, but I'm doing it for clarity
		//uniform mat4 matModel;		// "m"
		//uniform mat4 matView;		// "v"
		//uniform mat4 matProj;		// "p"
		GLint matModel_loc = glad_glGetUniformLocation(shadProgID, "matModel");
		GLint matView_loc = glad_glGetUniformLocation(shadProgID, "matView");
		GLint matProj_loc = glad_glGetUniformLocation(shadProgID, "matProj");

		glUniformMatrix4fv(matModel_loc, 1, GL_FALSE, glm::value_ptr(matModel));
		glUniformMatrix4fv(matView_loc, 1, GL_FALSE, glm::value_ptr(matView));
		glUniformMatrix4fv(matProj_loc, 1, GL_FALSE, glm::value_ptr(matProj));

		// ::g_triangleColour		(local variable)
		// "uniform vec3 triColour;\n"	  (shader variable)
		// triColour_Loc      (location of variable in shader)
		// This will copy the "local" variable into the shader
		glUniform3f( triColour_Loc, 
					 ::g_triangleColour.r, 
					 ::g_triangleColour.g, 
					 ::g_triangleColour.b );


//sMyVertex vertices[6] =
//{
//    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
//    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
//    {  0.0f,  0.6f, 0.f, 0.f, 1.f },
//
//    { -2.6f, -0.4f, 1.f, 0.f, 0.f },
//    { -1.4f, -0.4f, 0.f, 1.f, 0.f },
//    { -2.0f,  0.6f, 0.f, 0.f, 1.f }
//};
		glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


	// delete everything
	delete ::g_pMyShaderManager;

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
