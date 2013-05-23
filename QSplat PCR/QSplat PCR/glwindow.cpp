#include "glwindow.h"

#include <QEvent>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QCoreApplication>

GLWindow::GLWindow(const GLWindowDef &def, QWindow *parent) :
	QWindow(parent),
	_initialized(false),
	_updatePosted(false),
	_context(nullptr)
{
	setTitle(def.title);

	resize(def.size);
	setMinimumSize(size());
	setMaximumSize(size());

    setSurfaceType(QWindow::OpenGLSurface);
	setFormat(def.format);
}

void GLWindow::update()
{
    if(!_updatePosted)
    {
        _updatePosted = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
    }
}

bool GLWindow::event(QEvent *event)
{
    switch(event->type())
    {
    case QEvent::UpdateRequest:
        if(_updatePosted)
        {
            renderHandler();
        }
        return true;

    default:
        return QWindow::event(event);
    }
}

void GLWindow::initHandler()
{
    _initialized = true;
    _context = new QOpenGLContext(this);
    _context->setFormat(requestedFormat());
    _context->create();

    _context->makeCurrent(this);
    initializeOpenGLFunctions();
    initGL();
    _context->swapBuffers(this);
}

void GLWindow::renderHandler()
{
    if(!isExposed())
    {
        return;
    }

    if(!_initialized)
    {
        initHandler();
    }

    _updatePosted = false;
    _context->makeCurrent(this);
    renderGL();
    _context->swapBuffers(this);
}

