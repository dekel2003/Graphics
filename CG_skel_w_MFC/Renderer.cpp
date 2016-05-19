#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"

#include <algorithm>
#include <set>
#include <list>

#define INDEX(width,x,y,c) (x+y*width)*3+c
#define INDEXZ(width,x,y) (x+y*width)
#define INDEXSSAA(width,x,y,c) (x+y*width)*3+c

Renderer::Renderer() :m_width(512), m_height(512) { /////// PRIVATE C-TOR, NEVER CALLED ! :)
	/*InitOpenGLRendering();
	CreateBuffers();
	Init();*/
}

Renderer::Renderer(int width, int height) : m_width(width), m_height(height) {
	InitOpenGLRendering();
	CreateBuffers();
	Init();
}

Renderer::~Renderer(void)
{
}

void Renderer::Init(){
	R = B = G = 0.5;
	Invalidate();
}

void Renderer::SetRendererSize(int width, int height) {
	m_width = width;
	m_height = height;
	CreateBuffers();
}

void Renderer::CreateBuffers() {
	m_TotalNumberOfPixels = (m_width * m_height);
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3 * m_TotalNumberOfPixels];
	m_zbuffer = new float[m_TotalNumberOfPixels];
}

void Renderer::Invalidate(){
	for (int i = 0; i < 3 * m_width*m_height; ++i)
		m_outBuffer[i] = 0.1;
	globalClippedVertices.clear();
	for (int y = 0; y < m_height; ++y)
		for (int x = 0; x < m_width; ++x)
			m_zbuffer[INDEXZ(m_width, x, y)] = 5000; //TODO: max-z as back of the world...
}

void Renderer::SetDemoBuffer()
{
	vec2 a(m_width / 2, m_height / 2);
	vec2 b(0, 0);
	vec2 c(m_width/2, 0);
	vec2 d(m_width-1, 0);
	vec2 b2(0, m_height-1);
	vec2 c2(m_width / 2, m_height-1);
	vec2 d2(m_width-1, m_height-1);


	DrawLine(a, b);
	DrawLine(a, c);
	DrawLine(a, d);
	DrawLine(a, b2);
	DrawLine(a, c2);
	DrawLine(a, d2);
}

void Renderer::SetProjection(const mat4& projection){
	this->projectionMatrix = projection;
}

void Renderer::SetCameraTransform(const mat4& world_to_camera){
	this->world_to_camera = world_to_camera;
}

void Renderer::SetObjectMatrices(const mat4& oTransform, const mat3& nTransform){
	this->object_to_world = oTransform;
}

void Renderer::DrawLine(vec2 a, vec2 b){
	if (a.x > 1000 || a.x < -1000 || b.x>1000 || b.x>1000)
		return;
	// Takes to 2d vectors and draws line betwen them - Must be in the markings of the screen
	int xCounter = a.x < b.x ? 1 : -1;
	int yCounter = a.y < b.y ? 1 : -1;
	int deltaY = abs((int)a.y - (int)b.y) << 1;
	int deltaX = abs((int)a.x - (int)b.x) << 1;

	if (deltaX >= deltaY){	//slope<1
		int errorInteger = deltaY - deltaX >> 1;
		int deltaError = deltaY;
		int deltaErrorNegation = deltaY - deltaX;
		int y = a.y;
		for (int x = a.x; xCounter*x <= xCounter*b.x; x = x + xCounter){
			if (errorInteger > 0 && (errorInteger || (xCounter > 0))){
				errorInteger += deltaErrorNegation;
				y += yCounter;
			}
			else{
				errorInteger += deltaError;
			}
			if (x > 0 && y > 0 && x < m_width && y < m_height){
				m_outBuffer[INDEX(m_width, x, y, 0)] = R;	m_outBuffer[INDEX(m_width, x, y, 1)] = G;	m_outBuffer[INDEX(m_width, x, y, 2)] = B;
			}
		}
	}
	else{
		int errorInteger = deltaX - deltaY>>1;
		int deltaError = deltaX;
		int deltaErrorNegation = deltaX - deltaY;
		int x = a.x;
		for (int y = a.y; yCounter*y <= yCounter*b.y; y = y + yCounter){
			if (errorInteger > 0 && (errorInteger || (yCounter > 0))){
				errorInteger += deltaErrorNegation;
				x = x + xCounter;
			}
			else{
				errorInteger += deltaError;
			}
			if (x > 0 && y > 0 && x < m_width && y < m_height){
				m_outBuffer[INDEX(m_width, x, y, 0)] = R;	m_outBuffer[INDEX(m_width, x, y, 1)] = G;	m_outBuffer[INDEX(m_width, x, y, 2)] = B;
			}
		}
	}
}

