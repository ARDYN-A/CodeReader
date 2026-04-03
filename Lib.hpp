#pragma once

#include<string>
#include<unordered_set>
#include<unordered_map>
#include<filesystem>
#include<fstream>

struct DirectoryStatistics {
	size_t fileCount = 0;
	size_t lineCount = 0;
	size_t errorCount = 0;
	std::unordered_map<std::string, size_t> lineCountByExtension;
};

struct CountFileLinesResult {
	size_t count = 0;
	bool error = false;
};

std::vector<std::filesystem::path> collectFilePathsList(const std::filesystem::path& root, const std::unordered_set<std::string>& fileExtensions) {
	std::vector<std::filesystem::path> files;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(root, std::filesystem::directory_options::skip_permission_denied)) {
		std::string ext = entry.path().extension().string();
		if (fileExtensions.contains(ext)) {
			files.push_back(entry.path());
		}
	}

	return files;
}

CountFileLinesResult countFileLines(const std::filesystem::path& filePath) {
	CountFileLinesResult result;
	std::ifstream file(filePath);

	if (!file.is_open()) {
		result.error = true;
		return result;
	}

	result.count = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');

	if (!file.eof() && result.count > 0) {
		result.count += 1;
	}

	return result;
}