#pragma once

#include <vector>

#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera.h"
#include "lab_m1/Tema2/transform3D.h"




namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
        struct Tank {
            glm::vec3 position;         // Pozitia tancului
            float rotation;             // Rotatia tancului
            glm::vec3 move;          // Vector deplasare pe unghiul rotation

            Tank() : position(glm::vec3(0.0f)), rotation(0.0f), move(glm::vec3(1.0f, 0.0f, 0.0f)) {}

            void MoveForward(float deltaTime) {
                // Miscare inainte
                position += move * deltaTime;

            }

            void MoveBackward(float deltaTime) {
                // Miscare inapoi
                position -= move * deltaTime;
            }

            void RotateLeft(float deltaTime) {
                // Rotatie la stanga
                rotation += deltaTime;
                Move_rotated();
            }

            void RotateRight(float deltaTime) {
                // Rotatie la dreapta
                rotation -= deltaTime;
                Move_rotated();
            }

            void Move_rotated() {
                // Update the forward vector based on the rotation angle

                move.x = cos(rotation);
                move.z = -sin(rotation);
                move = glm::normalize(move);
            }
        };

        struct Enemy {
            glm::vec3 position;  // Pozitia tancului inamic
            float rotation;      // Rotatia tancului inamic
            glm::vec3 move;      // Vector deplasare al tancului inamic
            float timing = 0;
            int movement;
            

            Enemy() : position(glm::vec3(0.0f)), rotation(0.0f), move(glm::vec3(1.0f, 0.0f, 1.0f)) {}

            void MoveForward(float deltaTime) {
                // Miscare inainte
                position += move * deltaTime;
            }

            void RotateLeft(float deltaTime) {
                // Rotatie la stanga
                rotation -= deltaTime;
                Move_rotated();
            }

            void RotateRight(float deltaTime) {
                // Rotatie la dreapta
                rotation += deltaTime;
                Move_rotated();
            }

            void Move_rotated() {
                // Update the forward vector based on the rotation angle
                move.x = cos(rotation);
                move.z = sin(rotation);
                move = glm::normalize(move);
            }
        };

        struct Cladire {
            glm::vec3 position; 
            glm::vec3 size;
            //x - lungime
            //y - inaltime
            //z - latime
        };

        struct Proiectil {
            glm::vec3 position;
            glm::vec3 direction;
            float speed;
            float startTime;
            Proiectil() : position(0.0f), direction(0.0f), speed(2.0f), startTime(0.0f) {}
        };

     public:
         Tema2();
        ~Tema2();

        void Init() override;

        Mesh *CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices);

     private:
        Tank tank;
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;
        
        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix);
        void RepositionateCamera();
        void Shoot(float deltaTimeSeconds);
        bool CheckProjectileCollision(const Proiectil& proiectil);
        bool CheckTimeExpiration(const Proiectil& proiectil);

        //void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix);
        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void RenderScene();


    protected:
        implemented::Camera* camera;
        glm::mat4 projectionMatrix;
        GLfloat fov = 80;
        int OK;
        GLfloat right = 15.f, left = -1.f, bottom = -1.f, top = 1.f;
        GLfloat zNear = 0.1f, zFar = 50.0f;

        glm::mat4 modelMatrix;
        float translateX, translateY, translateZ;
        float scaleX, scaleY, scaleZ;
        float angularStepOX, angularStepOY, angularStepOZ;
        float move, scale;
        float sistemX, sistemY, sistemZ;

        glm::vec3 sinaColor;
        glm::vec3 corpColor;
        glm::vec3 turetaColor;
        glm::vec3 enemyColor;

     

        float rotationSpeed = 5.f;
        float angle = 30;
        std::vector<Enemy> enemies;
        std::vector<Cladire> cladiri;
        std::vector<Proiectil> proiectile;
        float shoot = 0;
        float rotationOY = 0;
        float rotationOX = 0;
        int ok = 0;
        bool put_camera = true;
        glm::vec3 rotation_tureta;
        double lastShootTime = 0.0;
    };
}   // namespace m1
