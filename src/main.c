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
#include "../include/mesh.h"
#define FNL_IMPL
#include "../include/FastNoiseLite.h"






//variable definitions





vec3 cp; //cross product
vec3 av; //added vector
vec3 subv; //subtracted vector
vec3 sv; //scaled vector
vec3 mv; //multiplied vector

vec3 cameraPos = {5.0f, 5.0f, 5.0f};
vec3 cameraFront = {0.0f, 0.0f, -1.0f};
vec3 cameraUp = {0.0f, 1.0f, 0.0f};
int cameraChunkX = 0;
int cameraChunkZ = 0;


bool firstMouse = true;
float lastX = 400;
float lastY = 300; //depends on the size of the window (should be in the middle of the screen)
float yaw = -90.0f;
float pitch = 0.0f;
const float sensitivity = 0.1f;
float fov = 70.0f;

float cameraSpeed = 2.5f;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

vec3 playerHitbox = {1.0f, 2.0f, 1.0f};
float gravity = 10.0f; //per second
float upVelocity = 0.0f;


vec3 backgroundColor = {0.26f, 0.40f, 0.46f};

float lightConstant = 1.0f;
float lightLinear = 0.09f;
float lightQuadratic = 0.032f;



//structs




//noise



fnl_state noise;





//process input

void processInput(GLFWwindow* window);

//glfw callbacks

void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


//textures

unsigned int loadTexture(char const * path);

//chunks

