#include<iostream>
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

	std::vector<std::filesystem::path> filePaths = collectFilePathsList(root, extensions);

	stats.fileCount = filePaths.size();

	std::cout << "Found " << stats.fileCount << " files\n" << std::endl;

	if (filePaths.empty()) { return 0; }

	size_t numThreads = std::max(1u, std::thread::hardware_concurrency());
	ThreadPool tp(numThreads);
	std::cout << "Using " << numThreads << " threads" << std::endl << std::endl;

	std::vector<std::future<CountFileLinesResult>> futures;
	futures.reserve(filePaths.size());

	for (const auto& filePath : filePaths) {
		futures.emplace_back(tp.enqueue(countFileLines, filePath));
	}

	for (std::future<CountFileLinesResult>& future : futures) {
		CountFileLinesResult result = future.get();
		if (result.error) {
			++stats.errorCount;
			std::cerr << "Error reading: " << result.path << std::endl;
		}
		else {
			++stats.fileCount;
			stats.lineCount += result.count;
			stats.lineCountByExtension[result.path.extension().string()] += result.count;
		}
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