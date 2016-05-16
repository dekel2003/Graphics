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
	int m_width;
	int m_height;
	int m_TotalNumberOfPixels;

	mat4 projectionMatrix; //(P)
	mat4 world_to_camera; //  (Tc)
	mat4 object_to_world; //  (Tw*Tm)
	// the projection matrix for all the objects in the world - should be set by scene based on the camera
	//Our private Funcs
	void DrawLine(vec2, vec2);
	vec2 vec4toVec2(const vec4 v);
	void CreateLocalBuffer();
	float R, G, B;
	//int GetRange(double y1, double y2, int& maxY);
	//Func<int, double> CreateFunc(vec2 pt1, vec2 pt2);
	float sign(vec2 p1, vec4 p2, vec4 p3);
	bool PointInTriangle(vec2 pt, vec4 v1, vec4 v2, vec4 v3);
	GLfloat getZ(vec2 p3, vec2 p2, vec2 p1, vec2 ps, vec4 z3, vec4 z2, vec4 z1);

public:
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
	void DrawLineBetween3Dvecs(const vec4& a, const vec4& b);
	void setColor(float red, float green, float blue); // 0..255
	void Invalidate();
	int GetWidth();
	int GetHeight();
	//vector<vec2> Renderer::PointsInTriangle(vec2 pt1, vec2 pt2, vec2 pt3);
};
