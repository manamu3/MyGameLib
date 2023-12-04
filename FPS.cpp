#include "FPS.h"
#include <iostream>

/// <summary>
/// ��ʍX�V�p�̃t���O
/// </summary>
bool FPS::updateFlag;

/// <summary>
/// �t���[�����[�g
/// </summary>
double FPS::frameRate;

/// <summary>
/// ��ʍX�V�Ԋu�̎���
/// </summary>
double FPS::deltaTime;

/// <summary>
/// ���ۂ�FPS
/// </summary>
double FPS::actualFPS;

/// <summary>
/// ���g��
/// </summary>
LARGE_INTEGER FPS::freq;

/// <summary>
/// �p�t�H�[�}���X�J�E���^
/// </summary>
LARGE_INTEGER FPS::updateTime;

void FPS::Initialize(const double _frameRate) {
	// ���g���̎擾
	QueryPerformanceFrequency(&freq);

	// �p�t�H�[�}���X�J�E���^�̎擾
	QueryPerformanceCounter(&updateTime);

	// �X�V�t���O��True�ɐݒ�
	updateFlag = true;

	// �Œ肵�����t���[�����[�g�̎w��i�f�t�H���g�F60fps�j
	frameRate = _frameRate;
}

bool FPS::Run(const std::function<bool()> func) {
	LARGE_INTEGER nowTime;

	// ��ʍX�V��������
	if (updateFlag) {
		// ��ʍX�V�����^�C�~���O�̃p�t�H�[�}���X�J�E���^�̎擾
		QueryPerformanceCounter(&updateTime);

		// �w�肵���֐��̌Ăяo��
		bool runFlag = func();

		// ���s�t���O��False��������
		if (!runFlag) {
			return false;
		}

		// ��ʍX�V���~�߂�
		updateFlag = false;
	}

	// ���݂̃p�t�H�[�}���X�J�E���^�̎擾
	QueryPerformanceCounter(&nowTime);

	// ��ʍX�V���Ă���̎��Ԃ��z������
	if ((((double)nowTime.QuadPart - (double)updateTime.QuadPart) / (double)freq.QuadPart) >= 1.0 / frameRate) {
		// ��ʍX�V�t���O��True�ɐݒ�
		updateFlag = true;

		// �o�ߎ��Ԃ̌v��
		deltaTime = ((double)nowTime.QuadPart - (double)updateTime.QuadPart) / (double)freq.QuadPart;

		// ���ۂ�FPS�̌v��
		actualFPS = 1.0 / deltaTime;
	}

	return true;
}

double FPS::GetFPS() {
	return actualFPS;
}

double FPS::GetDeltaTime() {
	return deltaTime;
}