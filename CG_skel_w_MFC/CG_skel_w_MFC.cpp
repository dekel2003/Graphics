// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CG_skel_w_MFC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/freeglut_ext.h"
#include "vec.h"
#include "mat.h"
#include "InitShader.h"
#include "Scene.h"
#include "Renderer.h"
#include <string>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

#define FILE_OPEN 1
#define SELECT_MESH 1
#define VIEW_ORTHOGONAL 1
#define VIEW_PERSPECTIVE 2
#define MAIN_DEMO 3
#define MAIN_ABOUT 4

Scene *scene;
Renderer *renderer;
int menuMesh, mainMenuRef, menuCamera, menuView;
char c[2];
int last_x,last_y;
bool lb_down,rb_down,mb_down;

//----------------------------------------------------------------------------
// Callbacks



void display( void )
{
//Call the scene and ask it to draw itself
	scene->draw();
}

void reshape( int width, int height )
{
//update the renderer's buffers
	renderer->CreateBuffers(width, height);
}

void keyboard( unsigned char key, int x, int y )
{
	switch ( key ) {
	case 033:
		exit( EXIT_SUCCESS );
		break;
	}
}

void motion(int x, int y)
{
	// calc difference in mouse movement
	int dx = x - last_x;
	int dy = y - last_y;
	// update last x,y
	last_x = x;
	last_y = y;

	cout << '(' << dx << ',' << dy << ')' << endl;

	static int updateCounter = 0;
	if (lb_down){
		updateCounter = (++updateCounter) % 10; // updating screen every frame is too fast.
		scene->moveCurrentModel(dx, -dy);
		if (!updateCounter)
			display();
	}
}

void mouse(int button, int state, int x, int y)
{
	//button = {GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON}
	//state = {GLUT_DOWN,GLUT_UP}
	
	motion(x, y);
	//set down flags
	switch(button) {
		case GLUT_LEFT_BUTTON:
			lb_down = (state==GLUT_UP)?0:1;
			break;
		case GLUT_RIGHT_BUTTON:
			rb_down = (state==GLUT_UP)?0:1;
			break;
		case GLUT_MIDDLE_BUTTON:
			mb_down = (state==GLUT_UP)?0:1;	
			break;
			
		case 3: // roll in
			scene->zoomIn();
			display();
			break;
		case 4: // roll out
			scene->zoomOut();
			display();
			break;
	}

	// add your code
}


//----------------------- Menus ------------------------------------


void fileMenu(int id)
{
	switch (id)
	{
		case FILE_OPEN:
			CFileDialog dlg(TRUE,_T(".obj"),NULL,NULL,_T("*.obj|*.*"));
			if(dlg.DoModal()==IDOK)
			{
				std::string s((LPCTSTR)dlg.GetPathName());
				scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
			}
			break;
	}
}

void mainMenu(int id)
{
	switch (id)
	{
	case MAIN_DEMO:
		scene->drawDemo();
		break;
	case MAIN_ABOUT:
		AfxMessageBox(_T("Computer Graphics"));
		break;
	}
}

void meshMenu(int id)
{
	scene->activeModel = id;
}

void cameraMenu(int id)
{
	if (id == 0){
		//TODO: add new camera
	}
	else
	{
		scene->activeCamera = id - 1;
	}
}

void viewMenu(int id)
{
	switch (id)
	{
	case VIEW_ORTHOGONAL:
		scene->setOrthogonalView();
		break;
	case VIEW_PERSPECTIVE:
		scene->setPerspectiveView();
		break;
	}
}

void addMeshToMenu(){
	static int numMeshes = 0;
	glutSetMenu(menuMesh);
	sprintf(c, "%s", to_string(numMeshes).c_str());
	cout << to_string(numMeshes) << endl << to_string(numMeshes).c_str() << endl;
	glutAddMenuEntry(c, numMeshes);
	glutSetMenu(mainMenuRef);
	glutChangeToSubMenu(menuMesh, "Choose Model", menuMesh);
	numMeshes++;
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void initMenu()
{

	int menuFile = glutCreateMenu(fileMenu);
	glutAddMenuEntry("Open..",FILE_OPEN);
	menuMesh = glutCreateMenu(meshMenu);
	menuView = glutCreateMenu(viewMenu);
	glutAddMenuEntry("Orthogonal", 0);
	glutAddMenuEntry("Perspective", 1);
	menuCamera = glutCreateMenu(cameraMenu);
	glutAddMenuEntry("Add camera", 0);
	glutAddMenuEntry("0", 1);
	mainMenuRef = glutCreateMenu(mainMenu);
	glutAddSubMenu("File",menuFile);
	glutAddSubMenu("Choose Model", menuMesh);
	glutAddSubMenu("Choose Camera", menuCamera);
	glutAddSubMenu("Choose View", menuView);
	glutAddMenuEntry("Demo",MAIN_DEMO);
	glutAddMenuEntry("About",MAIN_ABOUT);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	//glutAddMenuEntry("0", 0);
	//glutAddMenuEntry("1", 1);
	//glutAddMenuEntry("2", 2);
}


//----------------------------------------------------------------------------


int my_main( int argc, char **argv )
{
	//----------------------------------------------------------------------------
	// Initialize window
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA| GLUT_DOUBLE);
	glutInitWindowSize( 512, 512 );
	glutInitContextVersion( 3, 2 );
	glutInitContextProfile( GLUT_CORE_PROFILE );
	glutCreateWindow( "CG" );
	glewExperimental = GL_TRUE;
	glewInit();
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		/*		...*/
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	
	renderer = new Renderer(512,512);
	scene = new Scene(renderer);
	scene->setOrthogonalView();


	//Initialize Callbacks

	glutDisplayFunc( display );

	glutKeyboardFunc( keyboard );
	glutMouseFunc( mouse );
	glutMotionFunc ( motion );
	glutReshapeFunc( reshape );
	//TODO glutIdleFunc(); if no event occurs, can do optimizations
	initMenu();
	glutMainLoop();
	delete scene;
	delete renderer;
	return 0;
}

using namespace std;

int main( int argc, char **argv )
{
	int nRetCode = 0;
	
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		my_main(argc, argv );
	}
	
	return nRetCode;
}
