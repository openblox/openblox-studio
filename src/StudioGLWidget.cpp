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

#include "StudioGLWidget.h"

#include "StudioWindow.h"
#include "InstanceTree.h"

#include <openblox.h>
#include <instance/LogService.h>

#include <functional>

#include <QtGui>

namespace OB{
	namespace Studio{
		StudioGLWidget::StudioGLWidget(OBEngine* eng) : StudioTabWidget(eng){
			setAttribute(Qt::WA_OpaquePaintEvent);
			setFocusPolicy(Qt::StrongFocus);

			setAutoFillBackground(false);

			setUpdatesEnabled(false);
			setMouseTracking(true);

			draw_axis = false;

			has_focus = false;
			logHist = "";
		}

		StudioGLWidget::~StudioGLWidget(){}

		QSize StudioGLWidget::minimumSizeHint() const{
			return QSize(320, 240);
		}

		QSize StudioGLWidget::sizeHint() const{
			return QSize(320, 240);
		}

		void StudioGLWidget::do_init(){
			using namespace std::placeholders;

			if(!eng){
				throw OB::OBException("game is NULL!");
			}

			eng->setWindowId((void*)winId());
			eng->init();

			eng->setPostRenderFunc(std::bind(&StudioGLWidget::post_render_func, this, _1));

			StudioWindow* win = StudioWindow::static_win;

			shared_ptr<OB::Instance::DataModel> dm = eng->getDataModel();
			if(dm){
				shared_ptr<OB::Instance::LogService> ls = dm->getLogService();
				if(ls){
					std::function<void(std::vector<shared_ptr<Type::VarWrapper>>)> lsb = std::bind(&StudioGLWidget::handle_log_event, this, _1);
					ls->getMessageOut()->Connect(lsb);
				}
			}
		}

		void StudioGLWidget::do_render(){
			if(eng){
				eng->render();
			}
		}

		void StudioGLWidget::post_render_func(irr::video::IVideoDriver* videoDriver){
			if(draw_axis){
				puts("Drawing");

				glViewport(0, 0, 50, 50);

				glMatrixMode(GL_PROJECTION);
				glPushMatrix();

				GLdouble fH = tan(45.0 / 360 * M_PI) * 0.1;
				GLdouble fW = fH * 1.0;
				glFrustum(-fW, fW, -fH, fH, 0.1, 20);

				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();

				//TODO: Grab this from the CurrentCamera
				//gluLookAt(10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f);

				glColor3f(1.0f, 0.0f, 0.0f);
				glEnable(GL_LINE_SMOOTH);
				glLineWidth( 1.5 );
				glBegin(GL_LINES);
				glVertex3f(-1000, 0, 0);
				glVertex3f(1000, 0, 0);
				glEnd();

				//Restore View
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glViewport(0, 0, 960, 600);
			}
		}

		void StudioGLWidget::setAxisWidgetVisible(bool axisWidgetVisible){
			draw_axis = axisWidgetVisible;
		}

	    bool StudioGLWidget::isAxisWidgetVisible(){
			return draw_axis;
		}

