#include "ImageAlpha.h"
#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <vector>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    // 画像ファイルを選択
    std::vector<std::wstring> inputPaths = OpenMultipleFileDialog();
    if (inputPaths.empty())
    {
        MessageBox(nullptr, L"画像ファイルを選択してください。", L"エラー", MB_OK);
        GdiplusShutdown(gdiplusToken);
        return -1;
    }

    // 透明度の入力
    int alphaValue = GetAlphaValue(hInstance);
    if (alphaValue <= 0)
    {
        GdiplusShutdown(gdiplusToken);
        return -1;
    }

    // 保存先フォルダを選択
    std::wstring outputFolder = SelectFolderDialog();
    if (outputFolder.empty())
    {
        MessageBox(nullptr, L"保存先フォルダを選択してください。", L"エラー", MB_OK);
        GdiplusShutdown(gdiplusToken);
        return -1;
    }

    // 処理中の表示
    HWND hProgress = CreateWindowW(L"STATIC", L"処理中...", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 100, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hProgress, SW_SHOW);
    UpdateWindow(hProgress);

    // 透明度を0～255に変換
    BYTE alpha = static_cast<BYTE>((alphaValue * 255) / 100);

    // 各画像ファイルに対して処理を行う
    for (const auto& inputPath : inputPaths)
    {
        std::wstring outputPath = outputFolder + L"\\" + inputPath.substr(inputPath.find_last_of(L"\\") + 1);
        size_t dotPos = outputPath.find_last_of(L'.');
        if (dotPos != std::wstring::npos)
        {
            outputPath.insert(dotPos, L"_alpha" + std::to_wstring(alphaValue));
        }
        else
        {
            outputPath += L"_alpha" + std::to_wstring(alphaValue) + L".png";
        }

        if (!LoadImageAndSetAlpha(inputPath, outputPath, alpha))
        {
            MessageBox(nullptr, (L"画像の処理に失敗しました: " + inputPath).c_str(), L"エラー", MB_OK);
        }
    }

    // 処理中の表示を隠す
    DestroyWindow(hProgress);

    // 完了メッセージ
    MessageBox(nullptr, L"すべての画像の変換が完了しました。", L"完了", MB_OK);

    GdiplusShutdown(gdiplusToken);
    return 0;
}
