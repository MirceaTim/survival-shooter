#pragma once

#include "components/simple_scene.h"


namespace m1
{
    class Tema1 : public gfxc::SimpleScene
    {
    public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height, float angularStep)
                : x(x), y(y), width(width), height(height) {}
            float x;
            float y;
            float width;
            float height;
            float angularStep;
        };

        struct Enemy
        {
            Enemy() : startPointX(0), startPointY(0), translateX(0), translateY(0), angularStep(0), disabled(0) {}
            Enemy(float startPointX, float startPointY ,float translateX, float translateY, float angularStep, int disabled) :
                startPointX(startPointX), startPointY(startPointY), translateX(translateX), translateY(translateY), angularStep(angularStep), disabled(disabled) {}
            float startPointX, startPointY, translateX, translateY, angularStep;
            int disabled;
        };

        struct Projectile
        {
            Projectile() : translateX(0), translateY(0), time_elapsed(0), r(1), angularStep(0), disabled(0) {}
            Projectile(float translateX, float translateY, float time_elapsed, float r, float angularStep, int disabled) :
                translateX(translateX), translateY(translateY), time_elapsed(time_elapsed), r(r), angularStep(angularStep), disabled(disabled) {}
            float translateX, translateY, time_elapsed, r, angularStep;
            int disabled;
        };

        struct Obstacle
        {
            Obstacle() : x(0), y(0), w(0), h(0) {}
            Obstacle(float x, float y, float w, float h) :
                x(x), y(y), w(w), h(h) {}
            float x, y, w, h;
        };

    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        bool CheckCircleCollision(glm::vec3 centre1, glm::vec3 centre2, float dist);
        bool CheckCircleRectangleCollision(glm::vec3 centreCircle, glm::vec3 centreRectangle, float r, float w, float h);
        bool CheckCircleMapCollision(glm::vec3 centreCircle, glm::vec3 centreRectangle, float r, float w, float h);
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
        void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);
        
    protected:
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        glm::mat3 modelMatrix, visMatrix;
        std::vector<Projectile> projectiles;
        std::vector<Enemy> enemies;
        std::vector<Obstacle> obstacles;
        float translateX, translateY, angularStep, angularStepTemp; 
        float xdif, ydif, xdif2, ydif2;
        float projectile_rotation, enemy_rotation;
        float playerBigRadius, playerSmallRadius; 
        float projectileRadius, mapWidth, mapHeight;
        float enemyBigRadius, enemySmallRadius;
        float projectile_time, enemy_time;
        int score, health, max_health;
    };
}