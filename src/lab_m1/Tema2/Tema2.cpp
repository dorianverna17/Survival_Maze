#include "lab_m1/Tema2/Tema2.h"
#include "lab_m1/Tema2/Helpers.h"

#include <vector>
#include <string>
#include <iostream>
#include "utils/glm_utils.h"

using namespace std;
using namespace m1;
using namespace helpers;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    polygonMode = GL_FILL;

    ComputeMaze();
    ExpandMaze();

    CreatePlayer();
    CreateMaze();
    CreateGround();
    CreateProjectile();
    CreateEnemy();

    CreateLifeSquare();
    CreateTimeSquare();

    CreateShaderProgram();

    // set the camera
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

    camera_third_person = new Camera();
    camera_first_person = new Camera();

    target_third_person = glm::vec3(starting_point_x + 0 - 20, 2, starting_point_y + 0.5 - 20);
    position_third_person = glm::vec3(starting_point_x + 0 - 20, 8, starting_point_y + 7 - 20);

    target_first_person = glm::vec3(starting_point_x + 0 - 20, 4.5, starting_point_y - 6 - 20);
    position_first_person = glm::vec3(starting_point_x - 20, 4.5, starting_point_y - 20);

    camera_third_person->Set(position_third_person, target_third_person, glm::vec3(0, 1, 0));
    camera_third_person->distanceToTarget = distance(position_third_person, target_third_person);

    camera_first_person->Set(position_first_person, target_first_person, glm::vec3(0, 1, 0));
    camera_first_person->distanceToTarget = distance(position_first_person, target_first_person);

    zFar = 150.0f;
    zNear = 0.1f;

    right = 6.f;
    left = -2.0f;

    bottom = 0.01f;
    top = 10.0f;

}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::CreateShaderProgram()
{
    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
}


void Tema2::RenderMeshModified(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Camera* camera, int noise)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    if (mesh == meshes["head_enemy"])
        glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(glm::vec3(0.7, 0.2, 0.2)));
    if (mesh == meshes["body_enemy"])
        glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(glm::vec3(1, 0, 0)));
    if (mesh == meshes["leg1_enemy"])
        glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(glm::vec3(0.8, 0.1, 0.1)));
    if (mesh == meshes["leg2_enemy"])
        glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(glm::vec3(0.8, 0.1, 0.1)));
    if (mesh == meshes["hand1_sleeve_enemy"])
        glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(glm::vec3(0.8, 0.1, 0.1)));
    if (mesh == meshes["hand2_sleeve_enemy"])
        glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(glm::vec3(0.8, 0.1, 0.1)));
    if (mesh == meshes["hand1_enemy"])
        glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(glm::vec3(0.7, 0.2, 0.2)));
    if (mesh == meshes["hand2_enemy"])
        glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(glm::vec3(0.7, 0.2, 0.2)));

    GLint noise_aux = glGetUniformLocation(shader->program, "noise");
    glUniform1i(noise_aux, noise);

    glUniform1f(glGetUniformLocation(shader->program, "time"), (GLfloat)Engine::GetElapsedTime());

    mesh->Render();
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3 color, Camera* camera, int noise)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glUniform3fv(glGetUniformLocation(shader->program, "object_color"), 1, glm::value_ptr(color));
    
    GLint noise_aux = glGetUniformLocation(shader->program, "noise");
    glUniform1i(noise_aux, 0);

    glUniform1f(glGetUniformLocation(shader->program, "time"), (GLfloat)Engine::GetElapsedTime());

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema2::Update(float deltaTimeSeconds)
{
    if (life <= 0 || time_left <= 0) {
        cout << "You Lost!" << endl;
        while (true);
    }

    time_left -= 0.2;

    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    player_position_x = camera_third_person->GetTargetPosition().x - 0.5f;
    player_position_y = camera_third_person->GetTargetPosition().y + 2.1;
    player_position_z = camera_third_person->GetTargetPosition().z;

    if (checkIfPlayerWon(player_position_x, player_position_z)) {
        cout << "You Win!" << endl;
        while (true);
    }

    if (first_person == 0)
    {
        RenderPlayer(1, player_position_x, player_position_y, player_position_z, camera_third_person, 0);
        RenderMaze(camera_third_person);
        RenderGround(camera_third_person);
        RenderProjectilesAndEnemies(camera_third_person);
    }
    else
    {
        RenderPlayer(1, player_position_x, player_position_y, player_position_z, camera_first_person, 0);
        RenderMaze(camera_first_person);
        RenderGround(camera_first_person);
        RenderProjectilesAndEnemies(camera_first_person);
    }

    //window->DisablePointer();
}

void Tema2::RenderLifeObject(float x, float y, float z, int player, Camera* camera) {
    if (player == 1) {
        projectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
        modelMatrix = glm::mat4(1);
        if (camera == camera_third_person) {
            modelMatrix *= Translate(x + 5.4f, y + 10.f, z);
            modelMatrix *= Translate(-5.4f, 0, 0);
            modelMatrix *= Translate(0.5, 0, 0);
            modelMatrix *= RotateOY(angle_to_rotate);
            modelMatrix *= Translate(-0.5, 0, 0);
            modelMatrix *= Translate(5.4f, 0, 0);
        }
        else {
            modelMatrix *= Translate(x + 5.4f, y + 8.8f, z - 5.f);
            modelMatrix *= Translate(-5.4f, 0, 5.f);
            modelMatrix *= Translate(0.5, 0, -0);
            modelMatrix *= RotateOY(angle_to_rotate);
            modelMatrix *= Translate(-0.5, 0, 0);
            modelMatrix *= Translate(5.4f, 0, -5.f);
        }
        modelMatrix *= Translate(0.5, 0, 0.5);
        modelMatrix *= RotateOY(M_PI);
        modelMatrix *= Translate(-0.5, 0, -0.5);
        modelMatrix *= Scale(life / 100, 1, 1);
        RenderMeshModified(meshes["actual_life_mesh"], shaders["VertexNormal"], modelMatrix, camera, 0);
        modelMatrix *= Scale(100 / life, 1, 1);
        RenderMeshModified(meshes["life_mesh"], shaders["VertexNormal"], modelMatrix, camera, 0);
        projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
    }
}

