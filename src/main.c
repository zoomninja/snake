#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb/stb_image.h"
#include "../include/cglm/cglm.h"
#include "../include/shader.h"




//variable definitions





vec3 cp; //cross product
vec3 av; //added vector
vec3 subv; //subtracted vector
vec3 sv; //scaled vector
vec3 mv; //multiplied vector

vec3 cameraPos = {0.0f, 0.0f, 0.0f};
vec3 cameraFront = {0.0f, 0.0f, -1.0f};
vec3 cameraUp = {0.0f, 1.0f, 0.0f};


bool firstMouse = true;
float lastX = 400;
float lastY = 300; //depends on the size of the window (should be in the middle of the screen)
float yaw = -90.0f;
float pitch = 0.0f;
const float sensitivity = 0.1f;
float fov = 70.0f;

const float cameraSpeed = 2.5f;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float gravity = 1.0f;
float upVelocity = 0.0f;





//shader related functions










//process input

void processInput(GLFWwindow* window){
	// Don't mutate the base speed; compute a per-frame velocity using deltaTime.
	float velocity = cameraSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		glm_vec3_scale(cameraFront, velocity, sv);
		glm_vec3_add(cameraPos, sv, av);
		glm_vec3_copy(av, cameraPos);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		glm_vec3_scale(cameraFront, velocity, sv);
		glm_vec3_sub(cameraPos, sv, subv);
		glm_vec3_copy(subv, cameraPos);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		glm_vec3_cross(cameraFront, cameraUp, cp);
		glm_vec3_normalize(cp);
		glm_vec3_scale(cp, velocity, sv);
		glm_vec3_sub(cameraPos, sv, subv);
		glm_vec3_copy(subv, cameraPos);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		glm_vec3_cross(cameraFront, cameraUp, cp);
		glm_vec3_normalize(cp);
		glm_vec3_scale(cp, velocity, sv);
		glm_vec3_add(cameraPos, sv, av);
		glm_vec3_copy(av, cameraPos);
	}
}



//glfw callbacks





