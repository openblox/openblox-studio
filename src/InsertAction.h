/*
 * Copyright (C) 2019 John M. Harris, Jr. <johnmh@openblox.org>
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

#ifndef OB_STUDIO_INSERTACTION_H_
#define OB_STUDIO_INSERTACTION_H_

#include <QAction>

namespace OB{
	namespace Studio{
		class InsertAction: public QAction{
		public:
		    InsertAction(QString instanceClass, QObject* parent = 0);
			virtual ~InsertAction();

			void doInsert();
		};
	}
}

#endif

// Local Variables:
// mode: c++
// End:
