//=================================================================
// Keybindings for player controls
//=================================================================

#ifndef _KEY_BINDING_H_
#define _KEY_BINDING_H_
#include "Keyboard.h"

struct KeyBinding
{
public:
	static KeyBinding& instance()
	{
		static KeyBinding* instance = new KeyBinding();
		return *instance;
	}

	//Key Bindings
	static const Keyboard::Keys Forward = Keyboard::Keys::W;
	static const Keyboard::Keys Backward = Keyboard::Keys::S;
	static const Keyboard::Keys Up = Keyboard::Keys::Space;
	static const Keyboard::Keys Down = Keyboard::Keys::LeftShift;
	static const Keyboard::Keys Right = Keyboard::Keys::D;
	static const Keyboard::Keys Left = Keyboard::Keys::A;
	static const Keyboard::Keys RotLeft = Keyboard::Keys::Q;
	static const Keyboard::Keys RotRight = Keyboard::Keys::E;

private:
	KeyBinding() {};
};

#endif;
