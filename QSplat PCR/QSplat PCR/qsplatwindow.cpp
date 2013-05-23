#include "qsplatwindow.h"

#include <QFile>
#include <QScreen>
#include <QMatrix4x4>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

QSplatWindow::QSplatWindow(const GLWindowDef &def, QWindow *parent) :
    GLWindow(def, parent),
    _frame(0),
    _verticiesAttr(0),
    _colorsAttr(0),
    _matrixUni(0),
    _shaderProgram(nullptr)
{
	_camera = new Camera(this);
}

QSplatWindow::~QSplatWindow()
{
}

QPointer<Camera> QSplatWindow::camera() const
{
	return QPointer<Camera>(_camera);
}

void QSplatWindow::initGL()
{
    Q_ASSERT(loadShaders());
    _verticiesAttr = _shaderProgram->attributeLocation("posAttr");
    _colorsAttr = _shaderProgram->attributeLocation("colAttr");
    _matrixUni = _shaderProgram->uniformLocation("matrix");

	glViewport(0, 0, size().width(), size().height());
}

void QSplatWindow::renderGL()
{
	_camera->preFrame();

    glViewport(0, 0, width(), height());
	glClearColor(1,1,1,1);
    glClear(GL_COLOR_BUFFER_BIT);

    _shaderProgram->bind();

	 glm::mat4 pv = _camera->getProjection() * _camera->getView();
	 _shaderProgram->setUniformValue(_matrixUni, QMatrix4x4(&pv[0][0]));

	//QMatrix4x4 view = QMatrix4x4(&_camera->getView()[0][0]).inverted();
	//QMatrix4x4 proj = QMatrix4x4(&_camera->getProjection()[0][0]);
	//_shaderProgram->setUniformValue(_matrixUni, proj * view);

    GLfloat vertices[] = {
        0.0f, 0.707f,
        -0.5f, -0.5f,
        0.5f, -0.5f
    };

    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    glVertexAttribPointer(_verticiesAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(_colorsAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    _shaderProgram->release();

    ++_frame;
	_camera->postFrame();
}

bool QSplatWindow::loadShaders()
{
    bool ok;
    _shaderProgram = new QOpenGLShaderProgram(this);

    ok = _shaderProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, "Shaders/vertex.vert");
    if(!ok)
    {
        qDebug() << _shaderProgram->log();
        return ok;
    }

    ok = _shaderProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, "Shaders/fragment.frag");
    if(!ok)
    {
        qDebug() << _shaderProgram->log();
        return ok;
    }

    ok = _shaderProgram->link();
    if(!ok)
    {
        qDebug() << _shaderProgram->log();
        return ok;
    }

    return ok;
}