void Tema2::RenderTimeObject(float x, float y, float z, int player, Camera* camera) {
    if (player == 1) {
        projectionMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
        modelMatrix = glm::mat4(1);
        if (camera == camera_third_person) {
            modelMatrix *= Translate(x - 1.3f, y + 10.f, z);
            modelMatrix *= Translate(1.3f, 0, 0);
            modelMatrix *= Translate(0.5, 0, 0);
            modelMatrix *= RotateOY(angle_to_rotate);
            modelMatrix *= Translate(-0.5, 0, 0);
            modelMatrix *= Translate(-1.3f, 0, 0);
        }
        else {
            modelMatrix *= Translate(x - 1.3f, y + 8.8f, z - 5.f);
            modelMatrix *= Translate(1.3f, 0, 5.f);
            modelMatrix *= Translate(0.5, 0, -0);
            modelMatrix *= RotateOY(angle_to_rotate);
            modelMatrix *= Translate(-0.5, 0, 0);
            modelMatrix *= Translate(-1.3f, 0, -5.f);
        }
        modelMatrix *= Translate(0.5, 0, 0.5);
        modelMatrix *= RotateOY(M_PI);
        modelMatrix *= Translate(-0.5, 0, -0.5);
        modelMatrix *= Scale(time_left / 100, 1, 1);
        RenderMeshModified(meshes["actual_time_mesh"], shaders["VertexNormal"], modelMatrix, camera, 0);
        modelMatrix *= Scale(100 / time_left, 1, 1);
        RenderMeshModified(meshes["time_mesh"], shaders["VertexNormal"], modelMatrix, camera, 0);
        projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
    }
}

void Tema2::RenderProjectilesAndEnemies(Camera* camera) {
    std::vector<std::tuple<float, float, float, float>> new_projectiles;
    for (int i = 0; i < projectiles.size(); i++) {
        std::tuple<float, float, float, float> aux = projectiles[i];

        if (checkCollisionWithEnemy(get<0>(aux), get<2>(aux))) {
            continue;
        }

        modelMatrix = glm::mat4(1);
        modelMatrix *= Translate(get<0>(aux), get<1>(aux), get<2>(aux));
        modelMatrix *= Translate(0.5f, 0, 0);
        modelMatrix *= Scale(0.5f, 0.5f, 0.5f);
        RenderSimpleMesh(meshes["projectile"], shaders["LabShader"], modelMatrix, glm::vec3(0.7, 0.1, 0.1), camera, 0);

        get<2>(projectiles[i]) -= projectile_speed * cos(get<3>(aux));
        get<0>(projectiles[i]) -= projectile_speed * sin(get<3>(aux));
        
        new_projectiles.push_back(projectiles[i]);
    }
    projectiles = new_projectiles;

    std::vector<std::tuple<float, float, float, float>> new_enemies;
    for (int i = 0; i < enemies.size(); i++) {

        if (checkEnemyCollisionWithPlayer(get<2>(enemies[i]), get<3>(enemies[i]))) {
            life -= (float) 100 / 4;
            continue;
        }

        RenderPlayer(0, get<2>(enemies[i]), player_position_y, get<3>(enemies[i]), camera, 0);
        if (abs(get<2>(enemies[i]) - get<0>(enemies[i])) <= 0.01 || abs(get<2>(enemies[i]) - get<0>(enemies[i]) - 1) <= 0.01) {
            if (abs(get<2>(enemies[i]) - get<0>(enemies[i])) <= 0.01 && get<3>(enemies[i]) - get<1>(enemies[i]) > 0 && get<3>(enemies[i]) - get<1>(enemies[i]) < 1) {
                get<3>(enemies[i]) -= enemy_speed;
                if (abs(get<3>(enemies[i]) - get<1>(enemies[i])) <= 0.01)
                    get<2>(enemies[i]) += enemy_speed;
            }
            else if (abs(get<2>(enemies[i]) - (float)get<0>(enemies[i]) - 1) <= 0.01 && get<3>(enemies[i]) - get<1>(enemies[i]) > 0 && get<3>(enemies[i]) - get<1>(enemies[i]) < 1) {
                get<3>(enemies[i]) += enemy_speed;
                if (abs(get<3>(enemies[i]) - get<1>(enemies[i]) - 1) <= 0.01)
                    get<2>(enemies[i]) -= enemy_speed;
            }
        }
        else if (abs(get<3>(enemies[i]) - get<1>(enemies[i])) <= 0.01 || abs(get<3>(enemies[i]) - get<1>(enemies[i]) - 1) <= 0.01) {
            if (abs(get<3>(enemies[i]) - get<1>(enemies[i])) <= 0.01 && get<2>(enemies[i]) - get<0>(enemies[i]) > 0 && get<2>(enemies[i]) - get<0>(enemies[i]) < 1) {
                get<2>(enemies[i]) += enemy_speed;
                if (abs(get<2>(enemies[i]) - (float)get<0>(enemies[i]) - 1.f) <= 0.01)
                    get<3>(enemies[i]) += enemy_speed;
            }
            else if (abs(get<3>(enemies[i]) - (float)get<1>(enemies[i]) - 1) <= 0.01 && get<2>(enemies[i]) - get<0>(enemies[i]) > 0 && get<2>(enemies[i]) - get<0>(enemies[i]) < 1) {
                get<2>(enemies[i]) -= enemy_speed;
                if (abs(get<2>(enemies[i]) - get<0>(enemies[i])) <= 0.01)
                    get<3>(enemies[i]) -= enemy_speed;
            }
        }
        new_enemies.push_back(enemies[i]);
    }
    enemies = new_enemies;

    //render the hit enemies
    for (int i = 0; i < hit_enemies.size(); i++) {
        if (get<2>(hit_enemies[i]) > 0) {
            RenderPlayer(0, get<0>(hit_enemies[i]), player_position_y, get<1>(hit_enemies[i]), camera, 1);
            get<2>(hit_enemies[i]) -= 0.1;
        }
    }
}


