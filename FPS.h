#pragma once
#include <functional>
#include <windows.h>

class FPS
{
private:
	// 画面更新用のフラグ
	static bool updateFlag;

	// フレームレート
	static double frameRate;

	// 画面更新間隔の時間
	static double deltaTime;

	// 実際のFPS
	static double actualFPS;

	// 周波数
	static LARGE_INTEGER freq;

	// パフォーマンスカウンタ
	static LARGE_INTEGER updateTime;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="_frameRate">フレームレートの指定（デフォルト：60fps）</param>
	static void Initialize(const double _frameRate = 60.0);

	/// <summary>
	/// FPS固定
	/// </summary>
	/// <param name="func">実行する関数</param>
	
	/// <summary>
	/// FPS固定
	/// </summary>
	/// <param name="func">実行する関数</param>
	/// <returns>続行判定（False: 終了　True: 続行）</returns>
	static bool Run(const std::function<bool()> func);

	/// <summary>
	/// FPSの取得
	/// </summary>
	/// <returns></returns>
	static double GetFPS();

	/// <summary>
	/// 画面更新間隔の時間を取得
	/// </summary>
	/// <returns></returns>
	static double GetDeltaTime();
};