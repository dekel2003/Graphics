#pragma once
#include <vector>
#include "stdafx.h"
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"
#include "Light.h"


using namespace std;

class Polygon3{
public:
	vec4 a, b, c;
	vec3 baseColor;
	vec3 normal;
	Polygon3(){}
	Polygon3(vec4 _a, vec4 _b, vec4 _c) :a(_a), b(_b), c(_c){
		/*a /= a.w;
		b /= b.w;
		c /= c.w;*/

		/*if (a.y > b.y)
		swap(a, b);
		if (a.y > c.y)
		swap(a, c);
		if (b.y > c.y)
		swap(b, c);*/
	}
	Polygon3(vec4 _a, vec4 _b, vec4 _c, vec3 color) :a(_a), b(_b), c(_c), baseColor(color){}
	Polygon3(vec4 _a, vec4 _b, vec4 _c, vec3 color, vec3 _normal) :a(_a), b(_b), c(_c), baseColor(color), normal(_normal){}
	inline float minY() const{
		return min(a.y, min(b.y, c.y));
	}
	inline float maxY() const{
		return max(a.y, max(b.y, c.y));
	}
	inline float minX() const{
		return min(a.x, min(b.x, c.x));
	}
	inline float maxX() const{
		return max(a.x, max(b.x, c.x));
	}
	inline static bool Ysorting(Polygon3 s, Polygon3 t){
		return s.minY() < t.minY();
	}
	/*inline void getXrange(float currY, int& xStart, int& xEnd) const{
	float A, B, C;
	A = b.y - a.y;
	B = a.x - b.x;
	C = b.x * a.y - a.x * b.y;
	}*/

	bool operator< (const Polygon3& p) const {
		return minY() < p.minY();
	}
};

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

	vector<Polygon3> globalClippedVertices;
	vector<Light*>* lights = NULL;

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
	inline float sign(vec2& p1, vec3& p2, vec3& p3) const;
	//inline bool PointInTriangle(const vec2& pt, const  vec4 a, const  vec4 b, const  vec4 c) const;
	void PointInTriangle(vec2& pt, Polygon3* P);
	//bool PointInTriangle(const vec2& pt, Polygon3& P);
	GLfloat getZ(vec2 p3, vec2 p2, vec2 p1, vec2 ps, vec4 z3, vec4 z2, vec4 z1);
	void putColor(int x, int y, Polygon3* P);


	float AmbientIntensity = 1.0f;

public:
	void setAmbientLight(float intensity);
	void drawZBuffer();

	void CreateBuffers(int width, int height); // initially private
	Renderer();
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	//void DrawTriangles(const vector<vec4>* vertices, const vector<vec3>* normals=NULL);
	void AddTriangles(const vector<vec4>* vertices, const vec3 color, const vector<vec3>* normals = NULL);

	void SetCameraTransform(const mat4& world_to_camera);
	void SetProjection(const mat4& projection);
	void SetLights(vector<Light*>* lights);
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



	void testPointInTriangle(int x, int y);
};
