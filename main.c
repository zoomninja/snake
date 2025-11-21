#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include "include/glad/glad.h"
#include <GLFW/glfw3.h>





//glfw callbacks

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
        
}








//main code



int init(){
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()){
        printf("glwf init failed");
    }
	return 0;
}

int exitcode(){
	glfwTerminate();
	return 0;
}

int main(){
	init();

	//tell glfw what version of opengl we are using
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4); //number before the decimal (4).1
 	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1); //ohhhh this is what number is after the decimal in this case 4.(1)
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //idk
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //tell glfw what profile to use
	//in this case we are using core which means we can only use modern functions


    GLFWwindow *window = glfwCreateWindow(1000, 1000, "snake", NULL, NULL); //create a window named "snake" thats 1000x1000 pixels
    if (!window){ //checks if the window was created
        printf("window creation failed");
    }

	glfwSetKeyCallback(window, key_callback); //make the window call key_callback function whenever a key is pressed


	glfwMakeContextCurrent(window); //sets the current context to the window

	int version_glad = gladLoadGL(); //loads gl
  	if ( version_glad == 0 ) { //checks if gl loaded
    	fprintf( stderr, "ERROR: Failed to initialize OpenGL context.\n" );
    	return 1;
  	}
  	printf("Loaded OpenGL\n");

	printf( "Renderer: %s.\n", glGetString( GL_RENDERER ) );
  	printf( "OpenGL version supported %s.\n", glGetString( GL_VERSION ) );

	int width, height;
	glfwGetFramebufferSize(window, &width, &height); //gets width and height of the window
	glViewport(0, 0, width, height); //create a viewport

	glClearColor(0.07f, 0.13f, 0.17f, 1.0f); //tell gl to prepare this color in the back buffer
	glClear(GL_COLOR_BUFFER_BIT); //tell gl to execute the command?
	glfwSwapBuffers(window); //swap the buffers

    while (!glfwWindowShouldClose(window)){
		
		

        
        glfwPollEvents(); //makes the window do stuff
    }
	glfwDestroyWindow(window);
	exitcode();
    return 0;
}



