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

#include "PropertyTreeItemDelegate.h"

namespace OB{
	namespace Studio{
		PropertyTreeItemDelegate::PropertyTreeItemDelegate(PropertyTreeWidget* treeWidget){
		    this->treeWidget = treeWidget;
		}

	    PropertyTreeItemDelegate::~PropertyTreeItemDelegate(){}

		QSize PropertyTreeItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
			return QStyledItemDelegate::sizeHint(option, index) + QSize(4, 4);
		}
		
	    void PropertyTreeItemDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem &option, const QModelIndex &index) const{
			if(!editor){
				return;
			}
			
			editor->setGeometry(option.rect);
		}

		QWidget* PropertyTreeItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const{
			if(index.column() == 1){
					
			}
			return NULL;
		}

		void PropertyTreeItemDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const{
			if(!editor){
				return;
			}

			
		}

	    void PropertyTreeItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const{
			if(!editor){
				return;
			}

			
		}

		bool PropertyTreeItemDelegate::editorEvent(QEvent* evt, QAbstractItemModel* model, const QStyleOptionViewItem &option, const QModelIndex &index){
			if(index.column() == 1){
				
			}

			return false;
		}
	}
}
