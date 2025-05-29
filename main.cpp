#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

// Game Constants
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const float GRAVITY = 0.35f;
const float JUMP_FORCE = 13.5f;
const float MOVE_SPEED = 5.0f;
const float JUMP_BOOST = 1.3f;
const int PLANETS_PER_LEVEL = 20;
const int MAX_LEVELS = 5;
const int POINTS_PER_PLANET = 10;
const int LEVEL_COMPLETION_BONUS = 500;
const float BASE_SCROLL_SPEED = 1.2f;    // Faster base speed!
const float SPEED_MULTIPLIER = 0.6f;     // More dramatic speed increases!
const int PLANETS_FOR_BONUS = 10;
const int EXPLORATION_BONUS = 50;

// 3D and Space Constants
const int NUM_STARS = 200;
const int NUM_SHOOTING_STARS = 8;
const int NUM_ROSE_PETALS = 15;
const int NUM_STARDUST = 30;
const float CAMERA_DISTANCE = 250.0f;
const float CAMERA_HEIGHT_OFFSET = 150.0f;
const float PLATFORM_Z_RANGE = 30.0f;

// Star Structure for background
struct Star {
    float x, y, z;
    float brightness;
    Star(float _x, float _y, float _z, float _brightness) : x(_x), y(_y), z(_z), brightness(_brightness) {}
};

// Shooting Star Structure for magical effects
struct ShootingStar {
    float x, y, z;
    float vx, vy, vz;
    float life;
    float maxLife;
    ShootingStar(float _x, float _y, float _z) : x(_x), y(_y), z(_z), life(100), maxLife(100) {
        vx = (rand() % 100 - 50) / 10.0f;
        vy = -(rand() % 30 + 20) / 5.0f;
        vz = (rand() % 40 - 20) / 10.0f;
    }
};

// Rose Petal Structure for romantic atmosphere
struct RosePetal {
    float x, y, z;
    float vx, vy, vz;
    float rotation;
    float rotSpeed;
    float scale;
    RosePetal(float _x, float _y, float _z) : x(_x), y(_y), z(_z), rotation(0) {
        vx = (rand() % 20 - 10) / 20.0f;
        vy = -(rand() % 10 + 5) / 20.0f;
        vz = (rand() % 20 - 10) / 20.0f;
        rotSpeed = (rand() % 100 + 50) / 100.0f;
        scale = 0.5f + (rand() % 50) / 100.0f;
    }
};

// Stardust Structure for magical particles
struct Stardust {
    float x, y, z;
    float vx, vy, vz;
    float brightness;
    float pulse;
    Stardust(float _x, float _y, float _z) : x(_x), y(_y), z(_z), pulse(0) {
        vx = (rand() % 30 - 15) / 30.0f;
        vy = -(rand() % 20 + 10) / 30.0f;
        vz = (rand() % 30 - 15) / 30.0f;
        brightness = 0.3f + (rand() % 70) / 100.0f;
    }
};

// Planet Structure (Little Prince's planets)
struct Planet {
    float x, y, z;
    float width, depth;
    float rotation;
    int planetType;  // 0=normal, 1=rose planet, 2=fox planet, 3=king planet, etc.

    Planet(float _x, float _y, float _z, float _width, float _depth, int _type = 0)
        : x(_x), y(_y), z(_z), width(_width), depth(_depth), rotation(0), planetType(_type) {}
};

// Rose Structure for decoration
struct Rose {
    float x, y, z;
    float scale;
    float rotation;
    Rose(float _x, float _y, float _z) : x(_x), y(_y), z(_z), scale(1.0f), rotation(0) {}
};

// Fox Structure for decoration
struct Fox {
    float x, y, z;
    float rotation;
    Fox(float _x, float _y, float _z) : x(_x), y(_y), z(_z), rotation(0) {}
};

// Player Structure (The Little Prince)
struct Player {
    float x, y, z;
    float vx, vy;
    bool onGround;
    int jumpCount;
    int lastPlanetIndex;
    int planetsExplored;
    int combo;
    int comboTimer;
    float rotation;
    float bobOffset;
    float scarfWave;
    float timeInSpace;
    bool driftingIntoSpace;

    Player() : x(0), y(0), z(0), vx(0), vy(0), onGround(true), jumpCount(0),
               lastPlanetIndex(0), planetsExplored(0), combo(0), comboTimer(0),
               rotation(0), bobOffset(0), scarfWave(0), timeInSpace(0), driftingIntoSpace(false) {}
};

// Game Variables
bool gameRunning = false;
Player player;
std::vector<Planet> planets;
std::vector<Star> stars;
std::vector<Rose> roses;
std::vector<Fox> foxes;
std::vector<ShootingStar> shootingStars;
std::vector<RosePetal> rosePetals;
std::vector<Stardust> stardust;
float cameraY = 0;
int score = 0;
int highScore = 0;
int currentLevel = 1;
bool leftKey = false;
bool rightKey = false;
bool spaceKey = false;
bool spaceKeyWasPressed = false;
float gameTime = 0;
float currentScrollSpeed = BASE_SCROLL_SPEED;
int planetsVisited = 0;
int totalPlanetsExplored = 0;
float explorationBoostTimer = 0;

