#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
using namespace std;

class Model {
public:
	void virtual draw(Renderer* renderer) = 0;
protected:
	virtual ~Model() {}
	
};


class Light {

};

class Camera {
	mat4 world_to_camera;
	mat4 projection; // camera to screen


public:
	void setTransformation(const mat4& transform);
	void LookAt(const vec4& eye, const vec4& at, const vec4& up );
	void Ortho( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar ); //M*(ST) - see tutorial 4 - 13
	void Frustum( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar ); //See reference in Cpp
	mat4 Perspective( const float fovy, const float aspect,
		const float zNear, const float zFar);
	void zoomIn();
	void zoomOut();
};

class Scene {
	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *m_renderer;

public:
	Scene() {};
	Scene(Renderer *renderer) : m_renderer(renderer) {};
	void loadOBJModel(string fileName);
	void draw();
	void drawDemo();
	void  addCamera(Camera* camera); //CG_skel will create and add the camera
	void zoomIn();
	void zoomOut();
	Model*  getModel(int id); //returns the model
	vector<vec3> translateOrigin(vector<vec3>);
	int activeModel;
	int activeLight;
	int activeCamera;
};