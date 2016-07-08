#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"

#include <algorithm>
#include <set>
#include <list>
#include <sstream>

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
	//InitOpenGLRendering();
	CreateBuffers();
	Init();
}

Renderer::~Renderer(void) {}

void Renderer::Init() {
	R = B = G = 0.5;
	glGenVertexArrays(1, &VAO);
	glGenVertexArrays(1, &VAOLines);
	glGenVertexArrays(1, &VAOModelLines);

	program = InitShader("vshader.glsl", "fshader.glsl");
	skyBoxProgram = InitShader("vshaderskybox.glsl", "fshaderskybox.glsl");

	// Creates the Skybox object
	CreateSkyBoxObject();
	// Loads Cubemap
	vector<const GLchar*> faces;
	faces.push_back("posx.jpg");
	faces.push_back("negx.jpg");
	faces.push_back("posy.jpg");
	faces.push_back("negy.jpg");
	faces.push_back("posz.jpg");
	faces.push_back("negz.jpg");
	LoadCubemap(faces);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glutSwapBuffers();
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
	lines.clear();
	model_lines.clear();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	drawingColor = vec3(red, green, blue);
}

void Renderer::DrawLineBetween3Dvecs(const vec4& _vecA, const vec4& _vecB, bool modelOriented){


	if (modelOriented){
		model_lines.push_back(_vecA);
		model_lines.push_back(_vecB);
	}
	else{
		lines.push_back(_vecA);
		lines.push_back(_vecB);
	}

	//Function for Coordinate System for now
	/*vec4 vecA, vecB;
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
	

	GLuint numberOfVertices = 2;
	vector<vec4>* vertices = new vector<vec4>(2);
	vertices->push_back(_vecA);
	vertices->push_back(_vecB);

	glBindVertexArray(VAOLines);
	GLuint VBO;
	glGenBuffers(1, &VBO);
	// 2. Copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, numberOfVertices * sizeof(vec4), &(*vertices)[0], GL_STATIC_DRAW);

	// 3. Then set the vertex attributes pointers
	GLint  vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

	// 4. Unbind VAOLines
	glBindVertexArray(0);
	*/
}

/*vec4 v0, v1, v2;
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
	result = (a1 >= 0 && a2 >= 0 && a3 >= 0);
}

inline GLfloat Depth(Polygon3* P, vec2& p){
	Barycentric(p, P->ma, P->mb, P->mc);
	return (a3 * P->a.z + a1 * P->b.z + a2 *P->c.z);
}
*/

GLuint Renderer::AddTriangles(const vector<vec4>* vertices, const vec3 color,
	const vector<vec3>* normals, const vector<vec3>* normals2vertices, const vector<vec2>* textures, Material material){
	//objectToCamera = world_to_camera * object_to_world;
	//mat4 normalToCamera = world_to_camera * normalTransform;
	//mat4 objectToClip = projectionMatrix * objectToCamera;
	int numberOfVertices = vertices->size();
	//totalNumberOfVertices += numberOfVertices;
		GLuint VBO;
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, numberOfVertices * (sizeof(vec4) + (2 * sizeof(vec3)) + sizeof(vec2)), NULL, GL_STATIC_DRAW);
		
		glBufferSubData(GL_ARRAY_BUFFER, 0, numberOfVertices * sizeof(vec4), &((*vertices)[0]));
		GLint  vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		size_t start = numberOfVertices * sizeof(vec4);
		if (normals2vertices){
			glBufferSubData(GL_ARRAY_BUFFER, numberOfVertices * sizeof(vec4), numberOfVertices * sizeof(vec3), &((*normals2vertices)[0]));
			GLint  nPosition = glGetAttribLocation(program, "nPosition");
			glEnableVertexAttribArray(nPosition);
			glVertexAttribPointer(nPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(start));
		}

		if (normals){
			vector<vec3> fNormals;
			fNormals.reserve(numberOfVertices);
			for (vec3 nn : *normals){
				fNormals.push_back(nn);
				fNormals.push_back(nn);
				fNormals.push_back(nn);
			}
			start = numberOfVertices * (sizeof(vec4) + sizeof(vec3));
			glBufferSubData(GL_ARRAY_BUFFER, start, numberOfVertices * sizeof(vec3), &((fNormals)[0]));
			GLint  nPosition = glGetAttribLocation(program, "fnPosition");
			glEnableVertexAttribArray(nPosition);
			glVertexAttribPointer(nPosition, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(start));
		}

		if (textures) {
			start = numberOfVertices * (sizeof(vec4) + sizeof(vec3) + sizeof(vec3));
			glBufferSubData(GL_ARRAY_BUFFER, start, numberOfVertices * sizeof(vec2), &((*textures)[0]));
			GLint  texCoord = glGetAttribLocation(program, "texCoord");
			glEnableVertexAttribArray(texCoord);
			glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(start));
		}

		//GLint  vTexCoord = glGetAttribLocation(program, "vTexCoord");
		//glEnableVertexAttribArray(vTexCoord);
		//glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0,
		//	(GLvoid *) sizeof(vtc));
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);
		//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);
		//glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);


		int err = glGetError();

	return VBO;

	/*vec4 currentVertice, currentVerticeZ_A, currentVerticeZ_B, currentVerticeZ_C, currentNormal;
	vec4 PolygonVNormals[3];
	vec3 polygonColor;
	Polygon3 P;*/
	/*vector<vec4> cameraVertices;
	vector<vec4> clippedVertices;
	clippedVertices.reserve(numberOfVertices);
	cameraVertices.reserve(numberOfVertices);*/

	
	/*vector<Polygon3> currentClippedPolygon3;
	currentClippedPolygon3.reserve(numberOfVertices / 3);*/
	
	/*
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
		//currentClippedPolygon3.push_back(P);
		globalClippedPolygon3.push_back(P);
	}
	*/
}

