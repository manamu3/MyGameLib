#include "Sound.h"

Sound::Sound() {
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;
#ifdef _DEBUG
	eflags |= DirectX::AudioEngine_Debug;
#endif
	audioEngine = std::make_unique<DirectX::AudioEngine>(eflags);
}

void Sound::Update() {
	if (!audioEngine->Update()) {
		// No audio device is active
		if (audioEngine->IsCriticalError()) {

		}
	}
}

bool Sound::LoadWave(std::wstring fileName, std::string tag) {
	if (soundEffectInstances.find(tag) != soundEffectInstances.end()) {
		return false;
	}
	soundEffects[tag] = std::make_unique<DirectX::SoundEffect>(audioEngine.get(), fileName.c_str());
	soundEffectInstances[tag] = soundEffects[tag]->CreateInstance();

	return true;
}

DirectX::SoundEffectInstance* Sound::GetAudio(std::string tag) {
	if (soundEffectInstances.find(tag) == soundEffectInstances.end()) {
		return nullptr;
	}

	return soundEffectInstances[tag].get();
}