#pragma once
#include <windows.h>
#include <string>
#include <vector>

// �G���R�[�_CLSID���擾����֐�
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

// �摜�̓ǂݍ��݂Ɠ����x�ݒ�
bool LoadImageAndSetAlpha(const std::wstring& inputPath, const std::wstring& outputPath, BYTE alphaValue);

// �t�@�C���I���_�C�A���O��\���i�����I���\�j
std::vector<std::wstring> OpenMultipleFileDialog();

// �ۑ���t�H���_�I���_�C�A���O
std::wstring SelectFolderDialog();

// �����x�̓��͂��s���֐�
int GetAlphaValue(HINSTANCE hInstance);
