#pragma once
#include <vector>

// forwards declarations
class Player;
class Grid;
class GameObject;
struct enemy_spawn_info;
struct wave_spawn_info;

class EnemySpawner {

public:
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
    vector<enemy_spawn_info> wave;

    glm::vec3 getSpawnLocation();

};
