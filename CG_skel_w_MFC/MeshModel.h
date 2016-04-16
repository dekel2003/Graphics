#pragma once
#include "scene.h"
#include "vec.h"
#include "mat.h"
#include <string>

using namespace std;

class MeshModel : public Model
{
protected :
	MeshModel() {}
	vector<vec3> vertex_positions;
	vector<vec3> normals2vertices;
	int num_vertices;
	//add more attributes
	mat4 model_to_world_transform; //the model transformation
	mat4 _world_transform; //What the heck
	mat3 _normal_transform; //Pending question
	vector<vec4> projected_vecs;

public:

	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);
	void draw(Renderer* renderer);
	vector<vec4> GetModel();
	
	
};
