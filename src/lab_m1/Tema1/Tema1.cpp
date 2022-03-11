#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/Tema1/DrawObjects.h"
#include "lab_m1/Tema1/MatrixTransform.h"

using namespace std;
using namespace m1;

int projectile_launched = 0;

Tema1::Tema1() 
{
}

Tema1::~Tema1()
{
}

void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    logicSpace.x = 0;       
    logicSpace.y = 0;       
    logicSpace.width = 2;   
    logicSpace.height = 2;  

    mapWidth = 8;
    mapHeight = 6;

    playerBigRadius = 0.1;
    playerSmallRadius = 0.05;

    enemyBigRadius = 0.08;
    enemySmallRadius = 0.04;

    projectileRadius = 0.02;
    translateX = 0;
    translateY = 0;
    angularStep = 0;
    projectile_rotation = 1;
    enemy_rotation = 1;
    projectile_time = 0,5;
    enemy_time = 5;
    health = 100;
    score = 0;
    max_health = 100;

    projectiles = {};
    enemies = {};
    obstacles = {};

    //adaugarea celor 5 obstacole de pe harta intr-un vector
    Obstacle* o1 = new Obstacle(1.0f, 2.0f, 0.5f, 2.25f);
    obstacles.push_back(*o1);
    Obstacle* o2 = new Obstacle(5.0f, 3.0f, 0.5f, 2.25f);
    obstacles.push_back(*o2);
    Obstacle* o3 = new Obstacle(6.5f, 2.0f, 0.5f, 2.25f);
    obstacles.push_back(*o3);
    Obstacle* o4 = new Obstacle(2.75f, 4.5f, 2.25f, 0.5f);
    obstacles.push_back(*o4);
    Obstacle* o5 = new Obstacle(3.75f, 0.5f, 2.25f, 0.5f);
    obstacles.push_back(*o5);

	glm::vec3 mapCorner = glm::vec3(0, 0, 0);

    //harta
	Mesh* map = DrawObjects::CreateRectangle("map", mapCorner, mapWidth, mapHeight, glm::vec3(1, 0, 0));
	AddMeshToList(map);

    //figura principala a jucatorului
    Mesh* playerFigure1 = DrawObjects::CreateCircle("playerFigure1", mapCorner, playerBigRadius, glm::vec3(0.5, 0.75, 0));
    AddMeshToList(playerFigure1);

    //cele 2 figuri mai mici, adiacente ale jucatorului
    Mesh* playerFigure2 = DrawObjects::CreateCircle("playerFigure2", mapCorner, playerSmallRadius, glm::vec3(1, 1, 1));
    AddMeshToList(playerFigure2);

    Mesh* playerFigure3 = DrawObjects::CreateCircle("playerFigure3", mapCorner, playerSmallRadius, glm::vec3(1, 1, 1));
    AddMeshToList(playerFigure3);

    //proiectil
    Mesh* projectile = DrawObjects::CreateCircle("projectile", mapCorner, projectileRadius, glm::vec3(1, 1, 1));
    AddMeshToList(projectile);

    //obstacol
    Mesh* obstacle = DrawObjects::CreateRectangle("obstacle", mapCorner, 0.5, 2.25, glm::vec3(0, 1, 0), true);
    AddMeshToList(obstacle);

    //figura principala a unui inamic
    Mesh* enemyFigure1 = DrawObjects::CreateCircle("enemyFigure1", mapCorner, enemyBigRadius, glm::vec3(1, 0, 0));
    AddMeshToList(enemyFigure1);
    
    //cele 2 figuri mai mici, adiacente ale inamicului
    Mesh* enemyFigure2 = DrawObjects::CreateCircle("enemyFigure2", mapCorner, enemySmallRadius, glm::vec3(1, 0, 0));
    AddMeshToList(enemyFigure2);

    Mesh* enemyFigure3 = DrawObjects::CreateCircle("enemyFigure3", mapCorner, enemySmallRadius, glm::vec3(1, 0, 0));
    AddMeshToList(enemyFigure3);

    //healthbar
    Mesh* healthBar = DrawObjects::CreateRectangle("healthbar", mapCorner, 1, 0.1, glm::vec3(0.7, 0, 0), true);
    AddMeshToList(healthBar);

    //healthbar wireframe
    Mesh* healthBarBackground = DrawObjects::CreateRectangle("healthbarbackground", mapCorner, 1, 0.1, glm::vec3(1, 1, 1), true);
    AddMeshToList(healthBarBackground);
}

glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
    float sx, sy, tx, ty, smin;
    sx = viewSpace.width / logicSpace.width;
    sy = viewSpace.height / logicSpace.height;
    if (sx < sy)
        smin = sx;
    else
        smin = sy;
    tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
    ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

    return glm::transpose(glm::mat3(
        smin, 0.0f, tx,
        0.0f, smin, ty,
        0.0f, 0.0f, 1.0f));
}

void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
    glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);
    glEnable(GL_SCISSOR_TEST);
    glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);
    glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
    GetSceneCamera()->Update();
}

//verifica o posibila coliziune intre doua cercuri
bool Tema1::CheckCircleCollision(glm::vec3 centre1, glm::vec3 centre2, float dist) {
    float dist1 = (centre1.x - centre2.x) * (centre1.x - centre2.x);
    float dist2 = (centre1.y - centre2.y) * (centre1.y - centre2.y);
    float dist3 = sqrt(dist1 + dist2);
    if (dist3 <= dist) return true;
    else return false;
}

//verifica o posibila coliziune intre un cerc si un obstacol
bool Tema1::CheckCircleRectangleCollision(glm::vec3 centreCircle, glm::vec3 centreRectangle, float r, float w, float h) {
    glm::vec3 diff = centreCircle - centreRectangle;
    glm::vec3 half = glm::vec3(w / 2, h / 2, 0);
    glm::vec3 clamped = glm::clamp(diff, -half, half);
    glm::vec3 closest = centreRectangle + clamped;
    diff = closest - centreCircle;
    return glm::length(diff) < r;
}

//verifica o posibila coliziune intre un cerc si harta
bool Tema1::CheckCircleMapCollision(glm::vec3 centreCircle, glm::vec3 centreRectangle, float r, float w, float h) {
    glm::vec3 diff = centreCircle - centreRectangle;
    glm::vec3 half = glm::vec3(w / 2 - r, h / 2 - r, 0);
    glm::vec3 clamped = glm::clamp(diff, -half, half);
    glm::vec3 closest = centreRectangle + clamped;
    diff = closest - centreCircle;
    return glm::length(diff) > 0.0003;
}

