#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include "include/glad/glad.h"
#include "include/GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb/stb_image.h"



// Vertex Shader source code
const char* vertexShaderSource = "#version 410 core\n"
"layout (location = 0) in vec3 aPos;\n" // the position variable has attribute position 0
"layout (location = 1) in vec3 aColor;\n" // the color variable has attribute position 1
"layout (location = 2) in vec2 aTexCoord;\n" //texture coord
"out vec3 ourColor;\n" // output a color to the fragment shader
"out vec2 TexCoord;\n" //output a texcoord to the fragment shader
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n" // see how we directly give a vec3 to vec4's constructor
"	ourColor = aColor;\n" // set ourColor to the input color we got from the vertex data
"	TexCoord = aTexCoord;\n" //set texcoord to atexcoord
"}\0";



//Fragment Shader source code
const char* fragmentShaderSource = "#version 410 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
"   FragColor = texture(ourTexture, TexCoord);\n" //do this to combine our color with texture color: FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);  
"}\n\0";





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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  








//main code







int main(){
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()){
        printf("glwf init failed");
    }

	//configure opengl
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4); //number before the decimal (4).1
 	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1); //ohhhh this is what number is after the decimal in this case 4.(1)
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //idk
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //tell glfw what profile to use
	//in this case we are using core which means we can only use modern functions


	//vertices


	GLfloat vertices[] = { //x position, y position, z position //its on a normalized coordinate grid
		//positions				//colors         	//texture coords (0,0 is at the bottom left)
		0.05f, 0.05f, 0.0f, 	1.0f, 0.0f, 0.0f, 	1.0f, 1.0f, //top right
		0.05f, -0.05f, 0.0f, 	0.0f, 1.0f, 0.0f, 	1.0f, 0.0f, //bottom right
		-0.05f, -0.05f, 0.0f, 	0.0f, 0.0f, 1.0f, 	0.0f, 0.0f//bottom left
		-0.05f, 0.05f, 0.0f, 	1.0f, 0.0f, 1.0f, 	0.0f, 1.0f//top left
	};


	//indices


	GLuint indices[] = {
		3, 0, 1,
		1, 2, 3
	};



	//create window


    GLFWwindow *window = glfwCreateWindow(1000, 1000, "snake", NULL, NULL); //create a window named "snake" thats 1000x1000 pixels
    if (!window){ //checks if the window was created
        printf("window creation failed");
    }else{
		printf("window created");
	}
	
	//set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	glfwMakeContextCurrent(window); //sets the current context to the window


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


	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); //create the shader into a unsigned int IS A REFERENCE (POINTER?)!!!
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //fragment shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram(); //makes a shader program ALSO A REFERENCE/POINTER!! ALL GL STUFF IS POINTERS/REFERENCES

	glAttachShader(shaderProgram, vertexShader); //attaches the vertexShader to the shaderProgram
	glAttachShader(shaderProgram, fragmentShader); //same but with fragmentShader

	glLinkProgram(shaderProgram); //links the shader program or whatever

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); //check for errors
	if(!success) {
    	glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
	}

	

	glDeleteShader(vertexShader); //deletes the shaders because theyre already in the program itself
	glDeleteShader(fragmentShader);



	GLuint VAO, VBO, EBO; //vertex buffer object to send stuff from cpu to gpu in big batches
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//first input is the index of the vertex attribute we want to use //a vertex attribute is a way of communicating with a vertex shader from the outside
	//second input is how many values we have per vertex which is 3 in our case becaus we have 3 floats
	//third input is what type of values we have
	//fourth input only matters if we have coordinates as ints 
	//fifth input is the stride of our vertices which is just the amount of data between each vertex //should be same for all attributes
	//sixth input is called the offset which is a pointer to where our vertices begin in the array but since our vertices begin right at the start of the array were gonna give this weird pointer void
	glEnableVertexAttribArray(0); //enable the vertex attribarray and give it 0 because thats the position of our vertex attribute
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	glBindBuffer(GL_ARRAY_BUFFER, 0); //makes sure we dont accidentally change a VBO or VAO with a function
	//basically unbinds it by binding it to 0
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	//textures


	unsigned int texture;
	glGenTextures(1, &texture);
	//first input is how many textures to load
	//second input is where to load the textures
	glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
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

	printf("before stb stuff");

	//stb stuff
	int imageWidth, imageHeight, nrChannels;
	unsigned char *data = stbi_load("resources/textures/cat.jpg", &imageWidth, &imageHeight, &nrChannels, 0);
	//end

	printf("after stb stuff");

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





	//while loop




    while (!glfwWindowShouldClose(window)){
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f); //tell gl to prepare this color in the back buffer //a state setting function
		glClear(GL_COLOR_BUFFER_BIT); //tell gl to execute the command //uses the current state
		glUseProgram(shaderProgram); //activates the shader
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO); //binds the VAO to tell opengl that we want to use this one //not really necessary because we only have one object and one VAO but its good to get used to this


		


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
    return 0;
}



