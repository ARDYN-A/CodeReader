#pragma once

#include"Lib.h"
#include<filesystem>

uint32_t fileCount(std::string filter) {
	uint32_t count = 0;
	for (const auto& entry : std::filesystem::directory_iterator(".")) {
		if (entry.path().extension() == filter) {
			count++;
		}
	}

	return count;
}