void Tema1::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema1::Update(float deltaTimeSeconds)
{
    projectile_time += deltaTimeSeconds;
    enemy_time += deltaTimeSeconds;
    glm::ivec2 resolution = window->GetResolution();
    viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
    SetViewportArea(viewSpace, glm::vec3(0), true);
    visMatrix = glm::mat3(1);
    visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

    //se deseneaza jucatorul si healthbar-ul
    modelMatrix = visMatrix * MatrixTransform::Translate(logicSpace.x + 0.5, logicSpace.y + 1.8);
    modelMatrix *= MatrixTransform::Scale((float) health / max_health, 1);
    RenderMesh2D(meshes["healthbar"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * MatrixTransform::Translate(logicSpace.x + 0.5, logicSpace.y + 1.8);
    RenderMesh2D(meshes["healthbarbackground"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * MatrixTransform::Translate(logicSpace.width / 2 + logicSpace.x, logicSpace.height / 2 + logicSpace.y);
    modelMatrix *= MatrixTransform::Rotate(angularStep);
    RenderMesh2D(meshes["playerFigure1"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * MatrixTransform::Translate(logicSpace.width / 2 + logicSpace.x - playerBigRadius * sqrt(2) / 2, logicSpace.height / 2 + logicSpace.y + playerBigRadius * sqrt(2) / 2);
    modelMatrix *= MatrixTransform::Translate(playerBigRadius * sqrt(2) / 2, -(playerBigRadius * sqrt(2) / 2));
    modelMatrix *= MatrixTransform::Rotate(angularStep);
    modelMatrix *= MatrixTransform::Translate(-(playerBigRadius * sqrt(2) / 2), playerBigRadius * sqrt(2) / 2);
    RenderMesh2D(meshes["playerFigure2"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * MatrixTransform::Translate(logicSpace.width / 2 + logicSpace.x + playerBigRadius * sqrt(2) / 2, logicSpace.height / 2 + logicSpace.y + playerBigRadius * sqrt(2) / 2);
    modelMatrix *= MatrixTransform::Translate(-(playerBigRadius * sqrt(2) / 2 ), -(playerBigRadius * sqrt(2) / 2 ));
    modelMatrix *= MatrixTransform::Rotate(angularStep);
    modelMatrix *= MatrixTransform::Translate(playerBigRadius * sqrt(2) / 2, playerBigRadius * sqrt(2) / 2);
    RenderMesh2D(meshes["playerFigure3"], shaders["VertexColor"], modelMatrix);

    //verificam daca a trecut destul timp de la aparitia ultimului inamic
    if (enemy_time > 5) {
        float enemyX;
        float enemyY;
        //alegem la intamplare pozitiile inamicului, asigurandu-ne ca nu se vor spawna in imediata apropiere a player-ului
        do {
            enemyX = (float)mapWidth * (rand() % 100) / 100;
            enemyY = (float)mapHeight * (rand() % 100) / 100;
        } while ((glm::abs(enemyX - logicSpace.x - logicSpace.width / 2) < (5 * playerBigRadius) && 
                 glm::abs(enemyY - logicSpace.y - logicSpace.height / 2) < (5 * playerBigRadius)) ||
                enemyX < 0.5 || enemyX > mapWidth - 0.5 || enemyY < 0.5 || enemyY > mapWidth - 0.5);
        
        //initializam inamicul
        Enemy* e = new Enemy(enemyX, enemyY, 0, 0, 0, 0);
        enemies.push_back(*e);
        enemy_time = 0;
    }

    //se calculeaza noua pozitie a proiectilelor
    for (int i = 0; i < projectiles.size(); ++i) {
        if (!projectiles[i].disabled) {
            projectiles[i].translateX += (float)deltaTimeSeconds * glm::cos(projectiles[i].r) * 2;
            projectiles[i].translateY += (float)deltaTimeSeconds * glm::sin(projectiles[i].r) * 2;

            projectiles[i].time_elapsed += deltaTimeSeconds;
            if (projectiles[i].time_elapsed > 5) {
                projectiles[i].disabled = 1;
            }
        }
    }

    for (int i = 0; i < projectiles.size(); ++i) {
        if (!projectiles[i].disabled) {
            glm::vec3 CentreProjectile = glm::vec3(projectiles[i].translateX, projectiles[i].translateY, 0);
            //se verifica coliziunea dintre proiectil si obstacole
            for (int j = 0; j < obstacles.size(); ++j) {
                if (CheckCircleRectangleCollision(CentreProjectile, glm::vec3(obstacles[j].w / 2 + obstacles[j].x, obstacles[j].h / 2 + obstacles[j].y, 0), projectileRadius, obstacles[j].w, obstacles[j].h)) {
                    //proiectilul nu se va mai desena
                    projectiles[i].disabled = 1;
                }
            }
            //se verifica coliziunea dintre proiectil si harta
            if (CheckCircleMapCollision(CentreProjectile, glm::vec3(mapWidth/2, mapHeight/2, 0), projectileRadius, mapWidth, mapHeight)) {
                //proiectilul nu se va mai desena
                projectiles[i].disabled = 1;
            }
        }
    }
    
    for (int i = 0; i < enemies.size(); ++i) {
        //se realizeaza miscarea si orientarea inamicilor
        if (!enemies[i].disabled) {
            ydif2 = logicSpace.y + logicSpace.height / 2 - enemies[i].startPointY - enemies[i].translateY;
            xdif2 = logicSpace.x + logicSpace.width / 2 - enemies[i].startPointX - enemies[i].translateX;
            enemy_rotation = glm::atan2(ydif2, xdif2);
            enemies[i].angularStep = enemy_rotation - M_PI / 2;
            enemies[i].translateX += (float)deltaTimeSeconds * glm::cos(enemy_rotation) * 0.5;
            enemies[i].translateY += (float)deltaTimeSeconds * glm::sin(enemy_rotation) * 0.5;

            //se verifica coliziunea dintre jucator si oricare dintre inamici
            //se calculeaza cele 3 centre ale fiecarei figuri
            glm::vec3 centrePlayer1 = glm::vec3(logicSpace.width / 2 + logicSpace.x, logicSpace.height / 2 + logicSpace.y, 0);
            glm::vec3 centrePlayer2 = glm::vec3(logicSpace.width / 2 + logicSpace.x - playerBigRadius * glm::cos(angularStep - M_PI / 4), logicSpace.height / 2 + logicSpace.y - playerBigRadius * glm::sin(angularStep - M_PI / 4), 0);
            glm::vec3 centrePlayer3 = glm::vec3(logicSpace.width / 2 + logicSpace.x + playerBigRadius * glm::cos(angularStep + M_PI / 4), logicSpace.height / 2 + logicSpace.y + playerBigRadius * glm::sin(angularStep + M_PI / 4), 0);
            glm::vec3 centreEnemy1 = glm::vec3(enemies[i].startPointX + enemies[i].translateX, enemies[i].startPointY + enemies[i].translateY, 0);
            glm::vec3 centreEnemy2 = glm::vec3(enemies[i].startPointX + enemies[i].translateX - enemyBigRadius * glm::cos(enemy_rotation - M_PI / 4), enemies[i].startPointY - enemies[i].translateY + enemyBigRadius * glm::sin(enemy_rotation - M_PI / 4), 0);
            glm::vec3 centreEnemy3 = glm::vec3(enemies[i].startPointX + enemies[i].translateX + enemyBigRadius * glm::cos(enemy_rotation + M_PI / 4), enemies[i].startPointY + enemies[i].translateY + enemyBigRadius * glm::sin(enemy_rotation + M_PI / 4), 0);

            //se verifica toate cele 9 coliziuni posibile dintre 3 figuri ale jucatorului si 3 figuri ale unui inamic
            if (CheckCircleCollision(centreEnemy1, centrePlayer1, enemyBigRadius + playerBigRadius) || CheckCircleCollision(centreEnemy2, centrePlayer1, enemySmallRadius + playerBigRadius) || CheckCircleCollision(centreEnemy3, centrePlayer1, enemySmallRadius + playerBigRadius) ||
                CheckCircleCollision(centreEnemy1, centrePlayer2, enemyBigRadius + playerSmallRadius) || CheckCircleCollision(centreEnemy2, centrePlayer2, enemySmallRadius + playerSmallRadius) || CheckCircleCollision(centreEnemy3, centrePlayer2, enemySmallRadius + playerSmallRadius) ||
                CheckCircleCollision(centreEnemy1, centrePlayer3, enemyBigRadius + playerSmallRadius) || CheckCircleCollision(centreEnemy2, centrePlayer3, enemySmallRadius + playerSmallRadius) || CheckCircleCollision(centreEnemy3, centrePlayer3, enemySmallRadius + playerSmallRadius)) {
                //in cazul in care exista o coliziune, inamicul nu va mai fi desenat, si se scade din viata
                enemies[i].disabled = 1;
                health -= 10;
                if (health == 0) {
                    exit(0);
                }
            }
        }
    }

    //se verifica coliziunile intre oricare din inamici si oricare din proiectile
    for (int i = 0; i < enemies.size(); ++i) {
        for (int j = 0; j < projectiles.size(); ++j) {
            if (!enemies[i].disabled && !projectiles[j].disabled) {
                glm::vec3 centreEnemy1 = glm::vec3(enemies[i].startPointX + enemies[i].translateX, enemies[i].startPointY + enemies[i].translateY, 0);
                glm::vec3 centreEnemy2 = glm::vec3(enemies[i].startPointX + enemies[i].translateX - enemyBigRadius * glm::cos(enemy_rotation - M_PI / 4), enemies[i].startPointY - enemies[i].translateY + enemyBigRadius * glm::sin(enemy_rotation - M_PI / 4), 0);
                glm::vec3 centreEnemy3 = glm::vec3(enemies[i].startPointX + enemies[i].translateX + enemyBigRadius * glm::cos(enemy_rotation + M_PI / 4), enemies[i].startPointY + enemies[i].translateY + enemyBigRadius * glm::sin(enemy_rotation + M_PI / 4), 0);
                glm::vec3 CentreProjectile = glm::vec3(projectiles[j].translateX, projectiles[j].translateY, 0);
                if (CheckCircleCollision(centreEnemy1, CentreProjectile, enemyBigRadius + projectileRadius) ||
                    CheckCircleCollision(centreEnemy2, CentreProjectile, enemySmallRadius + projectileRadius) ||
                    CheckCircleCollision(centreEnemy3, CentreProjectile, enemySmallRadius + projectileRadius)) {
                    //in cazul unei coliziuni, inamicul si proiectilul in cauza nu se vor mai desena
                    enemies[i].disabled = 1;
                    projectiles[j].disabled = 1;
                    //creste scorul si se afiseaza in consola
                    score += 100;
                    printf("Total score: %d\n", score);
                }
            }
        }
    }

    //se realizeaza desenarea proiectilelor valide (care n-au facut contact cu mapa, obstacol sau inamic
    for (int i = 0; i < projectiles.size(); ++i) {
        if (!projectiles[i].disabled) {
            modelMatrix = visMatrix * MatrixTransform::Translate(projectiles[i].translateX, projectiles[i].translateY);
            modelMatrix *= MatrixTransform::Rotate(projectiles[i].angularStep);
            RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);
        }
    }

    //se realizeaza desenarea inamicilor valizi (care nu au fost contact cu jucatorul sau vreun proiectil)
    for (int i = 0; i < enemies.size(); ++i) {
        if (!enemies[i].disabled) {
            modelMatrix = visMatrix * MatrixTransform::Translate(enemies[i].startPointX + enemies[i].translateX, enemies[i].startPointY + enemies[i].translateY);
            modelMatrix *= MatrixTransform::Rotate(enemies[i].angularStep);
            RenderMesh2D(meshes["enemyFigure1"], shaders["VertexColor"], modelMatrix);


            modelMatrix = visMatrix * MatrixTransform::Translate(enemies[i].startPointX + enemies[i].translateX - enemyBigRadius * sqrt(2) / 2, enemies[i].startPointY + enemies[i].translateY + enemyBigRadius * sqrt(2) / 2);
            modelMatrix *= MatrixTransform::Translate(enemyBigRadius * sqrt(2) / 2, -(enemyBigRadius * sqrt(2) / 2));
            modelMatrix *= MatrixTransform::Rotate(enemies[i].angularStep);
            modelMatrix *= MatrixTransform::Translate(-(enemyBigRadius * sqrt(2) / 2), enemyBigRadius * sqrt(2) / 2);
            RenderMesh2D(meshes["enemyFigure2"], shaders["VertexColor"], modelMatrix);



            modelMatrix = visMatrix * MatrixTransform::Translate(enemies[i].startPointX + enemies[i].translateX + enemyBigRadius * sqrt(2) / 2, enemies[i].startPointY + enemies[i].translateY + enemyBigRadius * sqrt(2) / 2);
            modelMatrix *= MatrixTransform::Translate(-(enemyBigRadius * sqrt(2) / 2), -(enemyBigRadius * sqrt(2) / 2));
            modelMatrix *= MatrixTransform::Rotate(enemies[i].angularStep);
            modelMatrix *= MatrixTransform::Translate(enemyBigRadius * sqrt(2) / 2, enemyBigRadius * sqrt(2) / 2);
            RenderMesh2D(meshes["enemyFigure3"], shaders["VertexColor"], modelMatrix);
        }
    }
    //din moment ce inamicii sunt mai mici decat jucatorul, si il urmaresc constant
    //pe acesta, verificarea coliziunilor dintre inamici si peretii hartii este inutila

    //desenarea hartii si a obstacolelor
    modelMatrix = visMatrix * MatrixTransform::Translate(0, 0);
    RenderMesh2D(meshes["map"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * MatrixTransform::Translate(1, 2);
    RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * MatrixTransform::Translate(5, 3);
    RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * MatrixTransform::Translate(6.5, 2);
    RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * MatrixTransform::Translate(5, 4.5);
    modelMatrix *= MatrixTransform::Rotate(M_PI / 2);
    RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);

    modelMatrix = visMatrix * MatrixTransform::Translate(6, 0.5);
    modelMatrix *= MatrixTransform::Rotate(M_PI / 2);
    RenderMesh2D(meshes["obstacle"], shaders["VertexColor"], modelMatrix);
    
   
}

void Tema1::FrameEnd()
{
}
void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    //se realizeaza miscarea jucatorului
    //se verifica coliziunile dintre jucator si harta, respectiv jucator si obstacol, prin miscarea jucatorului
    //pentru fiecare dintre cele 4 taste, se modifica pozitia jucatorului, iar apoi se verifica
    //daca prin aceasta miscare se realizeaza o coliziune fie cu harta, fie cu vreun obstacol
    //daca da, se va realiza miscarea inapoi a jucatorului in pozitia initiala
    if (window->KeyHold(GLFW_KEY_A)) {
        logicSpace.x -= deltaTime / 3;
        glm::vec3 centrePlayer1 = glm::vec3(logicSpace.width / 2 + logicSpace.x, logicSpace.height / 2 + logicSpace.y, 0);
        glm::vec3 centrePlayer2 = glm::vec3(logicSpace.width / 2 + logicSpace.x - playerBigRadius * glm::cos(angularStep - M_PI / 4), logicSpace.height / 2 + logicSpace.y - playerBigRadius * glm::sin(angularStep - M_PI / 4), 0);
        glm::vec3 centrePlayer3 = glm::vec3(logicSpace.width / 2 + logicSpace.x + playerBigRadius * glm::cos(angularStep + M_PI / 4), logicSpace.height / 2 + logicSpace.y + playerBigRadius * glm::sin(angularStep + M_PI / 4), 0);
        if (CheckCircleMapCollision(centrePlayer1, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerBigRadius, mapWidth, mapHeight) ||
            CheckCircleMapCollision(centrePlayer2, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerSmallRadius, mapWidth, mapHeight) ||
            CheckCircleMapCollision(centrePlayer3, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerSmallRadius, mapWidth, mapHeight)) {
            logicSpace.x += deltaTime / 3;
        }
        for (int i = 0; i < obstacles.size(); ++i) {
            if (CheckCircleRectangleCollision(centrePlayer1, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerBigRadius, obstacles[i].w, obstacles[i].h) +
                CheckCircleRectangleCollision(centrePlayer2, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerSmallRadius, obstacles[i].w, obstacles[i].h) +
                CheckCircleRectangleCollision(centrePlayer3, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerSmallRadius, obstacles[i].w, obstacles[i].h) != 0) {
                logicSpace.x += deltaTime / 3;
            }
        }
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        logicSpace.x += deltaTime / 3;
        glm::vec3 centrePlayer1 = glm::vec3(logicSpace.width / 2 + logicSpace.x, logicSpace.height / 2 + logicSpace.y, 0);
        glm::vec3 centrePlayer2 = glm::vec3(logicSpace.width / 2 + logicSpace.x - playerBigRadius * glm::cos(angularStep - M_PI / 4), logicSpace.height / 2 + logicSpace.y - playerBigRadius * glm::sin(angularStep - M_PI / 4), 0);
        glm::vec3 centrePlayer3 = glm::vec3(logicSpace.width / 2 + logicSpace.x + playerBigRadius * glm::cos(angularStep + M_PI / 4), logicSpace.height / 2 + logicSpace.y + playerBigRadius * glm::sin(angularStep + M_PI / 4), 0);
        if (CheckCircleMapCollision(centrePlayer1, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerBigRadius, mapWidth, mapHeight) ||
            CheckCircleMapCollision(centrePlayer2, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerSmallRadius, mapWidth, mapHeight) ||
            CheckCircleMapCollision(centrePlayer3, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerSmallRadius, mapWidth, mapHeight)) {
            logicSpace.x -= deltaTime / 3;
        }
        for (int i = 0; i < obstacles.size(); ++i) {
            if (CheckCircleRectangleCollision(centrePlayer1, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerBigRadius, obstacles[i].w, obstacles[i].h) +
                CheckCircleRectangleCollision(centrePlayer2, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerSmallRadius, obstacles[i].w, obstacles[i].h) +
                CheckCircleRectangleCollision(centrePlayer3, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerSmallRadius, obstacles[i].w, obstacles[i].h) != 0) {
                logicSpace.x -= deltaTime / 3;
            }
        }
    }
    if (window->KeyHold(GLFW_KEY_W)) {
        logicSpace.y += deltaTime / 3;
        glm::vec3 centrePlayer1 = glm::vec3(logicSpace.width / 2 + logicSpace.x, logicSpace.height / 2 + logicSpace.y, 0);
        glm::vec3 centrePlayer2 = glm::vec3(logicSpace.width / 2 + logicSpace.x - playerBigRadius * glm::cos(angularStep - M_PI / 4), logicSpace.height / 2 + logicSpace.y - playerBigRadius * glm::sin(angularStep - M_PI / 4), 0);
        glm::vec3 centrePlayer3 = glm::vec3(logicSpace.width / 2 + logicSpace.x + playerBigRadius * glm::cos(angularStep + M_PI / 4), logicSpace.height / 2 + logicSpace.y + playerBigRadius * glm::sin(angularStep + M_PI / 4), 0);
        if (CheckCircleMapCollision(centrePlayer1, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerBigRadius, mapWidth, mapHeight) ||
            CheckCircleMapCollision(centrePlayer2, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerSmallRadius, mapWidth, mapHeight) ||
            CheckCircleMapCollision(centrePlayer3, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerSmallRadius, mapWidth, mapHeight)) {
            logicSpace.y -= deltaTime / 3;
        }
        for (int i = 0; i < obstacles.size(); ++i) {
            if (CheckCircleRectangleCollision(centrePlayer1, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerBigRadius, obstacles[i].w, obstacles[i].h) +
                CheckCircleRectangleCollision(centrePlayer2, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerSmallRadius, obstacles[i].w, obstacles[i].h) +
                CheckCircleRectangleCollision(centrePlayer3, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerSmallRadius, obstacles[i].w, obstacles[i].h) != 0) {
                logicSpace.y -= deltaTime / 3;
            }
        }
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        logicSpace.y -= deltaTime / 3;
        glm::vec3 centrePlayer1 = glm::vec3(logicSpace.width / 2 + logicSpace.x, logicSpace.height / 2 + logicSpace.y, 0);
        glm::vec3 centrePlayer2 = glm::vec3(logicSpace.width / 2 + logicSpace.x - playerBigRadius * glm::cos(angularStep - M_PI / 4), logicSpace.height / 2 + logicSpace.y - playerBigRadius * glm::sin(angularStep - M_PI / 4), 0);
        glm::vec3 centrePlayer3 = glm::vec3(logicSpace.width / 2 + logicSpace.x + playerBigRadius * glm::cos(angularStep + M_PI / 4), logicSpace.height / 2 + logicSpace.y + playerBigRadius * glm::sin(angularStep + M_PI / 4), 0);
        if (CheckCircleMapCollision(centrePlayer1, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerBigRadius, mapWidth, mapHeight) ||
            CheckCircleMapCollision(centrePlayer2, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerSmallRadius, mapWidth, mapHeight) ||
            CheckCircleMapCollision(centrePlayer3, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerSmallRadius, mapWidth, mapHeight)) {
            logicSpace.y += deltaTime / 3;
        }
        for (int i = 0; i < obstacles.size(); ++i) {
            if (CheckCircleRectangleCollision(centrePlayer1, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerBigRadius, obstacles[i].w, obstacles[i].h) +
                CheckCircleRectangleCollision(centrePlayer2, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerSmallRadius, obstacles[i].w, obstacles[i].h) +
                CheckCircleRectangleCollision(centrePlayer3, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerSmallRadius, obstacles[i].w, obstacles[i].h) != 0) {
                logicSpace.y += deltaTime / 3;
            }
        }
    }
}
void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    //se controleaza fire rate-ul
    if (projectile_time > 0.5) {
        //se initializeaza proiectilul
        projectile_rotation = angularStep + M_PI / 2;
        Projectile* p = new Projectile(logicSpace.x + logicSpace.width / 2, logicSpace.y + logicSpace.height / 2, 0, projectile_rotation, angularStep, 0);
        projectiles.push_back(*p);
        projectile_time = 0;
    }
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    //se calculeaza si realizeaza orientarea jucatorului
    angularStepTemp = angularStep;
    glm::ivec2 resolution = window->GetResolution();
    ydif = resolution.y/2 - mouseY;
    xdif = mouseX - resolution.x/2;
    angularStep = (float)(glm::atan(ydif, xdif) - M_PI / 2);
    //se verifica coliziunile dintre jucator si harta, respectiv jucator si obstacol, prin miscarea mouse-ului
    //se verifica daca prin miscarea mouse-ului, jucatorul se va lovi de harta
    glm::vec3 centrePlayer1 = glm::vec3(logicSpace.width / 2 + logicSpace.x, logicSpace.height / 2 + logicSpace.y, 0);
    glm::vec3 centrePlayer2 = glm::vec3(logicSpace.width / 2 + logicSpace.x - playerBigRadius * glm::cos(angularStep - M_PI / 4), logicSpace.height / 2 + logicSpace.y - playerBigRadius * glm::sin(angularStep - M_PI / 4), 0);
    glm::vec3 centrePlayer3 = glm::vec3(logicSpace.width / 2 + logicSpace.x + playerBigRadius * glm::cos(angularStep + M_PI / 4), logicSpace.height / 2 + logicSpace.y + playerBigRadius * glm::sin(angularStep + M_PI / 4), 0);
    if (CheckCircleMapCollision(centrePlayer1, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerBigRadius, mapWidth, mapHeight) +
        CheckCircleMapCollision(centrePlayer2, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerSmallRadius, mapWidth, mapHeight) +
        CheckCircleMapCollision(centrePlayer3, glm::vec3(mapWidth / 2, mapHeight / 2, 0), playerSmallRadius, mapWidth, mapHeight) != 0) {
        //daca da, se va pastra unghiul de dinainte de miscarea mouse-ului, pentru a nu depasi limita
        angularStep = angularStepTemp;
    }
    //se verifica daca prin miscarea mouse-ului, jucatorul se va lovi de obstacole
    for (int i = 0; i < obstacles.size(); ++i) {
        if (CheckCircleRectangleCollision(centrePlayer1, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerBigRadius, obstacles[i].w, obstacles[i].h) +
            CheckCircleRectangleCollision(centrePlayer2, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerSmallRadius, obstacles[i].w, obstacles[i].h) +
            CheckCircleRectangleCollision(centrePlayer3, glm::vec3(obstacles[i].w / 2 + obstacles[i].x, obstacles[i].h / 2 + obstacles[i].y, 0), playerSmallRadius, obstacles[i].w, obstacles[i].h) != 0) {
            //daca da, se va pastra unghiul de dinainte de miscarea mouse-ului, pentru a nu depasi limita
            angularStep = angularStepTemp;
        }
    }
}