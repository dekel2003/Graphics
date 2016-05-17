﻿#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"

#define INDEX(width,x,y,c) (x+y*width)*3+c
#define INDEXZ(width,x,y) (x+y*width)

Renderer::Renderer() :m_width(512), m_height(512)
{
	InitOpenGLRendering();
	CreateBuffers(512,512);
	Init();
}

Renderer::Renderer(int width, int height) : m_width(width), m_height(height)
{
	InitOpenGLRendering();
	CreateBuffers(width,height);
	Init();
}

Renderer::~Renderer(void)
{
}

void Renderer::Init(){
	R = B = G = 0.5;
	Invalidate();
}

void Renderer::CreateBuffers(int width, int height)
{
	m_width = width;
	m_height = height;
	m_TotalNumberOfPixels = (m_width * m_height);
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3 * m_TotalNumberOfPixels];
	m_zbuffer = new float[m_TotalNumberOfPixels];
}

void Renderer::Invalidate(){
	for (int i = 0; i < 3 * m_width*m_height; ++i)
		m_outBuffer[i] = 0.1;
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

void Renderer::draw_line_antialias(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, color_component r, color_component g, color_component b) {
	double dx = (double)x2 - (double)x1;
	double dy = (double)y2 - (double)y1;
	if (fabs(dx) > fabs(dy)) {
		if (x2 < x1) {
			swap_(x1, x2);
			swap_(y1, y2);
		}
		double gradient = dy / dx;
		double xend = round_(x1);
		double yend = y1 + gradient*(xend - x1);
		double xgap = rfpart_(x1 + 0.5);
		int xpxl1 = xend;
		int ypxl1 = ipart_(yend);
		plot_(xpxl1, ypxl1, rfpart_(yend)*xgap);
		plot_(xpxl1, ypxl1 + 1, fpart_(yend)*xgap);
		double intery = yend + gradient;

		xend = round_(x2);
		yend = y2 + gradient*(xend - x2);
		xgap = fpart_(x2 + 0.5);
		int xpxl2 = xend;
		int ypxl2 = ipart_(yend);
		plot_(xpxl2, ypxl2, rfpart_(yend) * xgap);
		plot_(xpxl2, ypxl2 + 1, fpart_(yend) * xgap);

		int x;
		for (x = xpxl1 + 1; x <= (xpxl2 - 1); x++) {
			plot_(x, ipart_(intery), rfpart_(intery));
			plot_(x, ipart_(intery) + 1, fpart_(intery));
			intery += gradient;
		}
	}
	else {
		if (y2 < y1) {
			swap_(x1, x2);
			swap_(y1, y2);
		}
		double gradient = dx / dy;
		double yend = round_(y1);
		double xend = x1 + gradient*(yend - y1);
		double ygap = rfpart_(y1 + 0.5);
		int ypxl1 = yend;
		int xpxl1 = ipart_(xend);
		plot_(xpxl1, ypxl1, rfpart_(xend)*ygap);
		plot_(xpxl1, ypxl1 + 1, fpart_(xend)*ygap);
		double interx = xend + gradient;

		yend = round_(y2);
		xend = x2 + gradient*(yend - y2);
		ygap = fpart_(y2 + 0.5);
		int ypxl2 = yend;
		int xpxl2 = ipart_(xend);
		plot_(xpxl2, ypxl2, rfpart_(xend) * ygap);
		plot_(xpxl2, ypxl2 + 1, fpart_(xend) * ygap);

		int y;
		for (y = ypxl1 + 1; y <= (ypxl2 - 1); y++) {
			plot_(ipart_(interx), y, rfpart_(interx));
			plot_(ipart_(interx) + 1, y, fpart_(interx));
			interx += gradient;
		}
	}
}
#undef swap_
#undef plot_
#undef ipart_
#undef fpart_
#undef round_
#undef rfpart_

void Renderer::DrawLine(vec2 a, vec2 b) {
	draw_line_antialias(a.x, a.y, b.x, b.y, R, G, B);
}

void Renderer::drawPoint(int x, int y, GLfloat intensity) {
	/*m_outBuffer[INDEX(m_width, x, y, 0)] = R;
	m_outBuffer[INDEX(m_width, x, y, 1)] = G;
	m_outBuffer[INDEX(m_width, x, y, 2)] = B;*/
	/*m_outBuffer[INDEX(m_width, x, y, 0)] = ((m_outBuffer[INDEX(m_width, x, y, 0)] * intensity) + (R * (1.0f - intensity)));
	m_outBuffer[INDEX(m_width, x, y, 1)] = ((m_outBuffer[INDEX(m_width, x, y, 1)] * intensity) + (G * (1.0f - intensity)));
	m_outBuffer[INDEX(m_width, x, y, 2)] = ((m_outBuffer[INDEX(m_width, x, y, 2)] * intensity) + (B * (1.0f - intensity)));*/
	m_outBuffer[INDEX(m_width, x, y, 0)] = min(m_outBuffer[INDEX(m_width, x, y, 0)] + R, 255);
	m_outBuffer[INDEX(m_width, x, y, 1)] = min(m_outBuffer[INDEX(m_width, x, y, 1)] + G, 255);
	m_outBuffer[INDEX(m_width, x, y, 2)] = min(m_outBuffer[INDEX(m_width, x, y, 2)] + B, 255);
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


void Renderer::DrawTriangles(const vector<vec4>* vertices, const vector<vec3>* normals){
	mat4 objectToCamera = world_to_camera * object_to_world;
	mat4 objectToClip = projectionMatrix * objectToCamera;
	int numberOfVertices = vertices->size();
	vec4 currentVertice;

	// Transfermations for the Normal colors
	vector<vec4> clippedVertices;
	clippedVertices.reserve(numberOfVertices);
	for (int i = 0; i < numberOfVertices; ++i){
		objectToClip.MultiplyVec((*vertices)[i], currentVertice);
		currentVertice.x = m_width*(currentVertice.x + 1) / 2;
		currentVertice.y = m_width*(currentVertice.y + 1) / 2;
		clippedVertices.push_back(currentVertice);
	}

	// Transfermations for the Z-Buffer
	vector<vec4> cameraVertices;
	cameraVertices.reserve(numberOfVertices);
	for (int i = 0; i < numberOfVertices; ++i){
		objectToCamera.MultiplyVec((*vertices)[i], currentVertice);
		cameraVertices.push_back(currentVertice);
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
				if (PointInTriangle(currentPoint, pointA, pointB, pointC)) {
					pointsInsideTriangle.push_back(currentPoint);
				}
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
	GLfloat tempR = R * 255;
	GLfloat tempG = G * 255;
	GLfloat tempB = B * 255;
	// Normal colors drawing
	for (int i = 0; i < numberOfVertices; ++i) {
		vec2 a, b, c;
		a = vec4toVec2(clippedVertices[i++]);
		b = vec4toVec2(clippedVertices[i++]);
		c = vec4toVec2(clippedVertices[i]);
		if (a.x < 0 || b.x < 0 || c.x < 0 || a.y < 0 || b.y < 0 || c.y < 0 ||
			a.x > m_width || b.x >= m_width || c.x >= m_width || a.y >= m_height || b.y >= m_height || c.y >= m_height){
			continue;
		}
		setColor(tempR, tempG, tempB);
		DrawLine(a, b);
		setColor(tempR, tempG, tempB);
		DrawLine(b, c);
		setColor(tempR, tempG, tempB);
		DrawLine(c, a);
	}
}

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

bool Renderer::PointInTriangle(vec2 pt, vec4 v1, vec4 v2, vec4 v3) {
	bool b1, b2, b3;

	b1 = sign(pt, v1, v2) < 0.0f;
	b2 = sign(pt, v2, v3) < 0.0f;
	b3 = sign(pt, v3, v1) < 0.0f;

	return ((b1 == b2) && (b2 == b3));
}

float Renderer::sign(vec2 p1, vec4 p2, vec4 p3) {
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