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

#include "InstanceTree.h"

#include <QtWidgets>

#include <instance/Instance.h>

#include "InstanceTreeItem.h"

namespace OB{
	namespace Studio{
		InstanceTree::InstanceTree(){
			setSelectionMode(QAbstractItemView::ExtendedSelection);
			setSelectionBehavior(QAbstractItemView::SelectItems);
			setAcceptDrops(true);
			setDragEnabled(true);
			setDragDropMode(QAbstractItemView::InternalMove);
			header()->close();

			connect(this, &QTreeWidget::itemChanged, this, &InstanceTree::itemEdited);
		}

		InstanceTree::~InstanceTree(){}

		void InstanceTree::dropEvent(QDropEvent* evt){
		    QTreeWidgetItem* dropTarg = itemAt(evt->pos());
			if(dropTarg){
				QList<QTreeWidgetItem*> dragItems = selectedItems();
				
				InstanceTreeItem* targItem = dynamic_cast<InstanceTreeItem*>(dropTarg);
				if(targItem){
					for(int i = 0; i < dragItems.size(); i++){
						InstanceTreeItem* srcItem = dynamic_cast<InstanceTreeItem*>(dragItems[i]);
						if(srcItem){
							shared_ptr<Instance::Instance> instPtr = srcItem->GetInstance();
							if(instPtr){
								instPtr->setParent(targItem->GetInstance(), true);
							}
						}
					}
				}
			}
		}

		void InstanceTree::itemEdited(QTreeWidgetItem* item){
			InstanceTreeItem* iti = dynamic_cast<InstanceTreeItem*>(item);
			if(iti){
				shared_ptr<Instance::Instance> inst = iti->GetInstance();
				if(inst){
					inst->setName(iti->text(0).toStdString().c_str());
				}
			}
		}
	}
}
