#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include "Canis/Canis.hpp"
#include "Canis/Entity.hpp"
#include "Canis/Graphics.hpp"
#include "Canis/Window.hpp"
#include "Canis/Shader.hpp"
#include "Canis/Debug.hpp"
#include "Canis/IOManager.hpp"
#include "Canis/InputManager.hpp"
#include "Canis/Camera.hpp"
#include "Canis/Model.hpp"
#include "Canis/World.hpp"
#include "Canis/Editor.hpp"
#include "Canis/FrameRateManager.hpp"

using namespace glm;

// git restore .
// git fetch
// git pull

// 3d array
std::vector<std::vector<std::vector<unsigned int>>> map = {};

// declaring functions
void SpawnLights(Canis::World &_world);
void LoadMap(std::string _path);
void Rotate(Canis::World &_world, Canis::Entity &_entity, float _deltaTime);
void LoadFireTextures();
void AnimateFire(Canis::World &_world, Canis::Entity &_entity, float _deltaTime);

//Create Fire
std::vector<Canis::GLTexture> fireTextures;

#ifdef _WIN32
#define main SDL_main
extern "C" int main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    Canis::Init();
    Canis::InputManager inputManager;
    Canis::FrameRateManager frameRateManager;
    frameRateManager.Init(60);

    /// SETUP WINDOW
    Canis::Window window;
    window.MouseLock(true);

    unsigned int flags = 0;

    if (Canis::GetConfig().fullscreen)
        flags |= Canis::WindowFlags::FULLSCREEN;

    window.Create("Hello Graphics", Canis::GetConfig().width, Canis::GetConfig().heigth, flags);
    /// END OF WINDOW SETUP

    Canis::World world(&window, &inputManager, "assets/textures/lowpoly-skybox/");
    SpawnLights(world);

    Canis::Editor editor(&window, &world, &inputManager);

    Canis::Graphics::EnableAlphaChannel();
    Canis::Graphics::EnableDepthTest();

    /// SETUP SHADER
    Canis::Shader shader;
    shader.Compile("assets/shaders/hello_shader.vs", "assets/shaders/hello_shader.fs");
    shader.AddAttribute("aPosition");
    shader.Link();
    shader.Use();
    shader.SetInt("MATERIAL.diffuse", 0);
    shader.SetInt("MATERIAL.specular", 1);
    shader.SetFloat("MATERIAL.shininess", 64);
    shader.SetBool("WIND", false);
    shader.UnUse();

    Canis::Shader grassShader;
    grassShader.Compile("assets/shaders/hello_shader.vs", "assets/shaders/hello_shader.fs");
    grassShader.AddAttribute("aPosition");
    grassShader.Link();
    grassShader.Use();
    grassShader.SetInt("MATERIAL.diffuse", 0);
    grassShader.SetInt("MATERIAL.specular", 1);
    grassShader.SetFloat("MATERIAL.shininess", 64);
    grassShader.SetBool("WIND", true);
    grassShader.SetFloat("WINDEFFECT", 0.1);
    grassShader.UnUse();
    /// END OF SHADER

    /// Load Image
    Canis::GLTexture dirtTexture = Canis::LoadImageGL("assets/textures/grass_block_top.png", true);
    Canis::GLTexture stoneTexture = Canis::LoadImageGL("assets/textures/cobblestone.png", true);
    Canis::GLTexture woodTexture = Canis::LoadImageGL("assets/textures/oak_log.png", true);
    Canis::GLTexture plankTexture = Canis::LoadImageGL("assets/textures/oak_planks.png", true);
    Canis::GLTexture glassTexture = Canis::LoadImageGL("assets/textures/glass.png", true);
    Canis::GLTexture leafTexture = Canis::LoadImageGL("assets/textures/leaf.png", true);
    Canis::GLTexture actualdirtTexture = Canis::LoadImageGL("assets/textures/dirt.png", true);
    Canis::GLTexture grassTexture = Canis::LoadImageGL("assets/textures/grass.png", false);
    Canis::GLTexture flowerTexture = Canis::LoadImageGL("assets/textures/blue_orchid.png", false);
    //Canis::GLTexture fireTexture = Canis::LoadImageGL("assets/textures/fire_textures/fire_1.png", true);
    Canis::GLTexture textureSpecular = Canis::LoadImageGL("assets/textures/container2_specular.png", true);
    /// End of Image Loading

    //Load Fire Images
    LoadFireTextures();
    // End of Fire Loading

    /// Load Models
    Canis::Model cubeModel = Canis::LoadModel("assets/models/cube.obj");
    Canis::Model fireModel = Canis::LoadModel("assets/models/fire.obj");
    Canis::Model grassModel = Canis::LoadModel("assets/models/plants.obj");
    /// END OF LOADING MODEL

    Canis::PointLight pointLight;
    pointLight.position = vec3(0.0f);
    pointLight.ambient = vec3(0.2f);
    pointLight.diffuse = vec3(1.0f, 0.5f, 0.0f);
    pointLight.specular = vec3(1.0f);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;

    // Load Map into 3d array
    LoadMap("assets/maps/level.map");

    // Loop map and spawn objects
    for (int y = 0; y < map.size(); y++)
    {
        for (int x = 0; x < map[y].size(); x++)
        {
            for (int z = 0; z < map[y][x].size(); z++)
            {
                Canis::Entity entity;
                entity.active = true;

                switch (map[y][x][z])
                {
                case 1: // places a grass dirt block
                    entity.tag = "dirt";
                    entity.albedo = &dirtTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 2: // places a grass block
                    entity.tag = "grass";
                    entity.albedo = &grassTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &grassModel;
                    entity.shader = &grassShader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    //entity.Update = &Rotate;
                    world.Spawn(entity);
                    break;
                case 3: // places a cobblestone block
                    entity.tag = "cobblestone";
                    entity.albedo = &stoneTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 4: // places a wood block
                    entity.tag = "wood";
                    entity.albedo = &woodTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 5: // places a plank block
                    entity.tag = "plank";
                    entity.albedo = &plankTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 6: // places a glass block
                    entity.tag = "glass";
                    entity.albedo = &glassTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 7: // places a fire block
                    entity.tag = "fire";
                    entity.albedo = &fireTextures[0];
                    entity.specular = &textureSpecular;
                    entity.model = &fireModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    entity.Update = &AnimateFire;
                    pointLight.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.SpawnPointLight(pointLight);
                    world.Spawn(entity);
                    break;
                case 8: // places a dirt block
                    entity.tag = "dirtdirt";
                    entity.albedo = &actualdirtTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 9: // places a leaf block
                    entity.tag = "leaves";
                    entity.albedo = &leafTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &cubeModel;
                    entity.shader = &shader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                case 10: // places a flower block
                    entity.tag = "flower";
                    entity.albedo = &flowerTexture;
                    entity.specular = &textureSpecular;
                    entity.model = &grassModel;
                    entity.shader = &grassShader;
                    entity.transform.position = vec3(x + 0.0f, y + 0.0f, z + 0.0f);
                    world.Spawn(entity);
                    break;
                default:
                    break;
                }
            }
        }
    }


    double deltaTime = 0.0;
    double fps = 0.0;

    // Application loop
    while (inputManager.Update(Canis::GetConfig().width, Canis::GetConfig().heigth))
    {
        deltaTime = frameRateManager.StartFrame();
        Canis::Graphics::ClearBuffer(COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT);

        world.Update(deltaTime);
        world.Draw(deltaTime);

        editor.Draw();

        window.SwapBuffer();

        // EndFrame will pause the app when running faster than frame limit
        fps = frameRateManager.EndFrame();

        Canis::Log("FPS: " + std::to_string(fps) + " DeltaTime: " + std::to_string(deltaTime));
    }

    return 0;
}

