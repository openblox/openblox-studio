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

#include "StudioWindow.h"

#include <QtWidgets>

#include "InstanceTreeItem.h"

#include <openblox.h>
#include <instance/Instance.h>
#include <instance/DataModel.h>
#include <instance/LogService.h>
#include <type/Event.h>
#include <type/Enum.h>

namespace OB{
	namespace Studio{
		void handle_log_event(std::vector<shared_ptr<OB::Type::VarWrapper>> evec, void* ud){
		    StudioWindow* win = (StudioWindow*)ud;
			
			if(evec.size() == 2){
				QString msg = evec.at(0)->asString().c_str();
				shared_ptr<OB::Type::LuaEnumItem> msgType = dynamic_pointer_cast<OB::Type::LuaEnumItem>(evec.at(1)->asType());

				if(msgType->getValue() == (int)OB::Enum::MessageType::MessageError){
					win->output->append("<font color=\"#FF3300\">" + msg.toHtmlEscaped().replace('\n', "<br/>") + "</font><br/>");
				}else if(msgType->getValue() == (int)OB::Enum::MessageType::MessageWarning){
					win->output->append("<font color=\"#F26100\">" + msg.toHtmlEscaped().replace('\n', "<br/>") + "</font><br/>");
				}else{
					win->output->append(msg.toHtmlEscaped().replace('\n', "<br/>") + "<br/>");
				}
			}
		}
		
		QMap<shared_ptr<Instance::Instance>, InstanceTreeItem*> treeItemMap;
		QMap<QString, QIcon> classIconMap;

		QIcon getClassIcon(QString className){
		    if(classIconMap.contains(className)){
				return classIconMap[className];
			}
		    
			QIcon ico;

			if(className.isEmpty()){
				return ico;
			}

			QString icop = ":rc/class_icons/" + className + ".png";
			
			if(QFile(icop).exists()){
				ico = QIcon(icop);
			}else{
				std::string parentClassStr = OB::ClassFactory::getParentClassName(className.toStdString());
				ico = getClassIcon(QString(parentClassStr.c_str()));
			}
			
			classIconMap[className] = ico;
			return ico;
		}

