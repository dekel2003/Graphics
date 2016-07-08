#version 330

in vec3 TexCoords;
out vec4 color;

uniform samplerCube skybox;

void main()
{    
	//vec3 _TexCoords = TexCoords;
	//_TexCoords.y = 1-TexCoords.y;
    color = texture(skybox, TexCoords);
}