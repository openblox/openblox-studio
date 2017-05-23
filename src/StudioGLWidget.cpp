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

		void StudioGLWidget::remove_focus(){
			has_focus = false;

			StudioWindow::static_win->explorer->invisibleRootItem()->takeChildren();
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
		}

		void StudioGLWidget::resizeEvent(QResizeEvent* evt){
			QWidget::resizeEvent(evt);

		    if(eng){
				QSize newSize = evt->size();
				eng->resized(newSize.width(), newSize.height());
			}
		}

		void StudioGLWidget::mousePressEvent(QMouseEvent* event){}
		void StudioGLWidget::mouseMoveEvent(QMouseEvent* event){}

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
			//Temporary
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
