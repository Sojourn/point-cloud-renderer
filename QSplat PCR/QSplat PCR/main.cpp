#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#include <memory>
#include <thread>
#include <iostream>
#include <sstream>

#include <Windows.h>

#include <glm/gtc/matrix_transform.hpp>

#include "trackball.h"
#include "camerarotator.h"

#include "TraversalController.h"
#include "DynamicBuffer.h"

#include "Renderer.h"
#include "MockBuffer.h"

#include "MockSplatter.h"
#include "BasicSplatter.h"
#include "PointSplatter.h"
#include "BlendSplatter.h"
#include "QuadSplatter.h"
#include "QuadSplatter_PC.h"
#include "BlendSplatterPreSortFrag.h"

//  Define M_PI in the case it's not defined in the math header file
#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif

using namespace std;

//----------------------------------------
//        State (Member Variables)
//----------------------------------------

// elapsed time
int elapsedTime;

// frame rate in ms for 30 frames/sec
int frameRate = 1000.0 / 30;

//Window Title
char title[200];

//Renderers Array State
shared_ptr<Renderer> *Renderers;
int RenderersLength = 0;
int CurrentRenderer = 0;
const char* RendererNames[] = {"Point", "3D", "Billboard", "Quad", "Quad PC", "Blending"};

//Objects used across the function
pRenderer renderer;
pProfiler profiler;
pBuffer buffer;

//World State
vec4 lightPosition = vec4(0,1000,0,1);
mat4 modelMatrix = mat4();

//Camera State
Camera &camera = Camera::inst();;
vec3 cameraCenter = vec3(0,0,0);
float cameraRadius = 5.0f;
vec3 cameraPosition = vec3(0,0, 1);
vec3 cameraUp = vec3(0,1,0);
vec3 cameraRight = vec3(1,0,0);

//Mouse State History
int mouseX = 0;
int mouseY = 0;
bool mouseIsLeftDown = false;
bool mouseIsRightDown = false;

ivec2 mouseLeftClickDownCoor = ivec2(-1,-1);
ivec2 mouseRightClickDownCoor = ivec2(-1,-1);

//Mode Flags
bool toggleCameraOrLight = true;
bool toggleRotation = false;

int hotCornerSize = 150;

//----------------------------------------
//         State Changers
//----------------------------------------

//Interacts with internal static state

void cyclePointSize(int i) {
	static int pointSize = 1;
	const int width = 60;
	pointSize = (pointSize + i+ width-1) % width + 1;
	glPointSize(pointSize);
}

void rotateCamera(float dx, float dy) {
	cameraPosition = vec3(rotate(mat4(), -dx, cameraUp) * vec4(cameraPosition,1));

	static float phi = 90.0f; //restricted to 0-180

	if (((phi - dy) > 1.0f) && ((phi - dy) < 179.0f)) {
		phi -=  dy;
		cameraPosition = vec3(rotate(mat4(), -dy, cameraRight) * vec4(cameraPosition,1));;
	}

	//cameraRight = vec3(rotate(mat4(), -.2f * dx, cameraUp) * vec4(cameraRight,1));
	cameraRight = normalize(cross(cameraUp, cameraPosition));
}

void click(int x, int y) {
	static int pX = x;
	static int pY = y;
	static int pT = -100000;
	int t = glutGet(GLUT_ELAPSED_TIME);

	int dx = abs(x - pX);
	int dy = abs(y - pY);
	int dt = t - pT;

	if ((dx <= 20) && (dy <= 20) && (dt <= 500)) {
		glutFullScreenToggle();
	}

	pX = x;
	pY = y;
	pT = t;
}

//Inlined (Internally Stateless)

inline void toggleCameraLight() {
	toggleCameraOrLight = !toggleCameraOrLight;
}

inline void cycleRenderers(int i = 1) {
	CurrentRenderer = (CurrentRenderer + i) % RenderersLength;
	renderer = Renderers[CurrentRenderer];
}

inline void setCamera() {
	camera.setView(lookAt(cameraRadius * cameraPosition, cameraCenter, cameraUp));
}

inline bool hotRightBottom(int x, int y) {
	ivec2 down = ivec2(camera.windowWidth() - mouseLeftClickDownCoor.x, camera.windowHeight() - mouseLeftClickDownCoor.y);
	ivec2 up = ivec2(camera.windowWidth() - x, camera.windowHeight() - y);

	if ((down.x <= hotCornerSize) && (down.y <= hotCornerSize) && (up.x <= hotCornerSize) && (up.y <= hotCornerSize) &&
		(down.x >= 0) && (down.y >= 0) && (up.x >= 0) && (up.y >= 0)) {
			cycleRenderers();
			return true;
	}
	return false;
}

