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

#include "StudioGLWidget.h"

#include "StudioWindow.h"
#include "InstanceTree.h"

#include <openblox.h>
#include <instance/LogService.h>

#include <functional>

#include <QtGui>

// Native keycodes
#ifdef _WIN32
#include <Winuser.h>
#elif __APPLE__
#else
#include <X11/keysym.h>
#endif

namespace OB{
	namespace Studio{
		StudioGLWidget::StudioGLWidget(OBEngine* eng) : StudioTabWidget(eng){
			setAttribute(Qt::WA_OpaquePaintEvent);
			setFocusPolicy(Qt::StrongFocus);

			setAutoFillBackground(false);

			setUpdatesEnabled(false);
			setMouseTracking(true);

			draw_axis = false;

			has_focus = false;
			logHist = "";
		}

		StudioGLWidget::~StudioGLWidget(){}

		QSize StudioGLWidget::minimumSizeHint() const{
			return QSize(320, 240);
		}

		QSize StudioGLWidget::sizeHint() const{
			return QSize(320, 240);
		}

		void StudioGLWidget::do_init(){
			using namespace std::placeholders;

			if(!eng){
				throw OB::OBException("game is NULL!");
			}

			eng->setWindowId((void*)winId());
			eng->init();

			eng->setPostRenderFunc(std::bind(&StudioGLWidget::post_render_func, this, _1));

			StudioWindow* win = StudioWindow::static_win;

			shared_ptr<OB::Instance::DataModel> dm = eng->getDataModel();
			if(dm){
				shared_ptr<OB::Instance::LogService> ls = dm->getLogService();
				if(ls){
					std::function<void(std::vector<shared_ptr<Type::VarWrapper>>)> lsb = std::bind(&StudioGLWidget::handle_log_event, this, _1);
					ls->getMessageOut()->Connect(lsb);
				}
			}
		}

		void StudioGLWidget::do_render(){
			if(eng){
				eng->render();
			}
		}

		void StudioGLWidget::post_render_func(irr::video::IVideoDriver* videoDriver){
			if(draw_axis){
				puts("Drawing");

				glViewport(0, 0, 50, 50);

				glMatrixMode(GL_PROJECTION);
				glPushMatrix();

				GLdouble fH = tan(45.0 / 360 * M_PI) * 0.1;
				GLdouble fW = fH * 1.0;
				glFrustum(-fW, fW, -fH, fH, 0.1, 20);

				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();

				//TODO: Grab this from the CurrentCamera
				//gluLookAt(10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.0f);

				glColor3f(1.0f, 0.0f, 0.0f);
				glEnable(GL_LINE_SMOOTH);
				glLineWidth( 1.5 );
				glBegin(GL_LINES);
				glVertex3f(-1000, 0, 0);
				glVertex3f(1000, 0, 0);
				glEnd();

				//Restore View
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glViewport(0, 0, 960, 600);
			}
		}

		void StudioGLWidget::setAxisWidgetVisible(bool axisWidgetVisible){
			draw_axis = axisWidgetVisible;
		}

	    bool StudioGLWidget::isAxisWidgetVisible(){
			return draw_axis;
		}

