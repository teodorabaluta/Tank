#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace m1;


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

    sinaColor = glm::vec3(0.5, 0.5, 0.5);  // Gri
    corpColor = glm::vec3(0.0, 0.5, 0.0);  // Verde inchis
    turetaColor = glm::vec3(0.0, 0.7, 0.0);  // Verde deschis

    enemyColor = glm::vec3(0.5, 0.0, 0.5);

    tank = Tank();  // Instantiate the tank
    camera = new implemented::Camera();

    glm::vec3 position = tank.position - 2.0f * tank.move;
    camera->Set(position, tank.position, glm::vec3(0, 1, 0));

    camera->distanceToTarget = distance(camera->position, tank.position);


  
    {
        Mesh* mesh = new Mesh("proiectil");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "my_models"), "proiectil.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("cladire");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("corp");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "my_models"), "corp.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sina");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "my_models"), "sina.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("tun");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "my_models"), "tun.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("tureta");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "my_models"), "tureta.obj");
        meshes[mesh->GetMeshID()] = mesh;

    }

    {
        Mesh* mesh = new Mesh("ground");
        mesh->LoadMesh(PATH_JOIN(window ->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    Mesh* mesh = new Mesh("sky");
    mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
    meshes[mesh->GetMeshID()] = mesh;

    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
    //projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, zNear, zFar);


    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader *shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // CREARE INAMICI

    for (int i = 0; i < 10; i++) {
        Enemy enemy;
        // Generare aleatorie a pozitiei inamicului
            enemy.position = glm::vec3(static_cast<float>(rand() % 100) - 50.0f, 0.0f, static_cast<float>(rand() % 100) - 50.0f);
            // Generare aleatorie a rotatiei inamicului
            enemy.rotation = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * glm::two_pi<float>();
            // Adauga inamicul in vectorul de inamici
            enemies.push_back(enemy);
    }

    // CREARE CLADIRE
    for (int i = 0; i < 10; i++) {
        Cladire cladire;
        cladire.position = glm::vec3(static_cast<float>(rand() % 100) - 50.0f, 0.0f, static_cast<float>(rand() % 100) - 50.0f);
        cladire.size = glm::vec3(static_cast<float>(rand() % 10) + 2, static_cast<float>(rand() % 10) + 2, static_cast<float>(rand() % 10) + 2);

        cladiri.push_back(cladire);
    }
}


Mesh* Tema2::CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices)
{
    unsigned int VAO = 0;
    // Create the VAO and bind it
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create the VBO and bind it
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Send vertices data into the VBO buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // Create the IBO and bind it
    unsigned int IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // Send indices data into the IBO buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // ========================================================================
    // This section demonstrates how the GPU vertex shader program
    // receives data.

    // TODO(student): If you look closely in the `Init()` and `Update()`
    // functions, you will see that we have three objects which we load
    // and use in three different ways:
    // - LoadMesh   + LabShader (this lab's shader)
    // - CreateMesh + VertexNormal (this shader is already implemented)
    // - CreateMesh + LabShader (this lab's shader)
    // To get an idea about how they're different from one another, do the
    // following experiments. What happens if you switch the color pipe and
    // normal pipe in this function (but not in the shader)? Now, what happens
    // if you do the same thing in the shader (but not in this function)?
    // Finally, what happens if you do the same thing in both places? Why?

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
    // ========================================================================

    // Unbind the VAO
    glBindVertexArray(0);

    // Check for OpenGL errors
    CheckOpenGLError();

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    meshes[name]->vertices = vertices;
    meshes[name]->indices = indices;
    return meshes[name];
}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::RenderScene() 
{


}
void Tema2::RepositionateCamera()
{
    //PutCamera();
    if (put_camera) {
        glm::vec3 position = tank.position - 2.0f * tank.move;
        position.y = 0.75f;
        camera->Set(position, tank.position, glm::vec3(0, 1, 0));
    }


}



void Tema2::Update(float deltaTimeSeconds)
{
    
    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(tank.position.x, tank.position.y, tank.position.z);
    modelMatrix *= transform3D::RotateOY(RADIANS(-90) + tank.rotation);
    modelMatrix *= transform3D::Scale(0.15f, 0.15f, 0.15f);

    RepositionateCamera();


    //RenderMesh(meshes["tun"], shaders["LabShader"], modelMatrix, sinaColor);
    //RenderMesh(meshes["tureta"], shaders["LabShader"], modelMatrix, turetaColor);
    RenderMesh(meshes["sina"], shaders["LabShader"], modelMatrix, sinaColor);
    RenderMesh(meshes["corp"], shaders["LabShader"], modelMatrix, corpColor);

    modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(tank.position.x, tank.position.y, tank.position.z);
    modelMatrix *= transform3D::RotateOY(RADIANS(-90) + tank.rotation + rotation_tureta.x);
    modelMatrix *= transform3D::Scale(0.15f, 0.15f, 0.15f);
    RenderMesh(meshes["tureta"], shaders["LabShader"], modelMatrix, turetaColor);
    RenderMesh(meshes["tun"], shaders["LabShader"], modelMatrix, sinaColor);


    //Shoot();

    for (auto& cladire : cladiri) {
        glm::mat4 cladireModelMatrix = glm::mat4(1);
        cladireModelMatrix *= transform3D::Translate(cladire.position.x, cladire.position.y, cladire.position.z);
        cladireModelMatrix *= transform3D::Scale(cladire.size.x, cladire.size.y, cladire.size.z);
        RenderMesh(meshes["cladire"], shaders["LabShader"], cladireModelMatrix, sinaColor);
    }

    for (auto& enemy : enemies) {
        if (enemy.timing == 0) {
            float randomAngle = rand() % 360;
            enemy.movement = rand() % 4;
            enemy.rotation = randomAngle;
        }

        enemy.timing += deltaTimeSeconds;

        if (enemy.timing < 15) {
            if (enemy.movement == 0) {
                enemy.MoveForward(deltaTimeSeconds);
            }

            if (enemy.movement == 1) {
                enemy.MoveForward(-deltaTimeSeconds);
            }

            if (enemy.movement == 2) {
                enemy.RotateLeft(deltaTimeSeconds);
                //enemy.Move_rotated();
                enemy.MoveForward(deltaTimeSeconds);
            }

            if (enemy.movement == 3) {
                enemy.RotateRight(deltaTimeSeconds);
                //enemy.Move_rotated();
                enemy.MoveForward(deltaTimeSeconds);
            }


            glm::mat4 enemyModelMatrix = glm::mat4(1);
            enemyModelMatrix *= transform3D::Translate(enemy.position.x, enemy.position.y, enemy.position.z);
            enemyModelMatrix *= transform3D::Scale(0.1f, 0.1f, 0.1f);

            RenderMesh(meshes["tun"], shaders["LabShader"], enemyModelMatrix, sinaColor);
            RenderMesh(meshes["tureta"], shaders["LabShader"], enemyModelMatrix, enemyColor);
            RenderMesh(meshes["sina"], shaders["LabShader"], enemyModelMatrix, sinaColor);
            RenderMesh(meshes["corp"], shaders["LabShader"], enemyModelMatrix, enemyColor);
        }
        else {
            enemy.timing = 0;
        }

    }
    if (shoot == 1) {
        Shoot(deltaTimeSeconds);
        //Proiectil proiectil;
       // proiectil.direction = tank.move;
        //shoot = 0;
    }

    if (Engine::GetElapsedTime() >= 120) {
        cout << "GAME OVER";
        exit(0);

    }

}

void Tema2::Shoot(float deltaTimeSeconds) {

    for (auto& proiectil : proiectile) {
        proiectil.position += deltaTimeSeconds * 10.0f * glm::normalize(proiectil.direction);

        glm::mat4 proiectilModelMatrix = glm::mat4(1);
        proiectilModelMatrix *= transform3D::Translate(proiectil.position.x, proiectil.position.y, proiectil.position.z);
        proiectilModelMatrix *= transform3D::Scale(0.2f, 0.2f, 0.2f);
        RenderMesh(meshes["proiectil"], shaders["LabShader"], proiectilModelMatrix, glm::vec3(1, 1, 1));

        // cout << proiectil.position << endl;

    }
}

bool Tema2::CheckProjectileCollision(const Proiectil& proiectil)
{
    // Iterare prin toate tancurile pentru verificarea coliziunii
    for (const auto& enemy : enemies) {
        float distance = glm::length(enemy.position - proiectil.position);
        if (distance < 0.5f) { // Considerm o coliziune dac distanta este mai mica decat o valoare de prag (poate fi ajustata)
            return true; // Proiectilul a lovit un tanc
        }
    }

    // Iterare prin toate cladirile pentru verificarea coliziunii
    for (const auto& cladire : cladiri) {
        float distance = glm::length(cladire.position - proiectil.position);
        if (distance < 1.0f) { 
            return true; 
            cout << "cladire" << endl;
        }
    }

    return false; // Proiectilul nu a lovit niciun obiect
}

bool Tema2::CheckTimeExpiration(const Proiectil& proiectil)
{
    const float projectileLifetime = 2.0f;

    if (Engine::GetElapsedTime() - proiectil.startTime > projectileLifetime) {
        return true;
        cout << "timp" << endl;
    }
    return false;
}

void Tema2::FrameEnd()
{
    glm::mat4 groundModelMatrix = glm::mat4(1);
    RenderMesh(meshes["ground"], shaders["LabShader"], groundModelMatrix, glm::vec3(0.15, 0.1, 0.1));

    glm::mat4 skyModelMatrix = glm::mat4(1);
    skyModelMatrix = glm::scale(skyModelMatrix, glm::vec3(300.0f));
    RenderMesh(meshes["sky"], shaders["LabShader"], skyModelMatrix, glm::vec3(0.3, 0.5, 0.9)); 
    //DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Set color uniform

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);


    // Render the mesh
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema2::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix)
{
//    if (!mesh || !shader || !shader->GetProgramID())
//        return;
//
//    // Render an object using the specified shader and the specified position
//    glUseProgram(shader->program);
//
//    // Set shader uniform "Model" to modelMatrix
//    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
//
//    // Set shader uniform "View" to viewMatrix
//    glm::mat4 viewMatrix = thirdPersonCamera->GetViewMatrix();
//    glUniformMatrix4fv(glGetUniformLocation(shader->program, "View"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
//
//    // Set shader uniform "Projection" to projectionMatrix
//    glUniformMatrix4fv(glGetUniformLocation(shader->program, "Projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
//
//    // Set eye position (camera position) uniform
//    glUniform3fv(glGetUniformLocation(shader->program, "eye_position"), 1, glm::value_ptr(thirdPersonCamera->position));
//
//    // Set time uniform
//    glUniform1f(glGetUniformLocation(shader->program, "Time"), Engine::GetElapsedTime());
//
//    // Draw the object
//    glBindVertexArray(mesh->GetBuffers()->m_VAO);
//    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
        if (window->KeyHold(GLFW_KEY_W)) {
            // inainte
            tank.MoveForward(deltaTime * 5);
            //camera->TranslateForward(deltaTime * 5);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            //inapoi
            tank.MoveBackward(deltaTime * 5);
            //camera->TranslateForward(-deltaTime * 5);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // rotatie la stanga
            tank.RotateLeft(deltaTime * 2);
            //cout << tank.rotation << endl;
            //rotation_tureta.x = tank.rotation;
        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // rotatie la dreapta
            tank.RotateRight(deltaTime * 2);
            //rotation_tureta.x = tank.rotation;
        }

       // cout << tank.position << endl;
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
}


