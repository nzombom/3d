#include <stdexcept>
#include <vector>
#include <map>
#include <SDL3/SDL.h>

#include "engine/input.hpp"

InputState::InputState(std::vector<InputControl> controls) {
	state.insert(state.begin(), controls.size(), false);
	for (unsigned int i = 0; i < controls.size(); i++) {
		InputControl control = controls.at(i);
		if (control.type == INPUT_KEYBOARD)
			kbControlMap[control.v.k] = i;
		else if (control.type == INPUT_MOUSE)
			mControlMap[control.v.m] = i;
	}
}

void InputState::update(SDL_Event e) {
	try {
		switch (e.type) {
			case SDL_EVENT_KEY_DOWN:
				state.at(kbControlMap.at(e.key.key)) = true;
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				state.at(mControlMap.at(e.button.button)) = true;
				break;
			case SDL_EVENT_KEY_UP:
				state.at(kbControlMap.at(e.key.key)) = false;
				break;
			case SDL_EVENT_MOUSE_BUTTON_UP:
				state.at(mControlMap.at(e.button.button)) = false;
				break;
		}
	} catch (std::out_of_range& e) {};
}

bool InputState::s(unsigned int v) {
	return state.at(v);
}
float InputState::getAxis(unsigned int l, unsigned int r) {
	return (s(l) ? -1 : 0) + (s(r) ? 1 : 0);
}
