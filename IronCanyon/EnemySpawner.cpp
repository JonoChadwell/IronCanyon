#include "EnemySpawner.h"

#include "Grid.h"
#include "Enemy.h"
#include "Walker.h"
#include "GameObject.h"
#include "Player.h"
#include <vector>
#include <glm/vec3.hpp>
#include <algorithm>
#include <cmath>

#define SPAWN_AREA 500.0f
#define CENTER_NO_SPAWN_RADIUS 100.0f
#define PLAYER_NO_SPAWN_RADIUS 50.0f

#define ENEMY_T int
#define FLOATER 1
#define WALKER 2

using namespace std;
using namespace glm;

struct enemy_spawn_info {
    ENEMY_T type;
    float speed;
    float spawnTime;
    int amount;
};

struct wave_spawn_info {
    float waveDuration;
    vector<enemy_spawn_info> spawns;
};

vector<wave_spawn_info> waves = 
{
    // wave 1
    {
        15.0f,
        {
            {FLOATER, 6.0f, 20.0f, 5},
        }
    },
    //wave 2
    {
        20.0f,
        {
            {FLOATER, 8.0f, 20.0f, 5},
            {FLOATER, 8.0f, 18.0f, 5},
            {FLOATER, 8.0f, 16.0f, 5},
            {FLOATER, 8.0f, 14.0f, 5},
        }
    },
};

namespace {
    float randf()
    {
        return (rand() * 1.0f) / (RAND_MAX);
    }

    wave_spawn_info generateWave(int wave)
    {
        if (wave < waves.size()) {
            return waves[wave];
        }
        wave_spawn_info rtn = {
            30.0f,
            {
                {FLOATER, 10.0f, 30.0f, 10 + wave},
                {WALKER, 5.0f, 30.0f, 5 + wave},
            }
        };
        return rtn;
    }
}

EnemySpawner::EnemySpawner(Grid* grid, Player* player) :
    grid(grid),
    player(player),
    nextWaveTimer(1.0f),
    waveNumber(0),
    active(false),
    wave({})
{
}

EnemySpawner::~EnemySpawner()
{
}

vec3 EnemySpawner::getSpawnLocation()
{
    float x = randf() * SPAWN_AREA - SPAWN_AREA / 2.0f;
    float z = randf() * SPAWN_AREA - SPAWN_AREA / 2.0f;
    while (!grid->inBounds(x, z)
            || length(vec2(x + z)) < CENTER_NO_SPAWN_RADIUS
            || distance(vec2(x,z), vec2(player->pos.x, player->pos.z)) < PLAYER_NO_SPAWN_RADIUS) {
        x = randf() * SPAWN_AREA - SPAWN_AREA / 2.0f;
        z = randf() * SPAWN_AREA - SPAWN_AREA / 2.0f;
    }
    return vec3(x, 0, z);
}

// returns all entities spawned during a game tick
vector<GameObject*> EnemySpawner::update(float dt)
{
    if (!active) return {};
    
    nextWaveTimer -= dt;
    if (nextWaveTimer <= 0.0f)
    {
        wave_spawn_info wi = generateWave(waveNumber);
        nextWaveTimer = wi.waveDuration;
        wave = wi.spawns;
        waveNumber++;
    }

    vector<GameObject*> newEnemies = {};

    for (int i = 0; i < wave.size(); i++)
    {
        enemy_spawn_info ei = wave[i];
        if (ei.spawnTime >= nextWaveTimer)
        {
            for (int j = 0; j < ei.amount; j++)
            {
                if (ei.type == FLOATER)
                {
                    newEnemies.push_back(
                            new Enemy(getSpawnLocation(),
                            0, 0, 0,
                            ei.speed,
                            2,
                            grid));
                }
                else
                {
                    newEnemies.push_back(
                            new Walker(getSpawnLocation(),
                            0, 0, 0,
                            ei.speed,
                            2,
                            grid));
                }
            }
            wave.erase(wave.begin() + i);
            i--;
        }
    }

    return newEnemies;
}
