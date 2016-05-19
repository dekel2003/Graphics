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
#define NORMAL_ON 1
#define NORMAL_OFF 2
#define MAIN_DEMO 10
#define MAIN_ABOUT 11
#define ADD_SPHERE 7

Scene *scene;
Renderer *renderer;
int menuMesh, mainMenuRef, menuCamera, menuLight, menuFog, menuView, menuVertexNormals, menuFaceNormals, menuBoundingBox;
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
enum SELECT_TYPE{SELECT_CAMERA, SELECT_MODEL };
static SELECT_TYPE selection_type = SELECT_MODEL;
enum AXIS_TYPE{ XY_AXIS, Z_AXIS };
static AXIS_TYPE axis = XY_AXIS;
void keyboard( unsigned char key, int x, int y )
{
	//cout << key << endl;
	CFrustumDialog dlg;
	std::pair<vec3, vec3> v;
	float t = 20;
	switch (key) {
	case 033:
		exit(EXIT_SUCCESS);
		break;
	case '`':
		if (axis == XY_AXIS){
			axis = Z_AXIS;
			cout << "turn on Z-axis oporations" << endl;
		}
		else{
		axis = XY_AXIS;
		cout << "turn off Z-axis oporations" << endl;
		}
		rotation = NO_ROTATION;
		return;
	case 'z':
		if (rotation == MODEL){
			rotation = NO_ROTATION;
			cout << "rotation OFF" << endl;
			return;
		}
		rotation = MODEL;
		cout << "model rotation ON" << endl;
		return;
	case 'Z':
		if (rotation == WORLD){
			rotation = NO_ROTATION;
			cout << "rotation OFF" << endl;
			return;
		}
		rotation = WORLD;
		cout << "model world rotation ON" << endl;
		return;
	case 'c':
		cout << "change selection using keyboard numbers to ";
		if (selection_type == SELECT_MODEL){
			selection_type = SELECT_CAMERA;
			cout << "camera" << endl;
		}
		else{
			selection_type = SELECT_MODEL;
			cout << "model" << endl;
		}
		return;

	case 'a':
		if (rotation == MODEL){
			scene->rotateCurrentCamera(-t, 0);
			cout << "Camera rotating Left" << endl;
			break;
		}
		scene->moveCamera(-t, 0);
		cout << "Camera moving Left" << endl;
		break;
	case 's':
		if (rotation == MODEL){
			if (axis == XY_AXIS)
				scene->rotateCurrentCamera(0, -t);
			else
				scene->rotateCurrentCamera(-t);
			cout << "Camera rotating Down" << endl;
			break;
		}
		cout << "Camera moving Down" << endl;
		if (axis == XY_AXIS)
			scene->moveCamera(0, -t);
		else
			scene->moveCamera(-t);
		break;
	case 'd':
		if (rotation == MODEL){
			scene->rotateCurrentCamera(t, 0);
			cout << "Camera rotating Right" << endl;
			break;
		}
		cout << "Camera moving Right" << endl;
		scene->moveCamera(t, 0);
		break;
	case 'w':
		if (rotation == MODEL){
			if (axis == XY_AXIS)
				scene->rotateCurrentCamera(0, t);
			else
				scene->rotateCurrentCamera(t);
			cout << "Camera rotating Up" << endl;
			break;
		}
		cout << "Camera moving up" << endl;
		if (axis == XY_AXIS)
			scene->moveCamera(0, t);
		else
			scene->moveCamera(t);
		break;
	}
	if (key >= '0' && key <= '9'){
		if (selection_type == SELECT_MODEL){
			int index = key - '0';
			if (scene->numModels() <= index){
				cout << "undefined model " << index << endl;
				//cout << "num models " << scene->numModels() << endl;
				return;
			}
			if (scene->activeModel == index)
				scene->LookAt();
			cout << "Change active model to " << index << endl;
			scene->activeModel = index;
		}
		if (selection_type == SELECT_CAMERA){
			int index = key - '0';
			if (scene->numCameras() <= index){
				cout << "undefined camera " << index << endl;
				//cout << "num cameras " << scene->numCameras() << endl;
				return;
			}
			cout << "Change active camera to " << index << endl;
			scene->activeCamera = index;
		}
	}
	display();
}

