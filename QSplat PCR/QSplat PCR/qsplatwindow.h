#ifndef QSPLATWINDOW_H
#define QSPLATWINDOW_H

#include "glwindow.h"
#include "Camera.h"

class QOpenGLShader;
class QOpenGLShaderProgram;

class QSplatWindow : public GLWindow
{
public:
    QSplatWindow(const GLWindowDef &def, QWindow *parent = 0);
	virtual ~QSplatWindow();

	QPointer<Camera> camera() const;

protected:
    virtual void initGL();
    virtual void renderGL();

private:
    bool loadShaders();

    GLuint _frame;
    GLuint _verticiesAttr;
    GLuint _colorsAttr;
    GLuint _matrixUni;

    QOpenGLShaderProgram *_shaderProgram;

	Camera *_camera;
};

#endif // QSPLATWINDOW_H
