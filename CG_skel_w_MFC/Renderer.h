#pragma once
#include <vector>
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"

using namespace std;
class Renderer
{
	//////////////////////////////
	// openGL stuff. Don't touch.
	GLuint gScreenTex;
	GLuint gScreenVtc;
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	//////////////////////////////



	float *m_outBuffer; // 3*width*height
	float *m_zbuffer; // width*height
	
	mat4 projectionMatrix; //(P)
	mat4 world_to_camera; //  (Tc)
	mat4 object_to_world; //  (Tw*Tm)
	// the projection matrix for all the objects in the world - should be set by scene based on the camera
	//Our private Funcs
	void DrawLine(vec2, vec2);
	vec2 vec4toVec2(const vec4 v);
	void CreateLocalBuffer();




public:

	int m_width, m_height;
	void CreateBuffers(int width, int height); // initially private
	Renderer();
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	void DrawTriangles(const vector<vec4>* vertices, const vector<vec3>* normals=NULL);
	void SetCameraTransform(const mat4& world_to_camera);
	void SetProjection(const mat4& projection);
	void SetObjectMatrices(const mat4& oTransform, const mat3& nTransform); //only The Active Model - ask Itay about nTransform
	void SwapBuffers();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();
	
	
};
