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

		axis_text_textures = new GLuint[3];
	}

	StudioGLWidget::~StudioGLWidget(){
		glDeleteTextures(3, &axis_text_textures[0]);
	}

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

		glGenTextures(3, &axis_text_textures[0]);
	}

	bool renderAxisWidgetText(GLuint tex, QString str, int x, int y){
		TTF_Font* fnt = OpenBlox::OBGame::getInstance()->internalBoldFont();
		if(fnt){
			return renderText(tex, fnt, str, x, y, 0, 0, ob_enum::TextXAlignment::Left, ob_enum::TextYAlignment::Top, {255, 255, 255, 255});
		}
		return false;
	}

	void StudioGLWidget::drawAxisWidget(){
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width(), height(), 0, -1000., 1000.);
		glTranslated(0., 0., 0.);
		glMatrixMode(GL_MODELVIEW);

		double l = 25;
		double o = 2;

		double cx = 5 + l;
		double cy = height() - 5 - l;
		double xx, xy, yx, yy, zx, zy;

		float fvViewMatrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, fvViewMatrix);
		glLoadIdentity();

		xx = l * fvViewMatrix[0];
		xy = l * fvViewMatrix[1];
		yx = l * fvViewMatrix[4];
		yy = l * fvViewMatrix[5];
		zx = l * fvViewMatrix[8];
		zy = l * fvViewMatrix[9];

		glLineWidth(1);

		float currentColor[4];
		glGetFloatv(GL_CURRENT_COLOR, currentColor);

		const GLubyte red[] = {
			255, 0, 0, 255
		};

		const GLubyte green[] = {
			0, 255, 0, 255
		};

		const GLubyte blue[] = {
			0, 0, 255, 255
		};

		glBegin(GL_LINES);
		{
			glColor4ubv(red);
			glVertex2d(cx, cy);
			glVertex2d(cx + xx, cy + xy);

			glColor4ubv(green);
			glVertex2d(cx, cy);
			glVertex2d(cx + yx, cy + yy);

			glColor4ubv(blue);
			glVertex2d(cx, cy);
			glVertex2d(cx + zx, cy + zy);
		}
		glEnd();
		renderAxisWidgetText(axis_text_textures[0], "X", cx + xx + o, cy + xy + o);
		renderAxisWidgetText(axis_text_textures[1], "Y", cx + yx + o, cy + yy + o);
		renderAxisWidgetText(axis_text_textures[2], "Z", cx + zx + o, cy + zy + o);

		glColor4fv(currentColor);
	}

	void StudioGLWidget::paintGL(){
		OpenBlox::OBGame* game = OpenBlox::OBGame::getInstance();
		if(!game){
			throw OpenBlox::OBException("game is NULL!");
		}
		game->render();
		drawAxisWidget();
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