void checkCameraChunk();
int getHeight(int x, int z);
void loadChunk(int chunkX, int chunkZ);





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

		// Enable sRGB framebuffer conversion so GL will convert sRGB textures
		// to linear for shading without changing shaders.
		glEnable(GL_FRAMEBUFFER_SRGB);

	//window

	int width, height;
	glfwGetFramebufferSize(window, &width, &height); //gets width and height of the window
	glViewport(0, 0, width, height); //create a viewport


	//shaders


	
	loadShaders();



	//vertices

	

	GLfloat cubeVertices[] = { //cube
	//position			//normal vectors 	// texcoords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 
	 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 
	 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 
	 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 

	-0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
	-0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 
	-0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 
	-0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
	 0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 
	 0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 
	 0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

	/*
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
	*/
	};

	GLuint cubeIndices[] = {
    // Front face
    4, 5, 6,  // Triangle 1
    4, 6, 7,  // Triangle 2

    // Back face
    0, 1, 2,  // Triangle 3
    0, 2, 3,  // Triangle 4

    // Top face
    23, 22, 21,  // Triangle 5
    23, 21, 20,  // Triangle 6

    // Bottom face
    16, 17, 18,  // Triangle 7
    16, 18, 19,  // Triangle 8

    // Right face
    15, 14, 13,  // Triangle 9
    15, 13, 12,  // Triangle 10

    // Left face
    10, 11, 8,  // Triangle 11
    10, 8, 9   // Triangle 12
	};



	vec3 pointLightVertices[] = {
		{0.0f, 2.0f, 0.0f}, 
		{10.0f, 2.0f, 0.0f}, 
		{0.0f, 2.0f, 10.0f}, 
		{10.0f, 2.0f, 10.0f}
	};

	


	//indices

	
	GLuint indices[] = {
		0, 2, 1, //crosshair
		1, 2, 11,
	};
	



	//objects


	GLuint VAO, lightVAO, VBO, EBO, FBO;
	glGenVertexArrays(1, &VAO); 
	glGenBuffers(1, &VBO); 
	glGenBuffers(1, &EBO); 
	glGenFramebuffers(1, &FBO);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW); 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texcoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// we only need to bind to the VBO, the container's VBO's data already contains the data.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// set the vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//texture attribute



	//unbinds



	glBindBuffer(GL_ARRAY_BUFFER, 0); //makes sure we dont accidentally change a VBO or VAO with a function
	//basically unbinds it by binding it to 0
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);





	

	//NOISE


	noise = fnlCreateState();
	noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
	noise.seed = 1254;
	noise.frequency = 0.01f;

	

	




	//TEXTURES

	unsigned int diffuseMap = loadTexture("resources/textures/grass.jpg");
    unsigned int specularMap = loadTexture("resources/textures/grass.jpg");




	//LIGHTING


	glUseProgram(shaderProgram);
	

	

	setUniformVec3(shaderProgram, "dirLight.direction", (vec3){-0.2f, -1.0f, -0.3f});
	setUniformVec3(shaderProgram, "dirLight.ambient", (vec3){0.05f, 0.05f, 0.05f});
	setUniformVec3(shaderProgram, "dirLight.diffuse", (vec3){0.4f, 0.4f, 0.4f});
	setUniformVec3(shaderProgram, "dirLight.specular", (vec3){0.5f, 0.5f, 0.5f});

	setUniformVec3(shaderProgram, "pointLights[0].position", pointLightVertices[0]);
	setUniformVec3(shaderProgram, "pointLights[0].ambient", (vec3){0.05f, 0.05f, 0.05f});
	setUniformVec3(shaderProgram, "pointLights[0].diffuse", (vec3){0.8f, 0.8f, 0.8f});
	setUniformVec3(shaderProgram, "pointLights[0].specular", (vec3){1.0f, 1.0f, 1.0f});
	setUniformFloat(shaderProgram, "pointLights[0].constant", lightConstant);
	setUniformFloat(shaderProgram, "pointLights[0].linear", lightLinear);
	setUniformFloat(shaderProgram, "pointLights[0].quadratic", lightQuadratic);

	setUniformVec3(shaderProgram, "pointLights[1].position", pointLightVertices[1]);
	setUniformVec3(shaderProgram, "pointLights[1].ambient", (vec3){0.05f, 0.05f, 0.05f});
	setUniformVec3(shaderProgram, "pointLights[1].diffuse", (vec3){0.8f, 0.8f, 0.8f});
	setUniformVec3(shaderProgram, "pointLights[1].specular", (vec3){1.0f, 1.0f, 1.0f});
	setUniformFloat(shaderProgram, "pointLights[1].constant", lightConstant);
	setUniformFloat(shaderProgram, "pointLights[1].linear", lightLinear);
	setUniformFloat(shaderProgram, "pointLights[1].quadratic", lightQuadratic);

	setUniformVec3(shaderProgram, "pointLights[2].position", pointLightVertices[2]);
	setUniformVec3(shaderProgram, "pointLights[2].ambient", (vec3){0.05f, 0.05f, 0.05f});
	setUniformVec3(shaderProgram, "pointLights[2].diffuse", (vec3){0.8f, 0.8f, 0.8f});
	setUniformVec3(shaderProgram, "pointLights[2].specular", (vec3){1.0f, 1.0f, 1.0f});
	setUniformFloat(shaderProgram, "pointLights[2].constant", lightConstant);
	setUniformFloat(shaderProgram, "pointLights[2].linear", lightLinear);
	setUniformFloat(shaderProgram, "pointLights[2].quadratic", lightQuadratic);

	setUniformVec3(shaderProgram, "pointLights[3].position", pointLightVertices[3]);
	setUniformVec3(shaderProgram, "pointLights[3].ambient", (vec3){0.05f, 0.05f, 0.05f});
	setUniformVec3(shaderProgram, "pointLights[3].diffuse", (vec3){0.8f, 0.8f, 0.8f});
	setUniformVec3(shaderProgram, "pointLights[3].specular", (vec3){1.0f, 1.0f, 1.0f});
	setUniformFloat(shaderProgram, "pointLights[3].constant", lightConstant);
	setUniformFloat(shaderProgram, "pointLights[3].linear", lightLinear);
	setUniformFloat(shaderProgram, "pointLights[3].quadratic", lightQuadratic);

	
	setUniformFloat(shaderProgram, "spotLight.cutOff", (float)cos(glm_rad(12.5f)));
	setUniformFloat(shaderProgram, "spotLight.outerCutOff", (float)cos(glm_rad(17.5f)));
	setUniformVec3(shaderProgram, "spotLight.ambient", (vec3){0.0f, 0.0f, 0.0f});
	setUniformVec3(shaderProgram, "spotLight.diffuse", (vec3){1.0f, 1.0f, 1.0f});
	setUniformVec3(shaderProgram, "spotLight.specular", (vec3){1.0f, 1.0f, 1.0f});
	setUniformFloat(shaderProgram, "spotLight.constant", lightConstant);
	setUniformFloat(shaderProgram, "spotLight.linear", lightLinear);
	setUniformFloat(shaderProgram, "spotLight.quadratic", lightQuadratic);
	

	setUniformFloat(shaderProgram, "material.shininess", 32.0f);
	setUniformInt(shaderProgram, "material.diffuse", 0);
	setUniformInt(shaderProgram, "material.specular", 1);








	//MODELS


	Model backpack = loadModel("resources/models/backpack/backpack.obj");
	



	//RENDER LOOP
	

	

	glEnable(GL_DEPTH_TEST); //enable depth buffer
	glDepthFunc(GL_LESS);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_ALWAYS, GL_ALWAYS, GL_ALWAYS);

	

	

	

    while (!glfwWindowShouldClose(window)){
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 1.0f); //tell gl to prepare this color in the back buffer //a state setting function
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //tell gl to clear the previous buffer //uses the current state to rewrite
		



		processInput(window);



		//physics

		/*
		upVelocity -= gravity * deltaTime;
		glm_vec3_add(cameraPos, (vec3){0.0f, upVelocity * deltaTime, 0.0f}, cameraPos);

		if (cameraPos[1] < 1){
			upVelocity = 0;
			cameraPos[1] = 1;
		}
		*/




		



		

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
		glm_perspective(glm_rad(fov), 800.0f / 600.0f, 0.1f, 200.0f, projection);
		


		//grid of cubes

		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

		glBindVertexArray(VAO);
		glUseProgram(shaderProgram); //activates the shader
		
		setUniformVec3(shaderProgram, "spotLight.position", cameraPos);
		setUniformVec3(shaderProgram, "spotLight.direction", cameraFront);
		setUniformVec3(shaderProgram, "viewPos", cameraPos);
		setUniform(shaderProgram, "view", view);
		setUniform(shaderProgram, "projection", projection);
		
		for (int x = cameraChunkX - 5; x < cameraChunkX + 5; x++){
			for (int z = cameraChunkZ - 5; z < cameraChunkZ + 5; z++){
				loadChunk(x, z);
			}
		}

		mat4 model;
		glm_mat4_identity(model);
        glm_translate(model, (vec3){5.0f, 5.0f, 3.0f});
		glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});
        setUniform(shaderProgram, "model", model);
		drawModel(shaderProgram, &backpack);

		


		//light cubes


		glBindVertexArray(lightVAO);
		glUseProgram(lightShaderProgram);
		setUniformVec3(lightShaderProgram, "lightColor", (vec3){1.0f, 1.0f, 1.0f});
		setUniform(lightShaderProgram, "view", view);
		setUniform(lightShaderProgram, "projection", projection);

		for (int i = 0; i < 4; i++){
			mat4 model;
			glm_mat4_identity(model);
			glm_translate(model, pointLightVertices[i]);
			glm_scale(model, (vec3){0.2f, 0.2f, 0.2f});
        	setUniform(lightShaderProgram, "model", model);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		}

		

		
		

		
		
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
	glDeleteFramebuffers(1, &FBO);
	glDeleteProgram(shaderProgram);
	freeModel(&backpack);

	glfwDestroyWindow(window);
	glfwTerminate();
    return 0;}









