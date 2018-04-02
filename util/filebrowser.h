#pragma once

#include <windows.h>
#include <Commdlg.h>
#include <string>

// TODO: this can be done way better
// TODO: find a portable alternative, possibly https://github.com/mlabbe/nativefiledialog

/*
filter must be string with pairs of:
a file description ending with \0 (i.e. [Text Files\0]
a file extension filter ending with \0 (i.e. [*.txt\0]
if more than one file extension is desired place semicolons between them

for example: L"Image Files\0*.jpg;*.png;*.bmp\0Text Files\0*.txt\0"
will accept image files of type .jpg, .png and .bmp, or text files of type .txt

default_ext is automatically appended (first 3 characters only) if no extension is provided, and should look like L"txt"
*/
std::string openFileDialog(const wchar_t* filter, const wchar_t* default_ext = nullptr){
	OPENFILENAME ofn;
	wchar_t szFile[MAX_PATH];

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFile = (LPSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = (LPSTR)filter;
	ofn.lpstrDefExt = (LPSTR)default_ext;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = nullptr;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

	GetOpenFileName(&ofn);

	std::wstring wfilename = std::wstring((wchar_t*)ofn.lpstrFile);
	std::string filename = std::string(wfilename.begin(), wfilename.end());
	return filename;
}

/*
filter must be string with pairs of:
a file description ending with \0 (i.e. [Text Files\0]
a file extension filter ending with \0 (i.e. [*.txt\0]
if more than one file extension is desired place semicolons between them

for example: L"Image Files\0*.jpg;*.png;*.bmp\0Text Files\0*.txt\0"
will accept image files of type .jpg, .png and .bmp, or text files of type .txt

default_ext is automatically appended (first 3 characters only) if no extension is provided, and should look like L"txt"
*/
std::string saveFileDialog(const wchar_t* filter, const wchar_t* default_ext = nullptr){
	wchar_t exepath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, (LPSTR)exepath);

	OPENFILENAME ofn;
	wchar_t szFile[MAX_PATH];
	HWND hwnd = nullptr;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = (LPSTR)szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = (LPSTR)filter;
	ofn.lpstrDefExt = (LPSTR)default_ext;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = nullptr;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = nullptr;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn)){
		std::wstring wfname = std::wstring((wchar_t*)ofn.lpstrFile);
		std::string fname = std::string(wfname.begin(), wfname.end());
		SetCurrentDirectory((LPSTR)exepath);
		return fname;
	} else {
		SetCurrentDirectory((LPSTR)exepath);
		return "";
	}
}