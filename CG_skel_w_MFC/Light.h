

#pragma once

#include "vec.h"
#include "mat.h"
#include "GL/glew.h"

enum LightType { LIGHT_POINT, LIGHT_PARALLEL };
class Light {
public:
	LightType lightType;
	vec3 color = vec3(1,1,1);
	vec4 location;
	Light(){
		location = vec4(-0.9, -0.9, -0.9, 1);
		lightType = LIGHT_POINT;
	}
	void move(GLfloat dx, GLfloat dy);
	void rotate(GLfloat dx, GLfloat dy);
	void move(GLfloat dz);
	void rotate(GLfloat dz);
	void setLightColor(float red, float green, float blue);
};



//void Light::rotate(GLfloat dx, GLfloat dy);
//void Light::rotate(GLfloat dz);
