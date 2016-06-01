#pragma once
#include "scene.h"
#include "vec.h"
#include "mat.h"
#include <string>
#include "MeshModel.h"

using namespace std;

class Our_Model : public MeshModel
{
protected:
	float normalVectorsSize = 0.02;
	vec3 colors[9];
	GLuint VCO[4];
public:
	Our_Model(Renderer* renderer);
	void draw(Renderer* renderer);
	void setModelColor(float R, float G, float B);
};
