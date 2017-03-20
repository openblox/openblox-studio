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

#include "InstanceTreeItem.h"

#include <openblox.h>
#include <instance/Instance.h>
#include <instance/DataModel.h>
#include <type/Event.h>

namespace OB{
	namespace Studio{
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

			kidItem->setText(0, QString(kid->Name.c_str()));
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

		void dm_instance_child_added_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, void* ud){
		    QTreeWidgetItem* parentItem = (QTreeWidgetItem*)ud;

			if(evec.size() == 1){
			    shared_ptr<Instance::Instance> newGuy = dynamic_pointer_cast<Instance::Instance>(evec[0]->asInstance());
				if(treeItemMap.contains(newGuy)){
					parentItem->addChild(treeItemMap[newGuy]);
				}else{
					addChildOfInstance(parentItem, newGuy);
				}
			}
		}

		void dm_instance_child_removed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, void* ud){
			QTreeWidgetItem* parentItem = (QTreeWidgetItem*)ud;
			
			if(evec.size() == 1){
				shared_ptr<Instance::Instance> newGuy = dynamic_pointer_cast<Instance::Instance>(evec[0]->asInstance());
				if(treeItemMap.contains(newGuy)){
					parentItem->removeChild(treeItemMap[newGuy]);
				}
			}
		}

		void addDM(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst){			
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
		}

		StudioWindow::StudioWindow(){
			glWidget = new StudioGLWidget();
			setCentralWidget(glWidget);

			//Menus
			QMenu* fileMenu = menuBar()->addMenu("File");
			fileMenu->addAction("New");
			fileMenu->addAction("Open...");
			fileMenu->addAction("Close")->setEnabled(false);

			fileMenu->addSeparator();

			fileMenu->addAction("Save")->setEnabled(false);
			fileMenu->addAction("Save As...")->setEnabled(false);
			fileMenu->addSeparator();
			fileMenu->addAction("Exit");

			QMenu* editMenu = menuBar()->addMenu("Edit");
			editMenu->addAction("Undo");
			editMenu->addAction("Redo");

			editMenu->addSeparator();

			editMenu->addAction("Cut")->setEnabled(false);
			editMenu->addAction("Copy")->setEnabled(false);
			editMenu->addAction("Paste")->setEnabled(false);
			editMenu->addAction("Duplicate")->setEnabled(false);

			editMenu->addSeparator();

			QAction* settingsAct = editMenu->addAction("Settings");
			connect(settingsAct, &QAction::triggered, this, &StudioWindow::showSettings);

			QMenu* viewMenu = menuBar()->addMenu("View");

			menuBar()->addSeparator();

			QAction* aboutAct = new QAction("About", this);
			aboutAct->setStatusTip("About OpenBlox Studio");
			connect(aboutAct, &QAction::triggered, this, &StudioWindow::about);

			QMenu* helpMenu = menuBar()->addMenu("Help");
			helpMenu->addAction(aboutAct);

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

		void StudioWindow::about(bool checked){
			Q_UNUSED(checked);

			QMessageBox::about(this, tr("About OpenBlox Studio"),
							   tr("<b>OpenBlox Studio</b><br/>"
								  "Version 0.1.1<br/>"
								  "OpenBlox"));
		}

		void StudioWindow::showSettings(bool checked){
			Q_UNUSED(checked);
			//TODO
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
				std::string lerr = Lua::handle_errors(L);

				std::cerr << "A Lua error occurred:" << std::endl;
				std::cerr << lerr << std::endl;

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
				addDM(rootItem, dynamic_pointer_cast<Instance::Instance>(eng->getDataModel()));
			}

		    cmdBar->lineEdit()->setDisabled(false);
		}
	}
}
