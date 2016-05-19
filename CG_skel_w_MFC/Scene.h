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
	vec4 virtual getOrigin() = 0;
	void virtual drawAxis(Renderer* renderer)=0;
	void virtual drawBoundingBox(Renderer* renderer)=0;
	void virtual drawFaceNormals(Renderer* renderer)=0;
	void virtual drawVertexNormals(Renderer* renderer)=0;
protected:
	virtual ~Model() {}
};


class Camera {
	
	mat4 projection; // camera to screen
	mat4 ST;
	float perspectiveD = 0.1;
	
	//float left, right, bottom, top, zNear, zFar;
	vec4 cube[8];

public:
	vec4 position = vec4(0, 0, 0, 1);

	Camera();
	mat4 world_to_camera; //Tc
	mat4 normalizedProjection();
	
	void setTransformation(const mat4& transform);
	void LookAt(const vec4& eye, const vec4& at, const vec4& up = vec4(0,1,0,1));
	void Ortho(const float left = -1, const float right = 1,
		const float bottom = -1, const float top=1,
		const float zNear = -1, const float zFar = 1);
	void Frustum(const float left = -1, const float right = 1,
		const float bottom = -1, const float top = 1,
		const float zNear = -1, const float zFar = 1); 
	mat4 Perspective( const float fovy, const float aspect,
		const float zNear, const float zFar);
	void zoomIn();
	void zoomOut();
	void move(GLfloat dx, GLfloat dy);
	void rotate(GLfloat dx, GLfloat dy);
	void move(GLfloat dz);
	void rotate(GLfloat dz);
	void draw(Renderer* renderer);

	
};


class Scene {
	vector<Model*> models;
	vector<Light*> lights;
	vector<Camera*> cameras;
	Renderer *m_renderer;
	bool orthogonalView = true;
	bool modelIsFocused = false;
	bool shouldDrawNormalsPerFace = false;
	bool shouldDrawNormalsPerVertex = false;
	bool shouldDrawBoundingBox = false;
	bool m_FogEnabled = false;
	vec3 fogColor = vec3(256, 256, 256);
	void drawCoordinateSystem();


public:
	mat4 model_to_world; // Tw
	Scene() {
		Camera * camera = new Camera;
		cameras.push_back(camera);
		activeCamera = 0;
		activeModel = -1;
		activeLight = -1;
	};
	Scene(Renderer *renderer) : m_renderer(renderer) {
		Camera * camera = new Camera;
		cameras.push_back(camera);
		activeCamera = 0;
		activeModel = -1;
		activeLight = -1;
		m_renderer->SetLights(&lights);
	};

	int numModels();
	int numCameras();
	void loadOBJModel(string fileName);
	void addPrimModel();
	void addCamera();
	void addLight();

	void LookAt();
	void draw();
	void drawXY();
	

	void drawDemo();
	void addCamera(Camera* camera); //CG_skel will create and add the camera
	void zoomIn();
	void zoomOut();
	void setOrthogonalView(const float left, const float right, const float bottom,
		const float top, const float zNear, const float zFar);
	void setPerspectiveView(const float left, const float right, const float bottom,
		const float top, const float zNear, const float zFar);

	void moveWorld(GLfloat dx, GLfloat dy); //TODO: implement

	void moveCurrentModel(GLfloat dx, GLfloat dy);
	void rotateCurrentModel(GLfloat dx, GLfloat dy);
	void rotateCurrentModelWorld(GLfloat dx, GLfloat dy);

	void moveCamera(GLfloat dx, GLfloat dy);
	void rotateCurrentCamera(GLfloat dx, GLfloat dy);

	void setNormalsPerFaceOn();
	void setNormalsPerFaceOff();

	void setNormalsPerVertexOn();
	void setNormalsPerVertexOff();

	void setDrawBoundingBoxOn();
	void setDrawBoundingBoxOff();

	void moveCurrentModel(GLfloat dz);
	void rotateCurrentModel(GLfloat dz);
	void rotateCurrentModelWorld(GLfloat dz);

	void moveCamera(GLfloat dz);
	void rotateCurrentCamera(GLfloat dz);

	void EnableFog();
	void DisableFog();
	void setFogColor(GLfloat R, GLfloat G, GLfloat B);
	
	Model*  getModel(int id); //returns the model
	vector<vec3> translateOrigin(vector<vec3>);
	int activeModel;
	int activeLight;
	int activeCamera;
};