inline bool hotRightBottomRightClick(int x, int y) {
	ivec2 down = ivec2(camera.windowWidth() - mouseRightClickDownCoor.x, camera.windowHeight() - mouseRightClickDownCoor.y);
	ivec2 up = ivec2(camera.windowWidth() - x, camera.windowHeight() - y);

	if ((down.x <= hotCornerSize) && (down.y <= hotCornerSize) && (up.x <= hotCornerSize) && (up.y <= hotCornerSize) &&
		(down.x >= 0) && (down.y >= 0) && (up.x >= 0) && (up.y >= 0)) {
			cycleRenderers(RenderersLength-1);
			return true;
	}
	return false;
}

inline bool hotLeftBottom(int x, int y) {
	ivec2 down = ivec2(mouseLeftClickDownCoor.x, camera.windowHeight() - mouseLeftClickDownCoor.y);
	ivec2 up = ivec2(x, camera.windowHeight() - y);

	if ((down.x <= hotCornerSize) && (down.y <= hotCornerSize) && (up.x <= hotCornerSize) && (up.y <= hotCornerSize) &&
		(down.x >= 0) && (down.y >= 0) && (up.x >= 0) && (up.y >= 0)) {
			toggleCameraLight();
			return true;
	}
	return false;
}

inline bool hotRightTop(int x, int y) {
	ivec2 down = ivec2(camera.windowWidth() - mouseLeftClickDownCoor.x, mouseLeftClickDownCoor.y);
	ivec2 up = ivec2(camera.windowWidth() - x, y);

	if ((down.x <= hotCornerSize) && (down.y <= hotCornerSize) && (up.x <= hotCornerSize) && (up.y <= hotCornerSize) &&
		(down.x >= 0) && (down.y >= 0) && (up.x >= 0) && (up.y >= 0)) {
			cyclePointSize(1);
			return true;
	}
	return false;
}

inline bool hotRightTopRightClick(int x, int y) {
	ivec2 down = ivec2(camera.windowWidth() - mouseRightClickDownCoor.x, mouseRightClickDownCoor.y);
	ivec2 up = ivec2(camera.windowWidth() - x, y);

	if ((down.x <= hotCornerSize) && (down.y <= hotCornerSize) && (up.x <= hotCornerSize) && (up.y <= hotCornerSize) &&
		(down.x >= 0) && (down.y >= 0) && (up.x >= 0) && (up.y >= 0)) {
			toggleRotation = !toggleRotation;
			return true;
	}
	return false;
}

inline bool hotLeftTop(int x, int y) {
	ivec2 down = ivec2(mouseLeftClickDownCoor.x,  mouseLeftClickDownCoor.y);
	ivec2 up = ivec2(x, y);

	if ((down.x <= hotCornerSize) && (down.y <= hotCornerSize) && (up.x <= hotCornerSize) && (up.y <= hotCornerSize) &&
		(down.x >= 0) && (down.y >= 0) && (up.x >= 0) && (up.y >= 0)) {
			cyclePointSize(-1);
			return true;
	}
	return false;
}


//----------------------------------------
//          GLUT Functions
//----------------------------------------

void display (void) {
	renderer->Draw();
	
	glViewport(camera.windowWidth() - hotCornerSize, camera.windowHeight() - hotCornerSize, hotCornerSize, hotCornerSize);
	renderer->DrawAxes();
	glViewport(0,0,camera.windowWidth(), camera.windowHeight());

	glutSwapBuffers();
}