		void StudioGLWidget::remove_focus(){
			has_focus = false;

			StudioWindow::static_win->explorer->invisibleRootItem()->takeChildren();

			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					ier->unfocus();
				}
			}
		}

		void StudioGLWidget::gain_focus(){
			using namespace std::placeholders;

			has_focus = true;

			shared_ptr<OB::Instance::DataModel> dm = eng->getDataModel();
			if(dm){
				addDM(StudioWindow::static_win->explorer->invisibleRootItem(), dynamic_pointer_cast<Instance::Instance>(dm));
			}

			StudioWindow* win = StudioWindow::static_win;
			if(win->output){
				win->output->setHtml(logHist);
			}

			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					ier->focus();
				}
			}
		}

		void StudioGLWidget::resizeEvent(QResizeEvent* evt){
			QWidget::resizeEvent(evt);

			if(eng){
				QSize newSize = evt->size();
				eng->resized(newSize.width(), newSize.height());
			}
		}

		void StudioGLWidget::mousePressEvent(QMouseEvent* event){
			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					OB::Enum::MouseButton mbtn = OB::Enum::MouseButton::Unknown;

					switch(event->button()){
						case Qt::LeftButton: {
							mbtn = OB::Enum::MouseButton::Left;
							break;
						}
						case Qt::MidButton: {
							mbtn = OB::Enum::MouseButton::Middle;
							break;
						}
						case Qt::RightButton: {
							mbtn = OB::Enum::MouseButton::Right;
							break;
						}
						case Qt::BackButton: {
							mbtn = OB::Enum::MouseButton::X1;
							break;
						}
						case Qt::ForwardButton: {
							mbtn = OB::Enum::MouseButton::X2;
							break;
						}
					}

					ier->input_mouseButton(mbtn, true);
				}
			}
		}

		void StudioGLWidget::mouseReleaseEvent(QMouseEvent* event){
			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					OB::Enum::MouseButton mbtn = OB::Enum::MouseButton::Unknown;

					switch(event->button()){
						case Qt::LeftButton: {
							mbtn = OB::Enum::MouseButton::Left;
							break;
						}
						case Qt::MidButton: {
							mbtn = OB::Enum::MouseButton::Middle;
							break;
						}
						case Qt::RightButton: {
							mbtn = OB::Enum::MouseButton::Right;
							break;
						}
						case Qt::BackButton: {
							mbtn = OB::Enum::MouseButton::X1;
							break;
						}
						case Qt::ForwardButton: {
							mbtn = OB::Enum::MouseButton::X2;
							break;
						}
					}

					ier->input_mouseButton(mbtn, true);
				}
			}
		}

		void StudioGLWidget::mouseMoveEvent(QMouseEvent* event){
			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					QPoint mousePos = event->pos();

					ier->input_mouseMoved(make_shared<OB::Type::Vector2>(mousePos.x(), mousePos.y()), NULL);
				}
			}
		}

		void StudioGLWidget::wheelEvent(QWheelEvent* event){
			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					QPoint wheelDelta = event->angleDelta() / 8;

					if(!wheelDelta.isNull()){
					    QPoint numSteps = wheelDelta / 15;
						ier->input_mouseWheel(make_shared<OB::Type::Vector2>(numSteps.x(), numSteps.y()));
					}
				}
			}
		}

		OB::Enum::KeyCode ob_studio_qt_key_to_ob(QKeyEvent* event){
			// We use native keys to catch keys Qt doesn't separate with their keycodes
			int virtKey = event->nativeVirtualKey();
			switch(virtKey){
#ifdef _WIN32
				case VK_RSHIFT: {
					return OB::Enum::KeyCode::RightShift;
				}
				case VK_RCONTROL: {
					return OB::Enum::KeyCode::RightControl;
				}
				case VK_NUMPAD0: {
					return OB::Enum::KeyCode::NumpadZero;
				}
				case VK_NUMPAD1: {
					return OB::Enum::KeyCode::NumpadOne;
				}
				case VK_NUMPAD2: {
					return OB::Enum::KeyCode::NumpadTwo;
				}
				case VK_NUMPAD3: {
					return OB::Enum::KeyCode::NumpadThree;
				}
				case VK_NUMPAD4: {
					return OB::Enum::KeyCode::NumpadFour;
				}
				case VK_NUMPAD5: {
					return OB::Enum::KeyCode::NumpadFive;
				}
				case VK_NUMPAD6: {
					return OB::Enum::KeyCode::NumpadSix;
				}
				case VK_NUMPAD7: {
					return OB::Enum::KeyCode::NumpadSeven;
				}
				case VK_NUMPAD8: {
					return OB::Enum::KeyCode::NumpadEight;
				}
				case VK_NUMPAD9: {
					return OB::Enum::KeyCode::NumpadNine;
				}
				case VK_MULTIPLY: {
					return OB::Enum::KeyCode::NumpadMultiply;
				}
				case VK_DECIMAL: {
					return OB::Enum::KeyCode::NumpadPeriod;
				}
				case VK_SUBTRACT: {
					return OB::Enum::KeyCode::NumpadMinus;
				}
				case VK_DIVIDE: {
					return OB::Enum::KeyCode::NumpadDivide;
				}
				case VK_ADD: {
					return OB::Enum::KeyCode::NumpadPlus;
				}
#elif __APPLE__
				//TODO: Figure out apple keycodes
#else
				case XK_Shift_R: {
					return OB::Enum::KeyCode::RightShift;
				}
				case XK_Control_R: {
					return OB::Enum::KeyCode::RightControl;
				}
				case XK_KP_0: {
					return OB::Enum::KeyCode::NumpadZero;
				}
				case XK_KP_1: {
					return OB::Enum::KeyCode::NumpadOne;
				}
				case XK_KP_2: {
					return OB::Enum::KeyCode::NumpadTwo;
				}
				case XK_KP_3: {
					return OB::Enum::KeyCode::NumpadThree;
				}
				case XK_KP_4: {
					return OB::Enum::KeyCode::NumpadFour;
				}
				case XK_KP_5: {
					return OB::Enum::KeyCode::NumpadFive;
				}
				case XK_KP_6: {
					return OB::Enum::KeyCode::NumpadSix;
				}
				case XK_KP_7: {
					return OB::Enum::KeyCode::NumpadSeven;
				}
				case XK_KP_8: {
					return OB::Enum::KeyCode::NumpadEight;
				}
				case XK_KP_9: {
					return OB::Enum::KeyCode::NumpadNine;
				}
				case XK_KP_Multiply: {
					return OB::Enum::KeyCode::NumpadMultiply;
				}
				case XK_KP_Decimal: {
					return OB::Enum::KeyCode::NumpadPeriod;
				}
				case XK_KP_Subtract: {
					return OB::Enum::KeyCode::NumpadMinus;
				}
				case XK_KP_Divide: {
					return OB::Enum::KeyCode::NumpadDivide;
				}
				case XK_KP_Add: {
					return OB::Enum::KeyCode::NumpadPlus;
				}
				case XK_KP_Enter: {
					return OB::Enum::KeyCode::NumpadEnter;
				}
#endif
			}

			// Otherwise, we can just use their keycodes
			int qtKey = event->key();

			switch(qtKey){
				case Qt::Key_Escape: {
					return OB::Enum::KeyCode::Escape;
				}
				case Qt::Key_Tab: {
					return OB::Enum::KeyCode::Tab;
				}
				case Qt::Key_Backspace: {
					return OB::Enum::KeyCode::Backspace;
				}
				case Qt::Key_Return: {
					return OB::Enum::KeyCode::Return;
				}
				case Qt::Key_Enter: {
					return OB::Enum::KeyCode::NumpadEnter;
				}
				case Qt::Key_Insert: {
					return OB::Enum::KeyCode::Insert;
				}
				case Qt::Key_Delete: {
					return OB::Enum::KeyCode::Delete;
				}
				case Qt::Key_Pause: {
					return OB::Enum::KeyCode::Pause;
				}
				case Qt::Key_Print: {
					return OB::Enum::KeyCode::Print;
				}
				case Qt::Key_SysReq: {
					return OB::Enum::KeyCode::SysRq;
				}
				case Qt::Key_Clear: {
					return OB::Enum::KeyCode::Clear;
				}
				case Qt::Key_Home: {
					return OB::Enum::KeyCode::Home;
				}
				case Qt::Key_End: {
					return OB::Enum::KeyCode::End;
				}
				case Qt::Key_Left: {
					return OB::Enum::KeyCode::Left;
				}
				case Qt::Key_Up: {
					return OB::Enum::KeyCode::Up;
				}
				case Qt::Key_Right: {
					return OB::Enum::KeyCode::Right;
				}
				case Qt::Key_Down: {
					return OB::Enum::KeyCode::Down;
				}
				case Qt::Key_PageUp: {
					return OB::Enum::KeyCode::PageUp;
				}
				case Qt::Key_PageDown: {
					return OB::Enum::KeyCode::PageDown;
				}
				case Qt::Key_Shift: {
					return OB::Enum::KeyCode::LeftShift;
				}
				case Qt::Key_Control: {
					return OB::Enum::KeyCode::LeftControl;
				}
				case Qt::Key_Alt: {
					return OB::Enum::KeyCode::LeftAlt;
				}
				case Qt::Key_CapsLock: {
					return OB::Enum::KeyCode::CapsLock;
				}
				case Qt::Key_NumLock: {
					return OB::Enum::KeyCode::NumLock;
				}
				case Qt::Key_ScrollLock: {
					return OB::Enum::KeyCode::ScrollLock;
				}
				case Qt::Key_F1: {
					return OB::Enum::KeyCode::F1;
				}
				case Qt::Key_F2: {
					return OB::Enum::KeyCode::F2;
				}
				case Qt::Key_F3: {
					return OB::Enum::KeyCode::F3;
				}
				case Qt::Key_F4: {
					return OB::Enum::KeyCode::F4;
				}
				case Qt::Key_F5: {
					return OB::Enum::KeyCode::F5;
				}
				case Qt::Key_F6: {
					return OB::Enum::KeyCode::F6;
				}
				case Qt::Key_F7: {
					return OB::Enum::KeyCode::F7;
				}
				case Qt::Key_F8: {
					return OB::Enum::KeyCode::F8;
				}
				case Qt::Key_F9: {
					return OB::Enum::KeyCode::F9;
				}
				case Qt::Key_F10: {
					return OB::Enum::KeyCode::F10;
				}
				case Qt::Key_F11: {
					return OB::Enum::KeyCode::F11;
				}
				case Qt::Key_F12: {
					return OB::Enum::KeyCode::F12;
				}
				case Qt::Key_F13: {
					return OB::Enum::KeyCode::F13;
				}
				case Qt::Key_F14: {
					return OB::Enum::KeyCode::F14;
				}
				case Qt::Key_F15: {
					return OB::Enum::KeyCode::F15;
				}
				case Qt::Key_F16: {
					return OB::Enum::KeyCode::F16;
				}
				case Qt::Key_F17: {
					return OB::Enum::KeyCode::F17;
				}
				case Qt::Key_F18: {
					return OB::Enum::KeyCode::F18;
				}
				case Qt::Key_F19: {
					return OB::Enum::KeyCode::F19;
				}
				case Qt::Key_F20: {
					return OB::Enum::KeyCode::F20;
				}
				case Qt::Key_F21: {
					return OB::Enum::KeyCode::F21;
				}
				case Qt::Key_F22: {
					return OB::Enum::KeyCode::F22;
				}
				case Qt::Key_F23: {
					return OB::Enum::KeyCode::F23;
				}
				case Qt::Key_F24: {
					return OB::Enum::KeyCode::F24;
				}
				//TODO: Consider adding F25-F35 to OB
				case Qt::Key_Super_L: {
					return OB::Enum::KeyCode::LeftSuper;
				}
				case Qt::Key_Super_R: {
					return OB::Enum::KeyCode::RightSuper;
				}
				case Qt::Key_Menu: {
					return OB::Enum::KeyCode::Menu;
				}
				case Qt::Key_Help: {
					return OB::Enum::KeyCode::Help;
				}
				case Qt::Key_Space: {
					return OB::Enum::KeyCode::Space;
				}
				case Qt::Key_Exclam: {
					return OB::Enum::KeyCode::Exclamation;
				}
				case Qt::Key_QuoteDbl: {
					return OB::Enum::KeyCode::DoubleQuote;
				}
				case Qt::Key_Dollar: {
					return OB::Enum::KeyCode::Dollar;
				}
				case Qt::Key_Percent: {
					return OB::Enum::KeyCode::Percent;
				}
				case Qt::Key_Ampersand: {
					return OB::Enum::KeyCode::Ampersand;
				}
				case Qt::Key_Apostrophe: {
					return OB::Enum::KeyCode::Quote;
				}
				case Qt::Key_ParenLeft: {
					return OB::Enum::KeyCode::LeftParenthesis;
				}
				case Qt::Key_ParenRight: {
				    return OB::Enum::KeyCode::RightParenthesis;
				}
				case Qt::Key_Asterisk: {
					return OB::Enum::KeyCode::Asterisk;
				}
				case Qt::Key_Plus: {
					return OB::Enum::KeyCode::Plus;
				}
				case Qt::Key_Comma: {
					return OB::Enum::KeyCode::Comma;
				}
				case Qt::Key_Minus: {
					return OB::Enum::KeyCode::Minus;
				}
				case Qt::Key_Period: {
					return OB::Enum::KeyCode::Period;
				}
				case Qt::Key_Slash: {
					return OB::Enum::KeyCode::Slash;
				}
				case Qt::Key_0: {
					return OB::Enum::KeyCode::Zero;
				}
				case Qt::Key_1: {
					return OB::Enum::KeyCode::One;
				}
				case Qt::Key_2: {
					return OB::Enum::KeyCode::Two;
				}
				case Qt::Key_3: {
					return OB::Enum::KeyCode::Three;
				}
				case Qt::Key_4: {
					return OB::Enum::KeyCode::Four;
				}
				case Qt::Key_5: {
					return OB::Enum::KeyCode::Five;
				}
				case Qt::Key_6: {
					return OB::Enum::KeyCode::Six;
				}
				case Qt::Key_7: {
					return OB::Enum::KeyCode::Seven;
				}
				case Qt::Key_8: {
					return OB::Enum::KeyCode::Eight;
				}
				case Qt::Key_9: {
					return OB::Enum::KeyCode::Nine;
				}
				case Qt::Key_Colon: {
					return OB::Enum::KeyCode::Colon;
				}
				case Qt::Key_Semicolon: {
					return OB::Enum::KeyCode::Semicolon;
				}
				case Qt::Key_Less: {
					return OB::Enum::KeyCode::LessThan;
				}
				case Qt::Key_Equal: {
					return OB::Enum::KeyCode::Equals;
				}
				case Qt::Key_Greater: {
					return OB::Enum::KeyCode::GreaterThan;
				}
				case Qt::Key_Question: {
					return OB::Enum::KeyCode::Question;
				}
				case Qt::Key_At: {
					return OB::Enum::KeyCode::At;
				}
				case Qt::Key_A: {
					return OB::Enum::KeyCode::A;
				}
				case Qt::Key_B: {
					return OB::Enum::KeyCode::B;
				}
				case Qt::Key_C: {
					return OB::Enum::KeyCode::C;
				}
				case Qt::Key_D: {
					return OB::Enum::KeyCode::D;
				}
				case Qt::Key_E: {
					return OB::Enum::KeyCode::E;
				}
				case Qt::Key_F: {
					return OB::Enum::KeyCode::F;
				}
				case Qt::Key_G: {
					return OB::Enum::KeyCode::G;
				}
				case Qt::Key_H: {
					return OB::Enum::KeyCode::H;
				}
				case Qt::Key_I: {
					return OB::Enum::KeyCode::I;
				}
				case Qt::Key_J: {
					return OB::Enum::KeyCode::J;
				}
				case Qt::Key_K: {
					return OB::Enum::KeyCode::K;
				}
				case Qt::Key_L: {
					return OB::Enum::KeyCode::L;
				}
				case Qt::Key_M: {
					return OB::Enum::KeyCode::M;
				}
				case Qt::Key_N: {
					return OB::Enum::KeyCode::N;
				}
				case Qt::Key_O: {
					return OB::Enum::KeyCode::O;
				}
				case Qt::Key_P: {
					return OB::Enum::KeyCode::P;
				}
				case Qt::Key_Q: {
					return OB::Enum::KeyCode::Q;
				}
				case Qt::Key_R: {
					return OB::Enum::KeyCode::R;
				}
				case Qt::Key_S: {
					return OB::Enum::KeyCode::S;
				}
				case Qt::Key_T: {
					return OB::Enum::KeyCode::T;
				}
				case Qt::Key_U: {
					return OB::Enum::KeyCode::U;
				}
				case Qt::Key_V: {
					return OB::Enum::KeyCode::V;
				}
				case Qt::Key_W: {
					return OB::Enum::KeyCode::W;
				}
				case Qt::Key_X: {
					return OB::Enum::KeyCode::X;
				}
				case Qt::Key_Y: {
					return OB::Enum::KeyCode::Y;
				}
				case Qt::Key_Z: {
					return OB::Enum::KeyCode::Z;
				}
				case Qt::Key_BracketLeft: {
					return OB::Enum::KeyCode::LeftBracket;
				}
				case Qt::Key_BracketRight: {
					return OB::Enum::KeyCode::RightBracket;
				}
				case Qt::Key_Backslash: {
					return OB::Enum::KeyCode::Backslash;
				}
				case Qt::Key_Underscore: {
					return OB::Enum::KeyCode::Underscore;
				}
				case Qt::Key_QuoteLeft: {
					return OB::Enum::KeyCode::Backquote;
				}
				case Qt::Key_MediaTogglePlayPause: {
					return OB::Enum::KeyCode::MediaPlayPause;
				}
				case Qt::Key_MediaPrevious: {
					return OB::Enum::KeyCode::MediaPrevious;
				}
				case Qt::Key_MediaNext: {
					return OB::Enum::KeyCode::MediaNext;
				}
				case Qt::Key_MediaStop: {
					return OB::Enum::KeyCode::MediaStop;
				}
				case Qt::Key_Undo: {
					return OB::Enum::KeyCode::Undo;
				}
				case Qt::Key_Redo: {
					return OB::Enum::KeyCode::Redo;
				}
				case Qt::Key_WWW: {
					return OB::Enum::KeyCode::WWW;
				}
			}

			return OB::Enum::KeyCode::Unknown;
		}

		void StudioGLWidget::keyPressEvent(QKeyEvent* event){
			if(event->isAutoRepeat()){
				return;
			}

			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					OB::Enum::KeyCode obKey = ob_studio_qt_key_to_ob(event);

					ier->input_keyEvent(obKey, true);
				}
			}
		}

		void StudioGLWidget::keyReleaseEvent(QKeyEvent* event){
			if(event->isAutoRepeat()){
				return;
			}

			if(eng){
				OBInputEventReceiver* ier = eng->getInputEventReceiver();
				if(ier){
					OB::Enum::KeyCode obKey = ob_studio_qt_key_to_ob(event);

					ier->input_keyEvent(obKey, false);
				}
			}
		}

		// Explorer/log handling
		void StudioGLWidget::sendOutput(QString msg){
			QString emsg = msg.toHtmlEscaped().replace('\n', "<br/>") + "<br/>";
			logHist = logHist + emsg;

			if(has_focus){
				StudioWindow* win = StudioWindow::static_win;

				if(win->output){
					win->output->append(emsg);
				}
			}
		}

		void StudioGLWidget::sendOutput(QString msg, QColor col){
			QString emsg = "<font color=\"" + col.name() + "\">" + msg.toHtmlEscaped().replace('\n', "<br/>") + "</font><br/>";
			logHist = logHist + emsg;

			if(has_focus){
				StudioWindow* win = StudioWindow::static_win;

				if(win->output){
					win->output->append(emsg);
				}
			}
		}

		void StudioGLWidget::handle_log_event(std::vector<shared_ptr<OB::Type::VarWrapper>> evec){
			// Temporary
			QColor errorCol(255, 51, 0);
			QColor warnCol(242, 97, 0);

			StudioWindow* win = StudioWindow::static_win;
			if(evec.size() == 2){
				QString msg = evec.at(0)->asString().c_str();
				shared_ptr<Type::LuaEnumItem> msgType = dynamic_pointer_cast<Type::LuaEnumItem>(evec.at(1)->asType());

				if(msgType->getValue() == (int)Enum::MessageType::MessageError){
					sendOutput(msg, errorCol);
				}else if(msgType->getValue() == (int)Enum::MessageType::MessageWarning){
					sendOutput(msg, warnCol);
				}else{
					sendOutput(msg);
				}
			}
		}

		void StudioGLWidget::instance_changed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, InstanceTreeItem* kidItem){
			if(!kidItem){
				return;
			}

			shared_ptr<Instance::Instance> kid = kidItem->GetInstance();
			if(!kid){
				return;
			}

			std::string prop = evec.at(0)->asString();

			if(StudioWindow::static_win){
				std::vector<shared_ptr<Instance::Instance>> selection = selectedInstances;

				if(!selection.empty()){
					if(std::find(selection.begin(), selection.end(), kid) != selection.end()){
						StudioWindow::static_win->properties->updateValue(prop);
					}
				}
			}

			if(prop == "Name"){
				const QSignalBlocker sigBlock(kidItem->treeWidget());
				kidItem->setText(0, QString(kid->getName().c_str()));
				return;
			}
			if(prop == "Parent" || prop == "ParentLocked"){
				kidItem->updateFlags();
				return;
			}
		}

		void StudioGLWidget::instance_child_added_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, QTreeWidgetItem* kidItem){
			if(!kidItem){
				return;
			}

			if(evec.size() == 1){
				if(kidItem->isSelected()){
					StudioWindow* win = StudioWindow::static_win;
					if(win){
						win->update_toolbar_usability();
					}
				}
				shared_ptr<Instance::Instance> newGuy = evec[0]->asInstance();
				if(treeItemMap.contains(newGuy)){
					InstanceTreeItem* ngti = treeItemMap.value(newGuy);
					QTreeWidgetItem* twi = ngti->parent();
					if(twi != kidItem){
						if(twi){
							twi->removeChild(ngti);
						}
						kidItem->addChild(ngti);
					}
				}else{
					addChildOfInstance(kidItem, newGuy);
				}
			}
		}

		void StudioGLWidget::instance_child_removed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec, QTreeWidgetItem* kidItem){
			if(!kidItem){
				return;
			}

			if(evec.size() == 1){
				if(kidItem->isSelected()){
					StudioWindow* win = StudioWindow::static_win;
					if(win){
						win->update_toolbar_usability();
					}
				}
				shared_ptr<Instance::Instance> newGuy = evec[0]->asInstance();
				InstanceTreeItem* kTi = treeItemMap.value(newGuy);
				if(kTi){
					if(kTi->parent() == kidItem){
						kidItem->removeChild(kTi);
					}
				}
			}
		}

		void StudioGLWidget::addChildOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> kid){
			using namespace std::placeholders;

			if(!parentItem || !kid){
				return;
			}

			InstanceTreeItem* kidItem = new InstanceTreeItem(kid);
			treeItemMap[kid] = kidItem;
			kidItem->setIcon(0, StudioWindow::getClassIcon(QString(kid->getClassName().c_str())));

			kid->Changed->Connect(std::bind(&StudioGLWidget::instance_changed_evt, this, _1, kidItem));
			kid->ChildAdded->Connect(std::bind(&StudioGLWidget::instance_child_added_evt, this, _1, kidItem));
			kid->ChildRemoved->Connect(std::bind(&StudioGLWidget::instance_child_removed_evt, this, _1, kidItem));

			addChildrenOfInstance(kidItem, kid);

			parentItem->addChild(kidItem);
		}

		void StudioGLWidget::addChildrenOfInstance(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst){
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

		void StudioGLWidget::dm_changed_evt(std::vector<shared_ptr<Type::VarWrapper>> evec){
			StudioWindow* sw = StudioWindow::static_win;
			if(!sw){
				return;
			}

			shared_ptr<Instance::DataModel> dm = eng->getDataModel();
			if(dm){
				if(sw->tabWidget){
					int studioWidgetIdx = sw->tabWidget->indexOf(this);
					sw->tabWidget->setTabText(studioWidgetIdx, QString(dm->getName().c_str()));
				}
			}
		}

		void StudioGLWidget::addDM(QTreeWidgetItem* parentItem, shared_ptr<Instance::Instance> inst){
			using namespace std::placeholders;

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

			inst->ChildAdded->Connect(std::bind(&StudioGLWidget::instance_child_added_evt, this, _1, parentItem));
			inst->ChildRemoved->Connect(std::bind(&StudioGLWidget::instance_child_removed_evt, this, _1, parentItem));
			inst->Changed->Connect(std::bind(&StudioGLWidget::dm_changed_evt, this, _1));
		}
	}
}