void Renderer::setColor(float red, float green, float blue){
	R = red / 256.0;
	G = green / 256.0;
	B = blue / 256.0;
}

void Renderer::DrawLineBetween3Dvecs(const vec4& _vecA,const vec4& _vecB){
	//Function for Coordinate System for now
	vec4 vecA, vecB;
	mat4 objectToClip = projectionMatrix * world_to_camera * object_to_world;
	vecA = objectToClip * _vecA;
	vecB = objectToClip * _vecB;

	vecA /= vecA.w;
	vecB /= vecB.w;

	vec2 a =  vec2(vecA.x, vecA.y);
	vec2 b = vec2(vecB.x, vecB.y);

	a.x = m_width*(a.x + 1) / 2;
	a.y = m_height*(a.y + 1) / 2;

	b.x = m_width*(b.x + 1) / 2;
	b.y = m_height*(b.y + 1) / 2;

	DrawLine(a, b);
}

vec4 v0, v1, v2;
float mag;
float a1, a2, a3;
void Barycentric(const vec2& p, const vec4& a, const vec4& b, const vec4& c)
{
	v0.x = b.x - a.x;
	v0.y = b.y - a.y;
	v1.x = c.x - a.x;
	v1.y = c.y - a.y;
	v2.x = p.x - a.x;
	v2.y = p.y - a.y;

	mag = v0.x * v1.y - v1.x * v0.y;
	a1 = (v2.x * v1.y - v1.x * v2.y) / mag;
	a2 = (v0.x * v2.y - v2.x * v0.y) / mag;
	a3 = 1.0f - a1 - a2;
}

/*
inline bool Renderer::PointInTriangle(const vec2& pt, const  vec4 a, const  vec4 b, const  vec4 c) const{

	Barycentric(pt, a, b, c);
	return (a1 >= 0 && a2 >= 0 && a3 >= 0);
}
*/

bool result;
vec4 a, b, c;
void Renderer::PointInTriangle(vec2& pt, Polygon3* P) {
	a = P->a;
	b = P->b;
	c = P->c;
	v0.x = b.x - a.x;
	v0.y = b.y - a.y;
	v1.x = c.x - a.x;
	v1.y = c.y - a.y;
	v2.x = pt.x - a.x;
	v2.y = pt.y - a.y;

	mag = v0.x * v1.y - v1.x * v0.y;
	a1 = (v2.x * v1.y - v1.x * v2.y) / mag;
	a2 = (v0.x * v2.y - v2.x * v0.y) / mag;
	a3 = 1.0f - a1 - a2;
	result = (a1 >= 0 && a2 >= 0 && a3 >= 0/* && test_a1 + test_a2 + test_a3 <= 1*/);
}

/*
bool Renderer::PointInTriangle(const vec2& pt, Polygon3& P) {

	Barycentric(pt, P.a, P.b, P.c);
	return (a1 >= 0 && a2 >= 0 && a3 >= 0);
}
*/

vec2 vec3TOvec2(vec4 v){
	return vec2(v.x, v.y);
}

void Renderer::testPointInTriangle(int x, int y){
	//Polygon3 p;
	/*vector<Polygon3>::iterator i = globalClippedVertices.begin();
	while (i != globalClippedVertices.end()){
		if PointInTriangle(vec2(x, y), it->a, it->b, it->c)){

		}
	}*/
	/*for (int i = 0; i < globalClippedVertices.size(); ++i){
		if (PointInTriangle(vec2(x, y), globalClippedVertices[i].a, globalClippedVertices[i].b, globalClippedVertices[i].c)){
			cout << i << endl;
			setColor(0, 233, 244);
			DrawLine(vec3TOvec2(globalClippedVertices[i].a+0.1), vec3TOvec2(globalClippedVertices[i].b+0.1));
			setColor(256, 233, 244);
		}
			

	}*/
}




inline GLfloat Depth(Polygon3* P, vec2& p){
	Barycentric(p, P->a, P->b, P->c);
	return a1 * P->a.z + a2 * P->b.z + a3 *P->c.z;
}

