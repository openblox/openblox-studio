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

		    if(!editingInstances.empty()){
				std::set<std::string> sharedProperties;
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
							auto fIt = tprops.find(*it);
							if(fIt == tprops.end()){
							    it = sharedProperties.erase(it);
							}else{
								Instance::_PropertyInfo pInfo = fIt->second;
								if(!pInfo.isPublic){
									it = sharedProperties.erase(it);
								}else{
									Instance::_PropertyInfo pInfoOrig = props[*it];
									if(pInfoOrig.type != pInfo.type){
										it = sharedProperties.erase(it);
									}else{
										++it;
									}
								}
							}
						}
					}
				}

				for(auto it = sharedProperties.begin(); it != sharedProperties.end(); ++it){
					std::string propName = *it;

					Instance::_PropertyInfo pInfo = props[propName];

				    PropertyItem* oldItem = curProps[propName];
					if(oldItem){
						if(oldItem->getPropertyType() == pInfo.type){
							updateValue(propName);
							continue;
						}else{
							invisibleRootItem()->removeChild(oldItem);
							curProps.erase(propName);
							delete oldItem;
						}
					}

					if(pInfo.type == "string"){
					    StringPropertyItem* pi = new StringPropertyItem(QString(propName.c_str()));
						curProps[propName] = pi;
						addTopLevelItem(pi);
						updateValue(propName);
						continue;
					}
				}
		    }else{
				clear();
				curProps.clear();
			}
		}

		void PropertyTreeWidget::updateValue(std::string prop){
		    PropertyItem* propItem = curProps[prop];
			if(propItem){
				shared_ptr<Type::VarWrapper> toSet;
				bool firstPass = true;
				bool hasMultiple = false;

				for(auto i = editingInstances.begin(); i != editingInstances.end(); ++i){
					shared_ptr<Instance::Instance> inst = *i;
					if(inst){
						shared_ptr<Type::VarWrapper> iVal = inst->getProperty(prop);
						if(firstPass){
							toSet = iVal;
							firstPass = false;
						}else{
							if(!iVal->valueEquals(toSet)){
							    hasMultiple = true;
								break;
							}
						}
					}
				}

				if(hasMultiple){
					toSet = make_shared<Type::VarWrapper>();
				}

				propItem->setValue(toSet);
			}
		}

		PropertyItem* PropertyTreeWidget::propertyItemAt(const QModelIndex &index){
			return dynamic_cast<PropertyItem*>(itemFromIndex(index));
		}
	}
}
