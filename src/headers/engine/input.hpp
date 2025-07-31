#include <vector>
#include <bitset>
#include <SDL3/SDL.h>

enum class INPUT_TYPE { keyboard, mouse, gamepad };
struct InputControl {
	INPUT_TYPE type;
	union {
		SDL_Scancode k;
		unsigned char m;
	} c;
};
struct InputAxis {
	InputControl l, r;
};

class InputState {
	std::bitset<SDL_SCANCODE_COUNT> kbState;
	std::bitset<16> mbState;
	std::vector<InputControl> controlMapping;
	std::vector<InputAxis> axisMapping;

  public:
	InputState() = default;
	InputState(std::vector<InputControl>, std::vector<InputAxis>);

	void update(SDL_Event);

	bool getControl(InputControl);
	float getAxis(InputAxis);

	bool getControl(unsigned int);
	float getAxis(unsigned int);
};
