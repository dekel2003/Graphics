#version 150

in  vec4 vPosition;
in  vec3 nPosition;
in  vec2 texCoord;

uniform mat4 Tmodel;
uniform mat4 Tcamera;
uniform mat4 Tprojection;

uniform vec3 MyColor;

out vec4 color;
out vec3 frag;
out vec3 norm;
out vec2 TexCoord;

void main()
{
    gl_Position = Tprojection * Tcamera * Tmodel * vPosition;
	color = vec4(MyColor, 1.0f);

	vec4 _frag = Tmodel * vPosition;
	frag = _frag.xyz / _frag.w;
	norm = normalize((   transpose(inverse(Tmodel)) * vec4(nPosition,1)   ).xyz);
	TexCoord = texCoord;
}
