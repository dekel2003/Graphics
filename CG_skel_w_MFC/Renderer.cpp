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
	projectionMatrix[0][0] = 0;
	projectionMatrix *= 1000; //scaling
	projectionMatrix[0][3] = m_width/2; //translations
	projectionMatrix[1][3] = m_width/2;
	projectionMatrix[2][3] = m_width/2;
	projectionMatrix[3][3] = 1;
}



void Renderer::CreateBuffers(int width, int height)
{
	m_width=width;
	m_height=height;	
	CreateOpenGLBuffer(); //Do not remove this line.
	m_outBuffer = new float[3*m_width*m_height];
}

void Renderer::SetDemoBuffer()
{
	//vertical line
	for(int i=0; i<m_width; i++)
	{
		m_outBuffer[INDEX(m_width,256,i,0)]=1;	m_outBuffer[INDEX(m_width,256,i,1)]=0;	m_outBuffer[INDEX(m_width,256,i,2)]=0;

	}


	//horizontal line
	for(int i=0; i<m_width; i++)
	{
		m_outBuffer[INDEX(m_width,i,256,0)]=1;	m_outBuffer[INDEX(m_width,i,256,1)]=0;	m_outBuffer[INDEX(m_width,i,256,2)]=1;

	}

	//diagonal line
	for (int i = 0; i<m_width; i++)
	{
		m_outBuffer[INDEX(m_width, i, i, 0)] = 0.5;	m_outBuffer[INDEX(m_width, i, i, 1)] = 1;	m_outBuffer[INDEX(m_width, i, i, 2)] = 0.5;

	}

}

void Renderer::DrawLine(vec4 a, vec4 b){
	//TODO: Takes to 2d vectors and draws line betwen them
	for (int t = 0; t<m_width; t++)
	{
		vec4 tmpVec = (1 - (GLfloat)t / m_width) * a + ((GLfloat)t / m_width) * b;
		int px = (int)tmpVec.y;
		int py = (int)tmpVec.z;
		m_outBuffer[INDEX(m_width, px, py, 0)] = 1;	m_outBuffer[INDEX(m_width, px, py, 1)] = 1;	m_outBuffer[INDEX(m_width, px, py, 2)] = 1;

	}
}

void Renderer::DrawTriangles(const vector<vec3>* vertices, const vector<vec3>* normals){
	//vertical line

	for (vector<vec3>::const_iterator it = vertices->begin(); it != vertices->end(); ++it){
		vec4 a, b, c;
		a = projectionMatrix * changeVec3toVec4(*it++);
		b = projectionMatrix * changeVec3toVec4(*it++);
		c = projectionMatrix * changeVec3toVec4(*it);
		DrawLine(a, b);
		DrawLine(b, c);
		DrawLine(c, a);
	}

}

vec4 Renderer::changeVec3toVec4(const vec3 v){
	return vec4(v.x, v.y, v.z, 1);
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