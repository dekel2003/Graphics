#pragma once

#include "gl/glew.h"
#include <vector>
#include <string>
#include "Renderer.h"
using namespace std;

class Model {
public:
	void virtual draw(Renderer* renderer) = 0;
	void virtual setModelTransformation(const mat4& T) = 0;
	void virtual setWorldTransformation(const mat4& T) = 0;
protected:
	virtual ~Model() {}
	
};


class Light {

};

class Camera {
	
	mat4 projection; // camera to screen

	float left, right, bottom, top, zNear, zFar;


public:
	Camera();
	mat4 world_to_camera; //Tc
	mat4& normalizedProjection();
	mat4 ST;

	void setTransformation(const mat4& transform);
	void LookAt(const vec4& eye, const vec4& at, const vec4& up );
	void Ortho( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar );//TODO: calculate ST, and put in ST //M*(ST) - see tutorial 4 - 13
	void Frustum( const float left, const float right,
		const float bottom, const float top,
		const float zNear, const float zFar ); //See reference in Cpp
	mat4 Perspective( const float fovy, const float aspect,
		const float zNear, const float zFar);
	void zoomIn();
	void zoomOut();
	void move(GLfloat dx, GLfloat dy);
};

class Scene {
	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *m_renderer;

public:
	mat4 model_to_world; // Tw
	Scene() {
		Camera * camera = new Camera;
		cameras.push_back(camera);
		activeCamera = 0;
		activeModel = 0;
	};
	Scene(Renderer *renderer) : m_renderer(renderer) {
		Camera * camera = new Camera;
		cameras.push_back(camera);
		activeCamera = 0;
		activeModel = 0;
	};
	void loadOBJModel(string fileName);
	void draw();
	void drawDemo();
	void addCamera(Camera* camera); //CG_skel will create and add the camera
	void zoomIn();
	void zoomOut();

	void moveWorld(GLfloat dx, GLfloat dy); //TODO: implement
	void moveCurrentModel(GLfloat dx, GLfloat dy); //TODO: implement Better - Make sure that It is called from a right place in CG

	Model*  getModel(int id); //returns the model
	vector<vec3> translateOrigin(vector<vec3>);
	int activeModel;
	int activeLight;
	int activeCamera;
};