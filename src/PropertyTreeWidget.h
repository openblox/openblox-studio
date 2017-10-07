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

#ifndef OB_STUDIO_PROPERTYTREEWIDGET_H_
#define OB_STUDIO_PROPERTYTREEWIDGET_H_

#include <QTreeWidgetItem>

#include <instance/Instance.h>

namespace OB{
	namespace Studio{
		class PropertyItem;

		class PropertyTreeWidget: public QTreeWidget{
		public:
			PropertyTreeWidget();
			virtual ~PropertyTreeWidget();

			void updateSelection(std::vector<shared_ptr<Instance::Instance>> selectedInstances);
			void updateValue(std::string prop);
			void setProp(std::string prop, shared_ptr<Type::VarWrapper> val);

			PropertyItem* propertyItemAt(const QModelIndex &index);

		private:
			std::vector<shared_ptr<Instance::Instance>> editingInstances;
			std::map<std::string, PropertyItem*> curProps;
		};
	}
}

#endif

// Local Variables:
// mode: c++
// End:
