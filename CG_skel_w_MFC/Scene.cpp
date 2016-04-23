#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include <string>

using namespace std;


void Scene::loadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName);
	models.push_back(model);
	activeModel = models.size() - 1;
}


void Scene::draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	int stupiDtMP = 0;
	m_renderer->CreateBuffers(m_renderer->m_width, m_renderer->m_height);
	if (m_renderer && cameras[activeCamera]){
		m_renderer->SetProjection(cameras[activeCamera]->normalizedProjection());
		m_renderer->SetCameraTransform(cameras[activeCamera]->world_to_camera);
	}
	for (vector<Model*>::iterator it = models.begin(); it != models.end(); it++){
		(*it)->draw(m_renderer);
	}
	m_renderer->SwapBuffers();
}

void Scene::drawDemo()
{
	m_renderer->SetDemoBuffer();
	m_renderer->SwapBuffers();
}

vector<vec3> Scene::translateOrigin(vector<vec3> vertices){ // currently not in use.
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
	float k = 1;
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
	// cameras[activeCamera]->zoomIn();
	models[activeModel]->setModelTransformation(Scale(1.1, 1.1, 1.1));
}
void Scene::zoomOut(){
	// cameras[activeCamera]->zoomOut();
	models[activeModel]->setModelTransformation(Scale(0.9, 0.9, 0.9));
}

void Scene::moveCurrentModel(GLfloat dx, GLfloat dy){
	//cameras[activeCamera]->move(dx/512.0, dy/512.0);
	models[activeModel]->setWorldTransformation(Translate(dx / (GLfloat)m_renderer->m_width, dy / (GLfloat)m_renderer->m_height, 0));

}

void Camera::zoomIn(){
	//
	//left = bottom = bottom - 0.1;
	//right = top = top - 0.1;
	// zNear -= 0.1;
	// zFar -= 0.1;
	//projection = Scale(1.1, 1.1, 1.1) * projection;
}

void Camera::zoomOut(){
	//left = bottom = bottom + 0.1;
	//right = top = top + 0.1;
	//zNear += 0.1;
	//zFar += 0.1;
	//projection = Scale(0.9, 0.9, 0.9) * projection;
}

void Camera::move(GLfloat dx, GLfloat dy){
	world_to_camera = Translate(dx, dy, 0) * world_to_camera;

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