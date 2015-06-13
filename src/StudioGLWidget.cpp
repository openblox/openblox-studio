#include "StudioGLWidget.h"

#include "OpenBlox.h"

#include <QtGui>

#include <DataModel.h>

#ifndef GL_MULTISAMPLE
	#define GL_MULTISAMPLE 0x809D
#endif

namespace ob_studio{
	StudioGLWidget::StudioGLWidget(QWidget* parent) : QGLWidget(makeFormat(), parent){
		setAutoFillBackground(false);
		setAttribute(Qt::WA_NoSystemBackground);

		QTimer *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(update()));
		timer->start(10);
	}

	StudioGLWidget::~StudioGLWidget(){}

	QGLFormat StudioGLWidget::makeFormat(){
		QGLFormat format = QGLFormat();
		format.setDepthBufferSize(24);
		format.setSampleBuffers(true);
		format.setSamples(OPENBLOX_AA_SAMPLES);
		format.setDoubleBuffer(true);

		return format;
	}

	QSize StudioGLWidget::minimumSizeHint() const{
		return QSize(640, 480);
	}

	QSize StudioGLWidget::sizeHint() const{
		return QSize(640, 480);
	}

	void StudioGLWidget::initializeGL(){
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
		Q_UNUSED(width)
		Q_UNUSED(height)

		OpenBlox::OBGame* game = OpenBlox::OBGame::getInstance();
		if(!game){
			throw OpenBlox::OBException("game is NULL!");
		}
		game->resized();
	}

	void StudioGLWidget::mousePressEvent(QMouseEvent* event){

	}

	void StudioGLWidget::mouseMoveEvent(QMouseEvent* event){

	}
}