		void addChildrenOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst);
		void addChildOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> kid);
		
		void instance_changed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, void* ud){
		    InstanceTreeItem* kidItem = (InstanceTreeItem*)ud;
			shared_ptr<Instance::Instance> kid = kidItem->GetInstance();

			kidItem->setText(0, QString(kid->getName().c_str()));
		}

		void instance_child_added_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, void* ud){
		    InstanceTreeItem* kidItem = (InstanceTreeItem*)ud;
			
		    if(evec.size() == 1){
			    shared_ptr<Instance::Instance> newGuy = dynamic_pointer_cast<Instance::Instance>(evec[0]->asInstance());
				if(treeItemMap.contains(newGuy)){
					kidItem->addChild(treeItemMap[newGuy]);
				}else{
					addChildOfInstance(kidItem, newGuy);
				}
			}
		}

		void instance_child_removed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, void* ud){
			InstanceTreeItem* kidItem = (InstanceTreeItem*)ud;
			
		    if(evec.size() == 1){
			    shared_ptr<Instance::Instance> newGuy = dynamic_pointer_cast<Instance::Instance>(evec[0]->asInstance());
				if(treeItemMap.contains(newGuy)){
					kidItem->removeChild(treeItemMap[newGuy]);
				}
			}
		}

		void addChildOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> kid){
			if(!parentItem || !kid){
				return;
			}

			InstanceTreeItem* kidItem = new InstanceTreeItem(kid);
			treeItemMap[kid] = kidItem;
			kidItem->setIcon(0, getClassIcon(QString(kid->getClassName().c_str())));

		    kid->Changed->Connect(instance_changed_evt, kidItem);
			kid->ChildAdded->Connect(instance_child_added_evt, kidItem);
			kid->ChildRemoved->Connect(instance_child_removed_evt, kidItem);

			addChildrenOfInstance(kidItem, kid);

			parentItem->addChild(kidItem);
		}

		void addChildrenOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst){
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
		
		void dm_changed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, void* ud){
			StudioWindow* sw = (StudioWindow*)ud;
			if(!sw){
				return;
			}
			
		    OBEngine* eng = OBEngine::getInstance();
			if(eng){
			    shared_ptr<Instance::DataModel> dm = eng->getDataModel();
				if(dm){
					if(sw->tabWidget && sw->glWidget){
						int studioWidgetIdx = sw->tabWidget->indexOf(sw->glWidget);
						sw->tabWidget->setTabText(studioWidgetIdx, QString(dm->getName().c_str()));
					}
				}
			}
		}

		void addDM(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst, StudioWindow* sw){			
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

			inst->ChildAdded->Connect(instance_child_added_evt, parentItem);
			inst->ChildRemoved->Connect(instance_child_removed_evt, parentItem);
			inst->Changed->Connect(dm_changed_evt, sw);
		}

		StudioWindow::StudioWindow(){
			glWidget = NULL;

			tabWidget = new QTabWidget();
			tabWidget->setTabsClosable(true);
			setCentralWidget(tabWidget);

			//Menus
			QMenu* fileMenu = menuBar()->addMenu("File");
			
			QAction* newAction = fileMenu->addAction("New");
			newAction->setIcon(QIcon::fromTheme("document-new"));
			newAction->setShortcut(QKeySequence::New);
			connect(newAction, &QAction::triggered, this, &StudioWindow::newInstance);
			
			QAction* openAction = fileMenu->addAction("Open");
			openAction->setIcon(QIcon::fromTheme("document-open"));
			openAction->setShortcut(QKeySequence::Open);

			fileMenu->addSeparator();

			QAction* saveAction = fileMenu->addAction("Save");
			saveAction->setEnabled(false);
			saveAction->setIcon(QIcon::fromTheme("document-save"));
			saveAction->setShortcut(QKeySequence::Save);
			
			QAction* saveAsAction = fileMenu->addAction("Save As");
			saveAsAction->setEnabled(false);
			saveAsAction->setIcon(QIcon::fromTheme("document-save-as"));
			saveAsAction->setShortcut(QKeySequence::SaveAs);
			
			fileMenu->addSeparator();
			
			QAction* closeAction = fileMenu->addAction("Close");
			closeAction->setIcon(QIcon::fromTheme("window-close"));
			closeAction->setShortcut(QKeySequence::Quit);
			connect(closeAction, &QAction::triggered, this, &StudioWindow::closeStudio);

			QMenu* editMenu = menuBar()->addMenu("Edit");
			QAction* undoAction = editMenu->addAction("Undo");
			undoAction->setIcon(QIcon::fromTheme("edit-undo"));
			undoAction->setEnabled(false);
			undoAction->setShortcut(QKeySequence::Undo);
			
			QAction* redoAction = editMenu->addAction("Redo");
			redoAction->setIcon(QIcon::fromTheme("edit-redo"));
			redoAction->setEnabled(false);
			redoAction->setShortcut(QKeySequence::Redo);

			editMenu->addSeparator();

			QAction* cutAction = editMenu->addAction("Cut");
			cutAction->setIcon(QIcon::fromTheme("edit-cut"));
			cutAction->setEnabled(false);
			cutAction->setShortcut(QKeySequence::Cut);
			
			QAction* copyAction = editMenu->addAction("Copy");
			copyAction->setIcon(QIcon::fromTheme("edit-copy"));
			copyAction->setEnabled(false);
			copyAction->setShortcut(QKeySequence::Copy);

			QAction* pasteAction = editMenu->addAction("Paste");
			pasteAction->setIcon(QIcon::fromTheme("edit-paste"));
			pasteAction->setEnabled(false);
			pasteAction->setShortcut(QKeySequence::Paste);

			QAction* deleteAction = editMenu->addAction("Delete");
		    deleteAction->setIcon(QIcon::fromTheme("edit-delete"));
			deleteAction->setEnabled(false);
			deleteAction->setShortcut(QKeySequence::Delete);

			editMenu->addSeparator();

			QAction* settingsAct = editMenu->addAction("Settings");
			settingsAct->setIcon(QIcon::fromTheme("configure-shortcuts", QIcon::fromTheme("configure", QIcon::fromTheme("document-properties"))));
			settingsAct->setShortcut(QKeySequence::Preferences);
			connect(settingsAct, &QAction::triggered, this, &StudioWindow::showSettings);

			QMenu* viewMenu = menuBar()->addMenu("View");

			menuBar()->addSeparator();

			QMenu* helpMenu = menuBar()->addMenu("Help");

			QAction* aboutAct = helpMenu->addAction("About");
			aboutAct->setStatusTip("About OpenBlox Studio");
			aboutAct->setIcon(QIcon::fromTheme("help-about"));
			connect(aboutAct, &QAction::triggered, this, &StudioWindow::about);

			//Dock Components

			setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
			setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);

			//Output
			QDockWidget* dock = new QDockWidget(tr("Output"), this);
			dock->setObjectName("studio_output");
			dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

			dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

			output = new QTextEdit();
			output->setReadOnly(true);

			dock->setWidget(output);
			addDockWidget(Qt::BottomDockWidgetArea, dock);

			viewMenu->addAction(dock->toggleViewAction());

			//Explorer
			dock = new QDockWidget(tr("Explorer"), this);
			dock->setObjectName("studio_explorer");
			dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

			dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);

			explorer = new QTreeWidget();
			explorer->setSelectionMode(QAbstractItemView::ExtendedSelection);
			explorer->setSelectionBehavior(QAbstractItemView::SelectItems);
			explorer->header()->close();

			dock->setWidget(explorer);
			addDockWidget(Qt::RightDockWidgetArea, dock);

			viewMenu->addAction(dock->toggleViewAction());

			//Last View Menu things
			viewMenu->addSeparator();

			QMenu* viewToolbarsMenu = viewMenu->addMenu("Toolbars");

			statusBar();

			QToolBar* commandBar = new QToolBar("Command");
			commandBar->setObjectName("studio_command_bar");
			commandBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
			viewToolbarsMenu->addAction(commandBar->toggleViewAction());

			//START COMMAND BAR
			//TODO: Implement saving history of commands
			cmdBar = new QComboBox();
			cmdBar->setMaxCount(10);
			cmdBar->setEditable(true);

			//Setup QLineEdit in QComboBox
			QLineEdit* cmdEdit = cmdBar->lineEdit();
			cmdEdit->setPlaceholderText("Run a command");
			connect(cmdEdit, &QLineEdit::returnPressed, this, &StudioWindow::commandBarReturn);
			cmdEdit->setDisabled(true);

			cmdBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
			commandBar->setBaseSize(300, 20);
			commandBar->setMinimumWidth(300);

			commandBar->addWidget(cmdBar);

			addToolBar(Qt::BottomToolBarArea, commandBar);
			//END COMMAND BAR
		}

		void StudioWindow::about(){
			QMessageBox::about(this, tr("About OpenBlox Studio"),
							   tr("<b>OpenBlox Studio</b><br/>"
								  "Version 0.1.1<br/>"
								  "OpenBlox"));
		}

		void StudioWindow::showSettings(){
			//TODO
		}

		void StudioWindow::closeStudio(){
		    close();
		}

		void StudioWindow::newInstance(){
			if(glWidget){
			    //New process of openblox_studio
			}else{
				glWidget = new StudioGLWidget();
				int tabIdx = tabWidget->addTab(glWidget, "Game");
			    QTabBar* tabBar = tabWidget->tabBar();
				if(tabBar){
					QWidget* tabBtn = tabBar->tabButton(tabIdx, QTabBar::RightSide);
					if(tabBtn){
						tabBtn->resize(0, 0);
						tabBtn->setVisible(false);
					}
				}
			}
		}

		void StudioWindow::commandBarReturn(){
			QLineEdit* cmdEdit = cmdBar->lineEdit();
			QString text = cmdEdit->text();

			output->append("> " + text + "<br/>");

		    OB::OBEngine* eng = OB::OBEngine::getInstance();
			if(!eng){
				return;
			}

			lua_State* gL = eng->getGlobalLuaState();
			if(!gL){
				//Prevents segfaults when commands are run before initialization
				return;
			}
			
			lua_State* L = Lua::initThread(gL);
			//lua_resume(L, NULL, 0);

			int s = luaL_loadstring(L, text.toStdString().c_str());
			if(s == 0){
				//s = lua_pcall(L, 0, LUA_MULTRET, 0);
				s = lua_resume(L, NULL, 0);
			}

			if(s != 0 && s != LUA_YIELD){
			    Lua::handle_errors(L);
				Lua::close_state(L);
			}
			if(s == LUA_OK){
				Lua::close_state(L);
			}
		}

		void StudioWindow::initGL(){
			QTreeWidgetItem* rootItem = explorer->invisibleRootItem();

		    OB::OBEngine* eng = OB::OBEngine::getInstance();
			if(eng){
				eng->init();
				shared_ptr<OB::Instance::DataModel> dm = eng->getDataModel();
				if(dm){
					shared_ptr<OB::Instance::LogService> ls = dm->getLogService();
					if(ls){
						ls->getMessageOut()->Connect(handle_log_event, this);
					}
					addDM(rootItem, dynamic_pointer_cast<Instance::Instance>(dm), this);
				}
			}

		    cmdBar->lineEdit()->setDisabled(false);
		}
	}
}