inline vec3 vec4TOvec3(vec4 v){
	return vec3(v.x,v.y,v.z);
}


void Renderer::AddTriangles(const vector<vec4>* vertices, const vec3 color, const vector<vec3>* normals){
	mat4 objectToCamera = world_to_camera * object_to_world;
	mat4 objectToClip = projectionMatrix * objectToCamera;
	int numberOfVertices = vertices->size();
	vec4 currentVertice, currentVerticeZ_A, currentVerticeZ_B, currentVerticeZ_C, currentNormal;
	vec4 eye = vec3(0.5, 0.5, 0), l,n,r,e;
	float teta;

	vector<vec4> cameraVertices;
	vector<vec4> clippedVertices;
	clippedVertices.reserve(numberOfVertices);
	cameraVertices.reserve(numberOfVertices);
	globalClippedVertices.reserve(numberOfVertices + globalClippedVertices.size());
	vec4 aa, bb, cc;
	for (int i = 0; i < numberOfVertices; ++i){
		objectToCamera.MultiplyVec((*vertices)[i], currentVerticeZ_A);
		projectionMatrix.MultiplyVec(currentVerticeZ_A, currentVertice);
		currentVertice /= currentVertice.w;
		aa.x = m_width*(currentVertice.x + 1) / 2;
		aa.y = m_height*(currentVertice.y + 1) / 2;
		aa.z = currentVerticeZ_A.z;
		aa.w = 1;
		++i;
		objectToCamera.MultiplyVec((*vertices)[i], currentVerticeZ_B);
		projectionMatrix.MultiplyVec(currentVerticeZ_B, currentVertice);
		currentVertice /= currentVertice.w;
		bb.x = m_width*(currentVertice.x + 1) / 2;
		bb.y = m_height*(currentVertice.y + 1) / 2;
		bb.z = currentVerticeZ_B.z;
		bb.w = 1;
		++i;
		objectToCamera.MultiplyVec((*vertices)[i], currentVerticeZ_C);
		projectionMatrix.MultiplyVec(currentVerticeZ_C, currentVertice);
		currentVertice /= currentVertice.w;
		cc.x = m_width*(currentVertice.x + 1) / 2;
		cc.y = m_height*(currentVertice.y + 1) / 2;
		cc.z = currentVerticeZ_C.z;
		cc.w = 1;

		vec3 polygonColor = (color / 500 + vec3(0.01))*AmbientIntensity;

		for (int j = 0; j < lights->size(); ++j){
			//(*lights)[i]->location
			world_to_camera.MultiplyVec(-(*lights)[j]->location, currentVertice); 
			objectToCamera.MultiplyVec(normals->at(i / 3), currentNormal);
			
			n = normalize(vec4TOvec3(currentNormal));
			l = normalize((vec4TOvec3((currentVerticeZ_A + currentVerticeZ_B + currentVerticeZ_C) / 3 - currentVertice)));
			teta = dot(l, n);
			r = normalize((2 * teta * n) + l);
			e = -normalize(eye - vec4TOvec3((currentVerticeZ_A + currentVerticeZ_B + currentVerticeZ_C) / 3));
			polygonColor += polygonColor *  max(1, 1 / AmbientIntensity) * max(0, teta);
			polygonColor += polygonColor * max(1, 100 / AmbientIntensity) * pow(max(0, dot(r,e)), 10);
		}

		globalClippedVertices.push_back(Polygon3(aa, bb, cc, polygonColor, normals->at(i / 3)));


		DrawLine(vec2(aa.x, aa.y), vec2(bb.x, bb.y));
		DrawLine(vec2(bb.x, bb.y), vec2(cc.x, cc.y));
		DrawLine(vec2(cc.x, cc.y), vec2(aa.x, aa.y));

	}
}


void Renderer::putColor(int x, int y, Polygon3* P){

	//m_outBuffer[INDEX(m_width, x, y, 0)] = min(m_outBuffer[INDEX(m_width, x, y, 0)] + R, 1);
	//m_outBuffer[INDEX(m_width, x, y, 1)] = min(m_outBuffer[INDEX(m_width, x, y, 1)] + G, 1);
	//m_outBuffer[INDEX(m_width, x, y, 2)] = min(m_outBuffer[INDEX(m_width, x, y, 2)] + B, 1);

	m_outBuffer[INDEX(m_width, x, y, 0)] = P->baseColor.x;	m_outBuffer[INDEX(m_width, x, y, 1)] = P->baseColor.y;	m_outBuffer[INDEX(m_width, x, y, 2)] = P->baseColor.z;
}