void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS){
		switch (key){
			case GLFW_KEY_ESCAPE:
				glfwSetWindowShouldClose(window, GLFW_TRUE);
				break;
			case GLFW_KEY_Q:
				{} //because for some reason you cant declare after a label
				GLint polygonMode[2]; //first value is for front facing polygons and second is for back facing polygons
				glGetIntegerv(GL_POLYGON_MODE, polygonMode);
				if (polygonMode[0] == GL_FILL){
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}else{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
				break;
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
	if (firstMouse) // initially set to true
	{
    	lastX = xpos;
    	lastY = ypos;
    	firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw   += xoffset;
	pitch += yoffset;

	if(pitch > 89.0f){
		pitch =  89.0f;
	}
	if(pitch < -89.0f){
		pitch = -89.0f;
	}

	vec3 cameraDir;
	cameraDir[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
	cameraDir[1] = sin(glm_rad(pitch));
	cameraDir[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
	glm_vec3_normalize(cameraDir);
	glm_vec3_copy(cameraDir, cameraFront);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 10.0f)
        fov = 10.0f;
    if (fov > 90.0f)
        fov = 90.0f; 
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  








//main code







int main(){
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()){
        printf("glwf init failed\n");
    }

	//configure opengl
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4); //number before the decimal (4).1
 	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1); //ohhhh this is what number is after the decimal in this case 4.(1)
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //idk
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //tell glfw what profile to use
	//in this case we are using core which means we can only use modern functions


	



	//create window





    GLFWwindow *window = glfwCreateWindow(800, 600, "snake", NULL, NULL); //create a window named "snake" thats 1000x1000 pixels
    if (!window){ //checks if the window was created
        printf("window creation failed\n");
    }else{
		printf("window created\n");
	}
	
	glfwMakeContextCurrent(window); //sets the current context to the window
	//set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback); 
	//set mouse mode
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 


	


	//load opengl //make sure this is after glfwMakeContextCurrent


	int version_glad = gladLoadGL(); //loads gl
  	if ( version_glad == 0 ) { //checks if gl loaded
    	fprintf( stderr, "ERROR: Failed to initialize OpenGL context.\n" );
    	return 1;
  	}
  	printf("Loaded OpenGL\n");

	printf( "Renderer: %s.\n", glGetString( GL_RENDERER ) );
  	printf( "OpenGL version supported %s.\n", glGetString( GL_VERSION ) );

	//window

	int width, height;
	glfwGetFramebufferSize(window, &width, &height); //gets width and height of the window
	glViewport(0, 0, width, height); //create a viewport


	//shaders


	
	loadShaders();
	 



	//vertices

	/*
	GLfloat vertices[] = { //x position, y position, z position //its on a normalized coordinate grid
		//positions				//colors         	//texture coords (0,0 is at the bottom left)
		0.05f, 0.05f, 0.0f, 	1.0f, 0.0f, 0.0f, 	1.0f, 1.0f, //top right
		0.05f, -0.05f, 0.0f, 	0.0f, 1.0f, 0.0f, 	1.0f, 0.0f, //bottom right
		-0.05f, -0.05f, 0.0f, 	0.0f, 0.0f, 1.0f, 	0.0f, 0.0f, //bottom left
		-0.05f, 0.05f, 0.0f, 	1.0f, 0.0f, 1.0f, 	0.0f, 1.0f //top left
	};
	*/

	GLfloat vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, //cube
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
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

	-0.01f, 0.05f, 0.0f, 0.0f, 0.0f, //crosshair
	0.01f, 0.05f, 0.0f, 0.0f, 0.0f, 
	0.01f, 0.01f, 0.0f, 0.0f, 0.0f, 
	0.05f, 0.01f, 0.0f, 0.0f, 0.0f, 
	0.05f, -0.01f, 0.0f, 0.0f, 0.0f, 
	0.01f, -0.01f, 0.0f, 0.0f, 0.0f, 
	0.01f, -0.05f, 0.0f, 0.0f, 0.0f, 
	-0.01f, -0.05f, 0.0f, 0.0f, 0.0f, 
	-0.01f, -0.01f, 0.0f, 0.0f, 0.0f, 
	-0.05f, -0.01f, 0.0f, 0.0f, 0.0f, 
	-0.05f, 0.01f, 0.0f, 0.0f, 0.0f, 
	-0.01f, 0.01f, 0.0f, 0.0f, 0.0f
	};

	


	//indices


	GLuint indices[] = {
		0, 2, 1, //crosshair
		1, 2, 11,
	};



	//objects


	GLuint VAO, lightVAO, VBO, EBO; //vertex buffer object to send stuff from cpu to gpu in big batches
	//index bufffer is EBO
	//vertex array object basically tells opengl what VBOs to use and where to find them
	//stores an array of VBOs probably
	//makes it easier to change VBOs
	glGenVertexArrays(1, &VAO); //1 because only 1 object
	//MAKE SURE TO CREATE VAO BEFORE VBO
	glGenBuffers(1, &VBO); //1 because only 1 3d object //creates the VBO //try to store vertices in big batches because it could be slow if sending data to gpu too often
	glGenBuffers(1, &EBO); //1 cuz one object //creates the EBO

	glBindVertexArray(VAO); //binds the current vertex array at the VAO

	glBindBuffer(GL_ARRAY_BUFFER, VBO); //binds the current "GL_ARRAY_BUFFER" as the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //to store the vertices in the VBO
	//last param is the way you want the vertices to be used
	//STREAM means the vertices will be modified once and used a few times 
	//STATIC means the vertices will be modified once and used many many times
	//DYNAMIC means the vertices will be modified multiple times and used many many times
	//DRAW means the vertices will be modified and used to draw an image on the screen
	//READ means the vertices will be read?
	//COPY will copy the vertices?

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//configuration of VAO
	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//first input is the index of the vertex attribute we want to use //a vertex attribute is a way of communicating with a vertex shader from the outside
	//second input is how many values we have per vertex which is 3 in our case becaus we have 3 floats
	//third input is what type of values we have
	//fourth input only matters if we have coordinates as ints 
	//fifth input is the stride of our vertices which is just the amount of data between each vertex //should be same for all attributes
	//sixth input is called the offset which is a pointer to where our vertices begin in the array but since our vertices begin right at the start of the array were gonna give this weird pointer void
	glEnableVertexAttribArray(0); //enable the vertex attribarray and give it 0 because thats the position of our vertex attribute
	//texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// we only need to bind to the VBO, the container's VBO's data already contains the data.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// set the vertex attribute 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//texture attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);



	//unbinds



	glBindBuffer(GL_ARRAY_BUFFER, 0); //makes sure we dont accidentally change a VBO or VAO with a function
	//basically unbinds it by binding it to 0
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	//textures


	unsigned int texture;
	glGenTextures(1, &texture);
	//first input is how many textures to load
	//second input is where to load the textures
	glBindTexture(GL_TEXTURE_2D, texture);

	
	//sets the configuration only to the currently bound texture object
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	//first argument is the texture type which is 2d
	//second argument says what option we want to set and for which axis (s, t, r is the same as x, y, z)
	//third argument is the texture wrapping mode //required

	//basically the texture coordinates will not be exactly the same as the texture pixels so we use this to configure how it estimates it
	//its called texture sampling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//min is the setting for when it scales downwards
	//mag is the setting for when it scales upwards
	//GL_NEAREST just takes the nearest pixel's color (looks pixelated)
	//GL_LINEAR takes the interpolated color of the four nearest pixels (looks better but prolly laggier)

	//stb stuff
	int imageWidth, imageHeight, nrChannels;
	unsigned char *data = stbi_load("resources/textures/container.jpg", &imageWidth, &imageHeight, &nrChannels, 0);
	//end

	if (data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//first input is type of texture to generate into (this means GL_TEXTURE_1D and 3D will not be affected) //generates into the current GL_TEXTURE_2D object
		//The second argument specifies the mipmap level for which we want to create a texture for if you want to set each mipmap level manually, but we'll leave it at the base level which is 0.
		//third input is what color format we want (should be the same as the image color format)
		//fourth and fifth inputs are for what we want the width and height of the resulting texture to be //we will just use the width and height we got from the texture before
		//sixth input should alawyas be 0 (legacy stuff)
		//The 7th and 8th argument specify the format and datatype of the source image. We loaded the image with RGB values and stored them as chars (bytes) so we'll pass in the corresponding values.
		//The last argument is the actual image data.
		//once this is called the currently bound texture object now stores the texture image 
		glGenerateMipmap(GL_TEXTURE_2D); //automatically genereates all the mipmaps for the current texture
	}else{
		printf("texture was not found in files");
	}

	stbi_image_free(data); //free data used by the data variable





	

	
	
	

	glEnable(GL_DEPTH_TEST); //enable depth buffer











	//while loop
	

    while (!glfwWindowShouldClose(window)){
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);
		glClearColor(0.5f, 0.3f, 0.3f, 1.0f); //tell gl to prepare this color in the back buffer //a state setting function
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //tell gl to clear the previous buffer //uses the current state to rewrite
		glBindTexture(GL_TEXTURE_2D, texture);

		//gravity



		

		//shader stuff

		

		//view matrix (moving coords to be in view of the user)
		
		
		mat4 view;
		glm_mat4_identity(view);
		glm_vec3_add(cameraPos, cameraFront, av);
		glm_lookat(cameraPos, av, cameraUp, view);
		//first argument is the camera position
		//second is target to look at
		//third is an up vector
		//fourth is where to store this matrix
        


		//projection matrix

		mat4 projection;
		glm_mat4_identity(projection);
		glm_perspective(glm_rad(fov), 800.0f / 600.0f, 0.1f, 100.0f, projection);
		


		//model matrix

		glBindVertexArray(VAO);
		glUseProgram(shaderProgram); //activates the shader
        setUniformVec3(shaderProgram, "lightColor", (vec3){1.0f, 1.0f, 1.0f});
		setUniform(shaderProgram, "view", view);
		setUniform(shaderProgram, "projection", projection);
		
		for (int x = 0; x < 10; x++){
			for (int y = 0; y < 10; y++){
				mat4 model;
				glm_mat4_identity(model);
            	glm_translate(model, (vec3){x, 0.0f, y});
            	setUniform(shaderProgram, "model", model);

            	glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}

		
		glBindVertexArray(lightVAO);
		glUseProgram(lightShaderProgram);
		setUniformVec3(lightShaderProgram, "lightColor", (vec3){1.0f, 1.0f, 1.0f});
		setUniform(lightShaderProgram, "view", view);
		setUniform(lightShaderProgram, "projection", projection);


		mat4 model;
		glm_mat4_identity(model);
        glm_translate(model, (vec3){0.0f, 5.0f, 0.0f});
		glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});
        setUniform(lightShaderProgram, "model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		/*first input is the type of primitive we want to use
		second input is the starting index of the vertices, 0
		third input is the amount of vertices we want to draw
		thats for glDrawArrays but for glDrawElements:
		first is the same
		second is number of indices we want to use (basically same as a vertex vertices whatever)
		third is the datatype of our indices
		fourth is the index of our indices which is 0 in our case
		*/

		glfwSwapBuffers(window); //swap the buffers
        glfwPollEvents(); //takes care of all glwf events
    }










	//delete objects we've created
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}



