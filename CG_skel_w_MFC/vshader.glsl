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
	vec4 _frag = Tmodel * vPosition;
	frag = _frag.xyz;
	vec3 normal = vec3(1,1,1);
	norm = mat3(transpose(inverse(Tmodel))) * nPosition;
}
