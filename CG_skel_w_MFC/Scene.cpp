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
	addMeshToMenu();
	//cameras[activeCamera]->LookAt(vec4(1.0, 1.0, 1.0, 1.0), model->getOrigin());
}

void Scene::drawXY(){
	float delta = 0.25;
	m_renderer->setColor(0,100,100);
	//m_renderer->SetProjection(cameras[activeCamera]->normalizedProjection());
	//m_renderer->SetCameraTransform(cameras[activeCamera]->world_to_camera);
	m_renderer->SetObjectMatrices(mat4(),mat3());
	for (float i = -1; i <= 1; i += delta){
		m_renderer->DrawLineBetween3Dvecs(vec4(i, -1.0, 0.0, 1.0), vec4(i, 1.0, 0.0, 1.0));
		m_renderer->DrawLineBetween3Dvecs(vec4(-1.0, i, 0.0, 1.0), vec4(1.0, i, 0.0, 1.0));
	}
}

void Scene::draw()
{
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	//m_renderer->CreateBuffers(m_renderer->m_width, m_renderer->m_height);
	m_renderer->Invalidate();
	
	if (m_renderer && cameras[activeCamera]){
		// Draw coordinates System - not working for now
		/*m_renderer->SetProjection(cameras[activeCamera]->normalizedProjection());
		m_renderer->SetCameraTransform(cameras[activeCamera]->world_to_camera);
		m_renderer->DrawLineBetween3Dvecs(vec4(0, 0, 0, 1), vec4(500, 0, 0, 1));
		m_renderer->DrawLineBetween3Dvecs(vec4(0, 0, 0, 1), vec4(0, 500, 0, 1));
		m_renderer->DrawLineBetween3Dvecs(vec4(0, 0, 0, 1), vec4(0, 0, 500, 1));*/

		m_renderer->SetProjection(cameras[activeCamera]->normalizedProjection());
		m_renderer->SetCameraTransform(cameras[activeCamera]->world_to_camera);
	}
	drawXY();
	m_renderer->setColor(200, 200, 200);
	for (vector<Model*>::iterator it = models.begin(); it != models.end(); it++){
		if (*it == models[activeModel]){
			m_renderer->setColor(256, 256, 256);
			(*it)->draw(m_renderer);
			m_renderer->setColor(200, 200, 200);
			continue;
		}
		(*it)->draw(m_renderer);
	}
	if (activeModel!=-1)
		models[activeModel]->drawAxis(m_renderer);
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

void Scene::zoomIn(){

	if (activeModel != -1)
		models[activeModel]->setModelTransformation(Scale(1.1, 1.1, 1.1));
}

void Scene::zoomOut(){
	// cameras[activeCamera]->zoomOut();.
	if (activeModel != -1)
		models[activeModel]->setModelTransformation(Scale(0.9, 0.9, 0.9));
}

void Scene::setOrthogonalView(const float left, const float right, const float bottom,
	const float top, const float zNear, const float zFar){
	cameras[activeCamera]->Ortho(left, right, bottom, top, zNear, zFar);
	this->orthogonalView = true;
}

void Scene::setPerspectiveView(const float left, const float right, const float bottom,
	const float top, const float zNear, const float zFar){
	cameras[activeCamera]->Frustum(left, right, bottom, top, zNear, zFar);
	this->orthogonalView = false;
}

void Scene::moveCurrentModel(GLfloat dx, GLfloat dy){
	//cameras[activeCamera]->move(dx/512.0, dy/512.0);
	if (activeModel != -1)
		models[activeModel]->setWorldTransformation(Translate(dx / (GLfloat)m_renderer->m_width, dy / (GLfloat)m_renderer->m_height, 0));

}

void Scene::rotateCurrentModel(GLfloat dx, GLfloat dy){
	models[activeModel]->setModelTransformation(RotateY((dx*180) / (GLfloat)m_renderer->m_width));
	models[activeModel]->setModelTransformation(RotateX((dy*180) / (GLfloat)m_renderer->m_width));
}

void Scene::rotateCurrentModelWorld(GLfloat dx, GLfloat dy){
	models[activeModel]->setWorldTransformation(RotateY((dx * 180) / (GLfloat)m_renderer->m_width));
	models[activeModel]->setWorldTransformation(RotateX((dy * 180) / (GLfloat)m_renderer->m_width));
}

void Scene::rotateCurrentCamera(GLfloat dx, GLfloat dy){
	cameras[activeCamera]->rotate((dx * 180) / (GLfloat)m_renderer->m_width, (dy * 180) / (GLfloat)m_renderer->m_width);
}

void Scene::moveCamera(GLfloat dx, GLfloat dy){
	cameras[activeCamera]->move(dx, dy);
}

void Scene::drawCoordinateSystem(){
	

}

//------------------------------Camera -----------------------------------------------------


Camera::Camera(){
	float k = 1;
	left = bottom = zNear = -k;
	right = top = zFar = k;
}

mat4& Camera::normalizedProjection(){
	mat4* tmp = new mat4();
	tmp = &(ST * projection);
	return *tmp;
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
	world_to_camera = Translate(dx / 512, dy / 512, 0) * world_to_camera;
}

void Camera::rotate(GLfloat dx, GLfloat dy){
	world_to_camera = world_to_camera * (RotateY(dx));
	world_to_camera = world_to_camera * (RotateX(dy));
}

void Camera::Ortho(const float left, const float right,const float bottom , 
	const float top, const float zNear , const float zFar){
	mat4 normalized;
	normalized[0][0] = 2.0 / (right - left);
	normalized[1][1] = 2.0 / (top - bottom);
	normalized[2][2] = 2.0 / (zNear - zFar);
	normalized[0][3] = -(right + left) / (right - left);
	normalized[1][3] = -(top + bottom) / (top - bottom);
	normalized[2][3] = -(zNear + zFar) / (zFar - zNear);
	//Because it makes something bad with floats
	if (left == -right){
		normalized[0][3] = 0;
	}
	if (top == -bottom){
		normalized[1][3] = 0;
	}
	if (zNear == -zFar){
		normalized[2][3] = 0;
	}
	ST = normalized;

	//Set projecion Matrix
	projection = mat4();
	projection[2][2] = 0; // projection Matrix
}

void Camera::Frustum(const float left, const float right,const float bottom,
	const float top, const float zNear, const float zFar){

	const float zdelta = (zFar - zNear);
	const float dir = (right - left);
	const float height = (top - bottom);
	const float znear2 = 2 * zNear;
	mat4 normalized;
	normalized[0][0] = 2.0f*zNear / dir;
	normalized[0][1] = 0.0f;
	normalized[0][2] = (right + left) / dir;
	normalized[0][3] = 0.0f;
	normalized[1][0] = 0.0f;
	normalized[1][1] = znear2 / height;
	normalized[1][2] = (top + bottom) / height;
	normalized[1][3] = 0.0f;
	normalized[2][0] = 0.0f;
	normalized[2][1] = 0.0f;
	normalized[2][2] = -(zFar + zNear) / zdelta;
	normalized[2][3] = -znear2*zFar / zdelta;
	normalized[3][0] = 0.0f;
	normalized[3][1] = 0.0f;
	normalized[3][2] = -1.0f;
	normalized[3][3] = 0.0f;
	ST = normalized;

	//Set projecion Matrix
	projection = mat4();
	projection[3][3] = 0;
	projection[3][2] = projection[2][2]/perspectiveD;
}

void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up){
	vec4 n = normalize(eye - at);
	vec4 u = normalize(cross(up, n));
	vec4 v = normalize(cross(n, u));
	vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
	mat4 c = transpose(mat4(u, v, n, t));
	world_to_camera = c * Translate(-eye);
}

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