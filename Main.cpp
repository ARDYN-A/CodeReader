#include<iostream>
#include<filesystem>
#include"Lib.hpp"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <directory> [extensions...]\n";
		std::cerr << "Example: " << argv[0] << " ./myproject .cpp .hpp .py\n";
		return 1;
	}

	std::filesystem::path root = argv[1];

	if (!std::filesystem::exists(root) || !std::filesystem::is_directory(root)) {
		std::cerr << "Invalid directory: " << root << "\n";
		return 1;
	}

	std::unordered_set<std::string> extensions;
	for (int i = 2; i < argc; i++) {
		std::string ext = argv[i];
		if (ext[0] != '.') {
			ext = '.' + ext;
		}
		extensions.insert(ext);
	}

	DirectoryStatistics stats;

	std::cout << "Scanning: " << root.string() << std::endl << std::endl;

	auto filePaths = collectFilePathsList(root, extensions);

	stats.fileCount = filePaths.size();

	std::cout << "Found " << stats.fileCount << " files\n" << std::endl;

	if (filePaths.empty()) { return 0; }

	for (auto filePath : filePaths) {
		CountFileLinesResult res;
		std::string ext = filePath.extension().string();
		res = countFileLines(filePath);
		stats.lineCount += res.count;
		stats.errorCount += res.error ? 1 : 0;
		stats.lineCountByExtension.insert({ ext, res.count });
	}

	std::cout << "----------------------------" << std::endl;
	std::cout << "Total Lines: " << stats.lineCount << std::endl;
	std::cout << "Total Errors: " << stats.errorCount << std::endl;
	std::cout << "----------------------------" << std::endl;
	std::cout << "Total Lines per Extension:" << std::endl;
	for (const auto& [ext, lineCount] : stats.lineCountByExtension) {
		std::cout << ext << ": " << lineCount << std::endl;
	}
}