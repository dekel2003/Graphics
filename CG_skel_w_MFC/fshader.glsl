#version 330

in vec4 color;
in vec3 frag;
in vec3 norm;
in vec2 TexCoord;

out vec4 fColor;
uniform mat4 Tcamera;

uniform vec4 lPosition[20];
uniform vec3 lColor[20];
uniform int numLights;

uniform int shadow; 
uniform sampler2D ourTexture;
uniform sampler2D normalMap;
uniform int useTexture;
uniform int useNormalMapping;
uniform float AmbientIntensity;

vec4 putColor(vec4 color, vec4 lPosition, vec3 lColor, vec3 n, vec3 frag);

void main() 
{ 
   //fColor = textureLod( texture, texCoord, 0 );
   //fColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
   //fColor = color;
	
	//if (lPosition[0].w!=0)
	//	pos /= pos.w;

	vec3 normalToUse = (useNormalMapping == 1) ? normalize(texture(normalMap, TexCoord).rgb * 2.0 - 1.0) * 0.5 + norm : norm;

	vec4 colorToUse = (useTexture == 1) ? texture2D(ourTexture, TexCoord) : color;

	if (useTexture==1){
		colorToUse*=AmbientIntensity/2;
	}

	if (shadow==2 || shadow==3){
		vec4 extraColor = vec4(0,0,0,0);
		for (int j=0; j<numLights; ++j){
		 extraColor += putColor(colorToUse, lPosition[j], lColor[j], normalToUse, frag);
		}
		fColor = colorToUse + extraColor;
		fColor.w=1;
	}
	else if (useTexture == 1)
		fColor = colorToUse * color;
	else
		fColor = colorToUse;

	if (shadow == 3){
		vec4 temp = Tcamera * vec4(norm,1);
		vec3 cameraNorm = (temp / temp.w).xyz;
		//if (abs(sqrt(cameraNorm.x*cameraNorm.x+cameraNorm.y*cameraNorm.y))>0.95)
		//	fColor = vec4(0,0,0,1);
		//else
			fColor.xyz = round(fColor.xyz * 8) / 8.0;
	}

} 

vec4 putColor(vec4 color, vec4 lPosition, vec3 lColor, vec3 normal, vec3 frag){

	vec4 tmpColor = vec4(0,0,0,0);
	vec3 l,r,e;
	float teta;
	vec3 eye = vec3(0, 0, 0);
	if (lPosition.w==1.0)
		l=normalize(lPosition.xyz - frag);
	else if (lPosition.w==0.0)
		l = -lPosition.xyz;
	else
		return vec4(1,1,1,1);
	teta = dot(l, normal);

	r = normalize((2 * teta * normal) - l);
	e = normalize(eye - frag);

	tmpColor += color * max(1, 2/1) * max(0, teta) * vec4(lColor, 1.0f);
	if (dot(r,normal)>0)
		tmpColor += color * max(1, 2/1) * pow(max(0, dot(r, e)), 10) * vec4(lColor, 1.0f);

	return tmpColor;
}