

#pragma once

#include "vec.h"
#include "mat.h"
#include "GL/glew.h"

class Light {

public:
	vec4 location;
	Light(){
		location = vec4(-1, 1, -1, 1);
	}
	void move(GLfloat dx, GLfloat dy);
	void rotate(GLfloat dx, GLfloat dy);
	void move(GLfloat dz);
	void rotate(GLfloat dz);
};

void Light::move(GLfloat dx, GLfloat dy){
	location.x += dx;
	location.y += dy;
}

void Light::move(GLfloat dz){
	location.z += dz;
}

//void Light::rotate(GLfloat dx, GLfloat dy);
//void Light::rotate(GLfloat dz);



