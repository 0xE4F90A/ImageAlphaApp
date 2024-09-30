#pragma once
#include <windows.h>
#include <string>
#include <vector>

// エンコーダCLSIDを取得する関数
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

// 画像の読み込みと透明度設定
bool LoadImageAndSetAlpha(const std::wstring& inputPath, const std::wstring& outputPath, BYTE alphaValue);

// ファイル選択ダイアログを表示（複数選択可能）
std::vector<std::wstring> OpenMultipleFileDialog();

// 保存先フォルダ選択ダイアログ
std::wstring SelectFolderDialog();

// 透明度の入力を行う関数
int GetAlphaValue(HINSTANCE hInstance);