void Renderer::drawFillAndFog(vec3 fog){
	if (globalClippedVertices.empty())
		return;
	int x, y;
	int minX, maxX;
	int minY, maxY;
	vec2 pt;
	int globalClippedVerticesSize = globalClippedVertices.size();
	Polygon3* currentPolygon;
	GLfloat newZ;
	int zIndex;
	for (int i = 0; i < globalClippedVerticesSize; ++i){
		currentPolygon = &globalClippedVertices[i];
		minY = max(0, floor(currentPolygon->minY()));
		maxY = min(m_height, ceil(currentPolygon->maxY()));
		minX = max(0, floor(currentPolygon->minX()));
		maxX = min(m_width, ceil(currentPolygon->maxX()));
		for (y = minY; y < maxY; ++y){
			pt.y = y;
			for (int x = minX; x <= maxX; ++x){
				pt.x = x;
				PointInTriangle(pt, currentPolygon); //set "result" for the next line instead of instantiating a new variable.
				if (result){
					//z = Depth(currentPolygon, pt);
					newZ = getZ(pt, currentPolygon->a, currentPolygon->b, currentPolygon->c);
					zIndex = INDEXZ(m_width, x, y);
					if (newZ < m_zbuffer[zIndex]) {
						m_zbuffer[zIndex] = newZ;
						putColor(x, y, currentPolygon);
					}
				}
			}
		}
	}
	if (fog) {
		GLfloat fogR = (fog.x / 256.0f);
		GLfloat fogG = (fog.y / 256.0f);
		GLfloat fogB = (fog.z / 256.0f);
		int indexA;
		int indexB;
		int indexC;
		GLfloat zValue;
		for (int x = 0; x < m_width; ++x) {
			for (int y = 0; y < m_width; ++y) {
				indexA = INDEX(m_width, x, y, 0);
				indexB = INDEX(m_width, x, y, 1);
				indexC = INDEX(m_width, x, y, 2);
				zValue = ((m_zbuffer[INDEXZ(m_width, x, y)] + 1.0f) / 2.0f);
				m_outBuffer[indexA] = min(m_outBuffer[indexA] + (zValue * fogR), 1.0f);
				m_outBuffer[indexB] = min(m_outBuffer[indexB] + (zValue * fogG), 1.0f);
				m_outBuffer[indexC] = min(m_outBuffer[indexC] + (zValue * fogB), 1.0f);
			}
		}
	}
	bool superSamling = true;
	if (superSamling) {
		int multiplier = 2;
		int ssaaScreenWidth = (m_width / multiplier);
		int ssaaScreenHeight = (m_height / multiplier);
		float* ssaaScreen = new float[((ssaaScreenWidth * ssaaScreenHeight) * 3)];
		for (int x = 0; x < ssaaScreenWidth; ++x) {
			for (int y = 0; y < ssaaScreenHeight; ++y) {
				ssaaScreen[INDEXSSAA(ssaaScreenWidth, x, y, 0)] = findSSAAOfColorElement(multiplier, x, y, 0);
				ssaaScreen[INDEXSSAA(ssaaScreenWidth, x, y, 1)] = findSSAAOfColorElement(multiplier, x, y, 1);
				ssaaScreen[INDEXSSAA(ssaaScreenWidth, x, y, 2)] = findSSAAOfColorElement(multiplier, x, y, 2);
			}
		}
		for (int x = 0; x < ssaaScreenWidth; ++x) {
			for (int y = 0; y < ssaaScreenHeight; ++y) {
				m_outBuffer[INDEX(m_width, x, y, 0)] = ssaaScreen[INDEXSSAA(ssaaScreenWidth, x, y, 0)];
				m_outBuffer[INDEX(m_width, x, y, 1)] = ssaaScreen[INDEXSSAA(ssaaScreenWidth, x, y, 1)];
				m_outBuffer[INDEX(m_width, x, y, 2)] = ssaaScreen[INDEXSSAA(ssaaScreenWidth, x, y, 2)];
			}
		}
		delete []ssaaScreen;
	}
}

