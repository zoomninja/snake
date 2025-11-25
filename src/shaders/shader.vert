#version 410 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec2 aTexCoord;
out vec2 TexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{ // note that we read the multiplication from right to left //so the first one we multiply is on the right (model matrix)
   gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = aTexCoord;
}