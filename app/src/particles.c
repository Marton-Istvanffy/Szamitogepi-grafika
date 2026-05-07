#include "particles.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void particles_init(ParticleSystem* ps) { memset(ps, 0, sizeof(ParticleSystem)); }

void particles_spawn_fireworks(ParticleSystem* ps) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        ps->particles[i].active = true;
        ps->particles[i].pos = (vec3){0.0f, 0.0f, 0.0f};
        
        float theta = ((float)rand() / RAND_MAX) * 2.0f * 3.14159f;
        float phi = acosf(2.0f * ((float)rand() / RAND_MAX) - 1.0f);
        float speed = 2.0f + ((float)rand() / RAND_MAX) * 12.0f; 
        
        ps->particles[i].vel.x = speed * sinf(phi) * cosf(theta);
        ps->particles[i].vel.y = speed * sinf(phi) * sinf(theta);
        ps->particles[i].vel.z = speed * cosf(phi);
        
        ps->particles[i].color[0] = ((float)rand() / RAND_MAX);
        ps->particles[i].color[1] = ((float)rand() / RAND_MAX);
        ps->particles[i].color[2] = ((float)rand() / RAND_MAX);
        int max_c = rand() % 3;
        ps->particles[i].color[max_c] = 1.0f;
        
        ps->particles[i].max_life = 1.5f + ((float)rand() / RAND_MAX) * 2.5f;
        ps->particles[i].life = ps->particles[i].max_life;
    }
}

void particles_update(ParticleSystem* ps, float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (ps->particles[i].active) {
            ps->particles[i].pos.x += ps->particles[i].vel.x * dt;
            ps->particles[i].pos.y += ps->particles[i].vel.y * dt;
            ps->particles[i].pos.z += ps->particles[i].vel.z * dt;
            ps->particles[i].vel.y -= 5.0f * dt;
            ps->particles[i].life -= dt;
            if (ps->particles[i].life <= 0) ps->particles[i].active = false;
        }
    }
}

void particles_render(const ParticleSystem* ps) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (ps->particles[i].active) {
            float alpha = ps->particles[i].life / ps->particles[i].max_life;
            glColor4f(ps->particles[i].color[0], ps->particles[i].color[1], ps->particles[i].color[2], alpha);
            glVertex3f(ps->particles[i].pos.x, ps->particles[i].pos.y, ps->particles[i].pos.z);
        }
    }
    glEnd();
    glPopAttrib();
}