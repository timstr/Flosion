#pragma once

#include <filesystem>
#include <string>

namespace util {

	/**
	 * Helper class for "Open File" and "Save File" dialogs
	 * Example usage:
	 *
	 *     auto fd = FileDialog{};
	 *     fd.addFilter(L"Text Files", L"txt");
	 *     fd.addFilter(L"HTML Files", {L"html", L"htm", L"xhtml"});
	 *     if (auto path = fd.openFile()){
	 *         // Do stuff with the file path
	 *     } else {
	 *         // Do stuff without a file :(
	 *     }
	 * 
	 */
	class FileDialog {
	public:

		void addFilter(std::wstring description, std::wstring extension);

		void addFilter(std::wstring description, std::vector<std::wstring> extensions);

		/**
		 * Opens a "Open File" dialog with the added filters and returns the path to the chosen file,
		 * or nothing if the user chose to cancel.
		 */
		std::filesystem::path openFile();

		/**
		 * Opens a "Save File" dialog with the added filters and returns the path to the chosen file,
		 * which may or may not exist, or nothing, if the user chose to cancel.
		 * The provided default extension is appended if none is provided.
		 */
		std::filesystem::path saveFile(const std::wstring& defaultExtension = L"");

	private:
		struct Filter {
			std::wstring description;
			std::wstring extension;
		};

		std::vector<Filter> m_filters;
	};

} // namespace util
