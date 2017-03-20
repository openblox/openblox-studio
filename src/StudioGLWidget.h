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

#ifndef OB_STUDIO_STUDIOGLWIDGET_H_
#define OB_STUDIO_STUDIOGLWIDGET_H_

#include <QWidget>
#include <QtOpenGL>

namespace OB{
	namespace Studio{
		class StudioGLWidget: public QGLWidget{
		  public:
			StudioGLWidget(QWidget* parent = 0);
			virtual ~StudioGLWidget();

			static QGLFormat makeFormat();

			QSize minimumSizeHint() const;
			QSize sizeHint() const;

		  signals:
			void glInitialized();

		  protected:
			void initializeGL();
			void paintGL();
			void resizeGL(int width, int height);
			void mousePressEvent(QMouseEvent* event);
			void mouseMoveEvent(QMouseEvent* event);
		};
	}
}

#endif
