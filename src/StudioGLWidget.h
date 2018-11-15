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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with OpenBlox Studio. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef OB_STUDIO_STUDIOGLWIDGET_H_
#define OB_STUDIO_STUDIOGLWIDGET_H_

#include "StudioTabWidget.h"

#include "InstanceTreeItem.h"

namespace OB{
	namespace Studio{
		class StudioWindow;
		class InstanceTree;

		class StudioGLWidget: public StudioTabWidget{
		public:
			StudioGLWidget(OBEngine* eng);
			virtual ~StudioGLWidget();

			QSize minimumSizeHint() const;
			QSize sizeHint() const;

			void do_init();
			void do_render();

			void setAxisWidgetVisible(bool axisWidgetVisible);
		    bool isAxisWidgetVisible();

			virtual void remove_focus();
			virtual void gain_focus();

			virtual void resizeEvent(QResizeEvent* evt);

			void setLogHistory(QString hist);
			QString getLogHistory();

			void populateTree(InstanceTree* explorer);

			void saveAct();
			void saveAsAct();

			QString fileOpened;

			std::vector<shared_ptr<Instance::Instance>> selectedInstances;

			// Explorer handling
			void sendOutput(QString msg, QColor col);
			void sendOutput(QString msg);

			void handle_log_event(std::vector<shared_ptr<OB::Type::VarWrapper>> evec);

			QMap<shared_ptr<Instance::Instance>, InstanceTreeItem*> treeItemMap;

			void post_render_func(irr::video::IVideoDriver* videoDriver);

			void instance_changed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, InstanceTreeItem* kidItem);
			void instance_child_added_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, QTreeWidgetItem* kidItem);
			void instance_child_removed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, QTreeWidgetItem* kidItem);
			void addChildOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> kid);
			void addChildrenOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst);
			void dm_changed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec);
			void addDM(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst);

		protected:
			void paintGL();
			void resizeGL(int width, int height);
			void mousePressEvent(QMouseEvent* event);
			void mouseReleaseEvent(QMouseEvent* event);
			void mouseMoveEvent(QMouseEvent* event);
			void wheelEvent(QWheelEvent* event);

			bool has_focus;
			bool draw_axis;

		private:
			QString logHist;
		};
	}
}

#endif

// Local Variables:
// mode: c++
// End:
