#include<iostream>
#include<filesystem>
#include"Lib.h"

int main() {
	const std::array<std::string, 2> filter = { ".cpp", ".h" };
	std::cout << "Number of C++ files: " << fileCount(filter) << std::endl;
}