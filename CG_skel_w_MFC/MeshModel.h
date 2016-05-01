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
	vector<vec4> vertex_positions;
	vector<vec3> normals2vertices;
	vector<pair<vec3, vec3>> normalsToFaces;
	vector<pair<vec3, vec3>> normalsToVertices;
	vec4 massCenter;
	int num_vertices;
	float normalVectorsSize = 0.01;
	bool shouldDrawNormals;
	//add more attributes
	mat4 model_to_world_transform; //the model transformation (Tm)
	mat4 _world_transform; //What the heck (Tw)
	mat3 _normal_transform; //Pending question
	GLfloat minX = -MAXUINT, minY = -MAXUINT, minZ = -MAXUINT,
		maxX = MAXUINT, maxY = MAXUINT, maxZ = MAXUINT;

	void computeNormalsPerFace();
public:

	MeshModel(string fileName);
	~MeshModel(void);
	void loadFile(string fileName);
	void draw(Renderer* renderer);
	vector<vec4> GetModel();
	void setModelTransformation(const mat4& T);
	void setWorldTransformation(const mat4& T);
	void drawFaceNormals(Renderer* renderer);
	void drawVertexNormals(Renderer* renderer);
	vec3 getTopRightFar();
	vec3 getBottomLeftNear();
	vec4 getOrigin(); // returns the origin of the model
	void drawAxis(Renderer* renderer);
};
