#pragma once
#include <vector>
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"

using namespace std;

class Polygon3{
public:
	vec4 a, b, c;
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
	inline void getXrange(float currY, int& xStart, int& xEnd) const{
		/*const vec3 *minP, *maxP, *midP;
		if (a.y == minY())
			minP = &a;
		if (b.y == minY())
			minP = &b;
		if (c.y == minY())
			minP = &c;

		if (a.y == maxY())
			maxP = &a;
		if (b.y == maxY())
			maxP = &b;
		if (c.y == maxY())
			maxP = &c;

		if (a != *minP && a != *maxP)
			midP = &a;
		if (b != *minP && b != *maxP)
			midP = &b;
		if (c != *minP && c != *maxP)
			midP = &c;

		if (currY < midP->y){
			float t1 = abs(currY - minP->y) / abs(maxP->y - minP->y);
			float t2 = abs(currY - minP->y) / abs(midP->y - minP->y);

			if (t1>1 || t2>1)
				return;

			int x1 = round(abs(maxP->x - minP->x) * t1 + minP->x);
			int x2 = round(abs(midP->x - minP->x) * t2 + minP->x);

			xStart = min(x1, x2);
			xEnd = max(x1, x2);
		}
		else{
			float t1 = abs(currY - maxP->y) / abs(minP->y - maxP->y);
			float t2 = abs(currY - maxP->y) / abs(midP->y - maxP->y);

			if (t1>1 || t2>1)
				return;

			int x1 = round(abs(minP->x - maxP->x) * t1 + maxP->x);
			int x2 = round(abs(midP->x - maxP->x) * t2 + maxP->x);

			xStart = min(x1, x2);
			xEnd = max(x1, x2);
		}*/
		float A, B, C;
		A = b.y - a.y;
		B = a.x - b.x;
		C = b.x * a.y - a.x * b.y;
	}

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

public:
	void ScanLineZBuffer();

	void CreateBuffers(int width, int height); // initially private
	Renderer();
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	void DrawTriangles(const vector<vec4>* vertices, const vector<vec3>* normals=NULL);
	void AddTriangles(const vector<vec4>* vertices, const vector<vec3>* normals = NULL);
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



	void testPointInTriangle(int x, int y);
};
