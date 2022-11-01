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

#pragma once

#include <string>
#include <cstdint>
#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_speech.h>

#include <glm/glm.hpp>

namespace AudioCore {

    static SoLoud::Soloud * _soloudCore;

    void setup_audio_core();
    void teardown_audio_core();
    void update_3d_audio();
}



class _Sound{
protected:
    uint8_t _soundHandler;
public:
    bool start_playback = false;
    _Sound();
    virtual ~_Sound();
    virtual void RunPlayback() = 0;
    virtual void StopPlayback() = 0;
    virtual void UpdatePositioning(glm::vec3 source_position, glm::vec3 listener_position, glm::vec3 listener_look_at);
};


class AudioPositioned: public _Sound{
protected:
    SoLoud::Wav sample;
    std::string file_name;
public:
    AudioPositioned(const char * file_path);
    void RunPlayback() override {};
    void RunPlayback(glm::vec3 source_position);
    void StopPlayback() override;
    std::string GetSoundName();
    void SetSoundName(std::string sound_name);
};

class AudioBackground: public AudioPositioned{
public:
    AudioBackground(const char * file_path);
    void RunPlayback() override;
    void UpdatePositioning(glm::vec3 source_position, glm::vec3 listener_position, glm::vec3 listener_look_at) override {};
};


class AudioSpeech: public _Sound{
protected:
    SoLoud::Speech speech;
public:
    std::string text_to_speak;
    AudioSpeech(const char * text_to_speak,
                unsigned int frequency,
                float speed,
                float declination,
                int wave_form);
    void RunPlayback() override;
    void StopPlayback() override;
    std::string GetTextToSpeak();
    void SetTextToSpeak(const char * text_to_speak);
};