/*
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
					if (z < m_zbuffer[INDEXZ(m_SSAAOutBufferWidth, x, y)]){
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
*/

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


vec2 Renderer::vec4toVec2(const vec4 v){
	return vec2(v.x, v.y);
}
*/

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
	/*
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
	*/
}

void Renderer::CreateOpenGLBuffer()
{
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, gScreenTex);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_OutBufferWidth, m_OutBufferHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, m_OutBufferWidth, m_OutBufferHeight);
}

void Renderer::EnableTexture() {
	m_UseTexture = true;
	glUniform1i(transformIdUseTexture, m_UseTexture ? 1 : 0);
}

void Renderer::DisableTexture() {
	m_UseTexture = false;
	glUniform1i(transformIdUseTexture, m_UseTexture ? 1 : 0);
}

void Renderer::EnableNormalMapping() {
	m_UseNormalMapping = true;
	glUniform1i(transformIdUseNormalMapping, m_UseNormalMapping ? 1 : 0);
}

void Renderer::DisableNormalMapping() {
	m_UseNormalMapping = false;
	glUniform1i(transformIdUseNormalMapping, m_UseNormalMapping ? 1 : 0);
}

void Renderer::EnableEnvironmentMapping() {
	m_UseEnvironmentMapping = true;
	glUniform1i(transformIdUseEnvironmentMapping, m_UseEnvironmentMapping ? 1 : 0);
}

void Renderer::DisableEnvironmentMapping() {
	m_UseEnvironmentMapping = false;
	glUniform1i(transformIdUseEnvironmentMapping, m_UseEnvironmentMapping ? 1 : 0);
}

void Renderer::drawModelsLines(){
	////////////////////////////////////////// draw all model's lines
	GLuint linesSize = model_lines.size();
	if (linesSize != 0){
		//while (lines.size() != 0){
		//vec4 _vecA = lines[0];
		//vec4 _vecB = lines[1];
		//lines.pop_back();
		//lines.pop_back();

		glBindVertexArray(VAOModelLines);
		GLuint VBO;
		glGenBuffers(1, &VBO);
		// 2. Copy our vertices array in a vertex buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, linesSize * sizeof(vec4), &(model_lines)[0], GL_STATIC_DRAW);

		// 3. Then set the vertex attributes pointers
		GLint  vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);


		vec3 color = (drawingColor / 512 + vec3(0.01))*AmbientIntensity;

		GLuint transformId = glGetUniformLocation(program, "Tmodel");
		glUniformMatrix4fv(transformId, 1, GL_TRUE, &(*object_to_world[0]));


		transformId = glGetUniformLocation(program, "Tcamera");
		glUniformMatrix4fv(transformId, 1, GL_TRUE, &(*world_to_camera[0]));

		transformId = glGetUniformLocation(program, "Tprojection");
		glUniformMatrix4fv(transformId, 1, GL_TRUE, &(*projectionMatrix[0]));

		transformId = glGetUniformLocation(program, "MyColor");
		glUniform3f(transformId, 1, 0, 0);

		transformIdUseTexture = glGetUniformLocation(program, "useTexture");
		glUniform1i(transformIdUseTexture, m_UseTexture ? 1 : 0);

		transformIdUseNormalMapping = glGetUniformLocation(program, "useNormalMapping");
		glUniform1i(transformIdUseNormalMapping, m_UseNormalMapping ? 1 : 0);

		glDrawArrays(GL_LINES, 0, linesSize);


		// 4. Unbind VAOLines
		glBindVertexArray(0);
		model_lines.clear();
	}
	/////////////////////////////////////////////////////////////////////
}

