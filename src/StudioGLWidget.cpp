#include "StudioGLWidget.h"

#include "OpenBlox.h"

#include <QtGui>


#ifndef GL_MULTISAMPLE
	#define GL_MULTISAMPLE 0x809D
#endif

namespace ob_studio{
	StudioGLWidget::StudioGLWidget(QWidget* parent) : QGLWidget(makeFormat(), parent){
		setAutoFillBackground(false);

		QTimer *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(update()));
		timer->start(10);
	}

	StudioGLWidget::~StudioGLWidget(){}

	QGLFormat StudioGLWidget::makeFormat(){
		QGLFormat format = QGLFormat();
		format.setSampleBuffers(true);
		format.setSamples(OPENBLOX_AA_SAMPLES);

		return format;
	}

	QSize StudioGLWidget::minimumSizeHint() const{
		return QSize(640, 480);
	}

	QSize StudioGLWidget::sizeHint() const{
		return QSize(640, 480);
	}

	void StudioGLWidget::initializeGL(){
		OpenBlox::mw = SDL_CreateWindowFrom((void*)this->winId());

		OpenBlox::OBGame* game = OpenBlox::OBGame::getInstance();
		if(!game){
			throw OpenBlox::OBException("game is NULL!");
		}
		game->initGL();
	}

	void StudioGLWidget::paintGL(){
		OpenBlox::OBGame* game = OpenBlox::OBGame::getInstance();
		if(!game){
			throw OpenBlox::OBException("game is NULL!");
		}
		game->render();
	}

	void StudioGLWidget::resizeGL(int width, int height){
		SDL_Event sdlevent;
		sdlevent.type = SDL_WINDOWEVENT;
		sdlevent.window.windowID = SDL_GetWindowID(OpenBlox::mw);
		sdlevent.window.event = SDL_WINDOWEVENT_RESIZED;
		sdlevent.window.data1 = width;
		sdlevent.window.data2 = height;

		SDL_PushEvent(&sdlevent);
		SDL_SetWindowSize(OpenBlox::mw, width, height);
	}

	void StudioGLWidget::mousePressEvent(QMouseEvent* event){

	}

	void StudioGLWidget::mouseMoveEvent(QMouseEvent* event){

	}
}
