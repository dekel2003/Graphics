#pragma once
#include <vector>
#include "stdafx.h"
#include "CG_skel_w_MFC.h"
#include "vec.h"
#include "mat.h"
#include "GL/glew.h"
#include "Light.h"
#include "SOIL.h"

using namespace std;


inline vec3 vec4TOvec3(vec4& v){
	/*if (v.w != 0){
		v /= v.w;
	}*/
	return vec3(v.x, v.y, v.z);
}

enum Material {
	EMISSIVE,
	DIFFUSE,
	SPECULAR,
	ALL
};//$(VC_IncludePath);$(WindowsSDK_IncludePath);

class Polygon3{

vec3 eye = vec3(0.5, 0.5, 0), n, r, e;
float teta;
Material material = ALL;
public:
	vec3 l;
	vec4 a, b, c; //world coords
	vec4 pa, pb, pc; //screen coords [-1,1]X[-1,1]
	vec4 ma, mb, mc; //monitor coords
	vec4 na, nb, nc; //normals to vertices
	vec3 aColor, bColor, cColor; // colors
	vec3 baseColor, facecolor, tmpColor;
	bool faceColorWasAlreadyCalculated = false;
	bool verticesColorsWasAlreadyCalculated = false;
	vec4 temVec;
	vec3 tmpNormal;
	mat4 projection;
	int m_width;
	int m_height;
	vec4* normalsToVertices;
	//vec3 normal;
	Polygon3(){}
	void setMaterial(Material _m){ material = _m; }
	Polygon3(vec4 _a, vec4 _b, vec4 _c, vec3 color, vec4 _normal, mat4& projection,
		int m_width, int m_height
		,vec4* normalsToVertices = NULL) 
		:a(_a), b(_b), c(_c), baseColor(color), facecolor(color), projection(projection), m_width(m_width), m_height(m_height), normalsToVertices(normalsToVertices) {
		projection.MultiplyVec(a, pa);
		projection.MultiplyVec(b, pb);
		projection.MultiplyVec(c, pc);
		pa /= pa.w;
		pb /= pb.w;
		pc /= pc.w;

		ma.x = m_width*(pa.x + 1) / 2;
		ma.y = m_height*(pa.y + 1) / 2;
		mb.x = m_width*(pb.x + 1) / 2;
		mb.y = m_height*(pb.y + 1) / 2;
		mc.x = m_width*(pc.x + 1) / 2;
		mc.y = m_height*(pc.y + 1) / 2;

		n = normalize(vec4TOvec3(_normal)); /// NEEDED LINE ?

		if (normalsToVertices){
			na = normalize(normalsToVertices[0]);
			nb = normalize(normalsToVertices[1]);
			nc = normalize(normalsToVertices[2]);
		}
	}
	inline float minY() const{
		return min(ma.y, min(mb.y, mc.y));
	}
	inline float maxY() const{
		return max(ma.y, max(mb.y, mc.y));
	}
	inline float minX() const{
		return min(ma.x, min(mb.x, mc.x));
	}
	inline float maxX() const{
		return max(ma.x, max(mb.x, mc.x));
	}
	inline static bool Ysorting(Polygon3& s, Polygon3& t){
		return s.minY() < t.minY();
	}
	inline static bool Zsorting(Polygon3& s, Polygon3& t){
		return min(s.a.z, min(s.b.z, s.c.z)) < min(t.a.z, min(t.b.z, t.c.z));
	}
	bool operator< (const Polygon3& p) const {
		return minY() < p.minY();
	}

	vec3& calculateColor(vector<Light*>* lights, mat4& world_to_camera, float& AmbientIntensity, vec3& location, vec3& normal);
	vec3& calculateFaceColor(vector<Light*>* lights, mat4& world_to_camera, float& AmbientIntensity);
	void calculateVertexColors(vector<Light*>* lights, mat4& world_to_camera, float& AmbientIntensity);

	vec3& getNormalOfBaricentricLocation(float& a1, float& a2, float& a3); //tmpNormal
};


enum Shadow { FLAT, GOUARD, PHONG, TOON };

class Renderer
{
	//////////////////////////////
	// openGL stuff. Don't touch.
	GLuint gScreenTex;
	GLuint gScreenVtc;
	void CreateOpenGLBuffer();
	void InitOpenGLRendering();
	//////////////////////////////

	GLuint VAO;
	GLuint VAOLines, VAOModelLines;



	vector<vec4> lines;
	vector<vec4> model_lines;

	GLuint m_CurrentTexture;
	GLuint m_CurrentNormalTextureTexture;
	GLuint m_CurrentSkyboxTexture;

	float* m_SSAAOutBuffer;
	float *m_outBuffer; // 3*width*height
	float *m_zbuffer; // width*height
	int m_OutBufferWidth;
	int m_OutBufferHeight;
	int m_SSAAOutBufferWidth;
	int m_SSAAOutBufferHeight;
	int m_TotalNumberOfOutBufferPixels;
	int m_SSAAMultiplier = 1;

