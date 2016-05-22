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

Renderer::Renderer() :m_OutBufferWidth(512), m_OutBufferHeight(512) // private c-tor, never to be called ;)
{
	/*InitOpenGLRendering();
	CreateBuffers(512,512);
	Init();*/
}

Renderer::Renderer(int width, int height) : m_OutBufferWidth(width), m_OutBufferHeight(height) {
	m_SSAAOutBufferWidth = (m_OutBufferWidth * m_SSAAMultiplier);
	m_SSAAOutBufferHeight = (m_OutBufferHeight * m_SSAAMultiplier);
	InitOpenGLRendering();
	CreateBuffers();
	Init();
}

Renderer::~Renderer(void) {}

void Renderer::Init() {
	R = B = G = 0.5;
	Invalidate();
}

void Renderer::SetRendererSize(int width, int height) {
	m_OutBufferWidth = width;
	m_OutBufferHeight = height;
	m_SSAAOutBufferWidth = (m_OutBufferWidth * m_SSAAMultiplier);
	m_SSAAOutBufferHeight = (m_OutBufferHeight * m_SSAAMultiplier);
	CreateBuffers();
}

void Renderer::SetSSAAMultiplier(int multiplier) {
	m_SSAAMultiplier = multiplier;
	SetRendererSize(m_OutBufferWidth, m_OutBufferHeight);
	//Init();
}

void Renderer::CreateBuffers() {
	m_TotalNumberOfOutBufferPixels = (m_OutBufferWidth * m_OutBufferHeight);
	CreateOpenGLBuffer(); //Do not remove this line.
	m_SSAAOutBuffer = new float[3 * m_TotalNumberOfOutBufferPixels * (m_SSAAMultiplier * m_SSAAMultiplier)];
	m_outBuffer = new float[3 * m_TotalNumberOfOutBufferPixels];
	m_zbuffer = new float[m_TotalNumberOfOutBufferPixels * (m_SSAAMultiplier * m_SSAAMultiplier)];
}

void Renderer::Invalidate(){
	for (int i = 0, end = 3 * m_SSAAOutBufferWidth * m_SSAAOutBufferHeight; i < end; ++i)
		m_SSAAOutBuffer[i] = 0.1;
	globalClippedPolygon3.clear();
	for (int y = 0; y < m_SSAAOutBufferHeight; ++y)
		for (int x = 0; x < m_SSAAOutBufferWidth; ++x)
			m_zbuffer[INDEXZ(m_SSAAOutBufferWidth, x, y)] = 5000; //TODO: max-z as back of the world...
}

