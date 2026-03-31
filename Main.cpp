#include<iostream>
#include<filesystem>
#include"Lib.h"

int main() {
	std::cout << "Number of C++ files: " << fileCount(".cpp") << std::endl;
}