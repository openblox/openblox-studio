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

namespace OB{
	namespace Studio{
		StudioGLWidget::StudioGLWidget(QWidget* parent) : QOpenGLWidget(parent){
			setAutoFillBackground(false);
			setAttribute(Qt::WA_NoSystemBackground);
			setAttribute(Qt::WA_NoBackground);
			setAttribute(Qt::WA_OpaquePaintEvent);

			QTimer* timer = new QTimer(this);
			connect(timer, SIGNAL(timeout()), this, SLOT(update()));
			timer->start(10);
			
			setUpdateBehavior(QOpenGLWidget::NoPartialUpdate);

			QSurfaceFormat fmt = format();
			fmt.setDepthBufferSize(24);
			fmt.setSamples(2);
			fmt.setSwapInterval(1);
			fmt.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
			fmt.setRenderableType(QSurfaceFormat::OpenGL);
			setFormat(fmt);
		}

		StudioGLWidget::~StudioGLWidget(){}

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
			if(eng->isRunning()){
				eng->render();
			}
		}

		void StudioGLWidget::mousePressEvent(QMouseEvent* event){}
		void StudioGLWidget::mouseMoveEvent(QMouseEvent* event){}
	}
}