void idle( void )
{
	int now = glutGet(GLUT_ELAPSED_TIME);
	int diff = now - elapsedTime;
	if (diff > frameRate)
	{
		elapsedTime = now;
		//F - frame rate
		//R - render time
		//U - update time
		//D - current depth
		//MD - maximum depth

		
		//It's not a race condition if it does not matter.
		//Why take the time locking it. int64 on x64 has atomic reads and writes
		buffer->Lock(IBuffer::Renderer);
		size_t bufferSize = buffer->Array(IBuffer::Renderer).size();
		buffer->Unlock(IBuffer::Renderer);

		std::stringstream ss;
		ss.precision(3);

		ss << RendererNames[CurrentRenderer];
		ss << " -";
		ss << " P: " << bufferSize;
		ss << " F: " << (1000.0f / diff);
		ss << " R: " << (profiler->GetCurrentFrameRate());
		ss << " U: " << (profiler->GetCurrentUpdateRate());
		ss << " D: " << (profiler->GetDepth());
		ss << " MD: " << (profiler->GetMaxDepth());
		glutSetWindowTitle(ss.str().c_str());

		//I want the decimal, I do not like 1.#J for Infinite when using precision(3)

		//sprintf(title, "%s - P: %i F: %3.1f R: %3.1f U: %3.1f D: %.0f MD: %.0f", RendererNames[CurrentRenderer], bufferSize , 1000.0f / diff, profiler->GetCurrentFrameRate(), profiler->GetCurrentUpdateRate(), profiler->GetDepth(), profiler->GetMaxDepth());
		//glutSetWindowTitle(title);

		if (toggleRotation) {
			vec4 viewLight = camera.getView() * lightPosition;
			rotateCamera((float) diff / 33.0f, 0);
			setCamera();
			lightPosition = glm::inverse(camera.getView()) *  viewLight;
		}
		glutPostRedisplay();
	}
}

void mouseMotion(int x, int y) {
	
	//Get window size (may change to global variables)
	double width = camera.windowWidth();
	double height = camera.windowHeight();

	//Convert to scale from 0,1 to -1,1
	double Wx = ((double) x)/width;
	double Wy = ((double) y)/height;
	Wx = 2*Wx -1;
	Wy = -2*Wy +1;


	//displacement
	int dx = x - mouseX;
	int dy = y - mouseY;

	//----------------------------------------------------------------------------------
	// Rotate camera
	//----------------------------------------------------------------------------------
	if (mouseIsLeftDown) {
		if (toggleCameraOrLight) {
			rotateCamera(.2f * dx, .2f * dy);
		} else  {
			lightPosition = rotate(mat4(), .2f * dy, cameraRight) * rotate(mat4(), .2f * dx, cross(cameraPosition, cameraRight)) * lightPosition;
		}
	}
	//----------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------
	// Move Camera in/out-ward
	//----------------------------------------------------------------------------------
	if (mouseIsRightDown) {
		cameraRadius += .003f * dy * cameraRadius;
		if (cameraRadius < .01f)
			cameraRadius = .01f;
	}
	//----------------------------------------------------------------------------------

	setCamera();

	//Set previous mouse state for next call
	mouseX = x;
	mouseY = y;
}

