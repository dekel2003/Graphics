#pragma once
#include "scene.h"
#include "vec.h"
#include "mat.h"
#include <string>

using namespace std;

struct FaceIdcs
{
	int v[4];
	int vn[4];
	int vt[4];

	FaceIdcs()
	{
		for (int i = 0; i<4; i++)
			v[i] = vn[i] = vt[i] = 0;
	}

	FaceIdcs(std::istream & aStream)
	{
		for (int i = 0; i<4; i++)
			v[i] = vn[i] = vt[i] = 0;

		char c;
		for (int i = 0; i < 3; i++)
		{
			aStream >> std::ws >> v[i] >> std::ws;
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> std::ws;
			if (aStream.peek() == '/')
			{
				aStream >> c >> std::ws >> vn[i];
				continue;
			}
			else
				aStream >> vt[i];
			if (aStream.peek() != '/')
				continue;
			aStream >> c >> vn[i];
		}
	}
};

class MeshModel : public Model
{
protected :
	MeshModel() {}
	vector<vec4> vertex_positions;
	vector<vec3> normals2vertices, normalsToFacesGeneralForm, normalsToVerticesGeneralForm;
	vector<pair<vec3, vec3>> normalsToFaces;
	vector<pair<vec3, vec3>> normalsToVertices;
	vector<vec2> m_Textures;
	vector<FaceIdcs> m_Faces;
	vec4 massCenter;
	float normalVectorsSize = 0.01;
	//add more attributes
	mat4 model_to_world_transform; //the model transformation (Tm)
	mat4 _world_transform; //What the heck (Tw)
	mat4 _normal_transform;
	GLfloat minX, minY, minZ ,maxX , maxY, maxZ;
	vec4 cube[8];
	vec3 baseColor = vec3(256, 50, 50);

	GLuint m_TextureID = 0;
	GLuint m_TextureNormalMapID = 0;

	void computeNormalsPerFace();
	GLint program;
public:
	vec3 color = vec3(0, 70, 70);

	MeshModel(string fileName, Renderer* r);
	~MeshModel(void);
	void loadFile(string fileName);
	void draw(Renderer* renderer);
	//vector<vec4> GetModel();
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

	void loadTextureMap(string file, Renderer* renderer);
	void loadNormalMapTexture(string file, Renderer* renderer);
	void generateTextureCoords(int type);


};
