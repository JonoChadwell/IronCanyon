#include "EnemySpawner.h"

#include "Grid.h"
#include "Enemy.h"
#include "Walker.h"
#include "GameObject.h"
#include "Player.h"
#include <vector>
#include <glm/vec3.hpp>
#include <algorithm>
#include <string>
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
    const char* flavor;
    float waveDuration;
    vector<enemy_spawn_info> spawns;
};

vector<wave_spawn_info> waves = 
{
    // wave 1
    {
        "Incoming!",
        15.0f,
        {
            {FLOATER, 6.0f, 20.0f, 5},
        }
    },
    //wave 2
    {
        "Scanners show bigger enemies on the horizon.",
        30.0f,
        {
            { FLOATER, 7.0f, 30.0f, 3 },
            { FLOATER, 7.0f, 28.0f, 3 },
            { FLOATER, 7.0f, 26.0f, 3 },
            { FLOATER, 7.0f, 24.0f, 3 },
        }
    },
    //wave 3
    {
        "Watch out these enemies have phasors.",
        30.0f,
        {
            { WALKER, 5.0f, 30.0f, 15 },
        }
    },
    //wave 4
    {
        "Big wave coming after this, better heal up.",
        50.0f,
        {
            { WALKER, 5.0f, 50.0f, 5 },
            { FLOATER, 9.0f, 50.0f, 10 },
        }
    },
    //wave 5
    {
        "Here they come!",
        60.0f,
        {
            { FLOATER, 8.0f, 60.0f, 15 },
            { FLOATER, 8.5f, 59.0f, 15 },
            { FLOATER, 9.0f, 58.0f, 15 },
            { FLOATER, 9.5f, 57.0f, 15 },
        }
    },
    //wave 6
    {
        "Are the enemies getting faster?",
        40.0f,
        {
            { WALKER, 7.0f, 40.0f, 8 },
            { FLOATER, 12.0f, 40.0f, 14 },
        }
    },
    //wave 7
    {
        "Yes. They definitly are getting faster.",
        40.0f,
        {
            { WALKER, 10.0f, 40.0f, 10 },
            { FLOATER, 14.0f, 40.0f, 14 },
        }
    },
    //wave 8
    {
        "SLOW DOWN DAMNIT!",
        40.0f,
        {
            { WALKER, 13.0f, 40.0f, 10 },
            { FLOATER, 20.0f, 40.0f, 15 },
        }
    },
    //wave 9
    {
        "Another big wave after this.",
        60.0f,
        {
            { WALKER, 8.0f, 60.0f, 5 },
            { FLOATER, 15.0f, 60.0f, 10 },
        }
    },
    //wave 10
    {
        "I hope you have some turrets by now.",
        120.0f,
        {
            { WALKER, 6.0f, 120.0f, 10 },
            { WALKER, 7.0f, 110.0f, 10 },
            { WALKER, 8.0f, 100.0f, 10 },
            { WALKER, 9.0f, 90.0f, 10 },
            { WALKER, 10.0f, 80.0f, 10 },
            { WALKER, 11.0f, 70.0f, 10 },
            { WALKER, 12.0f, 60.0f, 10 },
            { WALKER, 13.0f, 50.0f, 10 },
            { WALKER, 14.0f, 40.0f, 10 },
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
            "(im all out of flavor text)",
            30.0f,
            {
                {FLOATER, 15.0f, 30.0f, 10 + wave},
                {WALKER, 8.0f, 30.0f, 5 + wave},
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
    wave({}),
    flavorText("Welcome to Iron Canyon!"),
    flavorTextDisplayTime(5.0),
    spawnAmount(1.0)
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
        flavorTextDisplayTime = 5.0;
        wave_spawn_info wi = generateWave(waveNumber);
        nextWaveTimer = wi.waveDuration;
        wave = wi.spawns;
        flavorText = wi.flavor;
        waveNumber++;
    }

    vector<GameObject*> newEnemies = {};

    for (int i = 0; i < wave.size(); i++)
    {
        enemy_spawn_info ei = wave[i];
        if (ei.spawnTime >= nextWaveTimer)
        {
            for (int j = 0; j < (int) (ei.amount * spawnAmount) + 1; j++)
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
