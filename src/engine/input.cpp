#include <vector>
#include <map>
#include <SDL3/SDL.h>

#include "engine/input.hpp"

InputState::InputState(std::vector<InputControl> _controlMapping,
	std::vector<InputAxis> _axisMapping)
	: controlMapping(_controlMapping), axisMapping(_axisMapping) {}

void InputState::update(SDL_Event e) {
	switch (e.type) {
		case SDL_EVENT_KEY_DOWN:
			kbState.set(e.key.scancode);
			break;
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			mbState.set(e.button.button);
			break;
		case SDL_EVENT_KEY_UP:
			kbState.reset(e.key.scancode);
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			mbState.reset(e.button.button);
			break;
	}
}

bool InputState::getControl(InputControl control) {
	switch (control.type) {
		case INPUT_TYPE::keyboard:
			return kbState.test(control.c.k);
			break;
		case INPUT_TYPE::mouse:
			return mbState.test(control.c.m);
			break;
		default:
			return false;
	}
}
float InputState::getAxis(InputAxis axis) {
	return (getControl(axis.l) ? -1 : 0) + (getControl(axis.r) ? 1 : 0);
}

bool InputState::getControl(unsigned int controlIdx) {
	return getControl(controlMapping.at(controlIdx));
}
float InputState::getAxis(unsigned int axisIdx) {
	return getAxis(axisMapping.at(axisIdx));
}
