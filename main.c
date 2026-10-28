#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/ctrl.h>
#include <vita2d.h>

#define SCREEN_W 960
#define SCREEN_H 544

typedef struct {
    float x, y, z;
} Vec3;

typedef struct {
    float x, y;
} Vec2;

typedef struct {
    int a, b, c, d;
} Face;

static const Vec3 cubeVerts[8] = {
    {-1.0f, -1.0f, -1.0f},
    { 1.0f, -1.0f, -1.0f},
    { 1.0f,  1.0f, -1.0f},
    {-1.0f,  1.0f, -1.0f},
    {-1.0f, -1.0f,  1.0f},
    { 1.0f, -1.0f,  1.0f},
    { 1.0f,  1.0f,  1.0f},
    {-1.0f,  1.0f,  1.0f}
};

static const Face cubeFaces[6] = {
    {0, 1, 2, 3},
    {4, 5, 6, 7},
    {0, 1, 5, 4},
    {1, 2, 6, 5},
    {2, 3, 7, 6},
    {3, 0, 4, 7}
};

static void rotatePoint(Vec3 *p, float ax, float ay) {
    float y1 = p->y * cosf(ax) - p->z * sinf(ax);
    float z1 = p->y * sinf(ax) + p->z * cosf(ax);

    float x1 = p->x * cosf(ay) + z1 * sinf(ay);
    float z2 = -p->x * sinf(ay) + z1 * cosf(ay);

    p->x = x1;
    p->y = y1;
    p->z = z2;
}

static Vec2 projectPoint(Vec3 p) {
    const float cameraZ = 5.0f;
    const float scale = 220.0f;
    float z = cameraZ + p.z;
    float factor = scale / z;

    Vec2 r;
    r.x = SCREEN_W * 0.5f + p.x * factor;
    r.y = SCREEN_H * 0.5f + p.y * factor;
    return r;
}

static void drawCube(float rx, float ry) {
    Vec3 rot[8];
    Vec2 proj[8];

    for (int i = 0; i < 8; i++) {
        rot[i] = cubeVerts[i];
        rotatePoint(&rot[i], rx, ry);
        proj[i] = projectPoint(rot[i]);
    }

    for (int i = 0; i < 6; i++) {
        int a = cubeFaces[i].a;
        int b = cubeFaces[i].b;
        int c = cubeFaces[i].c;
        int d = cubeFaces[i].d;

        vita2d_draw_line(proj[a].x, proj[a].y, proj[b].x, proj[b].y, RGBA8(255, 255, 255, 255));
        vita2d_draw_line(proj[b].x, proj[b].y, proj[c].x, proj[c].y, RGBA8(255, 255, 255, 255));
        vita2d_draw_line(proj[c].x, proj[c].y, proj[d].x, proj[d].y, RGBA8(255, 255, 255, 255));
        vita2d_draw_line(proj[d].x, proj[d].y, proj[a].x, proj[a].y, RGBA8(255, 255, 255, 255));
    }
}

int main() {
    vita2d_init();
    vita2d_set_clear_color(RGBA8(0, 0, 0, 255));

    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

    float rotX = 0.0f;
    float rotY = 0.0f;

    while (1) {
        SceCtrlData pad;
        sceCtrlPeekBufferPositive(0, &pad, 1);

        float lx = ((float)pad.lx - 128.0f) / 128.0f;
        float ly = ((float)pad.ly - 128.0f) / 128.0f;

        rotY += lx * 0.07f;
        rotX += ly * 0.07f;

        vita2d_start_drawing();
        vita2d_clear_screen();
        drawCube(rotX, rotY);
        vita2d_end_drawing();
        vita2d_swap_buffers();

        sceKernelDelayThread(16666);
    }

    vita2d_fini();
    return 0;
}
