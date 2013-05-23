#pragma once
#include "IRenderer.h"
#include "glwindow.h"

class MainWindow : public GLWindow {
private:
	pRenderer renderer;

public:
	MainWindow(const GLWindowDef &def, pRenderer renderer, QWindow *parent = 0) : GLWindow(def, parent), renderer(renderer) {

	}

	~MainWindow(void) {
	}

	/// Called before first render call.
    virtual void initGL() {
		renderer->Initialize();
	}

    /// Called after an update is posted, or the screen is resized.
    virtual void renderGL() {
		renderer->Draw();
	}
};

