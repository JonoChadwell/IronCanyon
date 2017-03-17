#pragma once
#include <string>
#include <vector>
#include <glm/vec3.hpp>

// forwards declarations
class Player;
class Grid;
class GameObject;
struct enemy_spawn_info;
struct wave_spawn_info;

class EnemySpawner {

public:
    const char* flavorText;
    float flavorTextDisplayTime;
    float spawnAmount;
    int waveNumber;
    float nextWaveTimer;
    bool active;
    EnemySpawner(Grid* grid, Player* player);
    virtual ~EnemySpawner();

    // functions
    std::vector<GameObject*> update(float dt);

private:
    Grid* grid;
    Player* player;
    std::vector<enemy_spawn_info> wave;

    glm::vec3 getSpawnLocation();

};