void Rotate(Canis::World &_world, Canis::Entity &_entity, float _deltaTime)
{
    _entity.transform.rotation.y += _deltaTime;
}

void LoadMap(std::string _path)
{
    std::ifstream file;
    file.open(_path);

    if (!file.is_open())
    {
        printf("file not found at: %s \n", _path.c_str());
        exit(1);
    }

    int number = 0;
    int layer = 0;

    map.push_back(std::vector<std::vector<unsigned int>>());
    map[layer].push_back(std::vector<unsigned int>());

    while (file >> number)
    {
        if (number == -2) // add new layer
        {
            layer++;
            map.push_back(std::vector<std::vector<unsigned int>>());
            map[map.size() - 1].push_back(std::vector<unsigned int>());
            continue;
        }

        if (number == -1) // add new row
        {
            map[map.size() - 1].push_back(std::vector<unsigned int>());
            continue;
        }

        map[map.size() - 1][map[map.size() - 1].size() - 1].push_back((unsigned int)number);
    }
}

//Load Fire Images to Animate
void LoadFireTextures()
{
    for (int i = 1; i <= 31; ++i)
    {
        std::string path = "assets/textures/fire_textures/fire_" + std::to_string(i) + ".png";
        fireTextures.push_back(Canis::LoadImageGL(path, true));
    }
}

//Swap fire (Animating the fire)
void AnimateFire(Canis::World &_world, Canis::Entity &_entity, float _deltaTime)
{
    static float timer = 0.0f;
    static int frame = 0;

    timer += _deltaTime;

    if (timer >= 0.5f)
    {
        timer = 0.0f;
        frame = (frame + 1) % 31; // loop 0-30
        _entity.albedo = &fireTextures[frame];
    }
    
    // Light flicker effect
    static float flickerTimer = 0.0f;
    flickerTimer += _deltaTime;

    if (flickerTimer >= 0.05f) // Flicker every ~50ms
    {
        flickerTimer = 0.0f;

        // Random intensity between 0.8 and 1.2
        float intensity = 0.9f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.3f));

        // Access the light associated with this fire entity
        for (auto& light : _world.m_pointLights)
        {
            if (glm::distance(light.position, _entity.transform.position) < 0.1f)
            {
                light.diffuse = glm::vec3(1.0f, 0.5f, 0.0f) * intensity;
                break;
            }
        }
    }
}

void SpawnLights(Canis::World &_world)
{
    Canis::DirectionalLight directionalLight;
    directionalLight.direction = vec3(1.0f);
    directionalLight.ambient = vec3(0.4f);
    _world.SpawnDirectionalLight(directionalLight);

    Canis::PointLight pointLight;
    pointLight.position = vec3(0.0f);
    pointLight.ambient = vec3(0.2f);
    pointLight.diffuse = vec3(0.5f);
    pointLight.specular = vec3(1.0f);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;

    _world.SpawnPointLight(pointLight);

    pointLight.position = vec3(0.0f, 0.0f, 1.0f);
    pointLight.ambient = vec3(1.0f, 0.0f, 0.0f);

    _world.SpawnPointLight(pointLight);

    pointLight.position = vec3(-2.0f);
    pointLight.ambient = vec3(0.0f, 1.0f, 0.0f);

    _world.SpawnPointLight(pointLight);
}