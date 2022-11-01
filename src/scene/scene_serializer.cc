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

#include "scene_serializer.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

#include "entity.h"
#include "components.h"
#include "particles/particle.h"

namespace YAML {
    template<>
    struct convert<glm::vec3> {
        static Node encode(const glm::vec3 &rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node &node, glm::vec3 &rhs) {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4> {
        static Node encode(const glm::vec4 &rhs) {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node &node, glm::vec4 &rhs) {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };
}


namespace surfacepp {
    YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec3 &v) {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec4 &v) {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    SceneSerializer::SceneSerializer(Scene *scene)
            : scene(scene) {
    }

    static void SerializeEntity(YAML::Emitter &out, Entity entity) {
        auto &uuid = entity.getComponent<surfacepp::UuidComponent>().uuid;

        out << YAML::BeginMap; // Entity
        out << YAML::Key << "entity" << YAML::Value << uuid.c_str(); // TODO: Entity ID goes here

        if (entity.hasComponent<TagComponent>()) {
            auto &c = entity.getComponent<surfacepp::TagComponent>();

            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "tag" << YAML::Value << c.tag;
            out << YAML::EndMap;
        }

        if (entity.hasComponent<TransformComponent>()) {
            auto &c = entity.getComponent<TransformComponent>();

            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "position" << YAML::Value << c.position;
            out << YAML::Key << "rotation" << YAML::Value << c.rotation;
            out << YAML::Key << "size" << YAML::Value << c.size;
            out << YAML::EndMap;
        }

        if (entity.hasComponent<ShaderProgramComponent>()) {
            auto &c = entity.getComponent<ShaderProgramComponent>();

            out << YAML::Key << "ShaderProgramComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "v_shader_path" << YAML::Value << c.v_shader_path.c_str();
            out << YAML::EndMap;
        }

        if (entity.hasComponent<ModelComponent>()) {
            auto &c = entity.getComponent<ModelComponent>();

            out << YAML::Key << "ModelComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "model_path" << YAML::Value << c.model_path.c_str();
            out << YAML::EndMap;
        }

        if (entity.hasComponent<CameraComponent>()) {
            auto &c = entity.getComponent<CameraComponent>();
            std::string camera_type{};
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap;
            if (typeid(*c.GetCamera()) == typeid(ThirdPersonCamera)) {
                camera_type = THIRD_PERSON_CAMERA_CLASSNAME;
                out << YAML::Key << "spring_arm_length" <<
                YAML::Value << static_cast<ThirdPersonCamera *>(c.GetCamera())->spring_arm_length_;
            } else if (typeid(*c.GetCamera()) == typeid(PlatformerCamera)) {
                camera_type = PLATFORMER_CAMERA_CLASSNAME;
            } else {
                camera_type = FREE_CAMERA_CLASSNAME;
            }
            out << YAML::Key << "camera_type" << YAML::Value << camera_type;
            out << YAML::Key << "input_speed" << YAML::Value << c.input_speed;
            out << YAML::EndMap;
        }

        if (entity.hasComponent<IlluminateCacheComponent>()) {
            auto &c = entity.getComponent<IlluminateCacheComponent>();

            out << YAML::Key << "IlluminateCacheComponent";
            out << YAML::Flow;
            out << YAML::BeginSeq;
            for (auto const &value: c.light_sources) {
                out << value;
            }
            out << YAML::EndSeq;
        }

        if (entity.hasComponent<ThirdPersonCharacterComponent>()) {
            auto &c = entity.getComponent<ThirdPersonCharacterComponent>();

            out << YAML::Key << "ThirdPersonCharacterComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "is_third_person_char" << YAML::Value << c.is_third_person_char;
            out << YAML::EndMap;
        }

        if (entity.hasComponent<CubeObjectComponent>()) {
            auto &c = entity.getComponent<CubeObjectComponent>();

            out << YAML::Key << "CubeObjectComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "texture_path" << YAML::Value << c.texture_path;
            out << YAML::EndMap;
        }

        if (entity.hasComponent<ParticlesComponent>()) {
            auto &c = entity.getComponent<ParticlesComponent>();

            out << YAML::Key << "ParticlesComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "position" << YAML::Value << c.controller.referenceParameters.position;
            out << YAML::Key << "color" << YAML::Value << c.controller.referenceParameters.color;
            out << YAML::Key << "life_length" << YAML::Value << c.controller.referenceParameters.life_length;
            out << YAML::Key << "rotation" << YAML::Value << c.controller.referenceParameters.rotation;
            out << YAML::Key << "velocity" << YAML::Value << c.controller.referenceParameters.velocity;
            out << YAML::Key << "scale" << YAML::Value << c.controller.referenceParameters.scale;
            out << YAML::Key << "gravity_effect" << YAML::Value << c.controller.referenceParameters.gravity_effect;
            out << YAML::Key << "particles_number" << YAML::Value << c.controller.getParticlesNumber();
            out << YAML::EndMap;
        }

        if (entity.hasComponent<AudioPositionedComponent>()) {
            auto &c = entity.getComponent<AudioPositionedComponent>();

            out << YAML::Key << "AudioPositionedComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "sound_name" << YAML::Value << c.audio->GetSoundName();
            out << YAML::EndMap;
        }

        if (entity.hasComponent<AudioBackgroundComponent>()) {
            auto &c = entity.getComponent<AudioBackgroundComponent>();

            out << YAML::Key << "AudioBackgroundComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "sound_name" << YAML::Value << c.audio->GetSoundName();
            out << YAML::EndMap;
        }

        if (entity.hasComponent<AudioSpeechComponent>()) {
            auto &c = entity.getComponent<AudioSpeechComponent>();

            out << YAML::Key << "AudioSpeechComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "text_to_speak" << YAML::Value << c.audio->GetTextToSpeak();
            out << YAML::EndMap;
        }

        if (entity.hasComponent<PyScriptComponent>()) {
            auto &c = entity.getComponent<PyScriptComponent>();

            out << YAML::Key << "PyScriptComponent";
            out << YAML::BeginMap;
            out << YAML::Key << "script_path" << YAML::Value << c.script_path;
            out << YAML::EndMap;
        }

        /*
         * TODO: Think about macro
         *
         if (entity.hasComponent<>()) {
             auto &c = entity.getComponent<>();

             out << YAML::Key << "";
             out << YAML::BeginMap;
             out << YAML::Key << "" << YAML::Value << c.;
             out << YAML::EndMap;
         }
         */

        out << YAML::EndMap;
    }

    void SceneSerializer::Serialize(const std::string &filepath) {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "scene" << YAML::Value << "Unnamed";
        out << YAML::Key << "entities" << YAML::Value << YAML::BeginSeq;
        scene->registry.each([&](auto entityID) {
            Entity entity = {entityID, scene};
            SerializeEntity(out, entity);
        });
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream output_stream(filepath);
        output_stream << out.c_str();
    }

    void SceneSerializer::SerializeRuntime(const std::string &filepath) {
        log_err("Not implemented!");
        assert(false);
    }

    bool SceneSerializer::Deserialize(const std::string &filepath) {
        std::ifstream stream(filepath);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        log_info("Starting scene desirialization");
        if (!data["scene"])
            return false;

        std::string scene_name = data["scene"].as<std::string>();
        log_info("Deserializing scene: %s", scene_name.c_str());

        auto entities = data["entities"];
        if (entities) {
            for (auto entity : entities) {
                std::string uuid = entity["entity"].as<std::string>();
                std::string name = entity["TagComponent"]["tag"].as<std::string>();
                Entity deserializedEntity = scene->CreateEntity(name, uuid);
                log_dbg("Pulling components:");

                {
                    auto transformComponent = entity["TransformComponent"];
                    if (transformComponent) {
                        log_dbg("\ttransform component");
                        auto position = transformComponent["position"].as<glm::vec3>();
                        auto rotation = transformComponent["rotation"].as<glm::vec3>();
                        auto size = transformComponent["size"].as<glm::vec3>();
                        auto &c = deserializedEntity.addComponent<TransformComponent>(position, rotation, size);
                    }
                }

                {
                    auto shader_program_component = entity["ShaderProgramComponent"];
                    if (shader_program_component) {
                        log_dbg("\tshader program component");
                        auto v_shader_path = shader_program_component["v_shader_path"].as<std::string>();
                        auto &c = deserializedEntity.addComponent<ShaderProgramComponent>(v_shader_path.c_str());
                    }
                }

                {
                    auto model_component = entity["ModelComponent"];
                    if (model_component) {
                        log_dbg("\tmodel component");
                        auto model_path = model_component["model_path"].as<std::string>();
                        auto &c = deserializedEntity.addComponent<ModelComponent>(model_path.c_str());
                    }
                }

                {
                    auto camera_component = entity["CameraComponent"];
                    if (camera_component) {
                        log_dbg("\tcamera component");
                        auto camera_type = camera_component["camera_type"].as<std::string>();
                        auto input_speed = camera_component["input_speed"].as<float>();
                        BaseCamera * camera = nullptr;
                        if (camera_type == "ThirdPersonCamera") {
                            auto spring_arm_length = camera_component["spring_arm_length"].as<float>();
                            log_dbg("\t\tThird person camera component setting up");
                            camera = new ThirdPersonCamera(spring_arm_length);
                        } else if (camera_type == "PlatformerCamera"){
                            log_dbg("\t\tPlatformer Camera component setting up");
                            camera = new PlatformerCamera();
                        } else {
                            log_dbg("\t\tCamera component setting up");
                            camera = new BaseCamera();
                        }
                        auto &c = deserializedEntity.addComponent<CameraComponent>(camera, input_speed);
                    }
                }

                {
                    auto tpc_component = entity["ThirdPersonCharacterComponent"];
                    if (tpc_component) {
                        log_dbg("\tTPC component");
                        auto is_third_person_char = tpc_component["is_third_person_char"].as<bool>();
                        auto &c = deserializedEntity.addComponent<ThirdPersonCharacterComponent>(is_third_person_char);
                    }
                }

                {
                    auto cube_component = entity["CubeObjectComponent"];
                    if (cube_component) {
                        log_dbg("\tcube component");
                        auto texture_path = cube_component["texture_path"].as<std::string>();
                        auto &c = deserializedEntity.addComponent<CubeObjectComponent>(texture_path.c_str());
                    }
                }

                {
                    auto particles_component = entity["ParticlesComponent"];
                    if (particles_component) {
                        log_dbg("\tparticles component");
                        auto position = particles_component["position"].as<glm::vec3>();
                        auto color = particles_component["color"].as<glm::vec4>();
                        auto life_length = particles_component["life_length"].as<float>();
                        auto rotation = particles_component["rotation"].as<float>();
                        auto velocity = particles_component["velocity"].as<glm::vec3>();
                        auto scale = particles_component["scale"].as<float>();
                        auto gravity_effect = particles_component["gravity_effect"].as<float>();
                        auto particles_number = particles_component["particles_number"].as<int>();
                        ParticleParameters particles_parameters{position,
                                                                velocity,
                                                                color,
                                                                gravity_effect,
                                                                life_length,
                                                                rotation,
                                                                scale};

                        auto &c = deserializedEntity.addComponent<ParticlesComponent>(particles_parameters,
                                                                                      (uint32_t) particles_number);
                    }
                }

                {
                    auto audio_positioned_component = entity["AudioPositionedComponent"];
                    if (audio_positioned_component) {
                        audioPositioned = new AudioPositioned("s.mp3");
                        log_dbg("\taudio positioned component");
                        auto sound_name = audio_positioned_component["sound_name"].as<std::string>();
                        deserializedEntity.addComponent<AudioPositionedComponent>(audioPositioned);
                    }
                }

                {
                    auto audio_positioned_component = entity["AudioBackgroundComponent"];
                    if (audio_positioned_component) {
                        audioBackground = new AudioBackground("s.mp3");
                        log_dbg("\taudio background component");
                        auto sound_name = audio_positioned_component["sound_name"].as<std::string>();
                        deserializedEntity.addComponent<AudioBackgroundComponent>(audioBackground);
                    }
                }

                {
                    auto audio_speech_component = entity["AudioSpeechComponent"];
                    if (audio_speech_component) {
                        audioSpeech = new AudioSpeech("", (unsigned int) 530, (float) 10, (float) 0.5,
                                                            (int) KW_NOISE);
                        log_dbg("\taudio speech component");
                        auto text_to_speak = audio_speech_component["text_to_speak"].as<std::string>();
                        deserializedEntity.addComponent<AudioSpeechComponent>(audioSpeech);
                    }
                }

                {
                    auto py_script_component = entity["PyScriptComponent"];
                    if (py_script_component) {
                        log_dbg("\tpy script component");
                        auto script_name = py_script_component["script_path"].as<std::string>();
                        deserializedEntity.addComponent<PyScriptComponent>(script_name.c_str());
                    }
                }

                log_info("Deserialized entity with ID = %s, name = %s", uuid.c_str(), name.c_str());
                log_dbg("====================================================");
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string &filepath) {
        // Not implemented
        log_err("Not implemented");
        return false;
    }

}