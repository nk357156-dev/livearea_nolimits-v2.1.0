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

typedef struct {
    int indices[4];
    float avgZ;
    uint32_t color;
} DrawFace;

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

static const uint32_t faceColors[6] = {
    RGBA8(255, 80, 80, 255),
    RGBA8(80, 255, 80, 255),
    RGBA8(80, 120, 255, 255),
    RGBA8(255, 200, 80, 255),
    RGBA8(255, 80, 255, 255),
    RGBA8(80, 255, 255, 255)
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

static void drawFilledQuad(Vec2 p[4], uint32_t color) {
    int minX = (int)floorf(fminf(fminf(p[0].x, p[1].x), fminf(p[2].x, p[3].x)));
    int maxX = (int)ceilf(fmaxf(fmaxf(p[0].x, p[1].x), fmaxf(p[2].x, p[3].x)));
    int minY = (int)floorf(fminf(fminf(p[0].y, p[1].y), fminf(p[2].y, p[3].y)));
    int maxY = (int)ceilf(fmaxf(fmaxf(p[0].y, p[1].y), fmaxf(p[2].y, p[3].y)));

    for (int y = minY; y <= maxY; y++) {
        float xs[8];
        int count = 0;

        for (int i = 0; i < 4; i++) {
            Vec2 a = p[i];
            Vec2 b = p[(i + 1) % 4];

            if ((a.y > (float)y && b.y <= (float)y) || (b.y > (float)y && a.y <= (float)y)) {
                float t = (float)(y - a.y) / (b.y - a.y);
                xs[count++] = a.x + (b.x - a.x) * t;
            }
        }

        if (count >= 2) {
            for (int i = 0; i < count - 1; i++) {
                for (int j = i + 1; j < count; j++) {
                    if (xs[i] > xs[j]) {
                        float tmp = xs[i];
                        xs[i] = xs[j];
                        xs[j] = tmp;
                    }
                }
            }

            for (int i = 0; i + 1 < count; i += 2) {
                int x1 = (int)floorf(xs[i]);
                int x2 = (int)ceilf(xs[i + 1]);
                if (x2 > x1) {
                    vita2d_draw_line((float)x1, (float)y, (float)x2, (float)y, color);
                }
            }
        }
    }
}

static void drawCube(float rx, float ry) {
    Vec3 rot[8];
    Vec2 proj[8];
    DrawFace faces[6];

    for (int i = 0; i < 8; i++) {
        rot[i] = cubeVerts[i];
        rotatePoint(&rot[i], rx, ry);
        proj[i] = projectPoint(rot[i]);
    }

    for (int i = 0; i < 6; i++) {
        faces[i].indices[0] = cubeFaces[i].a;
        faces[i].indices[1] = cubeFaces[i].b;
        faces[i].indices[2] = cubeFaces[i].c;
        faces[i].indices[3] = cubeFaces[i].d;

        faces[i].avgZ = (rot[cubeFaces[i].a].z + rot[cubeFaces[i].b].z + rot[cubeFaces[i].c].z + rot[cubeFaces[i].d].z) / 4.0f;
        faces[i].color = faceColors[i];
    }

    for (int i = 0; i < 6; i++) {
        for (int j = i + 1; j < 6; j++) {
            if (faces[j].avgZ > faces[i].avgZ) {
                DrawFace tmp = faces[i];
                faces[i] = faces[j];
                faces[j] = tmp;
            }
        }
    }

    for (int i = 0; i < 6; i++) {
        Vec2 facePoints[4];
        for (int j = 0; j < 4; j++) {
            facePoints[j] = proj[faces[i].indices[j]];
        }

        drawFilledQuad(facePoints, faces[i].color);

        for (int j = 0; j < 4; j++) {
            Vec2 a = facePoints[j];
            Vec2 b = facePoints[(j + 1) % 4];
            vita2d_draw_line(a.x, a.y, b.x, b.y, RGBA8(20, 20, 20, 255));
        }
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