void Renderer::SwapBuffers()
{
	int a = glGetError();
	/*
	glActiveTexture(GL_TEXTURE0);
	a = glGetError();
	glBindTexture(GL_TEXTURE_2D, gScreenTex);
	a = glGetError();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_OutBufferWidth, m_OutBufferHeight, GL_RGB, GL_FLOAT, m_outBuffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	a = glGetError();
	glBindVertexArray(gScreenVtc);
	a = glGetError();
	*/

	


	/*
	glBindVertexArray(VAOLines);

	transformId = glGetUniformLocation(program, "Tmodel");
	glUniformMatrix4fv(transformId, 1, GL_TRUE, vec4());

	transformId = glGetUniformLocation(program, "Tcamera");
	glUniformMatrix4fv(transformId, 1, GL_TRUE, &(*world_to_camera[0]));

	transformId = glGetUniformLocation(program, "Tprojection");
	glUniformMatrix4fv(transformId, 1, GL_TRUE, &(*projectionMatrix[0]));

	transformId = glGetUniformLocation(program, "MyColor");
	glUniform3f(transformId, R,G,B);

	glDrawArrays(GL_LINE_STRIP, 0, 20);
	a = glGetError();
	*/


	////////////////////////////////////////// draw all lines
	GLuint linesSize = lines.size();

	if (linesSize != 0){
	//while (lines.size() != 0){
		//vec4 _vecA = lines[0];
		//vec4 _vecB = lines[1];
		//lines.pop_back();
		//lines.pop_back();

		glBindVertexArray(VAOLines);
		GLuint VBO;
		glGenBuffers(1, &VBO);
		// 2. Copy our vertices array in a vertex buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, linesSize * sizeof(vec4), &(lines)[0], GL_STATIC_DRAW);

		// 3. Then set the vertex attributes pointers
		GLint vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);


		vec3 color = (drawingColor / 512 + vec3(0.01))*AmbientIntensity;

		GLuint transformId = glGetUniformLocation(program, "Tmodel");
		glUniformMatrix4fv(transformId, 1, GL_TRUE, &(mat4()[0][0]));

		transformId = glGetUniformLocation(program, "Tcamera");
		glUniformMatrix4fv(transformId, 1, GL_TRUE, &(*world_to_camera[0]));

		transformId = glGetUniformLocation(program, "Tprojection");
		glUniformMatrix4fv(transformId, 1, GL_TRUE, &(*projectionMatrix[0]));

		transformId = glGetUniformLocation(program, "MyColor");
		glUniform3f(transformId, 1, 0, 0);

		glDrawArrays(GL_LINES, 0, linesSize);


		// 4. Unbind VAOLines
		glBindVertexArray(0);
	}
/////////////////////////////////////////////////////////////////////



	glBindVertexArray(0);
	glutSwapBuffers();
	a = glGetError();
}