// Function Prototypes
void init();
void display();
void update(int);
void keyPressed(unsigned char, int, int);
void keyReleased(unsigned char, int, int);
void specialKeyPressed(int, int, int);
void specialKeyReleased(int, int, int);
void reshape(int, int);
void drawText(float, float, const char*);
void createPlanets();
void createStars();
void createRoses();
void createFoxes();
void createShootingStars();
void createRosePetals();
void createStardust();
void resetGame();
void advanceToNextLevel();
void addPoints(int points, float x, float y);
void updateCombo();
bool isPlanetVisible(int planetIndex);
void drawLittlePrince();
void drawPlanet(const Planet& planet);
void drawBackground();
void setupLighting();
void drawStars();
void drawRoses();
void drawFoxes();
void drawShootingStars();
void drawRosePetals();
void drawStardust();
void updateAtmosphericEffects();
float getCurrentScrollSpeed();
void checkExplorationBonus();
void updateSpeedEffects();

// Initialize lighting
void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat ambient[] = {0.2f, 0.2f, 0.3f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

    GLfloat diffuse[] = {0.8f, 0.8f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    GLfloat position[] = {0.0f, 1000.0f, 200.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

// Create stars for background
void createStars() {
    stars.clear();
    for (int i = 0; i < NUM_STARS; i++) {
        float x = (rand() % 2000) - 1000;
        float y = (rand() % 4000) - 500;
        float z = (rand() % 1000) - 500;
        float brightness = 0.3f + (rand() % 100) / 100.0f * 0.7f;
        stars.push_back(Star(x, y, z, brightness));
    }
}

// Create roses for Little Prince decoration
void createRoses() {
    roses.clear();
    for (int i = 0; i < 15; i++) {
        float x = (rand() % 800) - 400;
        float y = 200 + (rand() % 2000);
        float z = (rand() % 200) - 100;
        roses.push_back(Rose(x, y, z));
    }
}

// Create foxes for Little Prince decoration
void createFoxes() {
    foxes.clear();
    for (int i = 0; i < 8; i++) {
        float x = (rand() % 600) - 300;
        float y = 150 + (rand() % 1500);
        float z = (rand() % 150) - 75;
        foxes.push_back(Fox(x, y, z));
    }
}

// Create magical shooting stars
void createShootingStars() {
    shootingStars.clear();
    for (int i = 0; i < NUM_SHOOTING_STARS; i++) {
        float x = (rand() % 2000) - 1000;
        float y = (rand() % 3000) + 500;
        float z = (rand() % 800) - 400;
        shootingStars.push_back(ShootingStar(x, y, z));
    }
}

// Create floating rose petals
void createRosePetals() {
    rosePetals.clear();
    for (int i = 0; i < NUM_ROSE_PETALS; i++) {
        float x = (rand() % 1000) - 500;
        float y = (rand() % 2000) + 200;
        float z = (rand() % 600) - 300;
        rosePetals.push_back(RosePetal(x, y, z));
    }
}

// Create magical stardust particles
void createStardust() {
    stardust.clear();
    for (int i = 0; i < NUM_STARDUST; i++) {
        float x = (rand() % 1500) - 750;
        float y = (rand() % 2500) + 300;
        float z = (rand() % 700) - 350;
        stardust.push_back(Stardust(x, y, z));
    }
}

// Draw stars with authentic Little Prince night sky feel
void drawStars() {
    glDisable(GL_LIGHTING);

    float speedMultiplier = currentScrollSpeed / BASE_SCROLL_SPEED;
    glPointSize(1.5f + speedMultiplier * 0.3f);

    glBegin(GL_POINTS);

    for (size_t i = 0; i < stars.size(); i++) {
        float twinkleSpeed = 0.05f + speedMultiplier * 0.1f;
        float twinkle = 0.7f + 0.3f * sin(gameTime * twinkleSpeed + stars[i].x * 0.005f);

        float warmth = 0.1f + (i % 10) * 0.05f;
        float red = (stars[i].brightness + warmth) * twinkle;
        float green = (stars[i].brightness + warmth * 0.8f) * twinkle;
        float blue = (stars[i].brightness + warmth * 0.6f) * twinkle;

        if (red > 1.0f) red = 1.0f;
        if (green > 1.0f) green = 1.0f;
        if (blue > 1.0f) blue = 1.0f;

        glColor3f(red, green, blue);
        glVertex3f(stars[i].x, stars[i].y, stars[i].z);
    }

    glEnd();

    // Special bright stars
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for (size_t i = 0; i < stars.size(); i += 25) {
        float specialTwinkle = 0.8f + 0.2f * sin(gameTime * 0.3f + i);
        glColor3f(1.0f * specialTwinkle, 0.95f * specialTwinkle, 0.8f * specialTwinkle);
        glVertex3f(stars[i].x, stars[i].y, stars[i].z);
    }
    glEnd();

    glEnable(GL_LIGHTING);
}

// Draw magical shooting stars
void drawShootingStars() {
    glDisable(GL_LIGHTING);

    for (size_t i = 0; i < shootingStars.size(); i++) {
        if (shootingStars[i].life > 0) {
            float alpha = shootingStars[i].life / shootingStars[i].maxLife;

            glColor4f(1.0f, 0.9f, 0.7f, alpha);
            glPointSize(4.0f);
            glBegin(GL_POINTS);
            glVertex3f(shootingStars[i].x, shootingStars[i].y, shootingStars[i].z);
            glEnd();

            glLineWidth(2.0f);
            glBegin(GL_LINES);
            glColor4f(1.0f, 0.8f, 0.5f, alpha * 0.7f);
            glVertex3f(shootingStars[i].x, shootingStars[i].y, shootingStars[i].z);
            glColor4f(1.0f, 0.6f, 0.3f, alpha * 0.3f);
            glVertex3f(shootingStars[i].x - shootingStars[i].vx * 15,
                      shootingStars[i].y - shootingStars[i].vy * 15,
                      shootingStars[i].z - shootingStars[i].vz * 15);
            glEnd();
        }
    }

    glEnable(GL_LIGHTING);
}

// Draw floating rose petals
void drawRosePetals() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);

    for (size_t i = 0; i < rosePetals.size(); i++) {
        if (rosePetals[i].y > cameraY - 200 && rosePetals[i].y < cameraY + 800) {
            glPushMatrix();
            glTranslatef(rosePetals[i].x, rosePetals[i].y, rosePetals[i].z);
            glRotatef(rosePetals[i].rotation, 1, 1, 0);
            glScalef(rosePetals[i].scale, rosePetals[i].scale, rosePetals[i].scale);

            glColor4f(0.9f, 0.4f, 0.5f, 0.7f);
            glBegin(GL_TRIANGLES);
            for (int j = 0; j < 8; j++) {
                float angle = j * 3.14159f / 4.0f;
                glVertex3f(0, 0, 0);
                glVertex3f(sin(angle) * 3, cos(angle) * 2, 0);
                glVertex3f(sin(angle + 0.785f) * 3, cos(angle + 0.785f) * 2, 0);
            }
            glEnd();

            glColor4f(1.0f, 0.8f, 0.8f, 0.3f);
            glutSolidSphere(2, 6, 6);

            glPopMatrix();
        }
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// Draw magical stardust particles
void drawStardust() {
    glDisable(GL_LIGHTING);

    for (size_t i = 0; i < stardust.size(); i++) {
        if (stardust[i].y > cameraY - 100 && stardust[i].y < cameraY + 600) {
            float pulse = 0.7f + 0.3f * sin(stardust[i].pulse);

            glPushMatrix();
            glTranslatef(stardust[i].x, stardust[i].y, stardust[i].z);

            glColor4f(1.0f, 0.9f, 0.6f, stardust[i].brightness * pulse);
            glutSolidSphere(0.8f, 6, 6);

            glColor4f(1.0f, 1.0f, 0.8f, stardust[i].brightness * pulse * 0.5f);
            glutSolidSphere(1.5f, 6, 6);

            for (int j = 0; j < 3; j++) {
                glPushMatrix();
                glRotatef(stardust[i].pulse * 2 + j * 120, 0, 1, 0);
                glTranslatef(3, 0, 0);
                glColor4f(1.0f, 1.0f, 0.9f, pulse * 0.6f);
                glutSolidSphere(0.3f, 4, 4);
                glPopMatrix();
            }

            glPopMatrix();
        }
    }

    glEnable(GL_LIGHTING);
}

// Draw authentic Little Prince roses
void drawRoses() {
    for (size_t i = 0; i < roses.size(); i++) {
        if (roses[i].y > cameraY - 100 && roses[i].y < cameraY + 600) {
            glPushMatrix();
            glTranslatef(roses[i].x, roses[i].y, roses[i].z);
            glRotatef(roses[i].rotation, 0, 1, 0);
            glScalef(roses[i].scale, roses[i].scale, roses[i].scale);

            // Rose stem
            glColor3f(0.15f, 0.5f, 0.15f);
            glPushMatrix();
            glScalef(0.6f, 15, 0.6f);
            glutSolidCube(1.0f);
            glPopMatrix();

            // Rose bloom
            glColor3f(0.8f, 0.15f, 0.2f);
            glPushMatrix();
            glTranslatef(0, 8, 0);
            glutSolidSphere(2.8f, 16, 16);
            glPopMatrix();

            // Rose petals
            for (int j = 0; j < 8; j++) {
                glPushMatrix();
                glTranslatef(0, 8, 0);
                glRotatef(j * 45, 0, 1, 0);
                glTranslatef(2.2f, 0, 0);
                glColor3f(0.9f, 0.2f + j * 0.05f, 0.25f + j * 0.02f);
                glutSolidSphere(1.2f, 8, 8);
                glPopMatrix();
            }

            glPopMatrix();
            roses[i].rotation += 0.3f;
        }
    }
}

// Draw Little Prince foxes
void drawFoxes() {
    for (size_t i = 0; i < foxes.size(); i++) {
        if (foxes[i].y > cameraY - 100 && foxes[i].y < cameraY + 600) {
            glPushMatrix();
            glTranslatef(foxes[i].x, foxes[i].y, foxes[i].z);
            glRotatef(foxes[i].rotation, 0, 1, 0);

            // Fox body
            glColor3f(0.8f, 0.5f, 0.2f);
            glPushMatrix();
            glScalef(8, 4, 6);
            glutSolidCube(1.0f);
            glPopMatrix();

            // Fox head
            glColor3f(0.9f, 0.6f, 0.3f);
            glPushMatrix();
            glTranslatef(0, 2, 4);
            glScalef(5, 4, 4);
            glutSolidCube(1.0f);
            glPopMatrix();

            glPopMatrix();
            foxes[i].rotation += 0.3f;
        }
    }
}

// Update all atmospheric effects
void updateAtmosphericEffects() {
    // Update shooting stars
    for (size_t i = 0; i < shootingStars.size(); i++) {
        shootingStars[i].x += shootingStars[i].vx;
        shootingStars[i].y += shootingStars[i].vy;
        shootingStars[i].z += shootingStars[i].vz;
        shootingStars[i].life -= 2.0f;

        if (shootingStars[i].life <= 0) {
            shootingStars[i].x = (rand() % 2000) - 1000;
            shootingStars[i].y = cameraY + 400 + (rand() % 200);
            shootingStars[i].z = (rand() % 800) - 400;
            shootingStars[i].life = shootingStars[i].maxLife;
            shootingStars[i].vx = (rand() % 100 - 50) / 10.0f;
            shootingStars[i].vy = -(rand() % 30 + 20) / 5.0f;
            shootingStars[i].vz = (rand() % 40 - 20) / 10.0f;
        }
    }

    // Update rose petals
    for (size_t i = 0; i < rosePetals.size(); i++) {
        rosePetals[i].x += rosePetals[i].vx;
        rosePetals[i].y += rosePetals[i].vy;
        rosePetals[i].z += rosePetals[i].vz;
        rosePetals[i].rotation += rosePetals[i].rotSpeed;

        rosePetals[i].vx += sin(gameTime * 0.5f + i) * 0.02f;
        rosePetals[i].vz += cos(gameTime * 0.3f + i) * 0.015f;

        if (rosePetals[i].y < cameraY - 300) {
            rosePetals[i].x = (rand() % 1000) - 500;
            rosePetals[i].y = cameraY + 600 + (rand() % 200);
            rosePetals[i].z = (rand() % 600) - 300;
            rosePetals[i].vx = (rand() % 20 - 10) / 20.0f;
            rosePetals[i].vy = -(rand() % 10 + 5) / 20.0f;
            rosePetals[i].vz = (rand() % 20 - 10) / 20.0f;
        }
    }

    // Update stardust
    for (size_t i = 0; i < stardust.size(); i++) {
        stardust[i].x += stardust[i].vx;
        stardust[i].y += stardust[i].vy;
        stardust[i].z += stardust[i].vz;
        stardust[i].pulse += 0.1f;

        stardust[i].vx += sin(gameTime * 0.3f + i) * 0.01f;
        stardust[i].vy += cos(gameTime * 0.2f + i) * 0.005f;

        if (stardust[i].y < cameraY - 200) {
            stardust[i].x = (rand() % 1500) - 750;
            stardust[i].y = cameraY + 500 + (rand() % 300);
            stardust[i].z = (rand() % 700) - 350;
            stardust[i].vx = (rand() % 30 - 15) / 30.0f;
            stardust[i].vy = -(rand() % 20 + 10) / 30.0f;
            stardust[i].vz = (rand() % 30 - 15) / 30.0f;
        }
    }
}

// Draw background with magical effects
void drawBackground() {
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    float speedIntensity = currentScrollSpeed / (BASE_SCROLL_SPEED + MAX_LEVELS * SPEED_MULTIPLIER);

    // Enhanced night sky with speed effects
    glBegin(GL_QUADS);
    glColor3f(0.05f + speedIntensity * 0.08f, 0.1f + speedIntensity * 0.08f, 0.25f + speedIntensity * 0.15f);
    glVertex3f(-1000, 5000, -1000);
    glVertex3f(1000, 5000, -1000);
    glColor3f(0.1f + speedIntensity * 0.08f, 0.05f + speedIntensity * 0.08f, 0.2f + speedIntensity * 0.12f);
    glVertex3f(1000, 2000, -1000);
    glVertex3f(-1000, 2000, -1000);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.1f + speedIntensity * 0.08f, 0.05f + speedIntensity * 0.08f, 0.2f + speedIntensity * 0.12f);
    glVertex3f(-1000, 2000, -1000);
    glVertex3f(1000, 2000, -1000);
    glColor3f(0.02f + speedIntensity * 0.05f, 0.02f + speedIntensity * 0.05f, 0.1f + speedIntensity * 0.08f);
    glVertex3f(1000, -1000, -1000);
    glVertex3f(-1000, -1000, -1000);
    glEnd();

    // Add nebula clouds
    glEnable(GL_BLEND);
    for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glTranslatef(-800 + i * 400, 1500 + sin(gameTime * 0.1f + i) * 200, -900);
        glColor4f(0.2f + speedIntensity * 0.1f, 0.1f + speedIntensity * 0.05f, 0.3f + speedIntensity * 0.1f, 0.15f);

        for (int j = 0; j < 8; j++) {
            glPushMatrix();
            glRotatef(j * 45 + gameTime * 2, 0, 0, 1);
            glTranslatef(50, 0, 0);
            glutSolidSphere(30 + sin(gameTime * 0.3f + i + j) * 10, 8, 8);
            glPopMatrix();
        }
        glPopMatrix();
    }
    glDisable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);

    // Draw all atmospheric effects
    drawStars();
    drawShootingStars();
    drawStardust();
    drawRosePetals();
    drawRoses();
    drawFoxes();

    glEnable(GL_LIGHTING);
}