		void StudioGLWidget::remove_focus(){
			has_focus = false;

			StudioWindow::static_win->explorer->invisibleRootItem()->takeChildren();

			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					ier->unfocus();
				}
			}
		}

		void StudioGLWidget::gain_focus(){
			using namespace std::placeholders;

			has_focus = true;

			shared_ptr<OB::Instance::DataModel> dm = eng->getDataModel();
			if(dm){
				addDM(StudioWindow::static_win->explorer->invisibleRootItem(), dynamic_pointer_cast<Instance::Instance>(dm));
			}

			StudioWindow* win = StudioWindow::static_win;
			if(win->output){
				win->output->setHtml(logHist);
			}

			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					ier->focus();
				}
			}
		}

		void StudioGLWidget::resizeEvent(QResizeEvent* evt){
			QWidget::resizeEvent(evt);

			if(eng){
				QSize newSize = evt->size();
				eng->resized(newSize.width(), newSize.height());
			}
		}

		void StudioGLWidget::mousePressEvent(QMouseEvent* event){
			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					OB::Enum::MouseButton mbtn = OB::Enum::MouseButton::Unknown;

					switch(event->button()){
						case Qt::LeftButton: {
							mbtn = OB::Enum::MouseButton::Left;
							break;
						}
						case Qt::MidButton: {
							mbtn = OB::Enum::MouseButton::Middle;
							break;
						}
						case Qt::RightButton: {
							mbtn = OB::Enum::MouseButton::Right;
							break;
						}
						case Qt::BackButton: {
							mbtn = OB::Enum::MouseButton::X1;
							break;
						}
						case Qt::ForwardButton: {
							mbtn = OB::Enum::MouseButton::X2;
							break;
						}
					}

					ier->input_mouseButton(mbtn, true);
				}
			}
		}

		void StudioGLWidget::mouseReleaseEvent(QMouseEvent* event){
			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					OB::Enum::MouseButton mbtn = OB::Enum::MouseButton::Unknown;

					switch(event->button()){
						case Qt::LeftButton: {
							mbtn = OB::Enum::MouseButton::Left;
							break;
						}
						case Qt::MidButton: {
							mbtn = OB::Enum::MouseButton::Middle;
							break;
						}
						case Qt::RightButton: {
							mbtn = OB::Enum::MouseButton::Right;
							break;
						}
						case Qt::BackButton: {
							mbtn = OB::Enum::MouseButton::X1;
							break;
						}
						case Qt::ForwardButton: {
							mbtn = OB::Enum::MouseButton::X2;
							break;
						}
					}

					ier->input_mouseButton(mbtn, true);
				}
			}
		}

		void StudioGLWidget::mouseMoveEvent(QMouseEvent* event){
			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					QPoint mousePos = event->pos();

					ier->input_mouseMoved(make_shared<OB::Type::Vector2>(mousePos.x(), mousePos.y()), NULL);
				}
			}
		}

		void StudioGLWidget::wheelEvent(QWheelEvent* event){
			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					QPoint wheelDelta = event->angleDelta() / 8;

					if(!wheelDelta.isNull()){
					    QPoint numSteps = wheelDelta / 15;
						ier->input_mouseWheel(make_shared<OB::Type::Vector2>(numSteps.x(), numSteps.y()));
					}
				}
			}
		}

		// Explorer/log handling
		void StudioGLWidget::sendOutput(QString msg){
			QString emsg = msg.toHtmlEscaped().replace('\n', "<br/>") + "<br/>";
			logHist = logHist + emsg;

			if(has_focus){
				StudioWindow* win = StudioWindow::static_win;

				if(win->output){
					win->output->append(emsg);
				}
			}
		}

		void StudioGLWidget::sendOutput(QString msg, QColor col){
			QString emsg = "<font color=\"" + col.name() + "\">" + msg.toHtmlEscaped().replace('\n', "<br/>") + "</font><br/>";
			logHist = logHist + emsg;

			if(has_focus){
				StudioWindow* win = StudioWindow::static_win;

				if(win->output){
					win->output->append(emsg);
				}
			}
		}

		void StudioGLWidget::handle_log_event(std::vector<shared_ptr<OB::Type::VarWrapper>> evec){
			// Temporary
			QColor errorCol(255, 51, 0);
			QColor warnCol(242, 97, 0);

			StudioWindow* win = StudioWindow::static_win;
			if(evec.size() == 2){
				QString msg = evec.at(0)->asString().c_str();
				shared_ptr<Type::LuaEnumItem> msgType = dynamic_pointer_cast<Type::LuaEnumItem>(evec.at(1)->asType());

				if(msgType->getValue() == (int)Enum::MessageType::MessageError){
					sendOutput(msg, errorCol);
				}else if(msgType->getValue() == (int)Enum::MessageType::MessageWarning){
					sendOutput(msg, warnCol);
				}else{
					sendOutput(msg);
				}
			}
		}

		void StudioGLWidget::instance_changed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, InstanceTreeItem* kidItem){
			if(!kidItem){
				return;
			}

			shared_ptr<Instance::Instance> kid = kidItem->GetInstance();
			if(!kid){
				return;
			}

			std::string prop = evec.at(0)->asString();

			if(StudioWindow::static_win){
				std::vector<shared_ptr<Instance::Instance>> selection = selectedInstances;

				if(!selection.empty()){
					if(std::find(selection.begin(), selection.end(), kid) != selection.end()){
						StudioWindow::static_win->properties->updateValue(prop);
					}
				}
			}

			if(prop == "Name"){
				const QSignalBlocker sigBlock(kidItem->treeWidget());
				kidItem->setText(0, QString(kid->getName().c_str()));
				return;
			}
			if(prop == "Parent" || prop == "ParentLocked"){
				kidItem->updateFlags();
				return;
			}
		}

		void StudioGLWidget::instance_child_added_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, QTreeWidgetItem* kidItem){
			if(!kidItem){
				return;
			}

			if(evec.size() == 1){
				if(kidItem->isSelected()){
					StudioWindow* win = StudioWindow::static_win;
					if(win){
						win->update_toolbar_usability();
					}
				}
				shared_ptr<Instance::Instance> newGuy = evec[0]->asInstance();
				if(treeItemMap.contains(newGuy)){
					InstanceTreeItem* ngti = treeItemMap.value(newGuy);
					QTreeWidgetItem* twi = ngti->parent();
					if(twi != kidItem){
						if(twi){
							twi->removeChild(ngti);
						}
						kidItem->addChild(ngti);
					}
				}else{
					addChildOfInstance(kidItem, newGuy);
				}
			}
		}

		void StudioGLWidget::instance_child_removed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, QTreeWidgetItem* kidItem){
			if(!kidItem){
				return;
			}

			if(evec.size() == 1){
				if(kidItem->isSelected()){
					StudioWindow* win = StudioWindow::static_win;
					if(win){
						win->update_toolbar_usability();
					}
				}
				shared_ptr<Instance::Instance> newGuy = evec[0]->asInstance();
				InstanceTreeItem* kTi = treeItemMap.value(newGuy);
				if(kTi){
					if(kTi->parent() == kidItem){
						kidItem->removeChild(kTi);
					}
				}
			}
		}

		void StudioGLWidget::addChildOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> kid){
			using namespace std::placeholders;

			if(!parentItem || !kid){
				return;
			}

			InstanceTreeItem* kidItem = new InstanceTreeItem(kid);
			treeItemMap[kid] = kidItem;
			kidItem->setIcon(0, StudioWindow::getClassIcon(QString(kid->getClassName().c_str())));

			kid->Changed->Connect(std::bind(&StudioGLWidget::instance_changed_evt, this, _1, kidItem));
			kid->ChildAdded->Connect(std::bind(&StudioGLWidget::instance_child_added_evt, this, _1, kidItem));
			kid->ChildRemoved->Connect(std::bind(&StudioGLWidget::instance_child_removed_evt, this, _1, kidItem));

			addChildrenOfInstance(kidItem, kid);

			parentItem->addChild(kidItem);
		}

		void StudioGLWidget::addChildrenOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst){
			if(!parentItem || !inst){
				return;
			}

			std::vector<shared_ptr<Instance::Instance>> kids = inst->GetChildren();
			for(std::vector<shared_ptr<Instance::Instance>>::size_type i = 0; i < kids.size(); i++){
				shared_ptr<Instance::Instance> kid = kids[i];
				if(kid){
					addChildOfInstance(parentItem, kid);
				}
			}
		}

		void StudioGLWidget::dm_changed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec){
			StudioWindow* sw = StudioWindow::static_win;
			if(!sw){
				return;
			}

			shared_ptr<Instance::DataModel> dm = eng->getDataModel();
			if(dm){
				if(sw->tabWidget){
					int studioWidgetIdx = sw->tabWidget->indexOf(this);
					sw->tabWidget->setTabText(studioWidgetIdx, QString(dm->getName().c_str()));
				}
			}
		}

		void StudioGLWidget::addDM(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst){
			using namespace std::placeholders;

			if(!parentItem || !inst){
				return;
			}

			std::vector<shared_ptr<Instance::Instance>> kids = inst->GetChildren();
			for(std::vector<shared_ptr<Instance::Instance>>::size_type i = 0; i < kids.size(); i++){
				shared_ptr<Instance::Instance> kid = kids[i];
				if(kid){
					addChildOfInstance(parentItem, kid);
				}
			}

			inst->ChildAdded->Connect(std::bind(&StudioGLWidget::instance_child_added_evt, this, _1, parentItem));
			inst->ChildRemoved->Connect(std::bind(&StudioGLWidget::instance_child_removed_evt, this, _1, parentItem));
			inst->Changed->Connect(std::bind(&StudioGLWidget::dm_changed_evt, this, _1));
		}
	}
}
