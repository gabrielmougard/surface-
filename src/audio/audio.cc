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

#include "audio/audio.h"
#include "log.h"


namespace AudioCore{
    void setup_audio_core(){
        _soloudCore = new SoLoud::Soloud;
        _soloudCore->init();
    }

    void teardown_audio_core(){
        _soloudCore->deinit();
        delete _soloudCore;
    }

    void update_3d_audio(){
        _soloudCore->update3dAudio();
    }
}

_Sound::_Sound() {}


_Sound::~_Sound()
{};


void _Sound::UpdatePositioning(glm::vec3 source_position, glm::vec3 listener_position, glm::vec3 listener_look_at){
    AudioCore::_soloudCore->set3dListenerPosition(
        listener_position.x,
        listener_position.y,
        listener_position.z
    );
    AudioCore::_soloudCore->set3dListenerAt(
            listener_look_at.x,
            listener_look_at.y,
            listener_look_at.z
    );
    AudioCore::_soloudCore->set3dSourcePosition(
        this->_soundHandler,
        source_position.x,
        source_position.y,
        source_position.z
    );
}


AudioPositioned::AudioPositioned(const char * file_path):
    _Sound()
{
    this->file_name = file_path;
    this->sample.load(file_path);
}


void AudioPositioned::SetSoundName(std::string sound_name) {
    this->file_name = sound_name;
    log_dbg("New sound name = %s", sound_name.c_str());
}


std::string AudioPositioned::GetSoundName() {
    return this->file_name;
}


void AudioPositioned::RunPlayback(glm::vec3 source_position){
    this->StopPlayback();
    this->sample = SoLoud::Wav();
    this->sample.load(file_name.c_str());
    log_dbg("Exec sound name = %s", file_name.c_str());
    this->_soundHandler = AudioCore::_soloudCore->play3d(
            this->sample,
            source_position.x,
            source_position.y,
            source_position.z
    );
}


void AudioPositioned::StopPlayback(){
    AudioCore::_soloudCore->stopAll();
}


AudioBackground::AudioBackground(const char * file_path):
    AudioPositioned(file_path)
{}


void AudioBackground::RunPlayback() {
    this->StopPlayback();
    this->sample = SoLoud::Wav();
    this->sample.load(file_name.c_str());
    this->_soundHandler = AudioCore::_soloudCore->play(this->sample);
}

AudioSpeech::AudioSpeech(const char *text_to_speak,
                         unsigned int frequency,
                         float speed,
                         float declination,
                         int wave_form):
_Sound()
{
    this->SetTextToSpeak(text_to_speak);
    speech.setParams(frequency, speed, declination, wave_form);
}

void AudioSpeech::RunPlayback() {
    log_dbg("Run text speech = %s", this->text_to_speak.c_str());
    this->_soundHandler = AudioCore::_soloudCore->play(speech);
}

void AudioSpeech::StopPlayback() {
    AudioCore::_soloudCore->stop(this->_soundHandler);
}

std::string AudioSpeech::GetTextToSpeak() {
    return this->text_to_speak;
}

void AudioSpeech::SetTextToSpeak(const char * text_to_speak) {
    speech.stop();
    this->text_to_speak = std::string(text_to_speak);
    speech.setText(text_to_speak);
}