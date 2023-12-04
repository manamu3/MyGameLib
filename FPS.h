#pragma once
#include <functional>
#include <windows.h>

class FPS
{
private:
	/// <summary>
	/// ��ʍX�V�p�̃t���O
	/// </summary>
	static bool updateFlag;

	/// <summary>
	/// �t���[�����[�g
	/// </summary>
	static double frameRate;

	/// <summary>
	/// ��ʍX�V�Ԋu�̎���
	/// </summary>
	static double deltaTime;

	/// <summary>
	/// ���ۂ�FPS
	/// </summary>
	static double actualFPS;

	/// <summary>
	/// ���g��
	/// </summary>
	static LARGE_INTEGER freq;

	/// <summary>
	/// �p�t�H�[�}���X�J�E���^
	/// </summary>
	static LARGE_INTEGER updateTime;

public:
	/// <summary>
	/// ������
	/// </summary>
	/// <param name="_frameRate">�t���[�����[�g�̎w��i�f�t�H���g�F60fps�j</param>
	static void Initialize(const double _frameRate = 60.0);

	/// <summary>
	/// FPS�Œ�
	/// </summary>
	/// <param name="func">���s����֐�</param>
	
	/// <summary>
	/// FPS�Œ�
	/// </summary>
	/// <param name="func">���s����֐�</param>
	/// <returns>���s����iFalse: �I���@True: ���s�j</returns>
	static bool Run(const std::function<bool()> func);

	/// <summary>
	/// FPS�̎擾
	/// </summary>
	/// <returns></returns>
	static double GetFPS();

	/// <summary>
	/// ��ʍX�V�Ԋu�̎��Ԃ��擾
	/// </summary>
	/// <returns></returns>
	static double GetDeltaTime();
};