#ifndef PARTICLES_H
#define PARTICLES_H
#include "math3d.h"
#include <stdbool.h>

#define MAX_PARTICLES 1000 

typedef struct {
    vec3 pos;
    vec3 vel;
    float color[3];
    float life;
    float max_life;
    bool active;
} Particle;

typedef struct {
    Particle particles[MAX_PARTICLES];
} ParticleSystem;

void particles_init(ParticleSystem* ps);
void particles_spawn_fireworks(ParticleSystem* ps); // ÚJ
void particles_update(ParticleSystem* ps, float dt);
void particles_render(const ParticleSystem* ps);
#endif