void Tema2::RenderPlayer(int player, float x, float y, float z, Camera* camera, int noise)
{
    // render the head
    modelMatrix = glm::mat4(1);
    modelMatrix *= Translate(x, y, z);
    if (player) {
        modelMatrix *= Translate(0.5, 0, 0);
        modelMatrix *= RotateOY(angle_to_rotate);
        modelMatrix *= Translate(-0.5, 0, -0);
    }
    if (player)
        RenderMeshModified(meshes["head"], shaders["VertexNormal"], modelMatrix, camera, 0);
    else
        RenderMeshModified(meshes["head_enemy"], shaders["LabShader"], modelMatrix, camera, noise);

    // render the body
    modelMatrix = glm::mat4(1);
    modelMatrix *= Translate(x - 0.5f, y - 2.05f, z);
    if (player) {
        modelMatrix *= Translate(1, 0, 0);
        modelMatrix *= RotateOY(angle_to_rotate);
        modelMatrix *= Translate(-1, 0, -0);
    }
    modelMatrix *= Scale(2.0f, 2.0f, 1.0f);
    if (player)
        RenderMeshModified(meshes["body"], shaders["VertexNormal"], modelMatrix, camera, 0);
    else
        RenderMeshModified(meshes["body_enemy"], shaders["LabShader"], modelMatrix, camera, noise);

    // render the left leg
    modelMatrix = glm::mat4(1);
    modelMatrix *= Translate(x - 0.5f, y - 4.1f, z);
    if (player) {
        modelMatrix *= Translate(1, 0, 0);
        modelMatrix *= RotateOY(angle_to_rotate);
        modelMatrix *= Translate(-1, 0, -0);
    }
    modelMatrix *= Scale(0.98f, 2.0f, 1.0f);
    if (player)
        RenderMeshModified(meshes["leg1"], shaders["VertexNormal"], modelMatrix, camera, 0);
    else
        RenderMeshModified(meshes["leg1_enemy"], shaders["LabShader"], modelMatrix, camera, noise);

    // render the right leg
    modelMatrix = glm::mat4(1);
    modelMatrix *= Translate(x + 0.52f, y - 4.1f, z);
    if (player) {
        modelMatrix *= Translate(-0.02, 0, 0);
        modelMatrix *= RotateOY(angle_to_rotate);
        modelMatrix *= Translate(0.02, 0, -0);
    }
    modelMatrix *= Scale(0.98f, 2.0f, 1.0f);
    if (player)
        RenderMeshModified(meshes["leg2"], shaders["VertexNormal"], modelMatrix, camera, 0);
    else
        RenderMeshModified(meshes["leg2_enemy"], shaders["LabShader"], modelMatrix, camera, noise);

    // render the left hand sleeve
    modelMatrix = glm::mat4(1);
    modelMatrix *= Translate(x - 1.05f, y - 1.05f, z);
    if (player) {
        modelMatrix *= Translate(1.55, 0, 0);
        modelMatrix *= RotateOY(angle_to_rotate);
        modelMatrix *= Translate(-1.55, 0, -0);
    }
    modelMatrix *= Scale(0.5f, 1.0f, 1.0f);
    if (player)
        RenderMeshModified(meshes["hand1_sleeve"], shaders["VertexNormal"], modelMatrix, camera, 0);
    else
        RenderMeshModified(meshes["hand1_sleeve_enemy"], shaders["LabShader"], modelMatrix, camera, noise);

    // render the right hand sleeve
    modelMatrix = glm::mat4(1);
    modelMatrix *= Translate(x + 1.55f, y - 1.05f, z);
    if (player) {
        modelMatrix *= Translate(-1.05, 0, 0);
        modelMatrix *= RotateOY(angle_to_rotate);
        modelMatrix *= Translate(1.05, 0, -0);
    }
    modelMatrix *= Scale(0.5f, 1.0f, 1.0f);
    if (player)
        RenderMeshModified(meshes["hand2_sleeve"], shaders["VertexNormal"], modelMatrix, camera, 0);
    else
        RenderMeshModified(meshes["hand2_sleeve_enemy"], shaders["LabShader"], modelMatrix, camera, noise);

    // render the left hand
    modelMatrix = glm::mat4(1);
    modelMatrix *= Translate(x - 1.05f, y - 2.05f, z);
    if (player) {
        modelMatrix *= Translate(1.55, 0, 0);
        modelMatrix *= RotateOY(angle_to_rotate);
        modelMatrix *= Translate(-1.55, 0, -0);
    }
    modelMatrix *= Scale(0.5f, 0.95f, 1.0f);
    if (player)
        RenderMeshModified(meshes["hand1"], shaders["VertexNormal"], modelMatrix, camera, 0);
    else
        RenderMeshModified(meshes["hand1_enemy"], shaders["LabShader"], modelMatrix, camera, noise);

    // render the right hand
    modelMatrix = glm::mat4(1);
    modelMatrix *= Translate(x + 1.55f, y - 2.05f, z);
    if (player) {
        modelMatrix *= Translate(-1.05, 0, 0);
        modelMatrix *= RotateOY(angle_to_rotate);
        modelMatrix *= Translate(1.05, 0, -0);
    }
    modelMatrix *= Scale(0.5f, 0.95f, 1.0f);
    if (player)
        RenderMeshModified(meshes["hand2"], shaders["VertexNormal"], modelMatrix, camera, 0);
    else
        RenderMeshModified(meshes["hand2_enemy"], shaders["LabShader"], modelMatrix, camera, noise);

    if (player == 1) {
        // setting the starting point for the projectiles
        start_projectile_x = x;
        start_projectile_y = y;
        start_projectile_z = z;
    }

    // render life and time objecs
    RenderLifeObject(x, y, z, player, camera);
    RenderTimeObject(x, y, z, player, camera);
}

