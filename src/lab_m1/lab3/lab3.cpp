#include "lab_m1/lab3/lab3.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab3::Lab3()
{
}


Lab3::~Lab3()
{
}


void Lab3::Init()
{
    position = window->GetCursorPosition();
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 100;

    // TODO(student): Compute coordinates of a square's center, and store
    // then in the `cx` and `cy` class variables (see the header). Use
    // `corner` and `squareSide`. These two class variables will be used
    // in the `Update()` function. Think about it, why do you need them?
    cx = corner.x + (squareSide / 2);
    cy = corner.y + (squareSide / 2);

    // Initialize tx and ty (the translation steps)
    translateX = 0;
    translateY = 0;

    // Initialize sx and sy (the scale factors)
    scaleX = 1;
    scaleY = 1;

    // Initialize angularStep
    angularStep = 0;

    coef1 = 1;
    coef2 = 1;
    x = position.x;
    y = position.y;

    Mesh* square1 = object2D::CreateSquare("square1", corner, squareSide, glm::vec3(1, 0, 0), true);
    AddMeshToList(square1);

    Mesh* square2 = object2D::CreateSquare("square2", corner, squareSide, glm::vec3(0, 1, 0));
    AddMeshToList(square2);

    Mesh* square3 = object2D::CreateSquare("square3", corner, squareSide, glm::vec3(0, 0, 1));
    AddMeshToList(square3);

    Mesh* square4 = object2D::CreateSquare("square4", corner, squareSide/2, glm::vec3(1, 1, 0), true);
    AddMeshToList(square4);

    Mesh* square5 = object2D::CreateSquare("square5", corner, squareSide/2, glm::vec3(0, 1, 1));
    AddMeshToList(square5);

    Mesh* square6 = object2D::CreateSquare("square6", corner, squareSide/2, glm::vec3(1, 0, 1));
    AddMeshToList(square6);

    vector<VertexFormat> vertices3
    {
        VertexFormat(corner, glm::vec3(1, 0, 0)),
        VertexFormat(corner + glm::vec3(squareSide, 0, 0), glm::vec3(1, 0, 0)),
        VertexFormat(corner + glm::vec3(squareSide/2, squareSide, 0), glm::vec3(1, 0, 0)),
    };

    vector<unsigned int> indices3 =
    {
        0, 1, 2,
    };

    Mesh* triangle = new Mesh("triangle");
    triangle->InitFromData(vertices3, indices3);
    AddMeshToList(triangle);
}


void Lab3::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab3::Update(float deltaTimeSeconds)
{
    // TODO(student): Update steps for translation, rotation and scale,
    // in order to create animations. Use the class variables in the
    // class header, and if you need more of them to complete the task,
    // add them over there!
    
    position = window->GetCursorPosition();
    x = position.x;
    y = position.y;
    printf("%f %f\n", x, y);

    ydif = 433.33333333 - y;
    xdif = x - 450;
    angularStep = glm::atan(ydif, xdif) - 3.1415629 / 2;

    if (translateX > 350) coef1 = -1;
    if (translateX < 0) coef1 = 1;
    translateX += 75 * coef1 * deltaTimeSeconds;
    translateY += 75 * coef1 * deltaTimeSeconds;
    
    if (scaleX > 2) coef2 = -1;
    if (scaleX < 1) coef2 = 1;
    scaleX += 0.5 * coef2 * deltaTimeSeconds;
    scaleY += 0.5 * coef2 * deltaTimeSeconds;

    //angularStep += 3 * deltaTimeSeconds;

    /*modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(150, 250);
    // TODO(student): Create animations by multiplying the current
    // transform matrix with the matrices you just implemented.
    // Remember, the last matrix in the chain will take effect first!
    modelMatrix *= transform2D::Translate(translateX, translateY);
    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(400, 250);
    // TODO(student): Create animations by multiplying the current
    // transform matrix with the matrices you just implemented
    // Remember, the last matrix in the chain will take effect first!
    modelMatrix *= transform2D::Translate(cx, cy);
    modelMatrix *= transform2D::Rotate(angularStep);
    modelMatrix *= transform2D::Translate((-1) * cx, (-1) * cy);
    RenderMesh2D(meshes["square2"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(650, 250);
    // TODO(student): Create animations by multiplying the current
    // transform matrix with the matrices you just implemented
    // Remember, the last matrix in the chain will take effect first!
    modelMatrix *= transform2D::Translate(cx, cy);
    modelMatrix *= transform2D::Scale(scaleX, scaleY);
    modelMatrix *= transform2D::Translate((-1) * cx, (-1) * cy);
    RenderMesh2D(meshes["square3"], shaders["VertexColor"], modelMatrix);

    //bonus
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(800, 400);
    RenderMesh2D(meshes["square4"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(1000, 400);
    modelMatrix *= transform2D::Translate(-175, 25);
    modelMatrix *= transform2D::Rotate(0.2 * angularStep);
    modelMatrix *= transform2D::Translate(175, -25);
    RenderMesh2D(meshes["square5"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(1100, 400);
    modelMatrix *= transform2D::Translate(-275, 25);
    modelMatrix *= transform2D::Rotate(0.2 * angularStep);
    modelMatrix *= transform2D::Translate(275, -25);
    modelMatrix *= transform2D::Translate(-75, 25);
    modelMatrix *= transform2D::Rotate(angularStep);
    modelMatrix *= transform2D::Translate(75, -25);
    RenderMesh2D(meshes["square6"], shaders["VertexColor"], modelMatrix);
    */


    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(400, 250);
    // TODO(student): Create animations by multiplying the current
    // transform matrix with the matrices you just implemented
    // Remember, the last matrix in the chain will take effect first!
    modelMatrix *= transform2D::Translate(cx, cy * 2 / 3);
    modelMatrix *= transform2D::Rotate(angularStep);
    modelMatrix *= transform2D::Translate((-1) * cx, (-1) * cy * 2 / 3);
    RenderMesh2D(meshes["triangle"], shaders["VertexColor"], modelMatrix);
    
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(350, 200);
    modelMatrix *= transform2D::Translate(100, 83.33333);
    modelMatrix *= transform2D::Rotate(angularStep);
    modelMatrix *= transform2D::Translate(-100, -83.333333);
    RenderMesh2D(meshes["square5"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(500, 200);
    modelMatrix *= transform2D::Translate(-175, 25);
    modelMatrix *= transform2D::Rotate(angularStep);
    modelMatrix *= transform2D::Translate(175, -25);
    RenderMesh2D(meshes["square6"], shaders["VertexColor"], modelMatrix);
}


void Lab3::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab3::OnInputUpdate(float deltaTime, int mods)
{
}


void Lab3::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Lab3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab3::OnWindowResize(int width, int height)
{
}
