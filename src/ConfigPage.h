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

#ifndef OB_STUDIO_CONFIGPAGE_H_
#define OB_STUDIO_CONFIGPAGE_H_

#include <QCheckBox>
#include <QSpinBox>

namespace OB{
	namespace Studio{
		class ConfigDialog;

		class ConfigPage: public QWidget{
		public:
			ConfigPage(ConfigDialog* dia);

			ConfigDialog* getConfigDialog();

			virtual void saveChanges() = 0;

		private:
			ConfigDialog* dia;
		};

		class GeneralConfigPage: public ConfigPage{
		public:
			GeneralConfigPage(ConfigDialog* dia);

			virtual void saveChanges();

		private:
			QCheckBox* opt_useDarkTheme;
		};

		class OutputConfigPage: public ConfigPage{
		public:
			OutputConfigPage(ConfigDialog* dia);

			virtual void saveChanges();

		private:
			QSpinBox* opt_history;
		};
	}
}

#endif

// Local Variables:
// mode: c++
// End:
