#ifndef GLWINDOW_H_
#define GLWINDOW_H_

#include <QWindow>
#include <QOpenGLFunctions>

class QOpenGLContext;

/// Construction arguments for GLWindow.
struct GLWindowDef
{
	QSize size;
	QString title;
	QSurfaceFormat format;
};

/// An object which manages an OpenGL context and window resources.
class GLWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
    
public:
    GLWindow(const GLWindowDef &def, QWindow *parent = 0);

public slots:

    /// Schedule a screen redraw (non-blocking).
    void update();

protected:

    /// Called before first render call.
    virtual void initGL() = 0;

    /// Called after an update is posted, or the screen is resized.
    virtual void renderGL() = 0;

    bool event(QEvent *event);

private:
    void renderHandler();
    void resizeHandler();
    void initHandler();

	///<summary>
	///Test
	///</summary>
    bool _initialized;
    bool _updatePosted;
    QOpenGLContext *_context;
};

#endif // GLWINDOW_H_
