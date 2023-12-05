#pragma once

#include "Audio.h"
#include <memory>
#include <string>
#include <map>

class Sound
{
private:
	std::unique_ptr<DirectX::AudioEngine> audioEngine;
	std::map<std::string, std::unique_ptr<DirectX::SoundEffect>> soundEffects;
	std::map<std::string, std::unique_ptr<DirectX::SoundEffectInstance>> soundEffectInstances;

public:
	Sound();
	void Update();

public:
	bool LoadWave(std::wstring fileName, std::string tag);

	DirectX::SoundEffectInstance* GetAudio(std::string tag);
};

