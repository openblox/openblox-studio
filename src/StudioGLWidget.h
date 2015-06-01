#ifndef STUDIOGLWIDGET_H_
#define STUDIOGLWIDGET_H_

#include <OpenBlox.h>

#include <QWidget>
#include <QtOpenGL>

namespace ob_studio{
	class StudioGLWidget: public QGLWidget{
		public:
			StudioGLWidget(QWidget* parent = 0);
			virtual ~StudioGLWidget();

			static QGLFormat makeFormat();

			QSize minimumSizeHint() const;
			QSize sizeHint() const;

		protected:
			void initializeGL();
			void paintGL();
			void resizeGL(int width, int height);
			void mousePressEvent(QMouseEvent* event);
			void mouseMoveEvent(QMouseEvent* event);
	};
}

#endif
