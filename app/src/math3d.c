#include "math3d.h"
#include <math.h>
#include <SDL2/SDL_opengl.h>

void math3d_lookAt(vec3 eye, vec3 center, vec3 up) {
    vec3 f = {center.x - eye.x, center.y - eye.y, center.z - eye.z};
    float flen = sqrt(f.x*f.x + f.y*f.y + f.z*f.z);
    if(flen > 0) { f.x /= flen; f.y /= flen; f.z /= flen; }

    vec3 u = up;
    float ulen = sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
    if(ulen > 0) { u.x /= ulen; u.y /= ulen; u.z /= ulen; }

    vec3 s = {f.y*u.z - f.z*u.y, f.z*u.x - f.x*u.z, f.x*u.y - f.y*u.x};
    float slen = sqrt(s.x*s.x + s.y*s.y + s.z*s.z);
    if(slen > 0) { s.x /= slen; s.y /= slen; s.z /= slen; }

    vec3 v = {s.y*f.z - s.z*f.y, s.z*f.x - s.x*f.z, s.x*f.y - s.y*f.x};

    float m[16] = {
        s.x, v.x, -f.x, 0.0f,
        s.y, v.y, -f.y, 0.0f,
        s.z, v.z, -f.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    glMultMatrixf(m); 
    glTranslatef(-eye.x, -eye.y, -eye.z);
}