// Create authentic Little Prince planetoids
void createPlanets() {
    planets.clear();

    planets.push_back(Planet(0, 50, 0, 120, 60, 0));

    int totalPlanets = PLANETS_PER_LEVEL * currentLevel;

    for (int i = 0; i < totalPlanets; i++) {
        float x = (rand() % 300) - 150;
        float y = 100 + i * 70;
        float z = (rand() % (int)(PLATFORM_Z_RANGE * 1.5f)) - (PLATFORM_Z_RANGE * 0.75f);

        float width = 50 + rand() % 40;
        float depth = 35 + rand() % 25;

        int planetType = 0;
        if (i % 12 == 3) planetType = 1;
        else if (i % 15 == 7) planetType = 2;
        else if (i % 18 == 11) planetType = 3;

        if (i >= PLANETS_PER_LEVEL) {
            int levelNum = (i / PLANETS_PER_LEVEL) + 1;
            width -= levelNum * 1.5f;
            depth -= levelNum * 1.0f;
            y += levelNum * 4;

            if (width < 25) width = 25;
            if (depth < 20) depth = 20;
        }

        planets.push_back(Planet(x, y, z, width, depth, planetType));
    }

    float finalY = 100 + totalPlanets * 70;
    planets.push_back(Planet(0, finalY, 0, 180, 90, 4));
}

