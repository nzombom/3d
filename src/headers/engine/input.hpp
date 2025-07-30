#include <vector>
#include <map>
#include <SDL3/SDL.h>

const unsigned char INPUT_KEYBOARD = 0;
const unsigned char INPUT_MOUSE = 1;
struct InputControl {
	unsigned char type;
	union {
		SDL_Keycode k;
		unsigned char m;
	} v;
};

class InputState {
	std::map<SDL_Keycode, unsigned int> kbControlMap;
	std::map<unsigned char, unsigned int> mControlMap;
	std::vector<bool> state;

  public:
	InputState(std::vector<InputControl>);

	void update(SDL_Event);
	bool s(unsigned int);
	float getAxis(unsigned int, unsigned int);
};
