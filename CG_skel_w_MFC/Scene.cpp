#include "stdafx.h"
#include "Scene.h"
#include "MeshModel.h"
#include "PrimMeshModel.cpp"
#include "Our_Model.h"
#include <string>

using namespace std;


int Scene::numModels(){
	if (models.size() == 0)
		return -1;
	return models.size();
}

int Scene::numCameras(){
	return cameras.size();
}

void Scene::addLight(vec3* direction, LightType lightType){
	Light * light = new Light;
	lights.push_back(light);
	activeLight = lights.size() - 1;
	addLightToMenu();
	if (direction){
		light->location = *direction;
	}
	light->lightType = lightType;
}

void Scene::changeLight(vec3* direction){
	lights[activeLight]->location = *direction;
}

void Scene::loadOBJModel(string fileName)
{
	MeshModel *model = new MeshModel(fileName);
	models.push_back(model);
	activeModel = models.size() - 1;
	addMeshToMenu();
	draw();
	//cameras[activeCamera]->LookAt(vec4(1.0, 1.0, 1.0, 1.0), model->getOrigin());
}

void Scene::LookAt(){
	if (orthogonalView)
		return;
	cout << cameras[activeCamera]->world_to_camera << endl;
	//vec4 camera_origin = cameras[activeCamera]->position;

	vec4 camera_origin = vec4(0, 0, 0, 1);

	cameras[activeCamera]->LookAt(camera_origin, models[activeModel]->getOrigin());
}

void Scene::addCamera(){
	Camera * camera = new Camera;
	orthogonalView = true;
	cameras.push_back(camera);
	activeCamera = cameras.size() - 1;
	addCameraToMenu();
}

void Scene::drawXY(){
	float delta = 0.2;
	m_renderer->setColor(0,100,100);
	m_renderer->SetObjectMatrices(mat4(),mat4());
	for (float i = -1; i <= 1; i += delta){
		m_renderer->setColor(60 + int(55 * i), 50, 0);
		m_renderer->DrawLineBetween3Dvecs(vec4(i, -1.0, -1, 1.0), vec4(i, 1.0, -1, 1.0));
		m_renderer->setColor(0, 50, 60 + int(55*i));
		m_renderer->DrawLineBetween3Dvecs(vec4(-1.0, i, -1, 1.0), vec4(1.0, i, -1, 1.0));
	}
}

void Scene::addPrimModel(){
	PrimMeshModel* primModel= new PrimMeshModel();
	primModel->setSphere();
	models.push_back(primModel);
	activeModel = models.size() - 1;
	addMeshToMenu();
	draw();
}

void Scene::addOurModel(){
	Model* ourModel = new Our_Model();
	models.push_back(ourModel);
	activeModel = models.size() - 1;
	addMeshToMenu();
	draw();
}

void Scene::draw() {
	// 1. Send the renderer the current camera transform and the projection
	// 2. Tell all models to draw themselves
	//m_renderer->CreateBuffers(m_renderer->m_width, m_renderer->m_height);
	m_renderer->Invalidate();
	mat4 normalizedProjection = cameras[activeCamera]->normalizedProjection();
	if (m_renderer && cameras[activeCamera]){
		m_renderer->SetProjection(normalizedProjection);
		m_renderer->SetCameraTransform(cameras[activeCamera]->world_to_camera);
	}
	drawXY();
	m_renderer->setColor(200, 200, 200);
	for (vector<Model*>::iterator it = models.begin(); it != models.end(); it++){
		if (*it == models[activeModel]){
			m_renderer->setColor(256, 256, 256);
			(*it)->draw(m_renderer);
			if (shouldDrawNormalsPerFace){
				(*it)->drawFaceNormals(m_renderer);
			}
			if (shouldDrawNormalsPerVertex){
				(*it)->drawVertexNormals(m_renderer);
			}
			if (shouldDrawBoundingBox){
				m_renderer->setColor(80, 50, 230);
				(*it)->drawBoundingBox(m_renderer);
			}
			m_renderer->setColor(200, 200, 200);
			continue;
		}
		(*it)->draw(m_renderer);
	}
	if (activeModel != -1)
		models[activeModel]->drawAxis(m_renderer);

	m_renderer->SetObjectMatrices(mat4(), mat4());
	cameras[activeCamera]->draw(m_renderer);

	m_renderer->setColor(0, 256, 256);
	vec4 loc, loc2;
	for (Light* l : lights){
		if (l->lightType == LIGHT_PARALLEL)
			continue;
		//cameras[activeCamera]->world_to_camera.MultiplyVec(l->location, loc);
		loc = l->location;
		//loc2 = loc + vec4(1, 0, 0, 0);
		for (int i = 0; i < 14; ++i){
			loc2 = loc + (RotateX(i*180/7) * vec4(0, 0.1, 0, 0));
			m_renderer->DrawLineBetween3Dvecs(loc, loc2);
			loc2 = loc + RotateY(i*180/7) * vec4(0.1, 0, 0, 0);
			m_renderer->DrawLineBetween3Dvecs(loc, loc2);
		}
	}
	m_renderer->drawFillAndFog(m_FogEnabled ? fogColor : NULL);

	m_renderer->SwapBuffers();
}

