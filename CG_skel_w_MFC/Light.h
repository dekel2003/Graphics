

#pragma once

#include "vec.h"
#include "mat.h"
#include "GL/glew.h"

class Light {
public:
	vec3 color = vec3(1,1,1);
	vec4 location;
	Light(){
		location = vec4(0.5, 0.5, 0, 1);
	}
	void move(GLfloat dx, GLfloat dy);
	void rotate(GLfloat dx, GLfloat dy);
	void move(GLfloat dz);
	void rotate(GLfloat dz);
	void setLightColor(float red, float green, float blue);
};



//void Light::rotate(GLfloat dx, GLfloat dy);
//void Light::rotate(GLfloat dz);



