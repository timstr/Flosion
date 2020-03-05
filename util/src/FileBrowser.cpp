#include <Flosion/Util/FileBrowser.hpp>

#include <sstream>

#ifdef _WIN32
	#define UNICODE
	#include <windows.h>
	#include <Commdlg.h>
#else
	// TODO: support for Mac and Linux
#endif

namespace util {

    // TODO: find a portable alternative, possibly https://github.com/mlabbe/nativefiledialog

    void FileDialog::addFilter(std::wstring description, std::wstring extension) {
        m_filters.push_back({ std::move(description), L"*." + extension });
    }

	void FileDialog::addFilter(std::wstring description, std::vector<std::wstring> extensions){
		m_filters.push_back({ std::move(description), L"" });
		bool first = true;
		for (const auto& ext : extensions) {
			auto& str = m_filters.back().extension;
			if (!first) {
				str += L';';
			}
			str += L"*.";
			str += ext;
			first = false;
		}
	}

    std::filesystem::path FileDialog::openFile() {
#ifdef _WIN32
		std::vector<wchar_t> filterBuf;

		for (const auto& f : m_filters) {
			copy(begin(f.description), end(f.description), back_inserter(filterBuf));
			filterBuf.push_back(L'\0');
			copy(begin(f.extension), end(f.extension), back_inserter(filterBuf));
			filterBuf.push_back(L'\0');
		}
		filterBuf.push_back(L'\0');

		OPENFILENAME ofn;
		wchar_t szFile[MAX_PATH];

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = filterBuf.data();
		ofn.lpstrDefExt = nullptr;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = nullptr;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

		if (!GetOpenFileName(&ofn)) {
			return std::filesystem::path{};
		}

		return std::filesystem::path{ ofn.lpstrFile };
#else
		// TODO: support for Mac and Linux
		return std::filesystem::path{};
#endif
    }

    std::filesystem::path FileDialog::saveFile(const std::wstring& defaultExtension){
#ifdef _WIN32
		std::vector<wchar_t> filterBuf;

		for (const auto& f : m_filters) {
			copy(begin(f.description), end(f.description), back_inserter(filterBuf));
			filterBuf.push_back(L'\0');
			copy(begin(f.extension), end(f.extension), back_inserter(filterBuf));
			filterBuf.push_back(L'\0');
		}
		filterBuf.push_back(L'\0');

		OPENFILENAME ofn;
		wchar_t szFile[MAX_PATH] = {};

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = filterBuf.data();
		ofn.lpstrDefExt = nullptr;
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = nullptr;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = nullptr;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

		if (!GetOpenFileName(&ofn)) {
			return std::filesystem::path{};
		}

		auto p = std::filesystem::path{ szFile };

		if (p.extension().empty()) {
			p /= (L'.' + defaultExtension);
		}

		return p;
#else
		// TODO: support for Mac and Linux
		return std::filesystem::path{};
#endif
    }

} // namespace util
