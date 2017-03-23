#pragma once
#include "GameObject.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "SFML\Audio.hpp"
#include <string>
#include <vector>
#include <memory>

class Sound {
public:
	std::vector<sf::SoundBuffer*> soundBuffers;
	std::vector<sf::Sound*> laserShots;
	
	sf::Sound* goodBeep;
	sf::Sound* laserBeam;

	sf::Music theme;

	Sound();
	static void setup(Sound* SoundP);
};
