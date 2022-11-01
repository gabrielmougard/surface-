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

#include "scene/scene.h"
#include "scene/components.h"
#include "scene/entity.h"
#include "embeddings/embeddings.h"
#include "ai/planner.h"
#include "ai/actions/ai_action_follow.h"
#include "entt/entt.hpp"


namespace surfacepp {
    Entity Scene::CreateEntity(const std::string &name, const std::string &uuid) {
        Entity entity = {registry.create(), this};
        entity.addComponent<StateComponent>();
        auto &tag = entity.addComponent<TagComponent>();
        tag.tag = name.empty() ? "Noname entity" : name;
        auto &generated_uuid = entity.addComponent<UuidComponent>(uuid);
        log_dbg("Scene: created entity with uuid %s", generated_uuid.uuid.c_str());
        return entity;
    }

    void Scene::OnAIUpdateRuntime(float ts) {
        auto AIEntitiesView = registry.view<UuidComponent, TagComponent, TransformComponent, AIComponent>();
        auto AITargetView = registry.view<AITargetComponent, UuidComponent, TransformComponent, TagComponent>();
        for (const auto ai_entity : AIEntitiesView) {
            auto[uuid, tag, transform, ai] = AIEntitiesView.get<UuidComponent, TagComponent, TransformComponent, AIComponent>(ai_entity);
            goap::Planner planner(ai.heuristicFunctionPointer);
            try {
                std::vector<const goap::Action *> plan = planner.plan(ai.initial_world_state, ai.goal_world_state, ai.actions_list);
                if (!plan.empty()){
                    auto current_action = plan.back();
                    std::string target_id_str = current_action->getTargetId();

                    for (const auto ai_target : AITargetView){
                        auto applier_entity_entt = entt::to_entity(registry, transform);
                        surfacepp::Entity applier_entity = {applier_entity_entt, this};
                        auto target_entity_entt = entt::to_entity(registry, ai_target);
                        surfacepp::Entity target_entity = {target_entity_entt, this};

                        (static_cast<const AIActionFollow *>(current_action))->perform(applier_entity, target_entity);
                    }
                }
            }
            catch (const std::exception&) {
                log_info("Path cannot be found");
            }
        }
    }

    void Scene::OnUpdateRuntime(float ts) {
        auto allEntitiesView = registry.view<UuidComponent, TagComponent, TransformComponent, StateComponent>();
        auto scriptedEntityView = registry.view<PyScriptComponent, UuidComponent, TagComponent, TransformComponent, StateComponent>();
        auto scriptedParticleSystemView = registry.view<PyScriptComponent, UuidComponent, TagComponent, StateComponent, ParticlesComponent>();
        auto stateView = registry.view<StateComponent>();

        std::vector<PyEntity> scene_entities;

        for (const auto commonEntity : allEntitiesView) {
            auto[uuid, tag, transform, state] = allEntitiesView.get<UuidComponent, TagComponent, TransformComponent, StateComponent>(commonEntity);
            scene_entities.emplace(scene_entities.end(), PyEntity(uuid, tag, transform));
        }

        // objects scripts processing
        {
            for (const auto pyScriptEntity : scriptedEntityView) {
                auto[py_script, uuid, tag, transform, state] = scriptedEntityView.get<PyScriptComponent, UuidComponent, TagComponent, TransformComponent, StateComponent>(
                        pyScriptEntity);
                auto scene_entity = scriptedEntityView.get<UuidComponent, TagComponent, TransformComponent>(
                        pyScriptEntity);

                py::module_ module = py::module_::import(py_script.script_path.c_str());
                if (state.reload_script_flag) {
                    py_script.script_path = py_script._script_input_path;
                    log_info("Reloading script %s", py_script.script_path.c_str());
                    state.reload_script_flag = false;
                    module.reload();
                }

                auto py_entity = module.attr("DerivedPyEntity")(uuid, tag, transform);

                if (state.create_flag) {
                    py_entity.attr("on_create")();
                }
                {
                    py_entity.attr("on_update")(ts);
                    const auto &cpp_entity = py_entity.cast<const PyEntity &>();
                    transform = static_cast<surfacepp::TransformComponent>(cpp_entity.transform);
                }
                if (state.destroy_flag) {
                    py_entity.attr("on_destroy")();
                }
            }
        }

        // scene scripts processing
        {
            auto scriptedSceneView = registry.view<PyScriptComponent, UuidComponent, TagComponent, StateComponent, InputComponent>();

            for (const auto pyScriptScene : scriptedSceneView) {
                auto[py_script, uuid, tag, state, input] = scriptedSceneView.get<PyScriptComponent, UuidComponent, TagComponent, StateComponent, InputComponent>(
                        pyScriptScene);
                py::module_ module = py::module_::import(py_script.script_path.c_str());
                auto py_entity = module.attr("DerivedPyScene")(scene_entities);

                py_entity.attr("on_update")(ts);
                const auto &cpp_entity = py_entity.cast<const PyScene>();
            }
        }


        // particle systems scripts processing
        {
            for (const auto pyScriptParticleSystem : scriptedParticleSystemView) {
                auto[py_script, uuid, tag, state, particles] = scriptedParticleSystemView.get<PyScriptComponent, UuidComponent, TagComponent, StateComponent, ParticlesComponent>(
                        pyScriptParticleSystem);
                py::module_ module = py::module_::import(py_script.script_path.c_str());
                if (state.reload_script_flag) {
                    py_script.script_path = py_script._script_input_path;
                    log_info("Reloading script %s", py_script.script_path.c_str());
                    state.reload_script_flag = false;
                    module.reload();
                }
                auto py_entity = module.attr("DerivedPyParticleSystem")(particles.controller.referenceParameters, particles.controller.getParticlesNumber());

                if (state.create_flag) {
                    py_entity.attr("on_create")();
                }
                {
                    py_entity.attr("on_update")(ts);
                    const auto &cpp_entity = py_entity.cast<const PyParticleSystem &>();
                    particles.controller.referenceParameters = static_cast<ParticleParameters>(cpp_entity.parameters);
                }
                if (state.destroy_flag) {
                    py_entity.attr("on_destroy")();
                }
            }
        }

        // manage object states
        {
            for (const auto entity : stateView) {
                auto &state = stateView.get<StateComponent>(entity);
                if (state.create_flag) {
                    state.create_flag = false;
                }
                if (state.destroy_flag) {
                    state.destroy_flag = false;
                    registry.destroy(entity);
                }
            }
        }
    }

