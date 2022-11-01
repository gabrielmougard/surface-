// Copyright 2022 gab
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "editor.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "scene/scene_serializer.h"

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 950;


namespace surfacepp {
    Editor::Editor() {
        this->OpenglSetup();
        this->FramebufferSetup();
        this->CreateSceneLayout();
        this->gui_ = new Gui(this->window_, this->framebuffer_, this->scene_);
        this->gui_->ImGuiSetup();
    }

    void Editor::OpenglSetup() {
        glfwSetErrorCallback(_glfwErrorCallback);

        auto init_result = glfwInit();
        assert(init_result == GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        window_ = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Surface++ Editor", nullptr, nullptr);
        assert(window_ != nullptr);
        glfwMakeContextCurrent(window_);

        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            log_err("Failed to initialize GLAD");
        }

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(_glMessageCallback, nullptr);

        // Define the viewport dimensions
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        // Setup OpenGL options
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Editor::FramebufferSetup() {
        FramebufferSpecification spec = FramebufferSpecification();
        spec.Width = SCR_WIDTH;
        spec.Height = SCR_HEIGHT;
        framebuffer_ = new Framebuffer(spec);
    }

    void Editor::CreateSceneLayout() {
        AudioCore::setup_audio_core();
        scene_ = new surfacepp::Scene();

        Shader *text_shader_program_ = Shader::LoadFromFile("src/shaders/text_vs.glsl", "src/shaders/text_fs.glsl");
        Shader *main_character_shader_program_ = Shader::LoadFromFile("src/shaders/main_vs.glsl",
                                                                      "src/shaders/main_fs.glsl");
        Shader *object_shader_program_ = Shader::LoadFromFile("src/shaders/object_vs.glsl",
                                                              "src/shaders/object_fs.glsl");
        Shader *lamp_shader_program_ = Shader::LoadFromFile("src/shaders/lamp_vs.glsl", "src/shaders/lamp_fs.glsl");
        Shader *particle_shader_program_ = Shader::LoadFromFile("src/shaders/particle_vs.glsl",
                                                                "src/shaders/particle_fs.glsl");

        ParticleParameters particles_parameters{glm::vec3(45, 0, -300),
                                                glm::vec3(50, 50, 50),
                                                glm::vec4(0.1, 0.1, 0.9, 0.9),
                                                13.8f,
                                                8,
                                                45,
                                                1.0f};

        // models load
        auto *cyborgModel = new Model("resources/objects/cyborg/cyborg.obj", false);
        auto *sphereModel = new Model("resources/objects/sphere/sphere.obj", false);
        auto *triangleSphereModel = new Model("resources/objects/sphere/triangle/sphere.obj", false);
        auto *thirdPersonCharacterModel = new Model("resources/objects/sphere/disco/sphere.obj", false);

        auto * audioPositioned = new AudioPositioned("s.mp3");
        auto * audioBackground = new AudioBackground("s.mp3");
        auto * audioSpeech = new AudioSpeech("", (unsigned int) 530, (float) 10, (float) 0.5, (int) KW_NOISE);

        std::map<std::string, Model> models_map = {
                {std::string("resources/objects/cyborg/cyborg.obj"),          *cyborgModel},
                {std::string("resources/objects/sphere/sphere.obj"),          *sphereModel},
                {std::string("resources/objects/sphere/triangle/sphere.obj"), *triangleSphereModel},
                {std::string("resources/objects/sphere/disco/sphere.obj"),    *thirdPersonCharacterModel}
        };
        std::map<std::string, Shader> shaders_map = {
                {std::string("src/shaders/text_vs.glsl"),     *text_shader_program_},
                {std::string("src/shaders/main_vs.glsl"),     *main_character_shader_program_},
                {std::string("src/shaders/object_vs.glsl"),   *object_shader_program_},
                {std::string("src/shaders/lamp_vs.glsl"),     *lamp_shader_program_},
                {std::string("src/shaders/particle_vs.glsl"), *particle_shader_program_},
        };

        surfacepp::Entity sceneContext = scene_->CreateEntity("SceneContext");
        BaseCamera *camera = new BaseCamera();
        sceneContext.addComponent<surfacepp::CameraComponent>(camera, 1.f);
        sceneContext.addComponent<surfacepp::InputComponent>(window_);
        sceneContext.addComponent<surfacepp::ScreenScaleComponent>((float) SCR_WIDTH / (float) SCR_HEIGHT);
        sceneContext.addComponent<surfacepp::ModelsCacheComponent>(models_map);
        sceneContext.addComponent<surfacepp::ShadersCacheComponent>(shaders_map);
        sceneContext.addComponent<surfacepp::IlluminateCacheComponent>(
                std::vector<glm::vec3>{glm::vec3(1), glm::vec3(2)}
        );
        sceneContext.addComponent<surfacepp::PyScriptComponent>("resources.blueprints.scene_blueprint");

        surfacepp::Entity tpc = scene_->CreateEntity("ThirdPersonCharacter");
        tpc.addComponent<surfacepp::ThirdPersonCharacterComponent>(false);
        tpc.addComponent<surfacepp::ModelComponent>("resources/objects/sphere/sphere.obj");
        tpc.addComponent<surfacepp::TransformComponent>(glm::vec3(1, 2, 3), glm::vec3(0), glm::vec3(4));
        tpc.addComponent<surfacepp::ShaderProgramComponent>("src/shaders/main_vs.glsl");

        surfacepp::Entity cyborgEntity = scene_->CreateEntity("Cyborg");
        cyborgEntity.addComponent<surfacepp::ModelComponent>("resources/objects/cyborg/cyborg.obj");
        cyborgEntity.addComponent<surfacepp::TransformComponent>(glm::vec3(0, 0, -100), glm::vec3(0), glm::vec3(6, 6, 6));
        cyborgEntity.addComponent<surfacepp::ShaderProgramComponent>("src/shaders/object_vs.glsl");

        for (int i = 0; i < 65; i++) {
            std::string name = std::string("Ball_") + std::to_string(i);
            glm::vec3 position = glm::vec3(-5 * i, cos(2 * i) * 10, -160);

            surfacepp::Entity ballEntity = scene_->CreateEntity(name);
            ballEntity.addComponent<surfacepp::ModelComponent>("resources/objects/sphere/sphere.obj");
            ballEntity.addComponent<surfacepp::PyScriptComponent>("resources.blueprints.ball_blueprint");
            ballEntity.addComponent<surfacepp::TransformComponent>(position, glm::vec3(0), glm::vec3(1));
            ballEntity.addComponent<surfacepp::ShaderProgramComponent>("src/shaders/object_vs.glsl");
        }

        surfacepp::Entity cubeEntity = scene_->CreateEntity("Cube");
        cubeEntity.addComponent<surfacepp::CubeObjectComponent>("resources/textures/minecraft_wood.png");
        cubeEntity.addComponent<surfacepp::TransformComponent>(glm::vec3(0, 0, -40), glm::vec3(0), glm::vec3(10));
        cubeEntity.addComponent<surfacepp::ShaderProgramComponent>("src/shaders/object_vs.glsl");
        cubeEntity.addComponent<surfacepp::AudioPositionedComponent>(audioPositioned);
        cubeEntity.addComponent<surfacepp::PyScriptComponent>("resources.blueprints.cube_blueprint");

        surfacepp::Entity floorEntity = scene_->CreateEntity("Floor");
        floorEntity.addComponent<surfacepp::CubeObjectComponent>("resources/textures/background.png");
        floorEntity.addComponent<surfacepp::TransformComponent>(glm::vec3(0, -5, 0), glm::vec3(0), glm::vec3(400, 0.3, 400));
        floorEntity.addComponent<surfacepp::ShaderProgramComponent>("src/shaders/object_vs.glsl");

        surfacepp::Entity particlesEmitterEntity = scene_->CreateEntity("ParticleEmitter");
        particlesEmitterEntity.addComponent<surfacepp::ParticlesComponent>(particles_parameters, (uint32_t) 1000);
        particlesEmitterEntity.addComponent<surfacepp::ShaderProgramComponent>("src/shaders/particle_vs.glsl");
        particlesEmitterEntity.addComponent<surfacepp::PyScriptComponent>("resources.blueprints.particle_system_blueprint");

        surfacepp::Entity audio_background = scene_->CreateEntity("AudioBackground");
        audio_background.addComponent<surfacepp::AudioBackgroundComponent>(audioBackground);

        surfacepp::Entity audio_speech = scene_->CreateEntity("AudioSpeech");
        audio_speech.addComponent<surfacepp::AudioSpeechComponent>(audioSpeech);
//                                                                */

        SceneSerializer serializer(scene_);
//        serializer.Serialize("scene.yaml");
//        serializer.Deserialize("scene.yaml");
    }

    void Editor::Run() {
        static GLfloat deltaTime = 0.0f;
        static GLfloat lastFrame = 0.0f;

        while (!glfwWindowShouldClose(window_)) {
            GLfloat currentFrame = (float) glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            glfwPollEvents();
            this->OnUpdate(deltaTime);
            this->OnOpenglRender(deltaTime);
            this->gui_->onRender();
            _flush_log();
        }
        this->Close();
    }

    void Editor::OnUpdate(float ts) {
        AudioCore::update_3d_audio();
        scene_->OnUpdateRuntime(ts);
    }

    void Editor::OnOpenglRender(float ts) {
        framebuffer_->Bind();
        glClearColor(0.3f, 0.f, .0f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        scene_->OnRenderRuntime(ts);
        glfwSwapBuffers(window_);
        framebuffer_->Unbind();
    }

    void Editor::Close() {
        this->gui_->Close();
        delete gui_;
        delete scene_;
        delete framebuffer_;
        glfwDestroyWindow(window_);
        glfwTerminate();
        log_info("Bye.");
    }
}

int main() {
    surfacepp::Editor editor = surfacepp::Editor();
    editor.Run();
    return 0;
}