void motion(int x, int y)
{
	// calc difference in mouse movement
	int dx = x - last_x;
	int dy = y - last_y;
	// update last x,y
	last_x = x;
	last_y = y;

	//cout << '(' << dx << ',' << dy << ')' << endl;

	static int updateCounter = 0;

	if (lb_down){
		updateCounter = (++updateCounter) % 5; // updating screen every frame is too fast.
		if (rotation == MODEL){
			if (axis == XY_AXIS)
				scene->rotateCurrentModel(dx, -dy);
			else
				scene->rotateCurrentModel(dy);
		}
		else if (rotation == WORLD){
			if (axis == XY_AXIS)
				scene->rotateCurrentModelWorld(dx, -dy);
			else
				scene->rotateCurrentModelWorld(dy);
		}
		else if (!camIsFocused){
			if (axis == XY_AXIS)
				scene->moveCurrentModel(dx, -dy);
			else
				scene->moveCurrentModel(dy);
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
	
	//renderer->testPointInTriangle(x, y);

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

bool fileMenuBeingUsed = false;

void fileMenu(int id)
{
	if (fileMenuBeingUsed == false) {
		fileMenuBeingUsed = true;
		switch (id)
		{
		case FILE_OPEN:
			CFileDialog dlg(TRUE, _T(".obj"), NULL, NULL, _T("*.obj|*.*"));
			if (dlg.DoModal() == IDOK)
			{
				std::string s((LPCTSTR)dlg.GetPathName());
				scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
			}
			break;
		}
		fileMenuBeingUsed = false;
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

	case ADD_SPHERE:
		scene->addPrimModel();
	}
}

void meshMenu(int id)
{
	scene->activeModel = id;
	display();
}

void cameraMenu(int id)
{
	if (id == 0){
		scene->addCamera();
	}
	else
	{
		scene->activeCamera = id - 1;
	}
	display();
}

bool lightMenuBeingUsed = false;
void lightMenu(int id)
{
	if (lightMenuBeingUsed == false) {
		lightMenuBeingUsed = true;
		if (id == 0){
			scene->addLight();
		}
		else if (id == 1){
			CCmdDialog dlg;
			if (dlg.DoModal() == IDOK){
				renderer->setAmbientLight(atof(dlg.GetCmd().c_str()));
			}

		}
		else
		{
			scene->activeLight = id - 2;
		}
		display();
		lightMenuBeingUsed = false;
	}
}

vector<GLfloat> split(const string &text, char sep) {
	vector<string> tokens;
	size_t start = 0, end = 0;
	while ((end = text.find(sep, start)) != string::npos) {
		tokens.push_back(text.substr(start, end - start));
		start = end + 1;
	}
	tokens.push_back(text.substr(start));
	vector<GLfloat> result;
	result.push_back(stoi(tokens[0]));
	result.push_back(stoi(tokens[1]));
	result.push_back(stoi(tokens[2]));
	return result;
}

bool fogMenuBeingUsed = false;
void fogMenu(int id)
{
	if (fogMenuBeingUsed == false) {
		fogMenuBeingUsed = true;
		if (id == 0){
			scene->EnableFog();
		} 
		else  if (id == 1) {
			scene->DisableFog();
		} else if (id == 2) {
			CCmdDialog dlg;
			if (dlg.DoModal() == IDOK){
				vector<GLfloat> input = split(dlg.GetCmd(), ',');
				scene->setFogColor(input[0], input[1], input[2]);
			}
		}
		else
		{
			scene->activeLight = id - 2;
		}
		display();
		fogMenuBeingUsed = false;
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
	display();
}

void normalsPerFaceMenu(int id){
	switch (id)
	{
	case NORMAL_OFF:
		scene->setNormalsPerFaceOff();
		break;
	case NORMAL_ON:
		scene->setNormalsPerFaceOn();
	}
	display();
}

void normalsPerVertexMenu(int id){
	switch (id)
	{
	case NORMAL_OFF:
		scene->setNormalsPerVertexOff();
		break;
	case NORMAL_ON:
		scene->setNormalsPerVertexOn();
	}
	display();
}

void boundingBoxMenu(int id){
	switch (id)
	{
	case NORMAL_OFF:
		scene->setDrawBoundingBoxOff();
		break;
	case NORMAL_ON:
		scene->setDrawBoundingBoxOn();
	}
	display();
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

void addCameraToMenu(){
	static int numCameras = 1;
	glutSetMenu(menuCamera);
	sprintf(c, "%s", to_string(numCameras).c_str());
	cout << to_string(numCameras) << endl << to_string(numCameras).c_str() << endl;
	glutAddMenuEntry(c, numCameras+1);
	glutSetMenu(mainMenuRef);
	glutChangeToSubMenu(menuCamera, "Choose Camera", menuCamera);
	numCameras++;
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void addLightToMenu(){
	cout << "menu:   view=" << menuView << " light=" << menuLight << endl;
	static int numLights = 2;
	glutSetMenu(menuLight);
	sprintf(c, "%s", to_string(numLights-1).c_str());
	glutAddMenuEntry(c, numLights);
	glutSetMenu(mainMenuRef);
	glutChangeToSubMenu(menuLight, "Choose Light", menuLight);
	numLights++;
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



void initMenu()
{
	int menuFile = glutCreateMenu(fileMenu);
	glutAddMenuEntry("Open..",FILE_OPEN);
	menuMesh = glutCreateMenu(meshMenu);
	menuCamera = glutCreateMenu(cameraMenu);
	glutAddMenuEntry("Add camera", 0);
	glutAddMenuEntry("0", 1);
	menuLight = glutCreateMenu(lightMenu);
	glutAddMenuEntry("Add light", 0);
	glutAddMenuEntry("Change Ambient Light", 1);
	menuFog = glutCreateMenu(fogMenu);
	glutAddMenuEntry("On", 0);
	glutAddMenuEntry("Off", 1);
	glutAddMenuEntry("Set Color", 2);
	menuFaceNormals = glutCreateMenu(normalsPerFaceMenu);
	glutAddMenuEntry("On", NORMAL_ON);
	glutAddMenuEntry("Off", NORMAL_OFF);
	menuVertexNormals = glutCreateMenu(normalsPerVertexMenu);
	glutAddMenuEntry("On", NORMAL_ON);
	glutAddMenuEntry("Off", NORMAL_OFF);
	menuBoundingBox = glutCreateMenu(boundingBoxMenu);
	glutAddMenuEntry("On", NORMAL_ON);
	glutAddMenuEntry("Off", NORMAL_OFF);
	menuView = glutCreateMenu(viewMenu);
	//cout << "menu:   view=" << menuView << " cam=" << menuCamera << endl;
	glutAddMenuEntry("Orthogonal", VIEW_ORTHOGONAL);
	glutAddMenuEntry("Perspective", VIEW_PERSPECTIVE);
	mainMenuRef = glutCreateMenu(mainMenu);
	glutAddSubMenu("File",menuFile);
	glutAddSubMenu("Choose Model", menuMesh);
	glutAddSubMenu("Choose Camera", menuCamera);
	glutAddSubMenu("Choose Light", menuLight);
	glutAddSubMenu("Choose Fog", menuFog);
	glutAddSubMenu("Choose View", menuView);
	glutAddSubMenu("Set Face Normals", menuFaceNormals);
	glutAddSubMenu("Set Vertex Normals", menuVertexNormals);
	glutAddSubMenu("Show Bounding Box", menuBoundingBox);
	glutAddMenuEntry("Add Sphere", ADD_SPHERE);
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

	//scene->loadOBJModel("C:\\לימודים\\גרפיקה ממוחשבת\\Projects\\TomShin2-cg_hw1-b680b2ab703e\\objects\\demo.obj"); // DELETE THIS
	scene->loadOBJModel("C:\\לימודים\\גרפיקה ממוחשבת\\Projects\\TomShin2-cg_hw1-b680b2ab703e\\objects\\chain.obj"); // DELETE THIS
	//scene->loadOBJModel("C:\\לימודים\\גרפיקה ממוחשבת\\Projects\\TomShin2-cg_hw1-b680b2ab703e\\objects\\dolphin.obj"); // DELETE THIS
	//scene->loadOBJModel("C:\\לימודים\\גרפיקה ממוחשבת\\Projects\\TomShin2-cg_hw1-b680b2ab703e\\objects\\cow.obj"); // DELETE THIS

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
