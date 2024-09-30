#include "ImageAlpha.h"
#include <gdiplus.h>
#include <commdlg.h>
#include <shlobj.h>
#include <string>
#include <vector>

using namespace Gdiplus;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT num = 0;
    UINT size = 0;

    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;

    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == nullptr)
        return -1;

    GetImageEncoders(num, size, pImageCodecInfo);
    if (pImageCodecInfo == nullptr)
    {
        free(pImageCodecInfo);
        return -1;
    }

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

bool LoadImageAndSetAlpha(const std::wstring& inputPath, const std::wstring& outputPath, BYTE alphaValue)
{
    Image* image = new Image(inputPath.c_str());
    if (image->GetLastStatus() != Ok)
    {
        delete image;
        return false;
    }

    UINT width = image->GetWidth();
    UINT height = image->GetHeight();

    Bitmap* bitmap = new Bitmap(width, height, PixelFormat32bppARGB);
    Graphics graphics(bitmap);
    graphics.DrawImage(image, 0, 0, width, height);
    delete image;

    for (UINT y = 0; y < height; y++)
    {
        for (UINT x = 0; x < width; x++)
        {
            Color color;
            bitmap->GetPixel(x, y, &color);

            if (color.GetAlpha() != 0)
            {
                Color newColor(alphaValue, color.GetRed(), color.GetGreen(), color.GetBlue());
                bitmap->SetPixel(x, y, newColor);
            }
        }
    }

    CLSID clsid;
    GetEncoderClsid(L"image/png", &clsid);
    bitmap->Save(outputPath.c_str(), &clsid, nullptr);
    delete bitmap;

    return true;
}

std::vector<std::wstring> OpenMultipleFileDialog()
{
    OPENFILENAME ofn;
    std::vector<WCHAR> fileNames(MAX_PATH * 100, L'\0');

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = L"画像ファイル\0*.bmp;*.jpg;*.jpeg;*.png\0すべてのファイル\0*.*\0";
    ofn.lpstrFile = fileNames.data();
    ofn.nMaxFile = static_cast<DWORD>(fileNames.size());
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    std::vector<std::wstring> files;
    if (GetOpenFileName(&ofn))
    {
        WCHAR* p = fileNames.data();
        std::wstring directory = p;
        p += directory.length() + 1;
        if (*p == '\0')
        {
            files.push_back(directory);
        }
        else
        {
            while (*p)
            {
                files.push_back(directory + L"\\" + p);
                p += wcslen(p) + 1;
            }
        }
    }

    return files;
}

std::wstring SelectFolderDialog()
{
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = L"保存先フォルダを選択してください";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl != nullptr)
    {
        WCHAR path[MAX_PATH];
        if (SHGetPathFromIDList(pidl, path))
        {
            CoTaskMemFree(pidl);
            return std::wstring(path);
        }
        CoTaskMemFree(pidl);
    }

    return L"";
}

LRESULT CALLBACK AlphaInputWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hEdit;
    static int* pAlphaValue;

    switch (msg)
    {
    case WM_CREATE:
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pAlphaValue = (int*)pCreate->lpCreateParams;

        hEdit = CreateWindowW(L"EDIT", L"50", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            20, 20, 100, 25, hwnd, (HMENU)1, pCreate->hInstance, nullptr);

        CreateWindowW(L"BUTTON", L"OK", WS_CHILD | WS_VISIBLE,
            130, 20, 50, 25, hwnd, (HMENU)2, pCreate->hInstance, nullptr);
    }
    break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 2)
        {
            wchar_t text[4];
            GetWindowTextW(hEdit, text, 4);
            int alphaValue = _wtoi(text);
            if (alphaValue < 1 || alphaValue > 100)
            {
                MessageBox(hwnd, L"1～100の範囲で入力してください。", L"エラー", MB_OK);
            }
            else
            {
                *pAlphaValue = alphaValue;
                DestroyWindow(hwnd);
                PostQuitMessage(0);
            }
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int GetAlphaValue(HINSTANCE hInstance)
{
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = AlphaInputWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"AlphaInputClass";
    RegisterClass(&wc);

    int alphaValue = 50;

    HWND hwnd = CreateWindowW(L"AlphaInputClass", L"透明度を入力してください", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 250, 120, nullptr, nullptr, hInstance, (LPVOID)&alphaValue);

    if (hwnd == nullptr)
    {
        return 0;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return alphaValue;
}
