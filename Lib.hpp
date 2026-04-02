#pragma once

#include<string>
#include<unordered_set>
#include<filesystem>

std::vector<std::filesystem::path> collectFiles(const std::filesystem::path& root, const std::unordered_set<std::string>& fileExtensions) {
	std::vector<std::filesystem::path> files;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(root, std::filesystem::directory_options::skip_permission_denied)) {
		auto ext = entry.path().extension().string();
		if (fileExtensions.contains(ext)) {
			files.push_back(entry.path());
		}
	}

	return files;
}