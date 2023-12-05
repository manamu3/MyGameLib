#include "FPS.h"
#include <iostream>

bool FPS::updateFlag;

double FPS::frameRate;

double FPS::deltaTime;

double FPS::actualFPS;

LARGE_INTEGER FPS::freq;

LARGE_INTEGER FPS::updateTime;

void FPS::Initialize(const double _frameRate) {
	// 周波数の取得
	QueryPerformanceFrequency(&freq);

	// パフォーマンスカウンタの取得
	QueryPerformanceCounter(&updateTime);

	// 更新フラグをTrueに設定
	updateFlag = true;

	// 固定したいフレームレートの指定（デフォルト：60fps）
	frameRate = _frameRate;
}

bool FPS::Run(const std::function<bool()> func) {
	LARGE_INTEGER nowTime;

	// 画面更新だったら
	if (updateFlag) {
		// 画面更新したタイミングのパフォーマンスカウンタの取得
		QueryPerformanceCounter(&updateTime);

		// 指定した関数の呼び出し
		bool runFlag = func();

		// 続行フラグがFalseだったら
		if (!runFlag) {
			return false;
		}

		// 画面更新を止める
		updateFlag = false;
	}

	// 現在のパフォーマンスカウンタの取得
	QueryPerformanceCounter(&nowTime);

	// 画面更新してからの時間が越えたら
	if ((((double)nowTime.QuadPart - (double)updateTime.QuadPart) / (double)freq.QuadPart) >= 1.0 / frameRate) {
		// 画面更新フラグをTrueに設定
		updateFlag = true;

		// 経過時間の計測
		deltaTime = ((double)nowTime.QuadPart - (double)updateTime.QuadPart) / (double)freq.QuadPart;

		// 実際のFPSの計測
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