void Tema2::OnKeyRelease(int key, int mods)
{
  
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    float sensivityOX = 0.001f;
    float sensivityOY = 0.001f;

    float sensivity_rot = 0.0055f;

    if (window->MouseHold(GLFW_MOUSE_BUTTON_2))
    {
        put_camera = false;

        camera->distanceToTarget = 2.f;

        if (window->GetSpecialKeyState() == 0) {
            camera->RotateThirdPerson_OX(-deltaY * sensivityOX);
            camera->RotateThirdPerson_OY(-deltaX * sensivityOY);
        }

        rotation_tureta.x = 0;
        rotation_tureta.y = 0;
        rotation_tureta.z = 0;
    } 

    if (!window->MouseHold(GLFW_MOUSE_BUTTON_2)) {
        rotation_tureta.x = mouseX * sensivity_rot;
        rotation_tureta.y = tank.move.y;
        rotation_tureta.z = mouseX * sensivity_rot;
    }

}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    double currentTime = Engine::GetElapsedTime();
    double timeSinceLastShoot = currentTime - lastShootTime;

    if (button == GLFW_MOUSE_BUTTON_2 && timeSinceLastShoot >= 1.0) {
        Proiectil proiectil;
        proiectil.position = glm::vec3(tank.position.x, -0.18f, tank.position.z + 1.65f);
        //rotation_tureta.x = tank.move.x;
        proiectil.direction = glm::normalize(glm::vec3(glm::cos(rotation_tureta.x), 0.0f, -glm::sin(rotation_tureta.x)));
        //proiectil.direction = glm::normalize(glm::vec3(rotation_tureta.x, 0.0f, rotation_tureta.x));

        proiectil.position += 1.25f * glm::normalize(proiectil.direction);
        proiectil.startTime = Engine::GetElapsedTime();
        proiectile.push_back(proiectil);
        shoot = 1;
        cout << rotation_tureta.x;
        lastShootTime = currentTime;
    }
}

//void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
//{
//    if (button == GLFW_MOUSE_BUTTON_2) {
//        Proiectil proiectil;
//        proiectil.position = glm::vec3(tank.position.x, -0.18f, tank.position.z + 1.65f);
//        //proiectil.position = glm::vec3
//        proiectil.direction = tank.move;
//        proiectil.position += 1.25f * glm::normalize(proiectil.direction);
//        proiectile.push_back(proiectil);
//        shoot = 1;
//
//    }
//}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
    if (button == GLFW_MOUSE_BUTTON_3) {
        cout << "hey ";
        put_camera = true;
    }

}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