void Renderer::loadTexture(GLuint& texture, const char* fileName, int myGL_TEXTURE){
	glActiveTexture(myGL_TEXTURE);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int width, height;
	unsigned char* image = SOIL_load_image(fileName, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::LoadCubemap(vector<const GLchar*> faces) {
	//glUseProgram(skyBoxProgram);
	glActiveTexture(GL_TEXTURE2);
	glGenTextures(1, &m_TextureCubemapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureCubemapID);

	int width, height;
	unsigned char* image;

	for (GLuint i = 0; i < faces.size(); ++i) {
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Renderer::CreateSkyBoxObject() {
#pragma region "object_initialization"
	// Set the object data (buffers, vertex attributes)
	GLfloat skyboxSize = 1.0f;
	GLfloat skyboxVertices[] = {
		// Positions          
		-skyboxSize, skyboxSize, -skyboxSize,
		-skyboxSize, -skyboxSize, -skyboxSize,
		skyboxSize, -skyboxSize, -skyboxSize,
		skyboxSize, -skyboxSize, -skyboxSize,
		skyboxSize, skyboxSize, -skyboxSize,
		-skyboxSize, skyboxSize, -skyboxSize,

		-skyboxSize, -skyboxSize, skyboxSize,
		-skyboxSize, -skyboxSize, -skyboxSize,
		-skyboxSize, skyboxSize, -skyboxSize,
		-skyboxSize, skyboxSize, -skyboxSize,
		-skyboxSize, skyboxSize, skyboxSize,
		-skyboxSize, -skyboxSize, skyboxSize,

		skyboxSize, -skyboxSize, -skyboxSize,
		skyboxSize, -skyboxSize, skyboxSize,
		skyboxSize, skyboxSize, skyboxSize,
		skyboxSize, skyboxSize, skyboxSize,
		skyboxSize, skyboxSize, -skyboxSize,
		skyboxSize, -skyboxSize, -skyboxSize,

		-skyboxSize, -skyboxSize, skyboxSize,
		-skyboxSize, skyboxSize, skyboxSize,
		skyboxSize, skyboxSize, skyboxSize,
		skyboxSize, skyboxSize, skyboxSize,
		skyboxSize, -skyboxSize, skyboxSize,
		-skyboxSize, -skyboxSize, skyboxSize,

		-skyboxSize, skyboxSize, -skyboxSize,
		skyboxSize, skyboxSize, -skyboxSize,
		skyboxSize, skyboxSize, skyboxSize,
		skyboxSize, skyboxSize, skyboxSize,
		-skyboxSize, skyboxSize, skyboxSize,
		-skyboxSize, skyboxSize, -skyboxSize,

		-skyboxSize, -skyboxSize, -skyboxSize,
		-skyboxSize, -skyboxSize, skyboxSize,
		skyboxSize, -skyboxSize, -skyboxSize,
		skyboxSize, -skyboxSize, -skyboxSize,
		-skyboxSize, -skyboxSize, skyboxSize,
		skyboxSize, -skyboxSize, skyboxSize
	};

	// Setup skybox VAO
	//GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

#pragma endregion
}
/*
// Environment mapping

// Draw skybox first
glDepthMask(GL_FALSE);// Remember to turn depth writing off
glUseProgram(skyBoxProgram);
mat4 view = mat4(vec4(world_to_camera[0].x, world_to_camera[0].y, world_to_camera[0].z, 0),
vec4(world_to_camera[1].x, world_to_camera[1].y, world_to_camera[1].z, 0),
vec4(world_to_camera[2].x, world_to_camera[2].y, world_to_camera[2].z, 0),
vec4(0, 0, 0, 1));	// Remove any translation component of the view matrix
//mat4 projection = perspective(1, (float)m_OutBufferWidth / (float)m_OutBufferHeight, 0.1f, 100.0f);
glUniformMatrix4fv(glGetUniformLocation(skyBoxProgram, "view"), 1, GL_FALSE, &(*view[0]));
glUniformMatrix4fv(glGetUniformLocation(skyBoxProgram, "projection"), 1, GL_FALSE, &(*projectionMatrix[0]));
glUniformMatrix4fv(glGetUniformLocation(skyBoxProgram, "model"), 1, GL_FALSE, &(*object_to_world[0]));

GLint current_vao;
glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current_vao);

// skybox cube
glBindVertexArray(skyboxVAO);
glActiveTexture(GL_TEXTURE2);
glUniform1i(glGetUniformLocation(skyBoxProgram, "skybox"), 0);
glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureCubemapID);
glDrawArrays(GL_TRIANGLES, 0, 36);
glBindVertexArray(0);
glDepthMask(GL_TRUE);

glBindVertexArray(current_vao);
*/
void Renderer::draw(){
	int a = glGetError();

	glUseProgram(program);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_CurrentTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_CurrentNormalTextureTexture);

	vec3 color = (drawingColor / 512 + vec3(0.01))*AmbientIntensity;

	GLuint transformId = glGetUniformLocation(program, "Tmodel");
	glUniformMatrix4fv(transformId, 1, GL_TRUE, &(*object_to_world[0]));

	transformId = glGetUniformLocation(program, "Tcamera");
	glUniformMatrix4fv(transformId, 1, GL_TRUE, &(*world_to_camera[0]));

	transformId = glGetUniformLocation(program, "Tprojection");
	glUniformMatrix4fv(transformId, 1, GL_TRUE, &(*projectionMatrix[0]));

	//glUniform3f(glGetUniformLocation(program, "cameraPos"), world_to_camera[0][3], world_to_camera[1][3], world_to_camera[2][3]); // For Environment mapping

	transformId = glGetUniformLocation(program, "MyColor");
	glUniform3f(transformId, color.x, color.y, color.z);


	transformId = glGetUniformLocation(program, "AmbientIntensity");
	glUniform1f(transformId, AmbientIntensity);

	//shadow

	transformId = glGetUniformLocation(program, "shadow");
	glUniform1i(transformId, shadow);

	// Texture

	transformId = glGetUniformLocation(program, "ourTexture");
	glUniform1i(transformId, 0);

	// Normal mapping

	transformId = glGetUniformLocation(program, "normalMap");
	glUniform1i(transformId, 1);

	transformIdUseTexture = glGetUniformLocation(program, "useTexture");
	glUniform1i(transformIdUseTexture, m_UseTexture ? 1 : 0);

	transformIdUseNormalMapping = glGetUniformLocation(program, "useNormalMapping");
	glUniform1i(transformIdUseNormalMapping, m_UseNormalMapping ? 1 : 0);

	transformIdUseEnvironmentMapping = glGetUniformLocation(program, "useEnvironmentMapping");
	glUniform1i(transformIdUseEnvironmentMapping, m_UseEnvironmentMapping ? 1 : 0);

	int numLights = min(lights->size(), 20);
	transformId = glGetUniformLocation(program, "numLights");
	glUniform1i(transformId, numLights);

	stringstream ss;
	for (int j = 0; j < numLights; ++j){
		ss << "lPosition[" << j << "]";
		GLuint lPositionId = glGetUniformLocation(program, ss.str().c_str());
		glUniform4fv(lPositionId, 1, &((*lights)[j]->location[0]));
		//cout << "[   " << (*lights)[j]->location << "   :   " << (*lights)[j]->color << "   ]" << endl;
		ss.str("");
		ss << "lColor[" << j << "]";
		GLuint lColor = glGetUniformLocation(program, ss.str().c_str());
		glUniform3fv(lColor, 1, &((*lights)[j]->color[0]));
		ss.str("");
	}

	glDrawArrays(GL_TRIANGLES, 0, totalNumberOfVertices);
	a = glGetError();

	// Environment mapping
	GLint current_vao;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current_vao);
	if (m_UseEnvironmentMapping) {

		/*glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureCubemapID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		*/
		
		glDepthFunc(GL_LEQUAL);
		glUseProgram(skyBoxProgram);
		
		mat4 view = mat4(vec4(world_to_camera[0].x, world_to_camera[0].y, world_to_camera[0].z, 0),
						 vec4(world_to_camera[1].x, world_to_camera[1].y, world_to_camera[1].z, 0),
						 vec4(world_to_camera[2].x, world_to_camera[2].y, world_to_camera[2].z, 0),
						 vec4(0, 0, 0, 1));	// Removes any translation component of the view matrix

		glUniformMatrix4fv(glGetUniformLocation(skyBoxProgram, "view"), 1, GL_TRUE, &(*view[0]));
		glUniformMatrix4fv(glGetUniformLocation(skyBoxProgram, "projection"), 1, GL_TRUE, &(*projectionMatrix[0]));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glUniform1i(glGetUniformLocation(skyBoxProgram, "skybox"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureCubemapID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		//glActiveTexture(GL_TEXTURE2);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureCubemapID);


		
		
		glBindVertexArray(current_vao);
		glUseProgram(program);
	}
}

void Renderer::SetCurrentTexture(GLuint texture) {
	m_CurrentTexture = texture;
}

void Renderer::SetCurrentNormalMappingTexture(GLuint texture) {
	m_CurrentNormalTextureTexture = texture;
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