void Tema2::RenderMaze(Camera* camera)
{
    for (int i = 0; i < maze_size * 4; i++) {
        for (int j = 0; j < maze_size * 4; j++) {
            if (expanded_maze[i][j] == 1) {
                // render the wall
                modelMatrix = glm::mat4(1);
                modelMatrix *= Translate(i - 20, 0, j - 20);
                modelMatrix *= Scale(1, 5, 1);
                RenderMeshModified(meshes["maze_box"], shaders["VertexNormal"], modelMatrix, camera, 0);
            }
        }
    }
}


void Tema2::RenderGround(Camera* camera)
{
    // Render ground
    {
        modelMatrix = glm::mat4(1);
        modelMatrix *= Translate(0, 0, 0);
        modelMatrix *= Scale(1, 1, 1);
        RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, glm::vec3(0.4, 0.4, 0.1), camera, 0);
    }
}


void Tema2::FrameEnd()
{
    // DrawCoordinateSystem();
    // DrawCoordinateSystem(camera_third_person->GetViewMatrix(), projectionMatrix);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float cameraSpeed = 5.0f;

    if (window->KeyHold(GLFW_KEY_W)) {
        glm::vec3 dir = glm::normalize(glm::vec3(camera_third_person->forward.x, 0, camera_third_person->forward.z));
        glm::vec3 position_aux = camera_third_person->position + dir * deltaTime * cameraSpeed;

        float position_x = (position_aux + camera_third_person->forward * camera_third_person->distanceToTarget).x;
        float position_z = (position_aux + camera_third_person->forward * camera_third_person->distanceToTarget).z;

        if (checkPlayerCollision(position_x, position_z) == 0) {
            camera_third_person->MoveForward(deltaTime * cameraSpeed);
            camera_first_person->TranslateForward(deltaTime * cameraSpeed);
        }
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        glm::vec3 position_aux = camera_third_person->position + glm::normalize(camera_third_person->right) * (-deltaTime * cameraSpeed);

        float position_x = (position_aux + camera_third_person->forward * camera_third_person->distanceToTarget).x;
        float position_z = (position_aux + camera_third_person->forward * camera_third_person->distanceToTarget).z;
        
        if (checkPlayerCollision(position_x, position_z) == 0) {
            camera_third_person->TranslateRight(-deltaTime * cameraSpeed);
            camera_first_person->TranslateRight(-deltaTime * cameraSpeed);
        }
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        glm::vec3 dir = glm::normalize(glm::vec3(camera_third_person->forward.x, 0, camera_third_person->forward.z));
        glm::vec3 position_aux = camera_third_person->position + dir * (-deltaTime * cameraSpeed);
        
        float position_x = (position_aux + camera_third_person->forward * camera_third_person->distanceToTarget).x;
        float position_z = (position_aux + camera_third_person->forward * camera_third_person->distanceToTarget).z;

        if (checkPlayerCollision(position_x, position_z) == 0) {
            camera_third_person->MoveForward(-deltaTime * cameraSpeed);
            camera_first_person->TranslateForward(-deltaTime * cameraSpeed);
        }
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        glm::vec3 position_aux = camera_third_person->position + glm::normalize(camera_third_person->right) * (deltaTime * cameraSpeed);

        float position_x = (position_aux + camera_third_person->forward * camera_third_person->distanceToTarget).x;
        float position_z = (position_aux + camera_third_person->forward * camera_third_person->distanceToTarget).z;

        if (checkPlayerCollision(position_x, position_z) == 0) {
            camera_third_person->TranslateRight(deltaTime * cameraSpeed);
            camera_first_person->TranslateRight(deltaTime * cameraSpeed);
        }
    }

    // cycle through cameras
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) || window->KeyHold(GLFW_KEY_LEFT_CONTROL))
    {
        // first person mode
        first_person = 1;
    }
    else
    {
        // third person mode
        first_person = 0;
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_SPACE && first_person == 1)
    {
        projectiles.push_back(make_tuple(start_projectile_x, start_projectile_y, start_projectile_z, angle_to_rotate));
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
    float sensivityOY = 0.01f;
    // compute the angle
    angle_to_rotate += -sensivityOY * deltaX;
    camera_third_person->RotateThirdPerson_OY(-sensivityOY * deltaX);
    camera_first_person->MoveForward(-0.5);
    camera_first_person->RotateFirstPerson_OY(-sensivityOY * deltaX);
    camera_first_person->MoveForward(0.5);
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (button == GLFW_MOUSE_BUTTON_2 && first_person == 1)
    {
        projectiles.push_back(make_tuple(start_projectile_x, start_projectile_y, start_projectile_z, angle_to_rotate));
    }
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}

void Tema2::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int IBO = 0;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
    glBindVertexArray(0);
    if (GetOpenGLError() == GL_INVALID_OPERATION)
    {
        cout << "\t[NOTE] : For students : DON'T PANIC! This error should go away when completing the tasks." << std::endl;
        cout << "\t[NOTE] : For developers : This happens because OpenGL core spec >=3.1 forbids null VAOs." << std::endl;
    }

    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
}