void Scene::EnableFog() {
	m_FogEnabled = true;
}

void Scene::DisableFog() {
	m_FogEnabled = false;
}

void Scene::setFogColor(GLfloat R, GLfloat G, GLfloat B) {
	fogColor = vec3(R, G, B);
}

void Scene::EnableSSAA() {
	m_renderer->SetSSAAMultiplier(2);
}

void Scene::DisableSSAA() {
	m_renderer->SetSSAAMultiplier(1);
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
	if (activeModel != -1)
		models[activeModel]->setModelTransformation(Scale(0.9, 0.9, 0.9));
}

void Scene::currentModelGeneralScaling(vec3 _scale){
	if (activeModel != -1){
		models[activeModel]->setModelTransformation(Scale(_scale.x, _scale.y, _scale.z));
		models[activeModel]->setNormalTransformation(Scale(1/_scale.x, 1/_scale.y, 1/_scale.z));
	}

}

void Scene::setOrthogonalView(const float left, const float right, const float bottom, const float top, const float zNear, const float zFar){
	cameras[activeCamera]->Ortho(left, right, bottom, top, zNear, zFar);
	this->orthogonalView = true;
}

void Scene::setPerspectiveView(const float left, const float right, const float bottom,
	const float top, const float zNear, const float zFar){
	cameras[activeCamera]->Frustum(left, right, bottom, top, zNear, zFar);
	this->orthogonalView = false;
}

void Scene::moveCurrentModel(GLfloat dx, GLfloat dy){
	if (activeModel == -1)
		return;
	mat4 tmp = mat4(1, 1, 1, 0,
		1, 1, 1, 0,
		1, 1, 1, 0,
		0, 0, 0, 1
		);
	mat4 toCameraMat = (matrixCompMult(cameras[activeCamera]->world_to_camera, tmp));
	vec4 camCor = transpose(toCameraMat) * vec4(dx / (GLfloat)m_renderer->GetWidth(), dy / (GLfloat)m_renderer->GetHeight(), 0, 1);
	camCor /= camCor.w;
	models[activeModel]->setWorldTransformation(Translate(camCor.x, camCor.y, camCor.z));
}

void Scene::moveCurrentModel(GLfloat dz){
	if (activeModel == -1)
		return;
	mat4 tmp = mat4(1, 1, 1, 0,
		1, 1, 1, 0,
		1, 1, 1, 0,
		0, 0, 0, 1
		);
	mat4 toCameraMat = (matrixCompMult(cameras[activeCamera]->world_to_camera, tmp));
	vec4 camCor = transpose(toCameraMat) * vec4(0, 0, dz / (GLfloat)m_renderer->GetHeight(), 1);
	camCor /= camCor.w;
	models[activeModel]->setWorldTransformation(Translate(camCor.x, camCor.y, camCor.z));
}

void Scene::rotateCurrentModel(GLfloat dx, GLfloat dy){
	if (activeModel == -1)
		return;

	models[activeModel]->setModelTransformation(RotateY((dx * 180) / (GLfloat)m_renderer->GetWidth()));
	models[activeModel]->setNormalTransformation(RotateY((dx * 180) / (GLfloat)m_renderer->GetWidth()));
	models[activeModel]->setModelTransformation(RotateX((dy * 180) / (GLfloat)m_renderer->GetHeight()));
	models[activeModel]->setNormalTransformation(RotateX((dy * 180) / (GLfloat)m_renderer->GetHeight()));
}

void Scene::rotateCurrentModelWorld(GLfloat dx, GLfloat dy){
	if (activeModel == -1)
		return;
	models[activeModel]->setWorldTransformation(RotateY((dx * 180) / (GLfloat)m_renderer->GetWidth()));
	models[activeModel]->setNormalTransformation(RotateY((dx * 180) / (GLfloat)m_renderer->GetWidth()));
	models[activeModel]->setWorldTransformation(RotateX((dy * 180) / (GLfloat)m_renderer->GetHeight()));
	models[activeModel]->setNormalTransformation(RotateX((dy * 180) / (GLfloat)m_renderer->GetHeight()));
}

