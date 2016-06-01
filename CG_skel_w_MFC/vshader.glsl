#version 330


in vec4 vPosition;


out vec4 color;


uniform mat4 Tmodel;
uniform mat4 Tcamera;
uniform mat4 Tprojection;

uniform vec3 MyColor;

void main()
{
    gl_Position = Tprojection * Tcamera * Tmodel * vPosition;
	

    //frag = vec3(model * vec4(position, 1.0f));
   //mat3 Norm = mat3(transpose(inverse(Tmodel))) * nPosition;

	color = vec4(MyColor, 1.0f);
}