void Tema2::CreateBox(float x, float y, float z, float length, const char* name, float color1, float color2, float color3, int enemy)
{
    vector<VertexFormat> vertices_bonus;
    if (enemy == 1) {
        vertices_bonus =
        {
            // sub
            VertexFormat(glm::vec3(x, y, z + length), glm::vec3(0, 0, 1), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x, y, z + length), glm::vec3(0, 1, 1), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x, y, z + length), glm::vec3(0, 1, 0), glm::vec3(color1, color2, color3)),

            VertexFormat(glm::vec3(x + length, y, z + length), glm::vec3(0, 0, 1), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x + length, y, z + length), glm::vec3(0, 1, 1), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x + length, y, z + length), glm::vec3(1, 0, 1), glm::vec3(color1, color2, color3)),

            VertexFormat(glm::vec3(x + length, y, z), glm::vec3(0, 0, 1), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x + length, y, z), glm::vec3(1, 0, 0), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x + length, y, z), glm::vec3(1, 0, 1), glm::vec3(color1, color2, color3)),

            VertexFormat(glm::vec3(x, y, z), glm::vec3(0, 0, 1), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x, y, z), glm::vec3(1, 0, 0), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x, y, z), glm::vec3(0, 1, 0), glm::vec3(color1, color2, color3)),

            // over
            VertexFormat(glm::vec3(x, y + length, z + length), glm::vec3(1, 1, 0), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x, y + length, z + length), glm::vec3(0, 1, 0), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x, y + length, z + length), glm::vec3(0, 1, 1), glm::vec3(color1, color2, color3)),

            VertexFormat(glm::vec3(x + length, y + length, z + length), glm::vec3(1, 1, 0), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x + length, y + length, z + length), glm::vec3(0, 1, 1), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x + length, y + length, z + length), glm::vec3(1, 0, 1), glm::vec3(color1, color2, color3)),

            VertexFormat(glm::vec3(x + length, y + length, z), glm::vec3(1, 1, 0), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x + length, y + length, z), glm::vec3(1, 0, 1), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x + length, y + length, z), glm::vec3(1, 0, 0), glm::vec3(color1, color2, color3)),

            VertexFormat(glm::vec3(x, y + length, z), glm::vec3(1, 1, 0), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x, y + length, z), glm::vec3(0, 1, 0), glm::vec3(color1, color2, color3)),
            VertexFormat(glm::vec3(x, y + length, z), glm::vec3(1, 0, 0), glm::vec3(color1, color2, color3)),
        };
    }
    else {
        vertices_bonus =
        {
            // sub
            VertexFormat(glm::vec3(x, y, z + length), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(x, y, z + length), glm::vec3(0, 1, 1)),
            VertexFormat(glm::vec3(x, y, z + length), glm::vec3(0, 1, 0)),

            VertexFormat(glm::vec3(x + length, y, z + length), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(x + length, y, z + length), glm::vec3(0, 1, 1)),
            VertexFormat(glm::vec3(x + length, y, z + length), glm::vec3(1, 0, 1)),

            VertexFormat(glm::vec3(x + length, y, z), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(x + length, y, z), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(x + length, y, z), glm::vec3(1, 0, 1)),

            VertexFormat(glm::vec3(x, y, z), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(x, y, z), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(x, y, z), glm::vec3(0, 1, 0)),

            // over
            VertexFormat(glm::vec3(x, y + length, z + length), glm::vec3(1, 1, 0)),
            VertexFormat(glm::vec3(x, y + length, z + length), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3(x, y + length, z + length), glm::vec3(0, 1, 1)),

            VertexFormat(glm::vec3(x + length, y + length, z + length), glm::vec3(1, 1, 0)),
            VertexFormat(glm::vec3(x + length, y + length, z + length), glm::vec3(0, 1, 1)),
            VertexFormat(glm::vec3(x + length, y + length, z + length), glm::vec3(1, 0, 1)),

            VertexFormat(glm::vec3(x + length, y + length, z), glm::vec3(1, 1, 0)),
            VertexFormat(glm::vec3(x + length, y + length, z), glm::vec3(1, 0, 1)),
            VertexFormat(glm::vec3(x + length, y + length, z), glm::vec3(1, 0, 0)),

            VertexFormat(glm::vec3(x, y + length, z), glm::vec3(1, 1, 0)),
            VertexFormat(glm::vec3(x, y + length, z), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3(x, y + length, z), glm::vec3(1, 0, 0)),
        };
    }

    vector<unsigned int> indices_bonus =
    {
        6, 3, 0,
        0, 9, 6,
        5, 8, 17,
        8, 19, 17,
        7, 23, 20,
        7, 10, 23,
        1, 4, 16,
        1, 16, 14,
        2, 13, 11,
        11, 13, 22,
        12, 15, 18,
        12, 18, 21,
    };

    CreateMesh(name, vertices_bonus, indices_bonus);
}

void Tema2::CreatePlayer()
{
    CreateBox(0, 0, 0, 1, "leg1", 0.19, 0.45, 0.92, 1);
    CreateBox(0, 0, 0, 1, "leg2", 0.19, 0.45, 0.92, 1);
    CreateBox(0, 0, 0, 1, "body", 0.13, 0.54, 0.13, 1);
    CreateBox(0, 0, 0, 1, "hand1_sleeve", 0.13, 0.54, 0.13, 1);
    CreateBox(0, 0, 0, 1, "hand2_sleeve", 0.13, 0.54, 0.13, 1);
    CreateBox(0, 0, 0, 1, "hand1", 0.87, 0.67, 0.41, 1);
    CreateBox(0, 0, 0, 1, "hand2", 0.87, 0.67, 0.41, 1);
    CreateBox(0, 0, 0, 1, "head", 0.87, 0.67, 0.41, 1);
}

void Tema2::CreateMaze()
{
    CreateBox(0, 0, 0, 1, "maze_box", 0.1, 0.1, 0.41, 1);
}