// Draw Little Prince planetoid with glass-domed roses
void drawPlanet(const Planet& planet) {
    glPushMatrix();
    glTranslatef(planet.x, planet.y, planet.z);
    glRotatef(planet.rotation * 0.1f, 0, 1, 0);

    // Planet colors based on type
    switch(planet.planetType) {
        case 0: glColor3f(0.6f, 0.5f, 0.4f); break;
        case 1: glColor3f(0.7f, 0.5f, 0.5f); break;
        case 2: glColor3f(0.7f, 0.6f, 0.4f); break;
        case 3: glColor3f(0.6f, 0.5f, 0.7f); break;
        case 4: glColor3f(0.8f, 0.7f, 0.5f); break;
    }

    // Planetoid body
    glPushMatrix();
    glScalef(1.0f, 0.3f, 1.0f);
    glutSolidSphere(planet.width/2.5f, 16, 12);
    glPopMatrix();

    // Rose stem base
    glColor3f(0.15f, 0.4f, 0.15f);
    glPushMatrix();
    glTranslatef(0, 8, 0);
    glScalef(0.8f, 6, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Rose bloom
    glColor3f(0.85f, 0.15f, 0.2f);
    glPushMatrix();
    glTranslatef(0, 12, 0);
    glutSolidSphere(2.2f, 12, 12);
    glPopMatrix();

    // Rose petals
    for (int i = 0; i < 6; i++) {
        glPushMatrix();
        glTranslatef(0, 12, 0);
        glRotatef(i * 60 + planet.rotation, 0, 1, 0);
        glTranslatef(1.8f, 0, 0);
        glColor3f(0.9f, 0.25f + i * 0.03f, 0.3f);
        glutSolidSphere(0.8f, 8, 8);
        glPopMatrix();
    }

    // GLASS DOME (key element from the book!)
    glEnable(GL_BLEND);
    glColor4f(0.9f, 0.95f, 1.0f, 0.3f);

    glPushMatrix();
    glTranslatef(0, 10, 0);

    // Main dome hemisphere
    for (int i = 0; i < 12; i++) {
        glPushMatrix();
        glRotatef(i * 30, 0, 1, 0);
        glBegin(GL_TRIANGLES);
        for (int j = 0; j < 8; j++) {
            float angle1 = j * 3.14159f / 16.0f;
            float angle2 = (j + 1) * 3.14159f / 16.0f;
            float radius = 4.5f;

            glVertex3f(0, radius, 0);
            glVertex3f(radius * sin(angle1), radius * cos(angle1), 0);
            glVertex3f(radius * sin(angle2), radius * cos(angle2), 0);
        }
        glEnd();
        glPopMatrix();
    }

    // Glass dome base ring
    glColor4f(0.8f, 0.85f, 0.9f, 0.6f);
    glPushMatrix();
    glTranslatef(0, -2, 0);
    glutSolidTorus(0.5, 4.5, 8, 16);
    glPopMatrix();

    glPopMatrix();
    glDisable(GL_BLEND);

    // Planet-specific decorations
    if (planet.planetType == 2) {
        glColor3f(0.8f, 0.5f, 0.2f);
        glPushMatrix();
        glTranslatef(planet.width/3, 6, 0);
        glScalef(0.4f, 0.4f, 0.4f);
        glutSolidCube(4);
        glPopMatrix();
    } else if (planet.planetType == 3) {
        glColor3f(0.7f, 0.6f, 0.2f);
        glPushMatrix();
        glTranslatef(-planet.width/3, 8, 0);
        glScalef(3, 4, 2);
        glutSolidCube(1.0f);
        glPopMatrix();
    } else if (planet.planetType == 4) {
        // Multiple roses for home planet
        for (int i = 0; i < 3; i++) {
            glPushMatrix();
            glRotatef(i * 120, 0, 1, 0);
            glTranslatef(planet.width/3, 0, 0);

            glColor3f(0.8f, 0.2f, 0.25f);
            glPushMatrix();
            glTranslatef(0, 8, 0);
            glutSolidSphere(1.5f, 8, 8);
            glPopMatrix();

            glEnable(GL_BLEND);
            glColor4f(0.9f, 0.95f, 1.0f, 0.25f);
            glPushMatrix();
            glTranslatef(0, 9, 0);
            glutSolidSphere(2.5f, 10, 8);
            glPopMatrix();
            glDisable(GL_BLEND);

            glPopMatrix();
        }
    }

    glPopMatrix();
}

// Draw The Little Prince character
void drawLittlePrince() {
    if (player.onGround) {
        glPushMatrix();
        glTranslatef(player.x, planets[player.lastPlanetIndex].y + 1, player.z);
        glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
        glBegin(GL_QUADS);
        glVertex3f(-4, 0, -4);
        glVertex3f(4, 0, -4);
        glVertex3f(4, 0, 4);
        glVertex3f(-4, 0, 4);
        glEnd();
        glPopMatrix();
    }

    glPushMatrix();
    glTranslatef(player.x, player.y + player.bobOffset, player.z);
    glRotatef(player.rotation, 0, 1, 0);

    // Royal blue coat
    glColor3f(0.15f, 0.35f, 0.65f);
    glPushMatrix();
    glTranslatef(0, -5, 0);
    glScalef(5, 9, 4);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Golden buttons
    glColor3f(1.0f, 0.85f, 0.2f);
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(0, -2 + i * 2, 2.5f);
        glutSolidSphere(0.3f, 6, 6);
        glPopMatrix();
    }

    // Head
    glColor3f(0.96f, 0.87f, 0.78f);
    glPushMatrix();
    glTranslatef(0, 3, 0);
    glutSolidSphere(3.5f, 14, 14);
    glPopMatrix();

    // Curly hair
    glColor3f(1.0f, 0.92f, 0.65f);
    glPushMatrix();
    glTranslatef(0, 6, 0);
    glutSolidSphere(3.2f, 12, 10);
    glPopMatrix();

    // Hair curls
    for (int i = 0; i < 6; i++) {
        glPushMatrix();
        glTranslatef(0, 6, 0);
        glRotatef(i * 60, 0, 1, 0);
        glTranslatef(2.8f, sin(gameTime + i) * 0.3f, 0);
        glutSolidSphere(0.6f, 6, 6);
        glPopMatrix();
    }

    // Arms
    glColor3f(0.15f, 0.35f, 0.65f);
    glPushMatrix();
    glTranslatef(-3.5f, -1, 0);
    glScalef(2, 6, 2);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.5f, -1, 0);
    glScalef(2, 6, 2);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Legs
    glColor3f(0.25f, 0.25f, 0.35f);
    glPushMatrix();
    glTranslatef(-1.5f, -12, 0);
    glScalef(2, 7, 2);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.5f, -12, 0);
    glScalef(2, 7, 2);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Boots
    glColor3f(0.4f, 0.25f, 0.1f);
    glPushMatrix();
    glTranslatef(-1.5f, -16, 1);
    glScalef(2.5f, 2, 3.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.5f, -16, 1);
    glScalef(2.5f, 2, 3.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Sword
    glColor3f(0.7f, 0.7f, 0.8f);
    glPushMatrix();
    glTranslatef(-4, -3, 0);
    glRotatef(25, 0, 0, 1);
    glScalef(0.3f, 6, 0.2f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Yellow scarf
    player.scarfWave += 0.12f;
    glColor3f(1.0f, 0.88f, 0.25f);

    glPushMatrix();
    glTranslatef(1.2f, 1, 0);
    glRotatef(sin(player.scarfWave) * 12 + 8, 0, 0, 1);
    glScalef(1.2f, 7, 0.6f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Scarf tail
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glTranslatef(2.5f + i * 1.5f, -1 - i * 2, 0);
        glRotatef(sin(player.scarfWave + i * 0.5f) * 18 + 35 + i * 10, 0, 0, 1);
        glScalef(0.8f - i * 0.1f, 4 - i * 0.5f, 0.5f);
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // Effects when jumping
    if (!player.onGround) {
        glColor4f(0.12f, 0.3f, 0.6f, 0.7f);
        glPushMatrix();
        glTranslatef(0, -3, -2);
        glRotatef(10, 1, 0, 0);
        glScalef(7, 6, 0.6f);
        glutSolidCube(1.0f);
        glPopMatrix();

        // Starlight aura
        glColor3f(1.0f, 1.0f, 0.95f);
        for (int i = 0; i < 8; i++) {
            glPushMatrix();
            float angle = gameTime * 1.2f + i * 0.785f;
            float radius = 10 + sin(gameTime * 2 + i) * 2;
            glTranslatef(sin(angle) * radius, cos(angle * 1.1f) * 6, cos(angle) * 4);
            glutSolidSphere(0.5f, 6, 6);
            glPopMatrix();
        }
    }

    glPopMatrix();
}

// Draw text function
void drawText(float x, float y, const char* text) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Game functions
float getCurrentScrollSpeed() {
    return BASE_SCROLL_SPEED + (currentLevel - 1) * SPEED_MULTIPLIER;
}

void checkExplorationBonus() {
    if (planetsVisited > 0 && planetsVisited % PLANETS_FOR_BONUS == 0) {
        int bonus = EXPLORATION_BONUS * currentLevel;
        addPoints(bonus, player.x, player.y + 50);
        explorationBoostTimer = 60;
        planetsVisited = 0;
    }
}

void updateSpeedEffects() {
    if (explorationBoostTimer > 0) {
        explorationBoostTimer--;
    }
    currentScrollSpeed = getCurrentScrollSpeed();
}

void updateCombo() {
    if (player.comboTimer > 0) {
        player.comboTimer--;
    } else if (player.combo > 0) {
        player.combo = 0;
    }
}

void addPoints(int points, float x, float y) {
    score += points;
    if (score > highScore) {
        highScore = score;
    }
}

bool isPlanetVisible(int planetIndex) {
    if (planetIndex >= (int)planets.size()) return false;
    float planetY = planets[planetIndex].y;
    return (planetY >= cameraY - 100 && planetY <= cameraY + 500);
}

void resetGame() {
    currentLevel = 1;
    createPlanets();

    player.x = 0;
    player.y = 50;
    player.z = 0;
    player.vx = 0;
    player.vy = 0;
    player.onGround = true;
    player.jumpCount = 0;
    player.lastPlanetIndex = 0;
    player.planetsExplored = 0;
    player.combo = 0;
    player.comboTimer = 0;
    player.rotation = 0;
    player.bobOffset = 0;
    player.scarfWave = 0;
    player.timeInSpace = 0;
    player.driftingIntoSpace = false;

    cameraY = 0;
    score = 0;
    planetsVisited = 0;
    totalPlanetsExplored = 0;
    currentScrollSpeed = BASE_SCROLL_SPEED;
    explorationBoostTimer = 0;
    gameRunning = true;
    gameTime = 0;
}

void advanceToNextLevel() {
    addPoints(LEVEL_COMPLETION_BONUS * currentLevel, player.x, player.y + 30);
    currentLevel++;

    if (currentLevel > MAX_LEVELS) {
        gameRunning = false;
        return;
    }

    createPlanets();

    player.x = 0;
    player.y = 50;
    player.z = 0;
    player.vx = 0;
    player.vy = 0;
    player.onGround = true;
    player.jumpCount = 0;
    player.lastPlanetIndex = 0;
    player.planetsExplored = 0;
    player.comboTimer = 100;

    planetsVisited = 0;
    explorationBoostTimer = 120;
    cameraY = 0;
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    setupLighting();
    createStars();
    createRoses();
    createFoxes();
    createShootingStars();
    createRosePetals();
    createStardust();
    resetGame();
}

void update(int value) {
    gameTime += 0.016f;

    if (gameRunning) {
        updateCombo();
        updateSpeedEffects();
        updateAtmosphericEffects();

        float oldCameraY = cameraY;
        cameraY += currentScrollSpeed;

        for (size_t i = 0; i < planets.size(); i++) {
            if (planets[i].y < oldCameraY && planets[i].y >= oldCameraY - currentScrollSpeed) {
                planetsVisited++;
                totalPlanetsExplored++;
                checkExplorationBonus();
                break;
            }
        }

        // Player movement
        if (leftKey) {
            player.vx = -MOVE_SPEED;
            player.rotation = 45;
        } else if (rightKey) {
            player.vx = MOVE_SPEED;
            player.rotation = -45;
        } else {
            player.vx = 0;
            player.rotation = 0;
        }

        // Auto-adjust Z position
        if (!player.onGround && planets.size() > 0) {
            float nearestPlanetZ = 0;
            float minDistance = 999999;
            for (size_t i = 0; i < planets.size(); i++) {
                if (planets[i].y > player.y - 100 && planets[i].y < player.y + 50) {
                    float distance = fabs(player.x - planets[i].x);
                    if (distance < minDistance) {
                        minDistance = distance;
                        nearestPlanetZ = planets[i].z;
                    }
                }
            }
            float zDiff = nearestPlanetZ - player.z;
            if (fabs(zDiff) > 5) {
                player.z += zDiff * 0.02f;
            }
        }

        // Jumping
        bool newSpacePress = spaceKey && !spaceKeyWasPressed;
        spaceKeyWasPressed = spaceKey;

        if (player.onGround && newSpacePress) {
            player.vy = JUMP_FORCE;
            player.onGround = false;
            player.jumpCount = 1;

            if (leftKey) player.vx = -MOVE_SPEED * JUMP_BOOST;
            else if (rightKey) player.vx = MOVE_SPEED * JUMP_BOOST;
        }

        // Gravity
        float currentGravity = GRAVITY;
        if (player.vy > 0) {
            currentGravity = GRAVITY * 0.85f;
        }
        player.vy -= currentGravity;

        // Update position
        player.x += player.vx;
        player.y += player.vy;

        // Screen wrap
        if (player.x < -320) player.x = 320;
        if (player.x > 320) player.x = -320;

        // Bobbing animation
        if (player.onGround) {
            player.bobOffset = sin(gameTime * 6) * 2;
        } else {
            player.bobOffset = 0;
        }

        // Planet collision
        player.onGround = false;
        for (size_t i = 0; i < planets.size(); i++) {
            if (player.vy <= 0) {
                float dx = player.x - planets[i].x;
                float dz = player.z - planets[i].z;

                if (fabs(dx) < planets[i].width/2 + 10 &&
                    fabs(dz) < planets[i].depth/2 + 10 &&
                    player.y > planets[i].y - 10 &&
                    player.y < planets[i].y + 20) {

                    if (fabs(dx) > planets[i].width/2) {
                        player.x = planets[i].x + (dx > 0 ? planets[i].width/2 : -planets[i].width/2);
                    }
                    if (fabs(dz) > planets[i].depth/2) {
                        player.z = planets[i].z + (dz > 0 ? planets[i].depth/2 : -planets[i].depth/2);
                    }

                    player.y = planets[i].y;
                    player.vy = 0;
                    player.onGround = true;
                    player.jumpCount = 0;

                    if (i != (size_t)player.lastPlanetIndex && i > (size_t)player.lastPlanetIndex) {
                        int planetsJumped = i - player.lastPlanetIndex;
                        player.planetsExplored++;
                        player.combo++;
                        player.comboTimer = 100;

                        int basePoints = POINTS_PER_PLANET * planetsJumped;
                        int comboMultiplier = player.combo;
                        if (comboMultiplier > 10) comboMultiplier = 10;
                        int earnedPoints = basePoints * comboMultiplier;

                        addPoints(earnedPoints, player.x, player.y + 30);
                    }

                    player.lastPlanetIndex = i;
                    break;
                }
            }
        }

        // Camera following
        if (player.y > cameraY + 240) {
            cameraY = player.y - 240;
        }

        // Drifting into space game over
        if (!player.onGround) {
            player.timeInSpace += 0.016f;

            if (player.timeInSpace > 3.0f && !player.driftingIntoSpace) {
                player.driftingIntoSpace = true;
                if (score > highScore) {
                    highScore = score;
                }
                gameRunning = false;
            }
        } else {
            player.timeInSpace = 0;
            player.driftingIntoSpace = false;
        }

        // Game over conditions
        if (player.y < cameraY - 50) {
            if (score > highScore) {
                highScore = score;
            }
            gameRunning = false;
        }

        if (player.onGround && planets[player.lastPlanetIndex].y < cameraY) {
            if (score > highScore) {
                highScore = score;
            }
            gameRunning = false;
        }

        // Level completion
        if ((size_t)player.lastPlanetIndex == planets.size() - 1) {
            advanceToNextLevel();
        }

        // Update planet rotations
        for (size_t i = 0; i < planets.size(); i++) {
            planets[i].rotation += 0.08f;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float cameraX = player.x * 0.3f;
    float cameraZ = CAMERA_DISTANCE;
    float cameraLookY = cameraY + 100;

    gluLookAt(cameraX, cameraY + CAMERA_HEIGHT_OFFSET, cameraZ,
              cameraX * 0.5f, cameraLookY, 0,
              0, 1, 0);

    drawBackground();

    // Draw planets
    for (size_t i = 0; i < planets.size(); i++) {
        if (planets[i].y > cameraY - 200 && planets[i].y < cameraY + 600) {
            if (i == planets.size() - 1) {
                glColor3f(1.0f, 0.95f, 0.7f);
            } else {
                float intensity = 0.6f + 0.4f * (static_cast<float>(i) / planets.size());
                glColor3f(0.7f * intensity, 0.6f * intensity, 0.5f * intensity);
            }
            drawPlanet(planets[i]);
        }
    }

    drawLittlePrince();

    // HUD
    glColor3f(1.0f, 1.0f, 0.9f);

    std::stringstream ss;
    ss << "Stars Collected: " << score << "   Best Journey: " << highScore;
    drawText(10, WINDOW_HEIGHT - 30, ss.str().c_str());

    std::stringstream ls;
    ls << "Chapter: " << currentLevel << " / " << MAX_LEVELS << "   Cosmic Speed: " << (int)(currentScrollSpeed * 60) << "%";
    drawText(10, WINDOW_HEIGHT - 50, ls.str().c_str());

    std::stringstream cs;
    cs << "Wonder: x" << player.combo << "   Planetoids: " << (PLANETS_FOR_BONUS - planetsVisited) << " until next discovery";
    drawText(10, WINDOW_HEIGHT - 70, cs.str().c_str());

    std::stringstream bs;
    bs << "Worlds Discovered: " << totalPlanetsExplored;
    drawText(10, WINDOW_HEIGHT - 90, bs.str().c_str());

    if (explorationBoostTimer > 0) {
        float alpha = explorationBoostTimer / 60.0f;
        glColor3f(1.0f, 0.9f + alpha * 0.1f, 0.6f + alpha * 0.2f);
        drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT - 120, "New Discovery!");
    }

    // Game over screens
    if (!gameRunning) {
        if (currentLevel > MAX_LEVELS) {
            glColor3f(1.0f, 0.95f, 0.7f);
            drawText(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 30, "Journey's End");
            glColor3f(1.0f, 1.0f, 0.9f);
            drawText(WINDOW_WIDTH / 2 - 140, WINDOW_HEIGHT / 2, "The Little Prince returns to his beloved rose...");

            std::stringstream finalScore;
            finalScore << "Stars Gathered: " << score;
            drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 30, finalScore.str().c_str());

            drawText(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 - 60, "Press SPACE for another tale");
        } else if (player.driftingIntoSpace) {
            glColor3f(0.8f, 0.9f, 1.0f);
            drawText(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 + 30, "Adrift Among the Stars");
            glColor3f(1.0f, 1.0f, 0.9f);
            drawText(WINDOW_WIDTH / 2 - 160, WINDOW_HEIGHT / 2, "The Little Prince floats gently in the cosmic void...");
            drawText(WINDOW_WIDTH / 2 - 140, WINDOW_HEIGHT / 2 - 20, "Perhaps the stars will guide him home.");

            std::stringstream finalScore;
            finalScore << "Worlds Visited: " << totalPlanetsExplored;
            drawText(WINDOW_WIDTH / 2 - 90, WINDOW_HEIGHT / 2 - 50, finalScore.str().c_str());

            drawText(WINDOW_WIDTH / 2 - 90, WINDOW_HEIGHT / 2 - 100, "Press SPACE to begin anew");
        } else {
            glColor3f(1.0f, 0.8f, 0.6f);
            drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, "Lost Among Stars");
            glColor3f(1.0f, 1.0f, 0.9f);
            drawText(WINDOW_WIDTH / 2 - 130, WINDOW_HEIGHT / 2, "The Little Prince drifts in the cosmic wind...");

            std::stringstream finalScore;
            finalScore << "Worlds Visited: " << totalPlanetsExplored;
            drawText(WINDOW_WIDTH / 2 - 90, WINDOW_HEIGHT / 2 - 30, finalScore.str().c_str());

            drawText(WINDOW_WIDTH / 2 - 90, WINDOW_HEIGHT / 2 - 80, "Press SPACE to begin anew");
        }
    }

    glutSwapBuffers();
}

// Input handlers
void keyPressed(unsigned char key, int x, int y) {
    if (key == ' ') {
        spaceKey = true;
        if (!gameRunning) {
            resetGame();
        }
    }
    if (key == 27) {
        exit(0);
    }
}

void keyReleased(unsigned char key, int x, int y) {
    if (key == ' ') {
        spaceKey = false;
    }
}

void specialKeyPressed(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            leftKey = true;
            break;
        case GLUT_KEY_RIGHT:
            rightKey = true;
            break;
    }
}

void specialKeyReleased(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_LEFT:
            leftKey = false;
            break;
        case GLUT_KEY_RIGHT:
            rightKey = false;
            break;
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, (double)w / (double)h, 1.0, 2000.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned int>(time(NULL)));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Le Petit Prince: A Journey Among the Stars");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyPressed);
    glutKeyboardUpFunc(keyReleased);
    glutSpecialFunc(specialKeyPressed);
    glutSpecialUpFunc(specialKeyReleased);
    glutTimerFunc(16, update, 0);

    init();
    glutMainLoop();

    return 0;
}
