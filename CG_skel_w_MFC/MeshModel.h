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
	vector<vec3> normals2vertices, normalsToFacesGeneralForm, normalsToVerticesGeneralForm;
	vector<pair<vec3, vec3>> normalsToFaces;
	vector<pair<vec3, vec3>> normalsToVertices;
	vec4 massCenter;
	int num_vertices;
	float normalVectorsSize = 0.01;
	bool shouldDrawNormals;
	//add more attributes
	mat4 model_to_world_transform; //the model transformation (Tm)
	mat4 _world_transform; //What the heck (Tw)
	mat4 _normal_transform;
	GLfloat minX, minY, minZ ,maxX , maxY, maxZ;
	vec4 cube[8];
	void computeNormalsPerFace();

	vec3 baseColor = vec3(256, 50, 50);


public:
	vec3 color = vec3(0, 70, 70);

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
	void MeshModel::drawBoundingBox(Renderer* renderer);
	void setModelColor(float R, float G, float B);
};
