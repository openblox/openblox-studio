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

#include <set>

#include "PropertyTreeWidget.h"
#include "PropertyTreeItemDelegate.h"

#include "PropertyItem.h"

namespace OB{
	namespace Studio{
		PropertyTreeWidget::PropertyTreeWidget(){
			setUniformRowHeights(true);
			setAlternatingRowColors(true);
			setRootIsDecorated(false);
			setColumnCount(2);
			setSelectionBehavior(QAbstractItemView::SelectRows);
			setAcceptDrops(false);
			setDragEnabled(false);

			QStringList headerLabels;
			headerLabels << "Property" << "Value";
			setHeaderLabels(headerLabels);
			setHeaderHidden(true);

			QHeaderView* hv = header();
			hv->setStretchLastSection(true);
			hv->setSectionResizeMode(0, QHeaderView::ResizeToContents);

			setEditTriggers(QAbstractItemView::NoEditTriggers);
			setItemDelegate(new PropertyTreeItemDelegate(this));
		}

	    PropertyTreeWidget::~PropertyTreeWidget(){}

		void PropertyTreeWidget::updateSelection(std::vector<shared_ptr<Instance::Instance>> selectedInstances){
			editingInstances = selectedInstances;

			//Remove all props
			clear();

			std::set<std::string> sharedProperties;

			if(!editingInstances.empty()){
			    std::map<std::string, Instance::_PropertyInfo> props = editingInstances[0]->getProperties();

				// Push names of all properties to sharedProperties
				for(auto it = props.begin(); it != props.end(); ++it){
				    sharedProperties.insert(it->first);
				}

				// Remove properties not all instances have
				for(auto i = editingInstances.begin(); i != editingInstances.end(); ++i){
					shared_ptr<Instance::Instance> inst = *i;
					if(inst){
						std::map<std::string, Instance::_PropertyInfo> tprops = inst->getProperties();

						for(auto it = sharedProperties.begin(); it != sharedProperties.end();){
							if(tprops.find(*it) == tprops.end()){
							    it = sharedProperties.erase(it);
							}else{
								++it;
							}
						}
					}
				}
			    
				for(auto it = sharedProperties.begin(); it != sharedProperties.end(); ++it){
					std::string propName = *it;

				    PropertyItem* itm = new PropertyItem(QString(propName.c_str()));
				    addTopLevelItem(itm);
				}
		    }
		}

		void PropertyTreeWidget::updateValues(){
			
		}

		PropertyItem* PropertyTreeWidget::propertyItemAt(const QModelIndex &index){
			return dynamic_cast<PropertyItem*>(itemFromIndex(index));
		}
	}
}
