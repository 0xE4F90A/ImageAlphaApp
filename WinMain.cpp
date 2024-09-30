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

    // �摜�t�@�C����I��
    std::vector<std::wstring> inputPaths = OpenMultipleFileDialog();
    if (inputPaths.empty())
    {
        MessageBox(nullptr, L"�摜�t�@�C����I�����Ă��������B", L"�G���[", MB_OK);
        GdiplusShutdown(gdiplusToken);
        return -1;
    }

    // �����x�̓���
    int alphaValue = GetAlphaValue(hInstance);
    if (alphaValue <= 0)
    {
        GdiplusShutdown(gdiplusToken);
        return -1;
    }

    // �ۑ���t�H���_��I��
    std::wstring outputFolder = SelectFolderDialog();
    if (outputFolder.empty())
    {
        MessageBox(nullptr, L"�ۑ���t�H���_��I�����Ă��������B", L"�G���[", MB_OK);
        GdiplusShutdown(gdiplusToken);
        return -1;
    }

    // �������̕\��
    HWND hProgress = CreateWindowW(L"STATIC", L"������...", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 100, nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hProgress, SW_SHOW);
    UpdateWindow(hProgress);

    // �����x��0�`255�ɕϊ�
    BYTE alpha = static_cast<BYTE>((alphaValue * 255) / 100);

    // �e�摜�t�@�C���ɑ΂��ď������s��
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
            MessageBox(nullptr, (L"�摜�̏����Ɏ��s���܂���: " + inputPath).c_str(), L"�G���[", MB_OK);
        }
    }

    // �������̕\�����B��
    DestroyWindow(hProgress);

    // �������b�Z�[�W
    MessageBox(nullptr, L"���ׂẲ摜�̕ϊ����������܂����B", L"����", MB_OK);

    GdiplusShutdown(gdiplusToken);
    return 0;
}