	float clippingXMin = -1.0f;
	float clippingYMin = -1.0f;
	float clippingXMax = 1.0f;
	float clippingYMax = 1.0f;

	vector<Polygon3> globalClippedPolygon3;
	vector<Light*>* lights = NULL;

	//vec2 pixleToScreen = vec2(1.0f / m_width, 1.0f / m_height);

	mat4 projectionMatrix; //(P)
	mat4 world_to_camera; //  (Tc)
	mat4 object_to_world; //  (Tw*Tm)
	mat4 objectToCamera; // = world_to_camera * object_to_world; (Tc*Tw*Tm)

	mat4 normalTransform;
	float AmbientIntensity = 1.0f;
	vec3 drawingColor;

	bool m_UseTexture = false;
	bool m_UseNormalMapping = false;
	bool m_UseEnvironmentMapping = false;
	int transformIdUseTexture;
	int transformIdUseNormalMapping;
	int transformIdUseEnvironmentMapping;

	//GLuint cubeVAO, cubeVBO;
	GLuint skyboxVAO, skyboxVBO;
	GLuint m_TextureCubemapID = 0;

	Renderer();
	// the projection matrix for all the objects in the world - should be set by scene based on the camera
	//Our private Funcs
	void DrawLine(vec2, vec2, float za=1000, float zb=1000);
	bool clip2D(float x1, float y1, float x2, float y2, float& outx1, float& outy1, float& outx2, float& outy2); // DELETE THIS
	bool clip3D(float x1, float y1, float z1, float x2, float y2, float z2,	float& outx1, float& outy1, float& outz1, float& outx2, float& outy2, float& outz2);
	vec2 vec4toVec2(const vec4 v);
	void CreateLocalBuffer();
	GLfloat R, G, B;
	//int GetRange(double y1, double y2, int& maxY);
	//Func<int, double> CreateFunc(vec2 pt1, vec2 pt2);
	inline float sign(vec2& p1, vec3& p2, vec3& p3) const;
	//inline bool PointInTriangle(const vec2& pt, const  vec4 a, const  vec4 b, const  vec4 c) const;
	void PointInTriangle(vec2& pt, Polygon3* P);
	//bool PointInTriangle(const vec2& pt, Polygon3& P);
	GLfloat getZ(vec2 p3, vec2 p2, vec2 p1, vec2 ps, vec4 z3, vec4 z2, vec4 z1);
	inline float findSSAAOfColorElement(int multiplier, int x, int y, int colorElement);
	void putColor(int x, int y, Polygon3* P);
	
public:
	void EnableTexture();
	void DisableTexture();
	void EnableNormalMapping();
	void DisableNormalMapping();
	void EnableEnvironmentMapping();
	void DisableEnvironmentMapping();
	void LoadCubemap(vector<const GLchar*> faces);
	void CreateSkyBoxObject();

	void loadTexture(GLuint&, const char* fileName, int myGL_TEXTURE);
	void setAmbientLight(float intensity);
	void drawFillAndFog(vec3& fog);

	void CreateBuffers(); // initially private
	Renderer(int width, int height);
	~Renderer(void);
	void Init();
	void SetRendererSize(int width, int height);
	void SetSSAAMultiplier(int multiplier);
	//void DrawTriangles(const vector<vec4>* vertices, const vector<vec3>* normals=NULL);
	GLuint AddTriangles(const vector<vec4>* vertices, const vec3 color,
		const vector<vec3>* normals = NULL, const vector<vec3>* normals2vertices = NULL,
		const vector<vec2>* textures = NULL, Material m = ALL);

	void SetCameraTransform(const mat4& world_to_camera);
	void SetProjection(const mat4& projection);
	void SetLights(vector<Light*>* lights);
	void SetObjectMatrices(const mat4& oTransform, const mat4& nTransform); //only The Active Model - ask Itay about nTransform
	void SwapBuffers();
	void drawModelsLines();
	void draw();
	void ClearColorBuffer();
	void ClearDepthBuffer();
	void SetDemoBuffer();
	void DrawLineBetween3Dvecs(const vec4& a, const vec4& b, bool modelOriented = false);
	void setColor(float red, float green, float blue); // 0..255
	void Invalidate();
	int GetWidth();
	int GetHeight();
	//vector<vec2> Renderer::PointsInTriangle(vec2 pt1, vec2 pt2, vec2 pt3);

	void testPointInTriangle(int x, int y);
	void SetCurrentTexture(GLuint texture);
	void SetCurrentNormalMappingTexture(GLuint texture);

	Shadow shadow = FLAT;
	GLuint totalNumberOfVertices = 0;
	GLuint program;
	GLuint skyBoxProgram;
};
