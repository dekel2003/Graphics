#version 330

in vec4 color;
in vec3 frag;
in vec3 norm;
in vec2 TexCoord;
out vec4 fColor;


uniform vec4 lPosition;
uniform vec3 lColor;

uniform int shadow; 
uniform sampler2D ourTexture;

vec4 putColor(vec4 color, vec4 lPosition, vec3 lColor, vec3 n, vec3 frag);

void main() 
{ 
   //fColor = textureLod( texture, texCoord, 0 );
   //fColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
   //fColor = color;
	vec4 pos = lPosition;
	//if (lPosition.w!=0)
	//	pos /= pos.w;

	if (shadow==2)
	   fColor = color + putColor(color, pos/pos.w, lColor, norm, frag);
	else
		fColor = color;

	fColor = texture2D(ourTexture, TexCoord);
} 

vec4 putColor(vec4 color, vec4 lPosition, vec3 lColor, vec3 normal, vec3 frag){

	vec4 tmpColor = vec4(0,0,0,0);
	vec3 l,r,e;
	float teta;
	vec3 eye = vec3(0.5, 0.5, 0);
	if (lPosition.w==1.0)
		l=normalize(lPosition.xyz - frag);
	else
		l = -frag;
	teta = dot(l, normal);

	r = normalize((2 * teta * normal) - l);
	e = normalize(eye - frag);

	tmpColor += color * max(1, 2/1) * max(0, teta) * vec4(lColor, 1.0f);
	//if (dot(r,normal)>0)
	//	tmpColor += color * max(1, 2/1) * pow(max(0, dot(r, e)), 10) * vec4(lColor, 1.0f);

	return tmpColor;
}