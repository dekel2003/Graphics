#version 150

in  vec4 vPosition;
in  vec3 nPosition;

uniform mat4 Tmodel;
uniform mat4 Tcamera;
uniform mat4 Tprojection;

uniform vec3 MyColor;

out vec4 color;
out vec3 frag;
out vec3 norm;

void main()
{
    gl_Position = Tprojection * Tcamera * Tmodel * vPosition;
	color = vec4(MyColor, 1.0f);

	frag = (Tmodel * vPosition).xyz;
	norm = normalize((   transpose(inverse(Tmodel)) * vec4(nPosition,1)   ).xyz);
}