void Tema2::CreateProjectile()
{
    {
        Mesh* mesh = new Mesh("projectile");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
}

void Tema2::CreateEnemy()
{
    CreateBox(0, 0, 0, 1, "leg1_enemy", 0.8, 0.1, 0.1, 0);
    CreateBox(0, 0, 0, 1, "leg2_enemy", 0.8, 0.1, 0.1, 0);
    CreateBox(0, 0, 0, 1, "body_enemy", 1, 0, 0, 0);
    CreateBox(0, 0, 0, 1, "hand1_sleeve_enemy", 0.8, 0.1, 0.1, 0);
    CreateBox(0, 0, 0, 1, "hand2_sleeve_enemy", 0.8, 0.1, 0.1, 0);
    CreateBox(0, 0, 0, 1, "hand1_enemy", 0.7, 0.2, 0.2, 0);
    CreateBox(0, 0, 0, 1, "hand2_enemy", 0.7, 0.2, 0.2, 0);
    CreateBox(0, 0, 0, 1, "head_enemy", 0.7, 0.2, 0.2, 0);
}

void Tema2::CreateGround()
{
    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
}

void Tema2::CreateLifeSquare()
{
    CreateBox(0, 0, 0, 1, "life_mesh", 0, 0.7, 0.7, 1);
    CreateBox(0, 0, 0, 1, "actual_life_mesh", 0, 1, 1, 1);
}

void Tema2::CreateTimeSquare()
{
    CreateBox(0, 0, 0, 1, "time_mesh", 1, 1, 0, 1);
    CreateBox(0, 0, 0, 1, "actual_time_mesh", 0.7, 0.7, 0, 1);
}

bool Tema2::checkBoundary(int x, int y) {
    if (x == 0 || y == 0 || x == maze_size - 1 || y == maze_size - 1)
        return true;
    return false;
}

bool Tema2::checkCorner(int x, int y) {
    if (x == 0 && y == 0)
        return true;
    if (x == 0 && y == maze_size - 1)
        return true;
    if (x == maze_size - 1 && y == 0)
        return true;
    if (x == maze_size - 1 && y == maze_size - 1)
        return true;
    return false;
}

bool Tema2::checkCondition(int x, int y) {
    if (maze[x - 1][y] == 0 && maze[x - 1][y - 1] == 0
        && maze[x][y - 1] == 0)
        return true;
    if (maze[x - 1][y] == 0 && maze[x - 1][y + 1] == 0
        && maze[x][y + 1] == 0)
        return true;
    if (maze[x + 1][y] == 0 && maze[x + 1][y - 1] == 0
        && maze[x][y - 1] == 0)
        return true;
    if (maze[x + 1][y] == 0 && maze[x + 1][y + 1] == 0
        && maze[x][y + 1] == 0)
        return true;
    return false;
}

int Tema2::countNeighbors(int x, int y) {
    int count = 0;
    if (maze[x - 1][y] == 0)
        count++;
    if (maze[x + 1][y] == 0)
        count++;
    if (maze[x][y - 1] == 0)
        count++;
    if (maze[x][y + 1] == 0)
        count++;
    if (maze[x - 1][y - 1] == 0)
        count++;
    if (maze[x + 1][y + 1] == 0)
        count++;
    if (maze[x + 1][y - 1] == 0)
        count++;
    if (maze[x - 1][y + 1] == 0)
        count++;
    return count;
}


float computeDistance(float x, float y, float start_x, float start_y) {
    return sqrt((x - start_x) * (x - start_x) + (y - start_y) * (y - start_y));
}


void Tema2::ComputeMaze() {
    for (int i = 0; i < maze_size; i++) {
        for (int j = 0; j < maze_size; j++) {
            maze[i][j] = 1;
        }
    }

    srand(time(NULL));

    // generate a starting cell from 0 to 9
    int starting_cell_column_line;
    
    do {
        starting_cell_column_line = rand() % 2;
        if (starting_cell_column_line)
        {
            firstCell_x = rand() % 2 * (maze_size - 1);
            firstCell_y = rand() % maze_size;
        }
        else
        {
            firstCell_x = rand() % maze_size;
            firstCell_y = rand() % 2 * (maze_size - 1);
        }
    } while (checkCorner(firstCell_x, firstCell_y));

    std::tuple<int, int> aux;
    int direction, new_x, new_y, max = 0, max_x, max_y;
    vector<tuple<int, int>> positions;

    maze[firstCell_x][firstCell_y] = 0;

    // first line
    if (firstCell_x == 0) {
        new_x = firstCell_x + 1;
        new_y = firstCell_y;
    }
    else if (firstCell_x == maze_size - 1) {
        new_x = firstCell_x - 1;
        new_y = firstCell_y;
    }
    else if (firstCell_y == 0) {
        new_x = firstCell_x;
        new_y = firstCell_y + 1;
    }
    else if (firstCell_y == maze_size - 1) {
        new_x = firstCell_x;
        new_y = firstCell_y - 1;
    }

    max = 1;
    max_x = new_x;
    max_y = new_y;
    
    positions.push_back(make_tuple(new_x, new_y));
    maze[new_x][new_y] = 0;

    while (positions.size() != 0) {
        aux = positions[positions.size() - 1];
        positions.pop_back();
        
        for (direction = 0; direction < 4; direction++) {
            if (direction == 0) {
                new_x = get<0>(aux) - 1;
                new_y = get<1>(aux);
            }
            else if (direction == 1) {
                new_x = get<0>(aux) + 1;
                new_y = get<1>(aux);
            }
            else if (direction == 2) {
                new_x = get<0>(aux);
                new_y = get<1>(aux) + 1;
            }
            else {
                new_x = get<0>(aux);
                new_y = get<1>(aux) - 1;
            }

            if (rand() % 9 == 1)
                continue;

            if (maze[new_x][new_y] == 0)
                continue;
            if (checkBoundary(new_x, new_y))
                continue;
            if (checkCondition(new_x, new_y))
                continue;
            if (countNeighbors(new_x, new_y) > 3 ||
                countNeighbors(new_x + 1, new_y) > 3 ||
                countNeighbors(new_x, new_y + 1) > 3 ||
                countNeighbors(new_x, new_y - 1) > 3 ||
                countNeighbors(new_x - 1, new_y) > 3)
                continue;
            positions.insert(positions.begin(), make_tuple(new_x, new_y));
            maze[new_x][new_y] = 0;
            count_zeros++;

            float d = computeDistance(new_x, new_y, firstCell_x, firstCell_y);
            if (d > max) {
                max = d;
                max_x = new_x;
                max_y = new_y;
            }
        }
    }

    // here is the player at the start
    maze[max_x][max_y] = 3;
    starting_point_x = max_x;
    starting_point_y = max_y;

    UpdateMazeWithEnemies();
}

void Tema2::UpdateMazeWithEnemies() {
    int count_aux = 0;

    for (int i = 0; i < maze_size; i++) {
        for (int j = 0; j < maze_size; j++) {
            if (maze[i][j] == 0) {
                count_aux++;
            }
            if (count_aux == 3 &&
                (maze[i - 1][j] != 2 &&
                maze[i + 1][j] != 2 &&
                maze[i][j - 1] != 2 &&
                maze[i][j + 1] != 2 &&
                maze[i - 1][j - 1] != 2 &&
                maze[i + 1][j - 1] != 2 &&
                maze[i - 1][j + 1] != 2 &&
                maze[i + 1][j + 1] != 2) &&
                maze[i][j] == 0) {
                
                count_aux = 0;
                // 2 is for the enemy
                maze[i][j] = 2;
                enemies.push_back(make_tuple(i * 4.f - 19.f, j * 4.f - 19.f, i * 4.f - 18.9f, j * 4.f - 19.f));
            }
            if (count_aux == 3)
                count_aux = 0;
        }
    }
}

void Tema2::ExpandMaze() {
    for (int i = 0; i < maze_size; i++) {
        for (int j = 0; j < maze_size; j++) {
            expanded_maze[i * 4 + 1][j * 4 + 1] = maze[i][j];
            if (maze[i][j] != 3 && maze[i][j] != 2) {
                expanded_maze[i * 4][j * 4] = maze[i][j];
                expanded_maze[i * 4][j * 4 + 1] = maze[i][j];
                expanded_maze[i * 4 + 2][j * 4 + 1] = maze[i][j];
                expanded_maze[i * 4 + 3][j * 4 + 1] = maze[i][j];
                expanded_maze[i * 4][j * 4 + 2] = maze[i][j];
                expanded_maze[i * 4 + 1][j * 4 + 2] = maze[i][j];\
                expanded_maze[i * 4 + 3][j * 4 + 2] = maze[i][j];
                expanded_maze[i * 4][j * 4 + 3] = maze[i][j];
                expanded_maze[i * 4 + 1][j * 4 + 3] = maze[i][j];
                expanded_maze[i * 4 + 2][j * 4 + 3] = maze[i][j];
                expanded_maze[i * 4 + 3][j * 4 + 3] = maze[i][j];
                expanded_maze[i * 4 + 1][j * 4] = maze[i][j];
                expanded_maze[i * 4 + 2][j * 4] = maze[i][j];
                expanded_maze[i * 4 + 3][j * 4] = maze[i][j];
                expanded_maze[i * 4 + 2][j * 4 + 2] = maze[i][j];
                expanded_maze[i * 4 + 3][j * 4 + 3] = maze[i][j];
            }
            else {
                expanded_maze[i * 4][j * 4] = 0;
                expanded_maze[i * 4][j * 4 + 1] = 0;
                expanded_maze[i * 4 + 2][j * 4 + 1] = 0;
                expanded_maze[i * 4 + 3][j * 4 + 1] = 0;
                expanded_maze[i * 4][j * 4 + 2] = 0;
                expanded_maze[i * 4 + 1][j * 4 + 2] = 0;
                expanded_maze[i * 4 + 3][j * 4 + 2] = 0;
                expanded_maze[i * 4][j * 4 + 3] = 0;
                expanded_maze[i * 4 + 1][j * 4 + 3] = 0;
                expanded_maze[i * 4 + 2][j * 4 + 3] = 0;
                expanded_maze[i * 4 + 3][j * 4 + 3] = 0;
                expanded_maze[i * 4 + 1][j * 4] = 0;
                expanded_maze[i * 4 + 2][j * 4] = 0;
                expanded_maze[i * 4 + 3][j * 4] = 0;
                expanded_maze[i * 4 + 2][j * 4 + 2] = 0;
                expanded_maze[i * 4 + 3][j * 4 + 3] = 0;
                if (maze[i][j] == 3) {
                    starting_point_x = i * 4 + 2;
                    starting_point_y = j * 4 + 2;
                }
            }
        }
    }

    for (int i = 0; i < maze_size * 4; i++) {
        for (int j = 0; j < maze_size * 4; j++) {
            cout << expanded_maze[i][j] << " ";
        }
        cout << endl;
    }
}

void Tema2::TransposeMaze() {
    int maze_transposed[maze_size][maze_size];

    for (int i = 0; i < maze_size; i++) {
        for (int j = 0; j < maze_size; j++) {
            maze_transposed[j][i] = maze[i][j];
        }
    }

    for (int i = 0; i < maze_size; i++) {
        for (int j = 0; j < maze_size; j++) {
            maze[i][j] = maze_transposed[i][j];
        }
    }
}

std::vector<std::tuple<int, int>> Tema2::getNearPoints(float x, float y) {
    std::vector<std::tuple<int, int>> res;
    int aux_x = x + 20;
    int aux_y = y + 20;

    // distance 1 cubes
    if (aux_x - 1 >= 0) {
        if (expanded_maze[aux_x - 1][aux_y] == 1)
            res.push_back(make_tuple(aux_x - 1, aux_y));
        if (aux_y - 1 >= 0)
            if (expanded_maze[aux_x - 1][aux_y - 1] == 1)
                res.push_back(make_tuple(aux_x - 1, aux_y - 1));
        if (aux_y + 1 < 40)
            if (expanded_maze[aux_x - 1][aux_y + 1] == 1)
                res.push_back(make_tuple(aux_x - 1, aux_y + 1));
    }
    
    if (aux_y - 1 >= 0)
        if (expanded_maze[aux_x][aux_y - 1] == 1)
            res.push_back(make_tuple(aux_x, aux_y - 1));
    if (aux_y + 1 < 40)
        if (expanded_maze[aux_x][aux_y + 1] == 1)
            res.push_back(make_tuple(aux_x, aux_y + 1));

    if (aux_x + 1 < 40) {
        if (expanded_maze[aux_x + 1][aux_y] == 1)
            res.push_back(make_tuple(aux_x + 1, aux_y));
        if (aux_y - 1 >= 0)
            if (expanded_maze[aux_x + 1][aux_y - 1] == 1)
                res.push_back(make_tuple(aux_x + 1, aux_y - 1));
        if (aux_y + 1 < 40)
            if (expanded_maze[aux_x + 1][aux_y + 1] == 1)
                res.push_back(make_tuple(aux_x + 1, aux_y + 1));
    }

    // distance 2 cubes
    if (aux_x - 2 >= 0) {
        if (expanded_maze[aux_x - 2][aux_y] == 1)
            res.push_back(make_tuple(aux_x - 2, aux_y));
        if (aux_y - 1 >= 0)
            if (expanded_maze[aux_x - 2][aux_y - 1] == 1)
                res.push_back(make_tuple(aux_x - 2, aux_y - 1));
        if (aux_y - 2 >= 0)
            if (expanded_maze[aux_x - 2][aux_y - 2] == 1)
                res.push_back(make_tuple(aux_x - 2, aux_y - 2));
        if (aux_y + 1 < 40)
            if (expanded_maze[aux_x - 2][aux_y + 1] == 1)
                res.push_back(make_tuple(aux_x - 2, aux_y + 1));
        if (aux_y + 2 < 40)
            if (expanded_maze[aux_x - 2][aux_y + 2] == 1)
                res.push_back(make_tuple(aux_x - 2, aux_y + 2));
    }

    if (aux_y - 2 >= 0) {
        if (expanded_maze[aux_x][aux_y - 2] == 1)
            res.push_back(make_tuple(aux_x, aux_y - 2));
        if (aux_x - 1 >= 0)
            if (expanded_maze[aux_x - 1][aux_y - 2] == 1)
                res.push_back(make_tuple(aux_x - 1, aux_y - 2));
        if (aux_x + 1 < 40)
            if (expanded_maze[aux_x + 1][aux_y - 2] == 1)
                res.push_back(make_tuple(aux_x + 1, aux_y - 2));
    }
    
    if (aux_y + 2 < 40) {
        if (expanded_maze[aux_x][aux_y + 2] == 1)
            res.push_back(make_tuple(aux_x, aux_y + 2));
        if (aux_x - 1 >= 0)
            if (expanded_maze[aux_x - 1][aux_y + 2] == 1)
                res.push_back(make_tuple(aux_x - 1, aux_y + 2));
        if (aux_x + 1 < 40)
            if (expanded_maze[aux_x + 1][aux_y + 2] == 1)
                res.push_back(make_tuple(aux_x + 1, aux_y + 2));
    }

    if (aux_x + 2 < 40) {
        if (expanded_maze[aux_x + 2][aux_y] == 1)
            res.push_back(make_tuple(aux_x + 2, aux_y));
        if (aux_y - 1 >= 0)
            if (expanded_maze[aux_x + 2][aux_y - 1] == 1)
                res.push_back(make_tuple(aux_x + 2, aux_y - 1));
        if (aux_y - 2 >= 0)
            if (expanded_maze[aux_x + 2][aux_y - 2] == 1)
                res.push_back(make_tuple(aux_x + 2, aux_y - 2));
        if (aux_y + 1 < 40)
            if (expanded_maze[aux_x + 2][aux_y + 1] == 1)
                res.push_back(make_tuple(aux_x + 2, aux_y + 1));
        if (aux_y + 2 < 40)
            if (expanded_maze[aux_x + 2][aux_y + 2] == 1)
                res.push_back(make_tuple(aux_x + 2, aux_y + 2));
    }

    return res;
}

int Tema2::checkPlayerCollision(float x, float z) {
    std::vector<std::tuple<int, int>> points = getNearPoints(x, z);
    float aux_x, aux_z;

    for (int i = 0; i < maze_size * 4; i++) {
        for (int j = 0; j < maze_size * 4; j++) {
            if (expanded_maze[i][j] == 1)
                if (computeDistance(x, z, i - 20 + 0.5, j - 20 + 0.5) <= 1.5) {
                    return 1;
                }
        }
    }

    return 0;
}

int Tema2::checkCollisionWithEnemy(float x, float z) {
    int aux_x, aux_z;
    int res = enemies.size();

    std::vector<std::tuple<float, float, float, float>> new_enemies;
    for (int i = 0; i < enemies.size(); i++) {
        aux_x = get<2>(enemies[i]);
        aux_z = get<3>(enemies[i]);

        if (computeDistance(x, z, aux_x, aux_z) <= 1.5) {
            hit_enemies.push_back(make_tuple(aux_x, aux_z, 2));
            continue;
        }
        else {
            new_enemies.push_back(enemies[i]);
        }
    }
    enemies = new_enemies;

    if (res != enemies.size())
        return 1;
    return 0;
}


int Tema2::checkEnemyCollisionWithPlayer(float x, float z) {
    float aux_x = camera_third_person->GetTargetPosition().x;
    float aux_z = camera_third_person->GetTargetPosition().z;

    if (computeDistance(aux_x, aux_z, x, z) <= 1) {
        return 1;
    }
    return 0;
}

bool Tema2::checkIfPlayerWon(float x, float y) {
    if (computeDistance(x, y, firstCell_x * 4 - 20 + 1, firstCell_y * 4 - 20 + 1) <= 1)
        return true;
    return false;
}