void Scene::rotateCurrentCamera(GLfloat dx, GLfloat dy){
	cameras[activeCamera]->rotate((dx * 180) / (GLfloat)m_renderer->GetWidth(), (dy * 180) / (GLfloat)m_renderer->GetHeight());
}

void Scene::moveCamera(GLfloat dx, GLfloat dy){
	cameras[activeCamera]->move(dx / (GLfloat)m_renderer->GetWidth(), dy / (GLfloat)m_renderer->GetHeight());
}

void Scene::rotateCurrentModel(GLfloat dz){
	if (activeModel == -1)
		return;
	models[activeModel]->setModelTransformation(RotateZ((dz * 180) / (GLfloat)m_renderer->GetWidth()));
	models[activeModel]->setNormalTransformation(RotateZ((dz * 180) / (GLfloat)m_renderer->GetWidth()));
}

void Scene::rotateCurrentModelWorld(GLfloat dz){
	if (activeModel == -1)
		return;
	models[activeModel]->setWorldTransformation(RotateZ((dz * 180) / (GLfloat)m_renderer->GetWidth()));
	models[activeModel]->setNormalTransformation(RotateZ((dz * 180) / (GLfloat)m_renderer->GetWidth()));
}

void Scene::moveCamera(GLfloat dz){
	cameras[activeCamera]->move(dz / (GLfloat)m_renderer->GetHeight());
}

void Scene::rotateCurrentCamera(GLfloat dz){
	cameras[activeCamera]->rotate(-dz / (GLfloat)m_renderer->GetHeight());
}

void Scene::setNormalsPerFaceOn(){
	shouldDrawNormalsPerFace = true;
}

void Scene::setNormalsPerFaceOff(){
	shouldDrawNormalsPerFace = false;
}

void Scene::setNormalsPerVertexOn(){
	shouldDrawNormalsPerVertex = true;
}

void Scene::setNormalsPerVertexOff(){
	shouldDrawNormalsPerVertex = false;
}

void Scene::setDrawBoundingBoxOn(){
	shouldDrawBoundingBox = true;
}

void Scene::setDrawBoundingBoxOff(){
	shouldDrawBoundingBox = false;
}

void Scene::setCurrentMeshColor(float R, float G, float B){
	models[activeModel]->setModelColor(R, G, B);
}
void Scene::setCurrentLightColor(float R, float G, float B){
	lights[activeLight]->setLightColor(R, G, B);
}



void Scene::cameraZoomIn(){
	cameras[activeCamera]->zoomIn();
}
void Scene::cameraZoomOut(){
	cameras[activeCamera]->zoomOut();
}


//------------------------------Camera -----------------------------------------------------


Camera::Camera(){
	//float k = 1;
	//left = bottom = zNear = -k;
	//right = top = zFar = k;
	Ortho();
}

mat4 Camera::normalizedProjection(){
	//mat4* tmp = new mat4();
	//tmp = &(ST * projection);
	//return *tmp;
	return  projection * ST;
}

void Camera::zoomOut(){
	//left = bottom = bottom - 0.1;
	//right = top = top - 0.1;
	// zNear -= 0.1;
	// zFar -= 0.1;
	//projection = Scale(1.1, 1.1, 1.1) * projection;
	_left -= 0.1;
	_right += 0.1;
	_bottom -= 0.1;
	_top += 0.1;
	_zNear -= 0.1;
	_zFar += 0.1;

	if (!state)
		Ortho(_left, _right, _bottom, _top, _zNear, _zFar);
	else
		Frustum(_left, _right, _bottom, _top, _zNear, _zFar);
}

void Camera::zoomIn(){
	//left = bottom = bottom + 0.1;
	//right = top = top + 0.1;
	//zNear += 0.1;
	//zFar += 0.1;
	//projection = Scale(0.9, 0.9, 0.9) * projection;
	_left += 0.1;
	_right -= 0.1;
	_bottom += 0.1;
	_top -= 0.1;
	_zNear += 0.1;
	_zFar -= 0.1;

	if (!state)
		Ortho(_left, _right, _bottom, _top, _zNear, _zFar);
	else
		Frustum(_left, _right, _bottom, _top, _zNear, _zFar);
}

