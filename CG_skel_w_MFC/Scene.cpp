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
	if (m_renderer && cameras[activeCamera])
		m_renderer->SetProjection(cameras[activeCamera]->normalizedProjection());
	for (vector<Model*>::iterator it = models.begin(); it != models.end(); it++){
		(*it)->draw(m_renderer);

		// vector<vec3> translatedModelVertices = translateOrigin(((MeshModel*)models[stupiDtMP])->projected_vecs);
		// m_renderer->DrawTriangles(&translatedModelVertices);
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


Camera::Camera(){
	int k = 0.5;
	left = bottom = zNear = -k;
	right = top = zFar = k;
}

mat4& Camera::normalizedProjection(){
	
	mat4 normalizationMatrix;
	normalizationMatrix[0][0] = 2.0 / (right - left);
	normalizationMatrix[1][1] = 2.0 / (top - bottom);
	normalizationMatrix[2][2] = -2.0 / (zNear - zFar);
	normalizationMatrix[0][1] = -(right + left) / (right - left);
	normalizationMatrix[0][2] = -(top + bottom) / (top - bottom);
	normalizationMatrix[0][3] = -(zNear + zFar) / (zFar - zNear);

	return normalizationMatrix * projection;
}


void Scene::zoomIn(){
	cameras[activeCamera]->zoomIn();
}
void Scene::zoomOut(){
	cameras[activeCamera]->zoomOut();
}


void Camera::zoomIn(){
	//


}

void Camera::zoomOut(){
	//
}


//void Frustum(T left, T right, T bottom, T top, T zNear, T zFar)
//{
//	T zDelta = (zFar - zNear);
//	T dir = (right - left);
//	T height = (top - bottom);
//	T zNear2 = 2 * zNear;
//
//	m[0][0] = 2.0f*zNear / dir;
//	m[0][1] = 0.0f;
//	m[0][2] = (right + left) / dir;
//	m[0][3] = 0.0f;
//	m[1][0] = 0.0f;
//	m[1][1] = zNear2 / height;
//	m[1][2] = (top + bottom) / height;
//	m[1][3] = 0.0f;
//	m[2][0] = 0.0f;
//	m[2][1] = 0.0f;
//	m[2][2] = -(zFar + zNear) / zDelta;
//	m[2][3] = -zNear2*zFar / zDelta;
//	m[3][0] = 0.0f;
//	m[3][1] = 0.0f;
//	m[3][2] = -1.0f;
//	m[3][3] = 0.0f;
//}
//
//void perspective(t fovy, t aspectratio, t znear, t zfar)
//{
//	t xmin, xmax, ymin, ymax;
//	ymax = znear* tan(fovy*math<t>::pi / 360.0);
//	ymin = -ymax;
//	xmin = ymin*aspectratio;
//	xmax = ymax*aspectratio;
//	frustum(xmin, xmax, ymin, ymax, znear, zfar);
//}