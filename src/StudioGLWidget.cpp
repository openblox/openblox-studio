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
		StudioGLWidget::StudioGLWidget(QWidget* parent) : QWidget(parent){
			setAttribute(Qt::WA_PaintOnScreen);
			setAttribute(Qt::WA_OpaquePaintEvent);
		    setFocusPolicy(Qt::StrongFocus);

			setAutoFillBackground(false);
		}

		StudioGLWidget::~StudioGLWidget(){}

		QSize StudioGLWidget::minimumSizeHint() const{
			return QSize(320, 240);
		}

		QSize StudioGLWidget::sizeHint() const{
			return QSize(320, 240);
		}

		void StudioGLWidget::init(){
		    OB::OBEngine* eng = OB::OBEngine::getInstance();
			if(!eng){
				throw OB::OBException("game is NULL!");
			}

			eng->setWindowId((void*)winId());

			startTimer(0);
		}

		void StudioGLWidget::paintEvent(QPaintEvent* evt){
			OB::OBEngine* eng = OB::OBEngine::getInstance();
			if(eng){
			    eng->render();
			}
		}

		void StudioGLWidget::resizeEvent(QResizeEvent* evt){
			puts("resize");
			QWidget::resizeEvent(evt);
		}

		void StudioGLWidget::timerEvent(QTimerEvent* evt){
			paintEvent(NULL);
			
			evt->accept();
		}

		void StudioGLWidget::mousePressEvent(QMouseEvent* event){}
		void StudioGLWidget::mouseMoveEvent(QMouseEvent* event){}
	}
}