void Renderer::SetDemoBuffer()
{
	vec2 a(m_OutBufferWidth / 2, m_OutBufferHeight / 2);
	vec2 b(0, 0);
	vec2 c(m_OutBufferWidth / 2, 0);
	vec2 d(m_OutBufferWidth - 1, 0);
	vec2 b2(0, m_OutBufferHeight - 1);
	vec2 c2(m_OutBufferWidth / 2, m_OutBufferHeight - 1);
	vec2 d2(m_OutBufferWidth - 1, m_OutBufferHeight - 1);


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

void Renderer::SetObjectMatrices(const mat4& oTransform, const mat4& nTransform){
	this->object_to_world = oTransform;
	this->normalTransform = nTransform;
}

void Renderer::DrawLine(vec2 a, vec2 b, float az, float bz) {
	/*if (a.x > 1000 || a.x < -1000 || b.x>1000 || b.x>1000)
	return;*/
	/*float outx1, outy1, outx2, outy2;
	if (!clip2D(a.x, a.y, b.x, b.y, outx1, outy1, outx2, outy2)) {
		return; // All of the line is outside of the window
	}
	a.x = outx1; a.y = outy1; b.x = outx2; b.y = outy2;*/
	int xCounter = a.x < b.x ? 1 : -1;
	int yCounter = a.y < b.y ? 1 : -1;
	int deltaY = abs((int)a.y - (int)b.y) << 1;
	int deltaX = abs((int)a.x - (int)b.x) << 1;
	if (deltaX >= deltaY) {	//slope<1
		int errorInteger = deltaY - deltaX >> 1;
		int deltaError = deltaY;
		int deltaErrorNegation = deltaY - deltaX;
		int y = a.y;
		for (int x = a.x, end = xCounter*b.x; xCounter*x <= end; x += xCounter) {
			if (errorInteger > 0 && (errorInteger || (xCounter > 0))) {
				errorInteger += deltaErrorNegation;
				y += yCounter;
			}
			else {
				errorInteger += deltaError;
			}
			if (x >= 0 && y >= 0 && x < m_SSAAOutBufferWidth && y < m_SSAAOutBufferHeight){ // DELETE ?
				m_SSAAOutBuffer[INDEX(m_SSAAOutBufferWidth, x, y, 0)] = R;	m_SSAAOutBuffer[INDEX(m_SSAAOutBufferWidth, x, y, 1)] = G;	m_SSAAOutBuffer[INDEX(m_SSAAOutBufferWidth, x, y, 2)] = B;
				m_zbuffer[INDEXZ(m_SSAAOutBufferWidth, x, y)] = (length(vec2(x, y) - a) / length(b - a)) * (az - bz);
			}
		}
	}
	else {
		int errorInteger = deltaX - deltaY >> 1;
		int deltaError = deltaX;
		int deltaErrorNegation = deltaX - deltaY;
		int x = a.x;
		for (int y = a.y, end = yCounter*b.y; yCounter*y <= end; y += yCounter) {
			if (errorInteger > 0 && (errorInteger || (yCounter > 0))) {
				errorInteger += deltaErrorNegation;
				x += xCounter;
			}
			else {
				errorInteger += deltaError;
			}
			if (x >= 0 && y >= 0 && x < m_SSAAOutBufferWidth && y < m_SSAAOutBufferHeight){ // DELETE ?
				m_SSAAOutBuffer[INDEX(m_SSAAOutBufferWidth, x, y, 0)] = R;	m_SSAAOutBuffer[INDEX(m_SSAAOutBufferWidth, x, y, 1)] = G;	m_SSAAOutBuffer[INDEX(m_SSAAOutBufferWidth, x, y, 2)] = B;
				m_zbuffer[INDEXZ(m_SSAAOutBufferWidth, x, y)] = (length(vec2(x, y) - a) / length(b - a)) * (az - bz);
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

	a.x = m_SSAAOutBufferWidth*(a.x + 1) / 2;
	a.y = m_SSAAOutBufferHeight*(a.y + 1) / 2;

	b.x = m_SSAAOutBufferWidth*(b.x + 1) / 2;
	b.y = m_SSAAOutBufferHeight*(b.y + 1) / 2;

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

bool result;
vec4 a, b, c;
void Renderer::PointInTriangle(vec2& pt, Polygon3* P) {
	a = P->ma;
	b = P->mb;
	c = P->mc;
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

inline GLfloat Depth(Polygon3* P, vec2& p){
	Barycentric(p, P->ma, P->mb, P->mc);
	return (a3 * P->a.z + a1 * P->b.z + a2 *P->c.z);
}


void Renderer::AddTriangles(const vector<vec4>* vertices, const vec3 color,
	const vector<vec3>* normals, const vector<vec3>* normals2vertices, Material material){
	objectToCamera = world_to_camera * object_to_world;
	mat4 normalToCamera = world_to_camera * normalTransform;
	//mat4 objectToClip = projectionMatrix * objectToCamera;
	int numberOfVertices = vertices->size();
	vec4 currentVertice, currentVerticeZ_A, currentVerticeZ_B, currentVerticeZ_C, currentNormal;
	vec4 PolygonVNormals[3];
	vec3 polygonColor;
	Polygon3 P;
	/*vector<vec4> cameraVertices;
	vector<vec4> clippedVertices;
	clippedVertices.reserve(numberOfVertices);
	cameraVertices.reserve(numberOfVertices);*/

	
	vector<Polygon3> currentClippedPolygon3;
	currentClippedPolygon3.reserve(numberOfVertices / 3);
	
	for (int i = 0; i < numberOfVertices; ++i){
		objectToCamera.MultiplyVec((*vertices)[i++], currentVerticeZ_A);
		objectToCamera.MultiplyVec((*vertices)[i++], currentVerticeZ_B);
		objectToCamera.MultiplyVec((*vertices)[i], currentVerticeZ_C);
		currentVerticeZ_A /= currentVerticeZ_A.w;
		currentVerticeZ_B /= currentVerticeZ_B.w;
		currentVerticeZ_C /= currentVerticeZ_C.w;
		if (material == ALL || material == EMISSIVE)
			polygonColor = (color / 512 + vec3(0.01))*AmbientIntensity;
		else
			polygonColor = (color / 512 + vec3(0.01));

		normalToCamera.MultiplyVec(normals->at(i / 3), currentNormal);
		if (normals2vertices){
			normalToCamera.MultiplyVec((*normals2vertices)[i - 2], PolygonVNormals[0]);
			normalToCamera.MultiplyVec((*normals2vertices)[i - 1], PolygonVNormals[1]);
			normalToCamera.MultiplyVec((*normals2vertices)[i], PolygonVNormals[2]);
			P = Polygon3(currentVerticeZ_A, currentVerticeZ_B, currentVerticeZ_C, polygonColor, currentNormal, projectionMatrix, m_SSAAOutBufferWidth, m_SSAAOutBufferWidth, PolygonVNormals);
		}
		else{
			P = Polygon3(currentVerticeZ_A, currentVerticeZ_B, currentVerticeZ_C, polygonColor, currentNormal, projectionMatrix, m_SSAAOutBufferWidth, m_SSAAOutBufferWidth);
		}
		P.setMaterial(material);
		currentClippedPolygon3.push_back(P);
		//globalClippedPolygon3.push_back(P);
	}

	// Determines if the model is inside/outside the view volume
	bool isModelInViewVolume = false;
	Polygon3& currentPolygon3 = currentClippedPolygon3[0];
	vec4& a = currentPolygon3.a;
	vec4& b = currentPolygon3.b;
	vec4& c = currentPolygon3.c;
	for (int i = 0, endi = currentClippedPolygon3.size(); i < endi; ++i){
		currentPolygon3 = currentClippedPolygon3[i];
		a = currentPolygon3.a;
		b = currentPolygon3.b;
		c = currentPolygon3.c;
		if ((((-1 <= a.x) && (a.x <= 1)) && ((-1 <= a.y) && (a.y <= 1))) ||
			(((-1 <= b.x) && (b.x <= 1)) && ((-1 <= b.y) && (b.y <= 1))) ||
			(((-1 <= c.x) && (c.x <= 1)) && ((-1 <= c.y) && (c.y <= 1)))) {
			isModelInViewVolume = true;
			break;
		}
	}
	if (isModelInViewVolume) {
		vector<vec4> currentVectors;
		currentVectors.reserve(6);
		vector<vec4> newVectors;
		newVectors.reserve(6);
		bool wasInside[6];
		float outx1[6];
		float outy1[6];
		float outx2[6];
		float outy2[6];
		bool wasAnyInside;
		bool wasAnyVerticeClipped;
		float wasClipped[6];
		for (int i = 0, endi = currentClippedPolygon3.size(); i < endi; ++i) {
			currentPolygon3 = currentClippedPolygon3[i];
			currentVectors.clear();
			currentVectors.push_back(currentPolygon3.a);
			currentVectors.push_back(currentPolygon3.b);
			currentVectors.push_back(currentPolygon3.c);

			// Top
			clippingXMin = -FLT_MAX;
			clippingYMin = -FLT_MAX;
			clippingXMax = FLT_MAX;
			clippingYMax = 1.0f;
			
			wasAnyInside = false;
			for (int j = 0, end = currentVectors.size(); j < end; ++j) {
				wasInside[j] = clip2D(currentVectors[j].x, currentVectors[j].y, currentVectors[((j + 1) % end)].x, currentVectors[((j + 1) % end)].y, outx1[j], outy1[j], outx2[j], outy2[j]);
				wasAnyInside |= wasInside[j];
			}
			if (!wasAnyInside) {
				continue; // Whole triangle was out of the view volume
			}
			wasAnyVerticeClipped = false;
			for (int j = 0, end = currentVectors.size(); j < end; ++j) {
				wasAnyVerticeClipped |= ((currentVectors[j].x != outx1[j]) || (currentVectors[j].y != outy1[j]) || (currentVectors[((j + 1) % end)].x != outx2[j]) || (currentVectors[((j + 1) % end)].y != outy2[j]));
			}
			if (wasAnyVerticeClipped) { // If at least 1 vertice was clipped
				for (int j = 0, end = currentVectors.size(); j < end; ++j) {
					wasClipped[j] = ((currentVectors[j].x != outx1[j]) || (currentVectors[j].y != outy1[j]));
				}
				newVectors.clear();
				if (wasClipped[0] && wasClipped[1]) {////// Place in a loop
					newVectors.push_back(vec4(currentVectors[2].x, currentVectors[2].y, 0.0f, 1.0f));
					newVectors.push_back(vec4(outx1[0], outy1[0], 0.0f, 1.0f));
					newVectors.push_back(vec4(outx1[2], outy1[2], 0.0f, 1.0f));
				} else {
					if (wasClipped[0] && wasClipped[2]) {
						newVectors.push_back(vec4(currentVectors[1].x, currentVectors[1].y, 0.0f, 1.0f));
						newVectors.push_back(vec4(outx1[0], outy1[0], 0.0f, 1.0f));
						newVectors.push_back(vec4(outx1[1], outy1[1], 0.0f, 1.0f));
					} else {
						if (wasClipped[1] && wasClipped[2]) {
							newVectors.push_back(vec4(currentVectors[0].x, currentVectors[0].y, 0.0f, 1.0f));
							newVectors.push_back(vec4(outx1[2], outy1[2], 0.0f, 1.0f));
							newVectors.push_back(vec4(outx1[1], outy1[1], 0.0f, 1.0f));
						} else {
							if (wasClipped[0]) {
								newVectors.push_back(vec4(outx1[0], outy1[0], 0.0f, 1.0f));
								newVectors.push_back(vec4(currentVectors[1].x, currentVectors[1].y, 0.0f, 1.0f));
								newVectors.push_back(vec4(currentVectors[2].x, currentVectors[2].y, 0.0f, 1.0f));
								newVectors.push_back(vec4(outx2[1], outy2[1], 0.0f, 1.0f));
							} else {
								if (wasClipped[1]) {
									newVectors.push_back(vec4(currentVectors[0].x, currentVectors[0].y, 0.0f, 1.0f));
									newVectors.push_back(vec4(outx2[0], outy2[0], 0.0f, 1.0f));
									newVectors.push_back(vec4(outx1[2], outy1[2], 0.0f, 1.0f));
									newVectors.push_back(vec4(currentVectors[2].x, currentVectors[2].y, 0.0f, 1.0f));
								} else {
									if (wasClipped[2]) {
										newVectors.push_back(vec4(currentVectors[0].x, currentVectors[0].y, 0.0f, 1.0f));
										newVectors.push_back(vec4(currentVectors[1].x, currentVectors[1].y, 0.0f, 1.0f));
										newVectors.push_back(vec4(outx2[2], outy2[2], 0.0f, 1.0f));
										newVectors.push_back(vec4(outx1[1], outy1[1], 0.0f, 1.0f));
									} else { /* Shouldn't get here */ }
								}
							}
						}
					}
				}
				currentVectors = newVectors;
			}
			
			else { // DELETE the whole else and continue to check the Bottom, Right and Left..
				currentVectors.push_back(vec4(currentVectors[2].x, currentVectors[2].y, 0.0f, 1.0f)); // DELETE
				currentVectors.push_back(vec4(currentVectors[0].x, currentVectors[0].y, 0.0f, 1.0f)); // DELETE
				currentVectors.push_back(vec4(currentVectors[1].x, currentVectors[1].y, 0.0f, 1.0f)); // DELETE
			} // DELETE
			
			// Bottom
			clippingXMin = -FLT_MAX;
			clippingYMin = -1.0f;
			clippingXMax = FLT_MAX;
			clippingYMax = FLT_MAX;


			// Right
			clippingXMin = -FLT_MAX;
			clippingYMin = -FLT_MAX;
			clippingXMax = 1.0f;
			clippingYMax = FLT_MAX;


			// Left
			clippingXMin = -1.0f;
			clippingYMin = -FLT_MAX;
			clippingXMax = FLT_MAX;
			clippingYMax = FLT_MAX;
			
			
			// Divides a polygon into triangle\s
			for (int j = (currentVectors.size() - 1); j >= 2; --j) {
				globalClippedPolygon3.push_back(Polygon3(vec4(currentVectors[j - 2].x, currentVectors[j - 2].y, 0.0f, 1.0f),
														 vec4(currentVectors[j - 1].x, currentVectors[j - 1].y, 0.0f, 1.0f),
														 vec4(currentVectors[j].x, currentVectors[j].y, 0.0f, 1.0f),
														 currentPolygon3.baseColor, NULL, currentPolygon3.projection,
														 currentPolygon3.m_width, currentPolygon3.m_height, currentPolygon3.normalsToVertices));
			}
		}
	}
}

bool Renderer::clip2D(float x1, float y1, float x2, float y2, float& outx1, float& outy1, float& outx2, float& outy2) {
	if ((clippingXMin <= x1 && x1 <= clippingXMax) && (clippingXMin <= x2 && x2 <= clippingXMax) && (clippingYMin <= y1 && y1 <= clippingYMax) && (clippingYMin <= y2 && y2 <= clippingYMax)) {
		outx1 = x1;
		outy1 = y1;
		outx2 = x2;
		outy2 = y2;
		return true;
	}
	float dx = (x2 - x1);
	float dy = (y2 - y1);
	float p[4] = { -dx, dx, -dy, dy };
	float q[4] = { (x1 - clippingXMin), (clippingXMax - x1), (y1 - clippingYMin), (clippingYMax - y1) };
	float a1 = 0.0f;
	float a2 = 1.0f;
	for (int i = 0; i < 4; ++i){
		if (p[i] < 0) {
			a1 = max(0, (q[i]) / p[i]);
		} else {
			if (p[i] > 0) {
				a2 = min(1, (q[i]) / p[i]);
			} else { // p[i] == 0
				if (q[i] < 0) { // All of the line is outside of the window
					return false;
				}
			}
		}
	}
	if (a1 > a2) {
		return false;
	}
	if (a1 == 0.0f) {
		outx1 = x1;
		outy1 = y1;
	} else {
		outx1 = (x1 + (a1 * dx));
		outy1 = (y1 + (a1 * dy));
	}
	if (a2 == 1.0f) {
		if (a1 == 0.0f) {
			outx2 = (x1 + dx);
			outy2 = (y1 + dy);
		} else {
			outx2 = x2;
			outy2 = y2;
		}
	} else {
		outx2 = (x1 + (a2 * dx));
		outy2 = (y1 + (a2 * dy)); 
	}
	return true;
}

bool Renderer::clip3D(float x1, float y1, float z1, float x2, float y2, float z2,
					  float& outx1, float& outy1, float& outz1, float& outx2, float& outy2, float& outz2) {
	return false;// TODO
}

vec2 pixel;
vec3 pixelColor;
vec3 pixelLocation;
void Renderer::putColor(int x, int y, Polygon3* P){
	if (shadow == GOUARD){
		P->calculateVertexColors(lights, world_to_camera, AmbientIntensity);
		pixel = vec2(x, y);
		Barycentric(pixel, P->ma, P->mb, P->mc); //use now a1, a2, a3 - global barycentric coordinates.
		pixelColor = (a3 * P->aColor + a1 * P->bColor + a2 * P->cColor);

	}
	else if (shadow == PHONG){
		pixel = vec2(x, y);
		Barycentric(pixel, P->ma, P->mb, P->mc); //use now a1, a2, a3 - global barycentric coordinates.
		pixelLocation = vec4TOvec3(a3 * P->a + a1 * P->b + a2 * P->c);
		//pixelLocation.z = -pixelLocation.z;
		pixelColor = P->calculateColor(lights, world_to_camera, AmbientIntensity, pixelLocation, P->getNormalOfBaricentricLocation(a1, a2, a3));
	}
	else if (shadow == FLAT){
		pixelColor = P->calculateFaceColor(lights, world_to_camera, AmbientIntensity);
	}

	m_SSAAOutBuffer[INDEX(m_SSAAOutBufferWidth, x, y, 0)] = pixelColor.x;	m_SSAAOutBuffer[INDEX(m_SSAAOutBufferWidth, x, y, 1)] = pixelColor.y;	m_SSAAOutBuffer[INDEX(m_SSAAOutBufferWidth, x, y, 2)] = pixelColor.z;

	//m_outBuffer[INDEX(m_width, x, y, 0)] = min(m_outBuffer[INDEX(m_width, x, y, 0)] + R, 1);
	//m_outBuffer[INDEX(m_width, x, y, 1)] = min(m_outBuffer[INDEX(m_width, x, y, 1)] + G, 1);
	//m_outBuffer[INDEX(m_width, x, y, 2)] = min(m_outBuffer[INDEX(m_width, x, y, 2)] + B, 1);

}

void Renderer::drawFillAndFog(vec3& fog){
	if (globalClippedPolygon3.empty() || m_outBuffer == NULL || m_SSAAOutBuffer == NULL)
		return;
	int x, y;
	int minX, maxX;
	int minY, maxY;
	vec2 pt;
	int globalClippedVerticesSize = globalClippedPolygon3.size();
	Polygon3* currentPolygon;
	for (int i = 0; i < globalClippedVerticesSize; ++i){
		currentPolygon = &globalClippedPolygon3[i];
		minY = max(0, floor(currentPolygon->minY()));
		maxY = min(m_SSAAOutBufferHeight, ceil(currentPolygon->maxY()));
		minX = max(0, floor(currentPolygon->minX()));
		maxX = min(m_SSAAOutBufferWidth, ceil(currentPolygon->maxX()));
		for (y = minY; y < maxY; ++y){
			pt.y = y;
			for (int x = minX; x <= maxX; ++x){
				pt.x = x;
				PointInTriangle(pt, currentPolygon); //set "result" for the next line instead of instantiating a new variable.
				if (result){
					GLfloat z = Depth(currentPolygon, pt);
					if (z < m_zbuffer[INDEXZ(m_SSAAOutBufferWidth, x, y)]/* && z>0*/){
						m_zbuffer[INDEXZ(m_SSAAOutBufferWidth, x, y)] = z;
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
		for (int x = 0; x < m_SSAAOutBufferWidth; ++x) {
			for (int y = 0; y < m_SSAAOutBufferWidth; ++y) {
				indexA = INDEX(m_SSAAOutBufferWidth, x, y, 0);
				indexB = INDEX(m_SSAAOutBufferWidth, x, y, 1);
				indexC = INDEX(m_SSAAOutBufferWidth, x, y, 2);
				zValue = ((m_zbuffer[INDEXZ(m_SSAAOutBufferWidth, x, y)] + 1.0f) / 2.0f);
				m_SSAAOutBuffer[indexA] = min(m_SSAAOutBuffer[indexA] + (zValue * fogR), 1.0f);
				m_SSAAOutBuffer[indexB] = min(m_SSAAOutBuffer[indexB] + (zValue * fogG), 1.0f);
				m_SSAAOutBuffer[indexC] = min(m_SSAAOutBuffer[indexC] + (zValue * fogB), 1.0f);
			}
		}
	}

	bool superSamling = true;
	if (superSamling) {
		for (int x = 0; x < m_OutBufferWidth; ++x) {
			for (int y = 0; y < m_OutBufferHeight; ++y) {
				m_outBuffer[INDEX(m_OutBufferWidth, x, y, 0)] = findSSAAOfColorElement(m_SSAAMultiplier, x, y, 0);
				m_outBuffer[INDEX(m_OutBufferWidth, x, y, 1)] = findSSAAOfColorElement(m_SSAAMultiplier, x, y, 1);
				m_outBuffer[INDEX(m_OutBufferWidth, x, y, 2)] = findSSAAOfColorElement(m_SSAAMultiplier, x, y, 2);
			}
		}
	}
}

float Renderer::findSSAAOfColorElement(int multiplier, int x, int y, int colorElement) {
	GLfloat sum = -1;
	for (int i = 0; i < multiplier; ++i) {
		for (int j = 0; j < multiplier; ++j) {
			if (sum == -1) { // first pixel
				sum = m_SSAAOutBuffer[INDEXSSAA(m_SSAAOutBufferWidth, ((x * multiplier) + i), ((y * multiplier) + j), colorElement)];
			} else { // the rest of the pixels
				sum += m_SSAAOutBuffer[INDEXSSAA(m_SSAAOutBufferWidth, ((x * multiplier) + i), ((y * multiplier) + j), colorElement)];
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

GLfloat Renderer::getZ(vec2 p3, vec2 p2, vec2 p1, vec2 ps, vec4 z3, vec4 z2, vec4 z1) {
	GLfloat aOne;
	GLfloat cOne;
	GLfloat aTwo = ((p2.y - p1.y) / (p2.x - p1.x));
	GLfloat cTwo = (p2.y - (((p2.y - p1.y) / (p2.x - p1.x)) * p2.x));
	GLfloat piX;
	vec2 pi;
	if (p3.x == ps.x) {
		pi = vec2(p3.x, ((aTwo * p3.x) + cTwo));
	} else {
		aOne = ((p3.y - ps.y) / (p3.x - ps.x));
		cOne = (p3.y - (((p3.y - ps.y) / (p3.x - ps.x)) * p3.x));
		piX = ((cTwo - cOne) / (aOne - aTwo));
		pi = vec2(piX, ((aOne * piX) + cOne));
	}
	GLfloat ti = (length(pi - p1) / length(p2 - p1));
	vec4 zi = ((z2 * ti) + (z1 * (1.0f - ti)));
	GLfloat t = (length(ps - p3) / length(pi - p3));
	vec4 zp = ((z3 * (1.0f - t)) + (zi * t));
	
	return zp.z;
}


float Renderer::sign(vec2& p1, vec3& p2, vec3& p3) const{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

vec2 Renderer::vec4toVec2(const vec4 v){
	return vec2(v.x, v.y);
}

int Renderer::GetWidth() {
	return m_OutBufferWidth;
}

int Renderer::GetHeight() {
	return m_OutBufferHeight;
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_OutBufferWidth, m_OutBufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, m_OutBufferWidth, m_OutBufferHeight);
}

void Renderer::SwapBuffers()
{

	int a = glGetError();
	glActiveTexture(GL_TEXTURE0);
	a = glGetError();
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	a = glGetError();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_OutBufferWidth, m_OutBufferHeight, GL_RGB, GL_FLOAT, m_outBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	a = glGetError();

	glBindVertexArray(gScreenVtc);
	a = glGetError();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	a = glGetError();
	glutSwapBuffers();
	a = glGetError();
}









vec3& Polygon3::calculateColor(vector<Light*>* lights, mat4& world_to_camera,
	float &AmbientIntensity, vec3& location, vec3& normal){
	tmpColor = baseColor;

	for (int j = 0; j < lights->size(); ++j){
		world_to_camera.MultiplyVec((*lights)[j]->location, temVec);
		temVec /= temVec.w;

		if ((*lights)[j]->lightType == LIGHT_POINT)
			l = normalize(vec4TOvec3(temVec) - location); //temVec used here as the light's location.
		else //PARRALEL LIGHT
			l = -vec4TOvec3(temVec);
		teta = dot(l, normal);
		// for debbuging:
		/*if (teta>0.77){
			tmpColor = vec3(1,0,0);
		}
		else if (teta > 0){
			tmpColor = vec3(1, 1, 1);
		}
		else if (teta > -0.77){
			tmpColor = vec3(0, 1, 0);
		}
		else{
			tmpColor = vec3(0, 0, 0);
		}*/
		r = normalize((2 * teta * normal) - l);
		e = normalize(eye - location);
		if (material==DIFFUSE || material == ALL)
		tmpColor += tmpColor *  max(1, 2 / AmbientIntensity) * max(0, teta) * (*lights)[j]->color;
		if (dot(r, n)>0 && (material == SPECULAR || material == ALL))
			tmpColor += tmpColor * max(1, 2 / AmbientIntensity) * pow(max(0, dot(r, e)), 10) * (*lights)[j]->color;

	}
	return tmpColor;
}



vec3& Polygon3::calculateFaceColor(vector<Light*>* lights, mat4& world_to_camera, float& AmbientIntensity){
	if (faceColorWasAlreadyCalculated)
		return facecolor;
	facecolor = calculateColor(lights, world_to_camera, AmbientIntensity, vec4TOvec3((a + b + c) / 3), n);
	faceColorWasAlreadyCalculated = true;
	return facecolor;
}

void Polygon3::calculateVertexColors(vector<Light*>* lights, mat4& world_to_camera, float &AmbientIntensity){
	if (verticesColorsWasAlreadyCalculated)
		return;
	aColor = calculateColor(lights, world_to_camera, AmbientIntensity, vec4TOvec3(a), vec4TOvec3(na));
	bColor = calculateColor(lights, world_to_camera, AmbientIntensity, vec4TOvec3(b), vec4TOvec3(nb));
	cColor = calculateColor(lights, world_to_camera, AmbientIntensity, vec4TOvec3(c), vec4TOvec3(nc));
	verticesColorsWasAlreadyCalculated = true;
}

vec3& Polygon3::getNormalOfBaricentricLocation(float& alpha1, float& alpha2, float& alpha3){
	// return it using tmpNormal
	tmpNormal = normalize(vec4TOvec3(alpha3 * na + alpha1 * nb + alpha2 * nc));
	//tmpNormal /= tmpNormal.w;
	return tmpNormal;
}