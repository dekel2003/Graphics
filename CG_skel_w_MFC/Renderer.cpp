#include "StdAfx.h"
#include "Renderer.h"
#include "CG_skel_w_MFC.h"
#include "InitShader.h"
#include "GL\freeglut.h"

#define INDEX(width,x,y,c) (x+y*width)*3+c

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
	m_width=width;
	m_height=height;	
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3*m_width*m_height];
}

void Renderer::Invalidate(){
	for (int i = 0; i < 3 * m_width*m_height; i++)
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

	//vertical line
	//for(int i=0; i<m_width; i++)
	//{
	//	m_outBuffer[INDEX(m_width,256,i,0)]=1;	m_outBuffer[INDEX(m_width,256,i,1)]=0;	m_outBuffer[INDEX(m_width,256,i,2)]=0;

	//}


	////horizontal line
	//for(int i=0; i<m_width; i++)
	//{
	//	m_outBuffer[INDEX(m_width,i,256,0)]=1;	m_outBuffer[INDEX(m_width,i,256,1)]=0;	m_outBuffer[INDEX(m_width,i,256,2)]=1;

	//}

	////diagonal line
	//for (int i = 0; i<m_width; i++)
	//{
	//	m_outBuffer[INDEX(m_width, i, i, 0)] = 0.5;	m_outBuffer[INDEX(m_width, i, i, 1)] = 1;	m_outBuffer[INDEX(m_width, i, i, 2)] = 0.5;

	//}

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

void Renderer::setColor(int red, int green, int blue){
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
	/*
	vec2 n = normalize(a - b);

	//Check collision with sides

	float t = -500, s = -500, q = -500, r = -500;
	if (n.x != 0){
		float t = -a.x / n.x; // for point on the floor
		float q = (m_width - 1 - a.x) / n.x; // for point on the top
	}
	if (n.y != 0){
		float s = -a.y / n.y;  // for point on the left wall
		float r = (m_height - 1 - a.y) / n.y;  // for point on the right wall
	}

	vector<vec2> drawablePoints;
	int count = 0;
	float check = a.y + t*n.y;
	if (check >= 0 && check < m_height){
		drawablePoints.push_back(vec2(0, check));
	}
	 check = a.x + s*n.x;
	if (check >= 0 && check < m_width){
		drawablePoints.push_back(vec2(check, 0));
	}
	 check = a.y + q*n.y;
	if (check >= 0 && check < m_height){
		drawablePoints.push_back(vec2(m_width - 1, check));
	}
	 check = a.x + r*n.x;
	if (check >= 0 && check < m_width){
		drawablePoints.push_back(vec2(m_height - 1, check));
	}

	if (count >= 2){
		DrawLine(drawablePoints[0], drawablePoints[1]);
	}
	*/
}


void Renderer::DrawTriangles(const vector<vec4>* vertices, const vector<vec3>* normals){
	mat4 objectToClip = projectionMatrix * world_to_camera * object_to_world;

	vector<vec4> clipVertices;
	int count = 0;
	/*for(vector<vec4>::const_iterator it = vertices->begin(); it != vertices->end(); ++it){
		vec4 v = objectToClip *(*it);
		v /= v.w; // normalizing in accordance to it's weight.
		clipVertices.push_back(v);
	}*/
	vec4 v;
	int numberOfVertices = vertices->size();
	clipVertices.reserve(numberOfVertices);
	for (int i = 0; i < numberOfVertices; ++i){
		objectToClip.MultiplyVec((*vertices)[i], v);
		clipVertices.push_back(v);
	}

	count = clipVertices.size();

	//Now resizing according to screen
	for(int i = 0; i < count; ++i){
		clipVertices[i].x = m_width*(clipVertices[i].x + 1) / 2;
		clipVertices[i].y = m_height*(clipVertices[i].y + 1) / 2;
	}

	//Now Drawing
	for (int i = 0; i < count; ++i){
		vec2 a, b, c;
		a = vec4toVec2(clipVertices[i++]);
		b = vec4toVec2(clipVertices[i++]);
		c = vec4toVec2(clipVertices[i]);
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