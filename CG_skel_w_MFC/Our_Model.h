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
public:
	Our_Model();
	void draw(Renderer* renderer);
	void setModelColor(float R, float G, float B);
};