//process input






void processInput(GLFWwindow* window){
	// Don't mutate the base speed; compute a per-frame velocity using deltaTime.
	float velocity = cameraSpeed * deltaTime;
	const float ylevel = cameraPos[1];
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		glm_vec3_scale(cameraFront, velocity, sv);
		glm_vec3_add(cameraPos, sv, av);
		glm_vec3_copy(av, cameraPos);
		checkCameraChunk();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		glm_vec3_scale(cameraFront, velocity, sv);
		glm_vec3_sub(cameraPos, sv, subv);
		glm_vec3_copy(subv, cameraPos);
		checkCameraChunk();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		glm_vec3_cross(cameraFront, cameraUp, cp);
		glm_vec3_normalize(cp);
		glm_vec3_scale(cp, velocity, sv);
		glm_vec3_sub(cameraPos, sv, subv);
		glm_vec3_copy(subv, cameraPos);
		checkCameraChunk();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		glm_vec3_cross(cameraFront, cameraUp, cp);
		glm_vec3_normalize(cp);
		glm_vec3_scale(cp, velocity, sv);
		glm_vec3_add(cameraPos, sv, av);
		glm_vec3_copy(av, cameraPos);
		checkCameraChunk();
	}
}







//glfw callbacks






void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}  

void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
}


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
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
			case GLFW_KEY_SPACE:
				upVelocity = 5.0f;
				break;
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    cameraSpeed -= yoffset;
	if (cameraSpeed < 1){
		cameraSpeed = 1;
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










//textures










unsigned int loadTexture(const char * path){
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
       	printf("Texture failed to load at path: %s", path);
        stbi_image_free(data);
    }

    return textureID;
}




//physics


void checkCollisions(){

}






//chunks



void checkCameraChunk(){
	cameraChunkX = round(cameraPos[0] / 16);
	cameraChunkZ = round(cameraPos[2] / 16);
}


int getHeight(int x, int z){
	return round(fnlGetNoise2D(&noise, x, z) * 5);
}




void loadChunk(int chunkX, int chunkZ){
	for (unsigned int x = 0; x < 16; x++){
		for (unsigned int z = 0; z < 16; z++){
			int height = getHeight(chunkX * 16 + x, chunkZ * 16 + z);
			mat4 model;
			glm_mat4_identity(model);
			glm_translate(model, (vec3){chunkX * 16.0f, 0.0f, chunkZ * 16.0f});
            glm_translate(model, (vec3){x+0.5f, height, z+0.5f});
            setUniform(shaderProgram, "model", model);
			if (getHeight(chunkX * 16 + x, chunkZ * 16 + z + 1) < height)
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //front face
			if (getHeight(chunkX * 16 + x, chunkZ * 16 + z - 1) < height)
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float)*6)); //back face
			if (getHeight(chunkX * 16 + x + 1, chunkZ * 16 + z) < height)
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float)*24)); //right face
			if (getHeight(chunkX * 16 + x - 1, chunkZ * 16 + z) < height)
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float)*30)); //left face
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(sizeof(float)*12)); //top face
			
            
		}
	}
}