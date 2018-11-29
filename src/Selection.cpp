/*
 * Copyright (C) 2017 John M. Harris, Jr. <johnmh@openblox.org>
 *
 * This file is part of OpenBlox.
 *
 * OpenBlox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenBlox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with OpenBlox. If not, see <https://www.gnu.org/licenses/>.
 */

#include "Selection.h"

#include <OBEngine.h>
#include <instance/DataModel.h>

#include <OBException.h>

#include "StudioWindow.h"
#include "StudioGLWidget.h"

namespace OB{
	namespace Instance{
		DEFINE_CLASS(Selection, false, isDataModel, Instance){
			registerLuaClass(eng, LuaClassName, register_lua_metamethods, register_lua_methods, register_lua_property_getters, register_lua_property_setters, register_lua_events);
		}

		Selection::Selection(OBEngine* eng) : Instance(eng){
			Name = ClassName;
			netId = OB_NETID_NOT_REPLICATED;

			Archivable = false;

			SelectionChanged = make_shared<Type::Event>("SelectionChanged");
		}

		Selection::~Selection(){}

	    void Selection::setArchivable(bool archivable){
			throw new OBException("The Archivable property of Selection is read-only.");
		}

		shared_ptr<Type::Event> Selection::getSelectionChanged(){
			return SelectionChanged;
		}

		shared_ptr<Instance> Selection::cloneImpl(){
			return NULL;
		}

		std::vector<shared_ptr<Instance>> Selection::Get(){
			Studio::StudioWindow* win = Studio::StudioWindow::static_win;

			if(win){
				Studio::StudioGLWidget* gW = win->getCurrentGLWidget(eng);
				if(gW){
					return gW->selectedInstances;
				}
			}
			return std::vector<shared_ptr<Instance>>();
		}

		int Selection::lua_Get(lua_State* L){
			shared_ptr<Instance> inst = checkInstance(L, 1, false);

			if(inst){
				shared_ptr<Selection> instS = dynamic_pointer_cast<Selection>(inst);
				if(instS){
					lua_newtable(L);
					int lIndex = 1;

					std::vector<shared_ptr<Instance>> curSelection = instS->Get();
					for(int i = 0; i < curSelection.size(); i++){
						shared_ptr<Instance> kid = curSelection.at(i);
						if(kid){
							kid->wrap_lua(L);
							lua_rawseti(L, -2, lIndex++);
						}
					}

					return 1;
				}
			}

			return luaL_error(L, COLONERR, "Get");
		}

		void Selection::register_lua_methods(lua_State* L){
			Instance::register_lua_methods(L);

			luaL_Reg methods[] = {
				{"Get", lua_Get},
				{NULL, NULL}
			};
			luaL_setfuncs(L, methods, 0);
		}

		void Selection::register_lua_events(lua_State* L){
			Instance::register_lua_events(L);

			luaL_Reg events[] = {
				{"SelectionChanged", WRAP_EVT(Selection, SelectionChanged)},
				{NULL, NULL}
			};
			luaL_setfuncs(L, events, 0);
		}
	}
}