    void Scene::OnRenderRuntime(float ts) {
        auto renderStepView = registry.view<CameraComponent, InputComponent, ScreenScaleComponent, ModelsCacheComponent, ShadersCacheComponent, IlluminateCacheComponent>();
        auto renderModelsDataView = registry.view<ShaderProgramComponent, ModelComponent, TransformComponent>(
                entt::exclude<surfacepp::ThirdPersonCharacterComponent>);
        auto renderTpcDataView = registry.view<ShaderProgramComponent, ModelComponent, TransformComponent, ThirdPersonCharacterComponent>();
        auto renderCubeDataView = registry.view<ShaderProgramComponent, TransformComponent, CubeObjectComponent>();
        auto renderParticlesDataView = registry.view<ParticlesComponent, ShaderProgramComponent>();
        auto playbackBackgroundSoundsView = registry.view<AudioBackgroundComponent>();
        auto playbackPositionedSoundsView = registry.view<AudioPositionedComponent, TransformComponent>();
        auto playbackSpeechSoundsView = registry.view<AudioSpeechComponent>();

        for (auto renderStepEntity : renderStepView) {  // single renderStepEntity will be unpacked
            auto[camera, input, screen_scale, models_cache, shaders_cache, lights_cache] = renderStepView.get<CameraComponent, InputComponent, ScreenScaleComponent, ModelsCacheComponent,
                    ShadersCacheComponent, IlluminateCacheComponent>(renderStepEntity);

            for (auto renderTpcEntity : renderTpcDataView) {
                auto[shader_path, model_path, transform, tpc] = renderTpcDataView.get<ShaderProgramComponent, ModelComponent, TransformComponent, ThirdPersonCharacterComponent>(
                        renderTpcEntity);

                // process input
                if (!input.input.IsCursorVisible()) {
                    if (input.input.Keys[GLFW_KEY_W]) {
                        transform.position = camera.ProcessKeyboard(CameraMovement::kForward, ts,
                                                                    camera.input_speed,
                                                                    transform.position);
                    }
                    if (input.input.Keys[GLFW_KEY_S]) {
                        transform.position = camera.ProcessKeyboard(CameraMovement::kBackward, ts,
                                                                    camera.input_speed, transform.position);
                    }
                    if (input.input.Keys[GLFW_KEY_A]) {
                        transform.position = camera.ProcessKeyboard(CameraMovement::kLeft, ts,
                                                                    camera.input_speed,
                                                                    transform.position);
                    }
                    if (input.input.Keys[GLFW_KEY_D]) {
                        transform.position = camera.ProcessKeyboard(CameraMovement::kRight, ts,
                                                                    camera.input_speed,
                                                                    transform.position);
                    }
                    if (input.input.Keys[GLFW_KEY_F5]) {
                        input.input.SetCursorVisible();
                    }

                    lights_cache.light_sources[0] = transform.position;

                    if (input.input.MouseOffsetUpdated) {
                        camera.ProcessMouseMovement(input.input.MouseOffsets[X_OFFSET],
                                                    input.input.MouseOffsets[Y_OFFSET]);
                        input.input.MouseOffsetUpdated = false;
                    }
                }
                if (input.input.Keys[GLFW_KEY_F6]) {
                    input.input.SetCursorInvisible();
                }

                auto shader_unpack = shaders_cache.cache.find(shader_path.v_shader_path);
                auto shader = shader_unpack->second;

                auto model_unpack = models_cache.cache.find(model_path.model_path);
                auto model = model_unpack->second;

                if (tpc.is_third_person_char)
                    renderer.RenderThirdPersonCharacter((ThirdPersonCamera *) camera.GetCamera(),
                                                        screen_scale.screen_scale, &model, &shader,
                                                        lights_cache.light_sources[0], transform.position,
                                                        transform.size);
            }
            // render models
            for (auto renderDataEntity : renderModelsDataView) {
                auto[shader_path, model_path, transform] = renderModelsDataView.get<ShaderProgramComponent, ModelComponent, TransformComponent>(
                        renderDataEntity);

                auto shader_unpack = shaders_cache.cache.find(shader_path.v_shader_path);
                auto shader = shader_unpack->second;
                auto model_unpack = models_cache.cache.find(model_path.model_path);
                auto model = model_unpack->second;

                renderer.Render(camera.GetCamera(), screen_scale.screen_scale, &model, &shader,
                                lights_cache.light_sources[0],
                                transform.getTransform());
            }
            // render cubes
            for (auto renderCubeEntity : renderCubeDataView) {
                auto[shader_path, transform, cube] = renderCubeDataView.get<ShaderProgramComponent, TransformComponent, CubeObjectComponent>(
                        renderCubeEntity);

                auto shader_unpack = shaders_cache.cache.find(shader_path.v_shader_path);
                auto shader = shader_unpack->second;

                renderer.RenderCube(camera.GetCamera(), screen_scale.screen_scale, cube.VAO_, cube.texture, &shader,
                                    lights_cache.light_sources[0], transform.getTransform());
            }

            // render particles
            for (auto renderParticleEntity : renderParticlesDataView) {
                auto[particle_controller, shader_path] = renderParticlesDataView.get<ParticlesComponent, ShaderProgramComponent>(
                        renderParticleEntity);

                auto shader_unpack = shaders_cache.cache.find(shader_path.v_shader_path);
                auto shader = shader_unpack->second;

                particle_controller.controller.update(ts);
                particle_controller.controller.renderParticles(camera.GetCamera(), &shader, screen_scale.screen_scale);
            }

            // playback background audio
            for (auto audioEntity : playbackBackgroundSoundsView) {
                auto &audio = playbackBackgroundSoundsView.get(audioEntity);
                if (audio.audio->start_playback) {
                    audio.audio->RunPlayback();
                    audio.audio->start_playback = false;
                }
            }

            // playback positioned audio
            for (auto audioEntity : playbackPositionedSoundsView) {
                auto[audio, transform] = playbackPositionedSoundsView.get<AudioPositionedComponent, TransformComponent>(
                        audioEntity);
                if (audio.audio->start_playback) {
                    audio.audio->RunPlayback(transform.position);
                    audio.audio->start_playback = false;
                }
                audio.audio->UpdatePositioning(transform.position, camera.GetCamera()->position_, camera.GetCamera()->front_);
            }

            // playback speech audio
            for (auto audioEntity : playbackSpeechSoundsView) {
                auto &audio = playbackSpeechSoundsView.get<AudioSpeechComponent>(audioEntity);
                if (audio.audio->start_playback) {
                    audio.audio->RunPlayback();
                    audio.audio->start_playback = false;
                }
            }
        }
    }
}