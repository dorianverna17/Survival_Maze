#include "lab_m1/lab4/lab4.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/lab4/transform3D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab4::Lab4()
{
}


Lab4::~Lab4()
{
}


void Lab4::Init()
{
    polygonMode = GL_FILL;

    Mesh* mesh = new Mesh("box");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
    meshes[mesh->GetMeshID()] = mesh;

    // Initialize tx, ty and tz (the translation steps)
    translateX = 0;
    translateY = 0;
    translateZ = 0;

    // Initialize sx, sy and sz (the scale factors)
    scaleX = 1;
    scaleY = 1;
    scaleZ = 1;

    // Initialize angular steps
    angularStepOX = 0;
    angularStepOY = 0;
    angularStepOZ = 0;

    angularStepTorso = 0;
    angularStepLeg = 0;
    translate_X = 0;
}


void Lab4::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab4::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(-2.5f, 0.5f, -1.5f);
    modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0.0f, 0.5f, -1.5f);
    modelMatrix *= transform3D::Scale(scaleX, scaleY, scaleZ);
    RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(2.5f, 0.5f, -1.5f);
    modelMatrix *= transform3D::RotateOX(angularStepOX);
    modelMatrix *= transform3D::RotateOY(angularStepOY);
    modelMatrix *= transform3D::RotateOZ(angularStepOZ);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // BONUS
    // left leg
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(1.5f, 0.0f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::Translate(0.0f, 1.2f, 0.0f);
    modelMatrix *= transform3D::RotateOX(angularStepLeg);
    modelMatrix *= transform3D::Translate(0.0f, -1.2f, 0.0f);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(1.5f, 0.6f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::RotateOX(angularStepLeg);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(1.5f, 1.2f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // right leg
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(2.2f, 0.0f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(2.2f, 0.6f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(2.2f, 1.2f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // body
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(1.85f, 1.8f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::RotateOY(angularStepTorso);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(1.85f, 2.4f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::RotateOY(angularStepTorso);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(1.85f, 3.0f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::RotateOY(angularStepTorso);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // head
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(1.85f, 3.55f, 1.5f);
    modelMatrix *= transform3D::Scale(0.4f, 0.4f, 0.4f);
    modelMatrix *= transform3D::RotateOY(angularStepTorso);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // arms
    // left arm
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(1.3f, 3.0f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::Translate(1.1f, 0.0f, 0.0f);
    modelMatrix *= transform3D::RotateOY(angularStepTorso);
    modelMatrix *= transform3D::Translate(-1.1f, 0.0f, 0.0f);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(0.8f, 2.5f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::Translate(2.1f, 0.0f, 0.0f);
    modelMatrix *= transform3D::RotateOY(angularStepTorso);
    modelMatrix *= transform3D::Translate(-2.1f, 0.0f, 0.0f);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    // right arm
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(2.4f, 3.0f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::Translate(-1.1f, 0.0f, 0.0f);
    modelMatrix *= transform3D::RotateOY(angularStepTorso);
    modelMatrix *= transform3D::Translate(1.1f, 0.0f, 0.0f);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(2.9f, 2.5f, 1.5f);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::Translate(-2.1f, 0.0f, 0.0f);
    modelMatrix *= transform3D::RotateOY(angularStepTorso);
    modelMatrix *= transform3D::Translate(2.1f, 0.0f, 0.0f);
    modelMatrix *= transform3D::Translate(0.0f, 0.0f, translate_X);
    RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
}


void Lab4::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

int decrease = 1;
int increase = 0;

void Lab4::OnInputUpdate(float deltaTime, int mods)
{
    // TODO(student): Add transformation logic
    
    // Translating the first cube
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        if (window->KeyHold(GLFW_KEY_W)) {
            translateY += deltaTime;
        }
        if (window->KeyHold(GLFW_KEY_S)) {
            translateY -= deltaTime;
        }
        if (window->KeyHold(GLFW_KEY_A)) {
            translateX -= deltaTime;
        }
        if (window->KeyHold(GLFW_KEY_D)) {
            translateX += deltaTime;
        }
        if (window->KeyHold(GLFW_KEY_R)) {
            translateZ -= deltaTime;
        }
        if (window->KeyHold(GLFW_KEY_F)) {
            translateZ += deltaTime;
        }
    }
    // Translating the second cube
    if (window->KeyHold(GLFW_KEY_1)) {
        scaleX -= deltaTime;
        scaleY -= deltaTime;
        scaleZ -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_2)) {
        scaleX += deltaTime;
        scaleY += deltaTime;
        scaleZ += deltaTime;
    }

    // Rotating the third cube
    if (window->KeyHold(GLFW_KEY_3)) {
        angularStepOX += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_4)) {
        angularStepOX -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_5)) {
        angularStepOY += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_6)) {
        angularStepOY -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_7)) {
        angularStepOZ += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_8)) {
        angularStepOX -= deltaTime;
    }

    // BONUS
    // Rotating the torso and the arms
    if (window->KeyHold(GLFW_KEY_U)) {
        angularStepTorso += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_I)) {
        angularStepLeg += deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_O)) {
        angularStepLeg -= deltaTime;
    }
    if (window->KeyHold(GLFW_KEY_P)) {
        if (decrease == 1 && increase == 0)
        {
            translate_X -= deltaTime;
            if (translate_X <= -3.7) {
                increase = 1;
                decrease = 0;
            }
        }
        else
        {
            translate_X += deltaTime;
            if (translate_X >= 0.0) {
                increase = 0;
                decrease = 1;
            }
        }
        cout << translate_X << endl;
    }
}


void Lab4::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_SPACE)
    {
        switch (polygonMode)
        {
        case GL_POINT:
            polygonMode = GL_FILL;
            break;
        case GL_LINE:
            polygonMode = GL_POINT;
            break;
        default:
            polygonMode = GL_LINE;
            break;
        }
    }
}


void Lab4::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab4::OnWindowResize(int width, int height)
{
}
