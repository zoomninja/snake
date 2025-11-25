#version 410 core
out vec4 FragColor;
in vec2 TexCoord;
uniform vec3 lightColor;
uniform sampler2D texture1;
void main()
{
   float ambientStrength = 0.1;
   vec3 ambient = ambientStrength * lightColor;

   vec4 result = texture(texture1, TexCoord) * vec4(ambient, 1);
   FragColor = result;
}