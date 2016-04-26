// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CG_skel_w_MFC.h"
#include "InputDialog.h"

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
bool camIsFocused = false;

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

enum ROTATION{ NO_ROTATION, MODEL, WORLD };
static ROTATION rotation = NO_ROTATION;
void keyboard( unsigned char key, int x, int y )
{
	CFrustumDialog dlg;
	std::pair<vec3, vec3> v;
	float t = 50;
	switch ( key ) {
	case 033:
		exit( EXIT_SUCCESS );
		break;
	case 'z':
		if (rotation == MODEL){
			rotation = NO_ROTATION;
			cout << "rotation OFF" << endl;
			break;
		}
		rotation = MODEL;
		cout << "model rotation ON" << endl;
		break;
	case 'Z':
		if (rotation == WORLD){
			rotation = NO_ROTATION;
			cout << "rotation OFF" << endl;
			break;
		}
		rotation = WORLD;
		cout << "model world rotation ON" << endl;
		break;
	case 'q':
		
		if (dlg.DoModal() == IDOK){
			//string command = dlg.GetCmd();
			v = dlg.GetXYZ();
			cout << v.first << " --- " << v.second << endl;
								//	left       right       bottom       top         near       far
			//scene->setFrustum(v.first.y, v.second.y, v.first.x, v.second.x, v.first.z, v.second.z)
		}
		break;
	case 'r':
		if (dlg.DoModal() == IDOK) {
			//string command = dlg.GetCmd();
			v = dlg.GetXYZ();
			cout << v.first << " --- " << v.second << endl;
							//	left       right       bottom       top         near       far
			//scene->setOrtho(v.first.y, v.second.y, v.first.x, v.second.x, v.first.z, v.second.z)
		}
		break;
		
	case 'a':
		if (rotation == MODEL){
			scene->rotateCurrentCamera(t, 0);
			cout << "Camera rotating Left" << endl;
			display();
			break;
		}
		scene->moveCamera(t, 0);
		cout << "Camera moving Left" << endl;
		display();
		break;
	case 's':
		if (rotation == MODEL){
			scene->rotateCurrentCamera(0, t);
			cout << "Camera rotating Down" << endl;
			display();
			break;
		}
		cout << "Camera moving Down" << endl;
		scene->moveCamera(0, t);
		display();
		break;
	case 'd':
		if (rotation == MODEL){
			scene->rotateCurrentCamera(-t, 0);
			cout << "Camera rotating Right" << endl;
			display();
			break;
		}
		cout << "Camera moving Right" << endl;
		scene->moveCamera(-t, 0);
		display();
		break;
	case 'w':
		if (rotation == MODEL){
			scene->rotateCurrentCamera(0, -t);
			cout << "Camera rotating Up" << endl;
			display();
			break;
		}
		cout << "Camera moving up" << endl;
		scene->moveCamera(0, -t);
		display();
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
		if (rotation == MODEL){
			scene->rotateCurrentModel(dx, -dy);
		}
		else if (rotation == WORLD){
			scene->rotateCurrentModelWorld(dx, -dy);
		}
		else if (!camIsFocused){
			scene->moveCurrentModel(dx, -dy);
		}
		else{
			//scene->movecamera(dx, -dy);
		}
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
	CFrustumDialog dlg;
	std::pair<vec3, vec3> v;
	switch (id)
	{
	case VIEW_ORTHOGONAL:
		if (dlg.DoModal() == IDOK){
			v = dlg.GetXYZ();
			cout << v.first << " --- " << v.second << endl;
		}
		scene->setOrthogonalView(v.first.y, v.second.y, v.first.x, v.second.x, v.first.z, v.second.z);
		break;
	case VIEW_PERSPECTIVE:
		if (dlg.DoModal() == IDOK) {
			v = dlg.GetXYZ();
			cout << v.first << " --- " << v.second << endl;
		}
		scene->setPerspectiveView(v.first.y, v.second.y, v.first.x, v.second.x, v.first.z, v.second.z);
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
	glutAddMenuEntry("Orthogonal", VIEW_ORTHOGONAL);
	glutAddMenuEntry("Perspective", VIEW_PERSPECTIVE);
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
	scene->setOrthogonalView(-1,1,-1,1,-1,1);


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
