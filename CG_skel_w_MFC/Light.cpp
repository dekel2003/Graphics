
#include "Light.h"
#include "stdafx.h"

void Light::move(GLfloat dx, GLfloat dy){
	location.x += dx;
	location.y += dy;
}

void Light::move(GLfloat dz){
	location.z += dz;
}

void Light::setLightColor(float red, float green, float blue){
	color.x = red / 256.0;
	color.y = green / 256.0;
	color.z = blue / 256.0;
}