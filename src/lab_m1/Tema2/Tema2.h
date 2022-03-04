#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/Helpers.h"

namespace m1
{
    #define maze_size 10

    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void RenderMeshModified(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, helpers::Camera* camera, int noise);
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3 color, helpers::Camera* camera, int noise);
        
    protected:
        // variables for the player
        glm::mat4 modelMatrix;
        bool renderCameraTarget;
        glm::mat4 projectionMatrix;

        float player_position_x;
        float player_position_y;
        float player_position_z;
        float angle_to_rotate = 0;

        // variables for the camera
        int first_person = 0; // start by default with third person
        int first_person_aux = 0;

        glm::vec3 target_third_person;
        glm::vec3 position_third_person;

        glm::vec3 target_first_person;
        glm::vec3 position_first_person;

        helpers::Camera* camera_third_person;
        helpers::Camera* camera_first_person;

        // auxilliary variables
        GLenum polygonMode;

        // maze variables;
        int maze[maze_size][maze_size];
        int expanded_maze[maze_size * 4][maze_size * 4];
        int count_zeros = 0;

        // starting point of the player
        int starting_point_x;
        int starting_point_y;

        // projectiles
        float start_projectile_x;
        float start_projectile_y;
        float start_projectile_z;
        float projectile_speed = 0.3;
        std::vector<std::tuple<float, float, float, float>> projectiles;

        // enemies
        std::vector<std::tuple<float, float, float, float>> enemies;
        std::vector<std::tuple<float, float, float>> hit_enemies;
        float enemy_speed = 0.1;

        // this is the way out of the maze
        int firstCell_x, firstCell_y;

    public:
        // auxilliary functions

        // functions for the maze
        void ComputeMaze();
        int countNeighbors(int x, int y);
        bool checkBoundary(int x, int y);
        bool checkCorner(int x, int y);
        bool checkCondition(int x, int y);
        bool checkIfPlayerWon(float x, float y);

        void TransposeMaze();
        void ExpandMaze();

        void UpdateMazeWithEnemies();

        void CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);
        void CreateBox(float x, float y, float z, float length, const char* name, float color1, float color2, float color3, int enemy);
        void CreatePlayer();
        void CreateMaze();
        void CreateGround();
        void CreateProjectile();
        void CreateEnemy();
        void CreateLifeSquare();
        void CreateTimeSquare();

        void CreateShaderProgram();

        void RenderPlayer(int player, float x, float y, float z, helpers::Camera* camera, int noise);
        void RenderMaze(helpers::Camera* camera);
        void RenderGround(helpers::Camera* camera);
        void RenderProjectilesAndEnemies(helpers::Camera* camera);
        void RenderLifeObject(float x, float y, float z, int player, helpers::Camera* camera);
        void RenderTimeObject(float x, float y, float z, int player, helpers::Camera* camera);

        std::vector<std::tuple<int, int>> getNearPoints(float x, float y);
        int checkPlayerCollision(float x, float y);
        int checkCollisionWithEnemy(float x, float z);
        int checkEnemyCollisionWithPlayer(float x, float z); 

        float life = 100.f;
        float time_left = 100.f;

        GLfloat left, right, bottom, top;
        float zFar, zNear;
    };
}   // namespace m1
