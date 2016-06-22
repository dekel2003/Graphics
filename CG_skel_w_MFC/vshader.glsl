#version 330

in  vec4 vPosition;
in  vec3 nPosition;
in  vec3 fnPosition;
in  vec2 texCoord;

uniform mat4 Tmodel;
uniform mat4 Tcamera;
uniform mat4 Tprojection;

uniform vec3 MyColor;
uniform int shadow; 

uniform vec4 lPosition[20];
uniform vec3 lColor[20];
uniform int numLights;

uniform int useTexture;

out vec4 color;
out vec3 frag;
out vec3 norm;
out vec2 TexCoord;

vec4 putColor(vec4 color, vec4 lPosition, vec3 lColor, vec3 n, vec3 frag);

void main()
{

//{ FLAT, GOUARD, PHONG }

	if (shadow==0)
		norm = normalize((   transpose(inverse(Tmodel)) * vec4(fnPosition,1)   ).xyz);
	else
		norm = normalize((   transpose(inverse(Tmodel)) * vec4(nPosition,1)   ).xyz);

	vec4 temp = Tcamera * vec4(norm,1);
	vec3 cameraNorm = (temp / temp.w).xyz;
	vec4 vertex = vPosition;
	if (shadow == 3 && cameraNorm.z>0){
		vertex += vec4(0.001 * norm, 0);
	}
    gl_Position = Tprojection * Tcamera * Tmodel * vertex;
	vec4 frag4 = Tmodel * vertex;
	frag = (frag4 / frag4.w).xyz;




	

	if (useTexture == 0)
		color = vec4(MyColor, 1.0f);
	else
		color = vec4(1,1,1,1);

	
	if (shadow == 0 || shadow == 1){
		vec4 extraColor = vec4(0,0,0,0);
		for (int j=0; j<numLights; ++j){
				extraColor += putColor(color, lPosition[j], lColor[j], norm, frag );
		}
		color += extraColor;
		color.w=1;
	}

	if (shadow == 3 && cameraNorm.z < 0)
		color = vec4(0,0,0,1);

	TexCoord = vec2(texCoord.x, 1-texCoord.y);

	//
	//norm = (transpose(inverse(mat3(Tmodel))) * nPosition);
	//
}


vec4 putColor(vec4 color, vec4 _lPosition, vec3 _lColor, vec3 normal, vec3 frag){

	vec4 tmpColor = vec4(0,0,0,0);
	vec3 l,r,e;
	float teta;
	vec3 eye = vec3(0, 0, 0);
	if (_lPosition.w==1.0)
		l=normalize(_lPosition.xyz-frag);
	else if (_lPosition.w==0.0)
		l = -_lPosition.xyz;
	teta = dot(l, normal);

	r = normalize((2 * teta * normal) - l);
	e = normalize(eye - frag);

	tmpColor += color * max(1, 2/1) * max(0, teta) * vec4(_lColor, 1.0f);
	if (dot(r,normal)>0)
		tmpColor += color * max(1, 2/1) * pow(max(0, dot(r, e)), 10) * vec4(_lColor, 1.0f);

	return tmpColor;
}