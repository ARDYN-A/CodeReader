#pragma once

#include<string>
#include<array>
#include<filesystem>

template<size_t N>
size_t fileCount(const std::array<std::string, N>& fileExtensions) {
	size_t count = 0;
	for (const auto& entry : std::filesystem::directory_iterator(".")) {
		auto ext = entry.path().extension().string();
		if (std::ranges::find(fileExtensions, ext) != fileExtensions.end()) {
			count++;
		}
	}

	return count;
}