float Renderer::findSSAAOfColorElement(int multiplier, int x, int y, int colorElement) {
	GLfloat sum = -1;
	for (int i = 0; i < multiplier; ++i) {
		for (int j = 0; j < multiplier; ++j) {
			if (sum == -1) { // first pixel
				sum = m_outBuffer[INDEX(m_width, ((x * multiplier) + i), ((y * multiplier) + j), colorElement)];
			} else { // the rest of the pixels
				sum += m_outBuffer[INDEX(m_width, ((x * multiplier) + i), ((y * multiplier) + j), colorElement)];
			}
		}
	}
	return (sum / (multiplier * multiplier));
}

/*
void Renderer::DrawTriangles(const vector<vec4>* vertices, const vector<vec3>* normals){
	mat4 objectToCamera = world_to_camera * object_to_world;
	mat4 objectToClip = projectionMatrix * objectToCamera;
	int numberOfVertices = vertices->size();
	vec4 currentVertice;


	// Transfermations for the Z-Buffer
	vector<vec4> cameraVertices;
	vector<vec4> clippedVertices;
	clippedVertices.reserve(numberOfVertices);
	cameraVertices.reserve(numberOfVertices);
	for (int i = 0; i < numberOfVertices; ++i){
		objectToCamera.MultiplyVec((*vertices)[i], currentVertice);
		cameraVertices.push_back(currentVertice);

		// Transfermations for the Normal colors
		projectionMatrix.MultiplyVec(currentVertice, currentVertice);
		currentVertice.x = m_width*(currentVertice.x + 1) / 2;
		currentVertice.y = m_height*(currentVertice.y + 1) / 2;
		clippedVertices.push_back(currentVertice);
	}

	// Init all the Z-buffer to 0
	for (int i = 0, end = (m_width * m_height); i < end; ++i) {
		m_zbuffer[i] = 0.0f;
	}
	// Create the current Z-buffer
	vec4 pointA;
	vec4 pointB;
	vec4 pointC;
	int triangleMinX;
	int triangleMinY;
	int triangleMaxX;
	int triangleMaxY;
	int m_zbufferIndex;
	vector<vec2> pointsInsideTriangle;
	vec2 currentPoint;
	GLfloat currentZ;
	int triangleWidth;
	int triangleHeight;
	for (int i = 0; i < numberOfVertices; i += 3) {
		pointA = clippedVertices[i];
		pointB = clippedVertices[i + 1];
		pointC = clippedVertices[i + 2];
		if (pointA.x < 0 || pointB.x < 0 || pointC.x < 0 || pointA.y < 0 || pointB.y < 0 || pointC.y < 0 ||
			pointA.x > m_width || pointB.x >= m_width || pointC.x >= m_width || pointA.y >= m_height || pointB.y >= m_height || pointC.y >= m_height){
			continue;
		}
		triangleMinX = min(pointA.x, min(pointB.x, pointC.x));
		triangleMaxX = max(pointA.x, max(pointB.x, pointC.x));
		triangleMinY = min(pointA.y, min(pointB.y, pointC.y));
		triangleMaxY = max(pointA.y, max(pointB.y, pointC.y));
		triangleWidth = (triangleMaxX - triangleMinX);
		triangleHeight = (triangleMaxY - triangleMinY);
		pointsInsideTriangle.reserve(((triangleWidth * triangleHeight) / 2)); // Maximum possible points in this triangle
		pointsInsideTriangle.clear();
		for (int j = 0; j < triangleWidth; ++j) {
			for (int k = 0; k < triangleHeight; ++k) {
				currentPoint = vec2((triangleMinX + j), (triangleMinY + k));
				//if (PointInTriangle(currentPoint, pointA, pointB, pointC)) {
				//	pointsInsideTriangle.push_back(currentPoint);
				//}
			}
		}
		for (int t = 0, numberOfPointsInsideTriangle = pointsInsideTriangle.size(); t < numberOfPointsInsideTriangle; ++t) {
			currentPoint = pointsInsideTriangle[t];
			currentZ = ((getZ(vec4toVec2(pointA), vec4toVec2(pointB), vec4toVec2(pointC), currentPoint, cameraVertices[i], cameraVertices[i + 1], cameraVertices[i + 2]) + 1.0f) / 2.0f);
			m_zbufferIndex = (INDEXZ(m_width, currentPoint.x, currentPoint.y));
			if (m_zbuffer[m_zbufferIndex] < currentZ) {
				//putColor(x, y, col(P)) ///////////////////// USE THIS TO DISPLAY ONLY THE VISIBLE PARTS OF THE MODELS
				m_zbuffer[m_zbufferIndex] = currentZ;
			}
		}
	}

	// Drawings
	// Z-buffer drawing
	for (int i = 0; i < m_height; ++i) {
		for (int j = 0; j < m_width; ++j) {
			m_outBuffer[INDEX(m_width, j, i, 1)] = m_zbuffer[INDEXZ(m_width, j, i)];	
		}
	}
	// Normal colors drawing
	for (int i = 0; i < numberOfVertices; ++i){
		vec2 a, b, c;
		a = vec4toVec2(clippedVertices[i++]);
		b = vec4toVec2(clippedVertices[i++]);
		c = vec4toVec2(clippedVertices[i]);
		if (a.x < 0 || b.x < 0 || c.x < 0 || a.y < 0 || b.y < 0 || c.y < 0 ||
			a.x > m_width || b.x >= m_width || c.x >= m_width || a.y >= m_height || b.y >= m_height || c.y >= m_height){
			continue;
		}
		DrawLine(a, b);
		DrawLine(b, c);
		DrawLine(c, a);
	}
}
*/

