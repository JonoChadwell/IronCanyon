#include "Sound.h"
#include "Program.h"
#include "math.h"
#include "Constants.h"
#include "Grid.h"
#include "Player.h"
#include "Scrap.h"
#include "Texture.h"
#include <iostream>
#include <cmath>

#include "SFML\Audio.hpp"

using namespace glm;
using namespace std;

Sound::Sound() {

}

void Sound::setup(Sound* soundP) {
	//soundP->soundBuffers.push_back();

	if (!soundP->theme.openFromFile("../resources/theme.ogg")) {
		cout << "FUCK" << endl;
		return;
	}

	//soundP->laserShots.push_back = new sf::Sound();
	//buffer.loadFromFile("../resources/LaserShot.ogg");
	//soundP->laserShots.back->setBuffer(buffer);

	//sound2 = new sf::Sound();
	//buffer2.loadFromFile("../resources/ChargeLaser.ogg");
	//sound2->setBuffer(buffer2);
}