void mouse(int button, int state, int x, int y) {
	mouseX = x;
	mouseY = y;
	if (button == GLUT_LEFT_BUTTON) {;
		if (mouseIsLeftDown = state == GLUT_DOWN) {
			mouseLeftClickDownCoor.x = x;
			mouseLeftClickDownCoor.y = y;
		} else {
			if (!(hotRightBottom(x,y) || hotLeftBottom(x,y) ||
				  hotRightTop(x,y) || hotLeftTop(x,y))) {
				click(x,y);
			}
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) {
		if (mouseIsRightDown = state == GLUT_DOWN) {
			mouseRightClickDownCoor.x = x;
			mouseRightClickDownCoor.y = y;
		} else {
			hotRightTopRightClick(x,y);
			hotRightBottomRightClick(x,y);
		}
	}
}

void reshape(int width, int height) {
	camera.setProjection(perspective(30.0f, (float) width / (float) height, .1f, 100.0f));
	glViewport(0,0,width, height);
	camera.resize(width, height);
}

void keyboardSpecial(int key, int x, int y) {
}

void keyboard( unsigned char key, int x, int y ) {

	switch (key) {
	case 033: //esc
		glutExit();
		break;
	case ' ':
		cycleRenderers();
		break;
	case 'l':
		toggleCameraLight();
		break;
	case '=':
	case '+':
		cyclePointSize(1);
		break;
	case '-':
		cyclePointSize(-1);
		break;
	case 'r':
		toggleRotation = !toggleRotation;
		break;
	case 'f':
		glutFullScreenToggle();
		break;
	case '1':
		hotCornerSize = 100;
		break;
	case '2':
		hotCornerSize = 150;
		break;
	case '3':
		hotCornerSize = 175;
		break;
	default:
		break;
	}
}

//----------------------------------------
//               Main
//----------------------------------------

int main(int argc, char *argv[])
{
	
	glutInit( &argc, argv );

	TraversalController *controller = nullptr;

	int height = 512;
	int width = 512;

	int maxheight = 1200;
	int maxwidth = 1920;

	bool useModel = false;
	bool useStress = false;
	string modelName;
	int blendingType = 0;

	float updateMultiplier = 1.0f;


	for (int i = 1; i < argc; i++) {
		string value(argv[i]);
		if (value == "-sphere") {
		} 
		else if (value == "-stress") {
			useStress = true;
		}
		else if (value == "-lion") {
			if (!useModel)
				modelName = "Model\\lion_color.qs";
			useModel = true;
		}
		else if ((value == "-blend_3D") || (value == "-blend_3d") || (value == "-blend3D") || (value == "-blend3d")) {
			blendingType = 1;
		}
		else if ((value == "-blend_Quad") || (value == "-blend_quad") || (value == "-blendQuad") || (value == "-blendquad")) {
			blendingType = 2;
		}
		else if (value == "-model") {
			if (++i < argc) {
				string value(argv[i]);
				modelName = value;
				useModel = true;
			}
		}
		else if (value == "-height") {
			if (++i < argc) {
				int temp;
				string value(argv[i]);
				istringstream ss(value);
				ss >> temp;
				if (!ss.fail()) {
					if ((temp > 0) && (temp <= 1200)) 
						height = temp;
				} else
					--i;
			}
		}
		else if (value == "-width") {
			if (++i < argc) {
				int temp;
				string value(argv[i]);
				istringstream ss(value);
				ss >> temp;
				if (!ss.fail()) {
					if ((temp > 0) && (temp <= 1920)) 
						width = temp;
				} else
					--i;
			}
		}
		else if ((value == "-max_height") || (value == "-maxheight")) {
			if (++i < argc) {
				int temp;
				string value(argv[i]);
				istringstream ss(value);
				ss >> temp;
				if (!ss.fail()) {
					if ((temp > 0) && (temp <= 1200)) 
						maxheight = temp;
				} else
					--i;
			}
		}
		else if ((value == "-max_width") || (value == "-maxwidth")) {
			if (++i < argc) {
				int temp;
				string value(argv[i]);
				istringstream ss(value);
				ss >> temp;
				if (!ss.fail()) {
					if ((temp > 0) && (temp <= 1920)) 
						maxwidth = temp;
				} else
					--i;
			}
		}
		else if ((value == "-frame_rate") || (value == "-framerate")) {
			if (++i < argc) {
				float rate;
				string value(argv[i]);
				istringstream ss(value);
				ss >> rate;
				if (ss.good()) {
					frameRate = (int) (1000.0f / rate);
				} else
					--i;
			}
		}
		else if ((value == "-update_rate") || (value == "-updaterate")) {
			if (++i < argc) {
				string value(argv[i]);
				istringstream ss(value);
				ss >> updateMultiplier;
				if (!ss.good()) --i;
			}
		}
		else if ((value[0] == '-')  && (value.length() == 3) && (value[1] != value[2]) && (value[1] != (value[2] + ('A' - 'a'))) && (value[1] != (value[2] - ('A' - 'a')))) {
			vec3 newPosition;
			vec3 newUp;

			switch (value[1]) {
			case 'X':
				newPosition = vec3(-1,0,0);
				break;
			case 'x':
				newPosition = vec3(1,0,0);
				break;
			case 'Y':
				newPosition = vec3(0,-1,0);
				break;
			case 'y':
				newPosition = vec3(0,1,0);
				break;
			case 'Z':
				newPosition = vec3(0,0,-1);
				break;
			case 'z':
				newPosition = vec3(0,0,1);
				break;
			default:
				continue;
			}
			switch (value[2]) {
			case 'X':
				newUp = vec3(-1,0,0);
				break;
			case 'x':
				newUp = vec3(1,0,0);
				break;
			case 'Y':
				newUp = vec3(0,-1,0);
				break;
			case 'y':
				newUp = vec3(0,1,0);
				break;
			case 'Z':
				newUp = vec3(0,0,-1);
				break;
			case 'z':
				newUp = vec3(0,0,1);
				break;
			default:
				continue;
			}

			cameraPosition = newPosition;
			cameraUp = newUp;
			lightPosition = vec4(cameraUp * 1000.0f, 1);
			cameraRight = cross(cameraPosition, cameraUp);
		}
		else if (value[0] == '-') {
		}
		else {
			if (!useModel)
				modelName = value;
			useModel = true;
		}

	}

	int updateRate = (int) (updateMultiplier * frameRate);

	//Set initial camera position
	setCamera();
	camera.resize(width, height);
	
	// Set up the graphics context with double-buffering (Leave commented)
	//glutInitContextVersion (renderer->GetOpenGLMajorVersion(), renderer->GetOpenGLMinorVersion());
	
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitContextFlags (GLUT_CORE_PROFILE | GLUT_DEBUG);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize( width, height );
	glutCreateWindow( "" );

	glewInit();

	buffer = make_shared<DynamicBuffer>(maxwidth*maxheight);
	profiler = make_shared<Profiler>();
	unique_ptr<QSplatModel> model(new QSplatModel());


	if (useModel) {
		if (!model->load(modelName)) {
			cout << "Model could not be loaded. (" << modelName << ")"<< endl;
			glutExit();
			exit(0);
		}

		Sphere_t worldSphere = model->worldSphere();
		modelMatrix = translate(scale(mat4(), vec3(1 / worldSphere.radius)), -worldSphere.center);

		controller = new TraversalController(std::move(model), buffer, profiler);
		controller->setRefinementDurations(
			std::chrono::milliseconds(frameRate),
			std::chrono::milliseconds(updateRate));
		
		controller->start();
	} else {
		if (useStress)
			buffer = make_shared<MockBuffer>(1);
		else 
			buffer = make_shared<MockBuffer>();

		modelMatrix = mat4();
	}

	int major;
	int minor;

	sscanf_s((const char *) glGetString(GL_VERSION), "%i.%i", &major, &minor);

	if ((major >= 4) && (minor >= 2)) {
		RenderersLength = 6;
		CurrentRenderer = 3;
		
		Renderers = new shared_ptr<Renderer>[RenderersLength];
		Renderers[0] = make_shared<Renderer>(buffer, make_shared<PointSplatter>(), profiler, modelMatrix, lightPosition);
		Renderers[1] = make_shared<Renderer>(buffer, make_shared<MockSplatter>(), profiler, modelMatrix, lightPosition);
		Renderers[2] = make_shared<Renderer>(buffer, make_shared<BasicSplatter>(), profiler, modelMatrix, lightPosition);
		Renderers[3] = make_shared<Renderer>(buffer, make_shared<QuadSplatter>(), profiler, modelMatrix, lightPosition);
		Renderers[4] = make_shared<Renderer>(buffer, make_shared<QuadSplatter_PC>(), profiler, modelMatrix, lightPosition);
		Renderers[5] = make_shared<Renderer>(buffer, make_shared<BlendSplatter>(maxwidth, maxheight, blendingType), profiler, modelMatrix, lightPosition);
	} else if ((major >= 4) && (minor >= 0)) {
		RenderersLength = 4;
		CurrentRenderer = 3;
		
		Renderers = new shared_ptr<Renderer>[RenderersLength];
		Renderers[0] = make_shared<Renderer>(buffer, make_shared<PointSplatter>(), profiler, modelMatrix, lightPosition);
		Renderers[1] = make_shared<Renderer>(buffer, make_shared<MockSplatter>(), profiler, modelMatrix, lightPosition);
		Renderers[2] = make_shared<Renderer>(buffer, make_shared<BasicSplatter>(), profiler, modelMatrix, lightPosition);
		Renderers[3] = make_shared<Renderer>(buffer, make_shared<QuadSplatter>(), profiler, modelMatrix, lightPosition);
	} else if ((major >= 3) && (minor >= 3)) {
		RenderersLength = 2;
		CurrentRenderer = 1;

		Renderers = new shared_ptr<Renderer>[RenderersLength];
		Renderers[0] = make_shared<Renderer>(buffer, make_shared<PointSplatter>(), profiler, modelMatrix, lightPosition);
		Renderers[1] = make_shared<Renderer>(buffer, make_shared<MockSplatter>(), profiler, modelMatrix, lightPosition);
	} else {
		cout<<"OpenGL 3.3 or higher is required. (" << major << "." << minor << ")" <<endl;
		
		if (controller != nullptr)
			controller->end();
		glutExit();
		exit(0);
	}

	renderer = Renderers[CurrentRenderer];

	Renderer::StaticInitialize(maxwidth*maxheight);

	for (int i = 0; i < RenderersLength; i++) 
		Renderers[i]->Initialize(width * height);


	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutSpecialFunc( keyboardSpecial );
	glutMotionFunc(mouseMotion);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutIdleFunc( idle );

	elapsedTime = glutGet(GLUT_ELAPSED_TIME);

	glutMainLoop();

	if (controller != nullptr)
		controller->end();

    return 0;
}