GLfloat Renderer::getZ(const vec2& ps, const vec4& z3, const vec4& z2, const vec4& z1) {
	GLfloat aOne;
	GLfloat cOne;
	GLfloat aTwo = ((z2.y - z1.y) / (z2.x - z1.x));
	GLfloat cTwo = (z2.y - (((z2.y - z1.y) / (z2.x - z1.x)) * z2.x));
	GLfloat piX;
	vec2 pi;
	if (z3.x == ps.x) {
		pi = vec2(z3.x, ((aTwo * z3.x) + cTwo));
	} else {
		aOne = ((z3.y - ps.y) / (z3.x - ps.x));
		cOne = (z3.y - (((z3.y - ps.y) / (z3.x - ps.x)) * z3.x));
		piX = ((cTwo - cOne) / (aOne - aTwo));
		pi = vec2(piX, ((aOne * piX) + cOne));
	}
	GLfloat ti = (length(pi - vec4toVec2(z1)) / length(z2 - z1));
	GLfloat t = (length(ps - vec4toVec2(z3)) / length(pi - vec4toVec2(z3)));
	vec4 zp = ((z3 * (1.0f - t)) + (((z2 * ti) + (z1 * (1.0f - ti))) * t));
	
	return zp.z;
}


float Renderer::sign(vec2& p1, vec3& p2, vec3& p3) const{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

vec2 Renderer::vec4toVec2(const vec4 v){
	return vec2(v.x, v.y);
}

int Renderer::GetWidth() {
	return m_width;
}

int Renderer::GetHeight() {
	return m_height;
}

void Renderer::SetLights(vector<Light*>* l){
	lights = l;
}

void Renderer::setAmbientLight(float intensity){
	AmbientIntensity = intensity;
}



/////////////////////////////////////////////////////
//OpenGL stuff. Don't touch.

void Renderer::InitOpenGLRendering()
{
	int a = glGetError();
	a = glGetError();
	glGenTextures(1, &gScreenTex);
	a = glGetError();
	glGenVertexArrays(1, &gScreenVtc);
	GLuint buffer;
	glBindVertexArray(gScreenVtc);
	glGenBuffers(1, &buffer);
	const GLfloat vtc[]={
		-1, -1,
		1, -1,
		-1, 1,
		-1, 1,
		1, -1,
		1, 1
	};
	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
	glUseProgram( program );
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );

	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 2, GL_FLOAT, GL_FALSE, 0,
		0 );

	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		(GLvoid *) sizeof(vtc) );
	glUniform1i( glGetUniformLocation(program, "texture"), 0 );
	a = glGetError();
}

void Renderer::CreateOpenGLBuffer()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_width, m_height, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, m_width, m_height);
}

void Renderer::SwapBuffers()
{

	int a = glGetError();
	glActiveTexture(GL_TEXTURE0);
	a = glGetError();
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	a = glGetError();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGB, GL_FLOAT, m_outBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	a = glGetError();

	glBindVertexArray(gScreenVtc);
	a = glGetError();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	a = glGetError();
	glutSwapBuffers();
	a = glGetError();
}