void Camera::move(GLfloat dx, GLfloat dy){
	world_to_camera = Translate(-dx, -dy, 0) * world_to_camera;
	position += vec4(dx, dy, 0, 0);
}

void Camera::rotate(GLfloat dx, GLfloat dy){
	world_to_camera = RotateY(-dx) * world_to_camera;
	world_to_camera = RotateX(dy) * world_to_camera;
}

void Camera::move(GLfloat dz){
	world_to_camera = Translate(0, 0, dz) * world_to_camera;
	position -= vec4(0, 0, dz, 0);
}

void Camera::rotate(GLfloat dz){
	world_to_camera = Translate(0, 0, dz) * world_to_camera;
	position -= vec4(0, 0, dz, 0);
}

void Camera::Ortho(const float left, const float right,const float bottom , 
	const float top, const float zNear , const float zFar){

	_left = left;
	_right = right;
	_bottom = bottom;
	_top = top;
	_zNear = zNear;
	_zFar = zFar;
	state = 0;

	cube[0] = vec3(left, bottom, zNear);
	cube[1] = vec3(left, bottom, zFar);
	cube[2] = vec3(left, top, zNear);
	cube[3] = vec3(left, top, zFar);
	cube[4] = vec3(right, bottom, zNear);
	cube[5] = vec3(right, bottom, zFar);
	cube[6] = vec3(right, top, zNear);
	cube[7] = vec3(right, top, zFar);

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

void Camera::draw(Renderer* renderer){
	renderer->setColor(256, 256, 256);

	renderer->DrawLineBetween3Dvecs(cube[0], cube[1]);
	renderer->DrawLineBetween3Dvecs(cube[2], cube[3]);
	renderer->DrawLineBetween3Dvecs(cube[4], cube[5]);
	renderer->DrawLineBetween3Dvecs(cube[6], cube[7]);

	renderer->DrawLineBetween3Dvecs(cube[0], cube[2]);
	renderer->DrawLineBetween3Dvecs(cube[1], cube[3]);
	renderer->DrawLineBetween3Dvecs(cube[4], cube[6]);
	renderer->DrawLineBetween3Dvecs(cube[5], cube[7]);

	renderer->DrawLineBetween3Dvecs(cube[0], cube[4]);
	renderer->DrawLineBetween3Dvecs(cube[1], cube[5]);
	renderer->DrawLineBetween3Dvecs(cube[2], cube[6]);
	renderer->DrawLineBetween3Dvecs(cube[3], cube[7]);
}

void Camera::Frustum(const float left, const float right,const float bottom,
	const float top, const float zNear, const float zFar){

	_left = left;
	_right = right;
	_bottom = bottom;
	_top = top;
	_zNear = zNear;
	_zFar = zFar;
	state = 1;

	const float zdelta = (zFar - zNear);
	const float dir = (right - left);
	const float height = (top - bottom);
	const float znear2 = 2 * zNear;


	ST = mat4();
	ST[0][0] = -zNear;
	ST[1][1] = -zNear;
	ST[2][2] = -(zNear+zFar)/(zNear-zFar);
	ST[2][3] = -(2 * zNear * zFar) / (zNear - zFar);
	ST[3][2] = 1;
	ST[3][3] = 0;


	//Set projecion Matrix
	projection = mat4();
	/*projection[3][3] = 0;
	projection[3][2] = 1.0/perspectiveD;*/

	cube[0] = vec3(left, bottom, zNear);
	cube[1] = vec3(left, bottom, zFar);
	cube[2] = vec3(left, top, zNear);
	cube[3] = vec3(left, top, zFar);
	cube[4] = vec3(right, bottom, zNear);
	cube[5] = vec3(right, bottom, zFar);
	cube[6] = vec3(right, top, zNear);
	cube[7] = vec3(right, top, zFar);
}

void Camera::LookAt(const vec4& eye, const vec4& at, const vec4& up){
	vec4 n;
	if (length(eye-at)<0.2)
		n = normalize(eye + vec4(0.1,0.1,0.1,0) - at);
	else 
		n = normalize(eye - at);
	//vec3 tmp1 = normalize(cross(up, n));
	
	vec4 u = vec4(normalize(cross(up, n)),0);

	//vec3 tmp2 = normalize(cross(n, u));
	vec4 v = vec4(normalize(cross(n, u)), 0);
	vec4 t = vec4(0.0, 0.0, 0.0, 1.0);
	mat4 c = transpose(mat4(u, v, n, t));
	world_to_camera = /*Translate(-eye) * */ c * Translate(-eye);
	cout << eye << "   at: " << at << endl;
	cout << c << endl;
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

