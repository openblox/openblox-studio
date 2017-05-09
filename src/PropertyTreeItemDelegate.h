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

#ifndef OB_STUDIO_PROPERTYTREEITEMDELEGATE_H_
#define OB_STUDIO_PROPERTYTREEITEMDELEGATE_H_

#include <PropertyTreeWidget.h>
#include <QStyledItemDelegate>

namespace OB{
	namespace Studio{
		class PropertyTreeItemDelegate: public QStyledItemDelegate{
		  public:
		    PropertyTreeItemDelegate(PropertyTreeWidget* treeWidget);
			virtual ~PropertyTreeItemDelegate();

			virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
			virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

			virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
			virtual void setEditorData(QWidget* editor, const QModelIndex &index) const;
			virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex &index) const;
			virtual bool editorEvent(QEvent* evt, QAbstractItemModel* model, const QStyleOptionViewItem &option, const QModelIndex &index);

		  private:
		    PropertyTreeWidget* treeWidget;
		};
	}
}

#endif
