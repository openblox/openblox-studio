/*
 * Copyright (C) 2017 John M. Harris, Jr. <johnmh@openblox.org>
 *
 * This file is part of OpenBlox Studio.
 *
 * OpenBlox Studio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenBlox Studio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with OpenBlox Studio.	 If not, see <https://www.gnu.org/licenses/>.
 */

#include "StudioGLWidget.h"

#include "StudioWindow.h"

#include "openblox.h"

#include <QtGui>

#ifndef GL_MULTISAMPLE
	#define GL_MULTISAMPLE 0x809D
#endif

namespace OB{
	namespace Studio{
		StudioGLWidget::StudioGLWidget(QWidget* parent) : QGLWidget(makeFormat(), parent){
			setAutoFillBackground(false);
			setAttribute(Qt::WA_NoSystemBackground);

			QTimer* timer = new QTimer(this);
			connect(timer, SIGNAL(timeout()), this, SLOT(update()));
			timer->start(10);
		}

		StudioGLWidget::~StudioGLWidget(){}

		QGLFormat StudioGLWidget::makeFormat(){
			QGLFormat format = QGLFormat();
			format.setDepthBufferSize(24);
			format.setSampleBuffers(true);
			format.setSamples(2);
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
		    OB::OBEngine* eng = OB::OBEngine::getInstance();
			if(!eng){
				throw OB::OBException("game is NULL!");
			}

			eng->setWindowId((void*)winId());
		    ((StudioWindow*)(window()))->initGL();
		}

		void StudioGLWidget::paintGL(){
		    OB::OBEngine* eng = OB::OBEngine::getInstance();
			if(!eng){
				throw OB::OBException("game is NULL!");
			}
			if(eng->isRunning()){
				eng->tick();
				eng->render();
			}
		}

		void StudioGLWidget::resizeGL(int width, int height){
			Q_UNUSED(width);
			Q_UNUSED(height);

		    OB::OBEngine* eng = OB::OBEngine::getInstance();
			if(!eng){
				throw OB::OBException("game is NULL!");
			}
		    //eng->resized();
		}

		void StudioGLWidget::mousePressEvent(QMouseEvent* event){}
		void StudioGLWidget::mouseMoveEvent(QMouseEvent* event){}
	}
}
