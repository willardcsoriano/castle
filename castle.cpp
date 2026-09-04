#include <GL/freeglut.h>
#include <cstring>
#include <vector>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#include "third_party/stb_image_write.h"
#pragma GCC diagnostic pop

const int WIDTH = 512;
const int HEIGHT = 412;

void setColor(float r, float g, float b) {
    glColor3f(r, g, b);
}

void drawLine(float x1, float y1, float x2, float y2, float width = 2.0f) {
    glLineWidth(width);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void drawLoop3(float x1, float y1, float x2, float y2, float x3, float y3, float width = 2.0f) {
    glLineWidth(width);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void drawLoop4(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float width = 2.0f) {
    glLineWidth(width);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glVertex2f(x4, y4);
    glEnd();
}

float mx(float x, bool mirror) {
    return mirror ? (WIDTH - x) : x;
}

void drawTower(float cx, bool mirror, float &pillarLeft, float &pillarRight) {
    float poleX = mx(cx, mirror);

    setColor(0.0f, 0.0f, 0.0f);
    drawLine(poleX, 8, poleX, 55);

    float flagTip = mx(cx + 48, mirror);
    drawLoop3(poleX, 8, flagTip, 23, poleX, 38);

    float roofLeft = mx(cx - 35, mirror);
    float roofRight = mx(cx + 35, mirror);
    setColor(0.9f, 0.1f, 0.1f);
    drawLoop3(poleX, 55, roofLeft, 122, roofRight, 122);

    float boxLeft = mx(cx - 35, mirror);
    float boxRight = mx(cx + 35, mirror);
    setColor(0.0f, 0.0f, 0.0f);
    drawLoop4(boxLeft, 122, boxRight, 122, boxRight, 215, boxLeft, 215);

    float winLeft = mx(cx - 12, mirror);
    float winRight = mx(cx + 12, mirror);
    drawLoop4(winLeft, 150, winRight, 150, winRight, 175, winLeft, 175);

    float pLeft = mx(cx - 45, mirror);
    float pRight = mx(cx + 45, mirror);
    drawLine(pLeft, 215, pLeft, 400);
    drawLine(pRight, 215, pRight, 400);

    pillarLeft = pLeft;
    pillarRight = pRight;
}

void drawCastle() {
    float leftInner, leftOuter, rightInner, rightOuter;
    drawTower(135, false, leftInner, leftOuter);
    drawTower(135, true, rightInner, rightOuter);

    setColor(0.0f, 0.0f, 0.0f);
    drawLine(leftOuter, 215, leftOuter, 235);
    drawLine(rightInner, 215, rightInner, 235);
    drawLine(leftOuter, 235, rightInner, 235);

    float doorLeft = 225, doorRight = 290;
    float doorMid = (doorLeft + doorRight) / 2.0f;
    drawLoop4(doorLeft, 235, doorRight, 235, doorRight, 400, doorLeft, 400);
    drawLine(doorMid, 235, doorMid, 400);

    setColor(0.9f, 0.1f, 0.1f);
    drawLine(20, 235, leftInner, 235);
    drawLine(rightOuter, 235, 495, 235);

    setColor(0.0f, 0.0f, 0.0f);
    drawLine(20, 235, 20, 400);
    drawLine(495, 235, 495, 400);
    drawLine(20, 400, 495, 400);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawCastle();
    glutSwapBuffers();
}

void exportPng(const char *path) {
    glClear(GL_COLOR_BUFFER_BIT);
    drawCastle();
    glFinish();

    std::vector<unsigned char> pixels(WIDTH * HEIGHT * 3);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, WIDTH, HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // OpenGL's origin is bottom-left; PNG rows go top-down, so flip vertically.
    std::vector<unsigned char> flipped(WIDTH * HEIGHT * 3);
    for (int y = 0; y < HEIGHT; ++y) {
        std::memcpy(&flipped[y * WIDTH * 3],
                    &pixels[(HEIGHT - 1 - y) * WIDTH * 3],
                    WIDTH * 3);
    }

    stbi_write_png(path, WIDTH, HEIGHT, 3, flipped.data(), WIDTH * 3);
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Castle");
    glClearColor(0.96f, 0.94f, 0.90f, 1.0f);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    reshape(WIDTH, HEIGHT);

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--export") == 0 && i + 1 < argc) {
            exportPng(argv[i + 1]);
            return 0;
        }
    }

    glutMainLoop();
    return 0;
}
