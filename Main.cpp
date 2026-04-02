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

	std::cout << "Scanning: " << root.string() << std::endl;

	auto files = collectFiles(root, extensions);

	std::cout << "Found " << files.size() << " files:" << std::endl;

	if (files.empty()) { return 0; }

	for (auto file : files) {
		std::cout << file.string() << std::endl;
	}
}