#include <glad/glad.h>
#include <GLFW/glfw3.h>
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

struct sVertex
{
    float x, y;
    float r, g, b;
};

sVertex vertices[6] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f },

    { -0.6f + 3.0f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f + 3.0f, -0.4f, 0.f, 1.f, 0.f },
    {  0.0f + 3.0f,  0.6f, 0.f, 0.f, 1.f }
};
//glDrawArrays(GL_TRIANGLES, 0, 6);


glm::vec3 g_cameraEye = glm::vec3( 0.0, 0.0, +12.0f ); 

// Step 3: Add a C++ side variable for colour
//         (so we can change it externally)
glm::vec3 g_triangleColour = glm::vec3(0.5f, 0.1f, 0.75f);

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    vec2 finalPos = vPos;\n"
"    gl_Position = MVP * vec4(finalPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

// Step 1: Add a newColour uniform variable to the shader...
static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"uniform vec3 newColour;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(newColour, 1.0);\n"
"}\n";

// "color" changed to "newColour" in line 55

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

	// WASD
	// A & D =  x axis (left and right)
	// W & S =  z axis (towards and away)
	// Q & E =  y axis (up and down)
	const float CAMERASPEED = 0.1f;

	if ( key == GLFW_KEY_A ) {	::g_cameraEye.x += CAMERASPEED;	}
	if ( key == GLFW_KEY_D ) {	::g_cameraEye.x -= CAMERASPEED;	}

	if ( key == GLFW_KEY_W ) {	::g_cameraEye.z += CAMERASPEED;	}
	if ( key == GLFW_KEY_S ) {	::g_cameraEye.z -= CAMERASPEED;	}

	if ( key == GLFW_KEY_Q ) {	::g_cameraEye.y += CAMERASPEED;	}
	if ( key == GLFW_KEY_E ) {	::g_cameraEye.y -= CAMERASPEED;	}

	const float COLOURCHANGERATE = 0.01f;
	if ( key == GLFW_KEY_1 ) { ::g_triangleColour.x += COLOURCHANGERATE; }
	if ( key == GLFW_KEY_2 ) { ::g_triangleColour.x -= COLOURCHANGERATE; }

	if ( key == GLFW_KEY_3 ) { ::g_triangleColour.y += COLOURCHANGERATE; }
	if ( key == GLFW_KEY_4 ) { ::g_triangleColour.y -= COLOURCHANGERATE; }

	if ( key == GLFW_KEY_5 ) { ::g_triangleColour.z += COLOURCHANGERATE; }
	if ( key == GLFW_KEY_6 ) { ::g_triangleColour.z -= COLOURCHANGERATE; }

	std::cout << "tricolour = (R:" 
		<< ::g_triangleColour.x << ", "
		<< ::g_triangleColour.y << ", "
		<< ::g_triangleColour.z << ")" << std::endl;

}


int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
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
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//vertices[6]

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);


//	"uniform mat4 MVP;\n"
//	"attribute vec3 vCol;\n"
//	"attribute vec2 vPos;\n"
    mvp_location = glGetUniformLocation(program, "MVP");

	//"uniform vec3 newColour;\n"
	// 
	// Step 2: Get the "location" (i.e. number) of that
	//         "uniform" variable inside the shader
	GLint newColour_location = glGetUniformLocation(program, "newColour");

    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

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
        glm::mat4 m, p, v, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        //         mat4x4_identity(m);
		m = glm::mat4(1.0f);	

		//mat4x4_rotate_Z(m, m, (float) glfwGetTime());
		glm::mat4 rotateZ = glm::rotate( glm::mat4(1.0f), 
									0.0f, // (float) glfwGetTime(), 
									glm::vec3( 0.0f, 0.0, 1.0f ) );

		m = m * rotateZ;

        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		p = glm::perspective( 0.6f, 
							  ratio, 
							  0.1f, 
							  1000.0f );

		v = glm::mat4(1.0f);

 //		glm::vec3 cameraEye = glm::vec3( 0.0, 0.0, +12.0f ); 
		glm::vec3 cameraTarget = glm::vec3( 0.0f, 0.0f, 0.0f ); 
		glm::vec3 upVector = glm::vec3( 0.0f, 1.0f, 0.0f );


		v = glm::lookAt( g_cameraEye, 
						 cameraTarget,
						 upVector );
     		
		//mat4x4_mul(mvp, p, m);
		mvp = p * v * m; 


        glUseProgram(program);


        //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
		glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

		// Step 4: Copy the value from our C++ side into our shader
		// 
		//       uniform vec3 newColour;
		//  
		// a vec3 is 3 floats, so we use glUniform3f()

		glUniform3f( newColour_location, 
					 ::g_triangleColour.x,
					 ::g_triangleColour.y, 
					 ::g_triangleColour.z );



 //       glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
