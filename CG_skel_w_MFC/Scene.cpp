#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>

using namespace std;


void Scene::loadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName);
	models.push_back(model);
}

void Scene::draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	int stupiDtMP = 0;
	for (vector<Model*>::iterator it = models.begin(); it != models.end(); it++){
		((MeshModel*)models[stupiDtMP])->draw();
		vector<vec3> translatedModelVertices = translateOrigin(((MeshModel*)models[stupiDtMP])->projected_vecs);
		m_renderer->DrawTriangles(&translatedModelVertices);
		//TODO: make it less stupid
	}
	m_renderer->SwapBuffers();
}

void Scene::drawDemo()
{
	m_renderer->SetDemoBuffer();
	m_renderer->SwapBuffers();
}


vector<vec3> Scene::translateOrigin(vector<vec3> vertices){
	vector<vec3> translatedVertices;
	for (vector<vec3>::iterator it = vertices.begin(); it != vertices.end(); ++it)
	{
		//TODO: change 256  to center origin - based on the current camera position
		vec3 tmp = (*it) + vec3(256,256);
		translatedVertices.push_back(tmp);
	}
	return translatedVertices;
}