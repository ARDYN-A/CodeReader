#pragma once

#include<string>
#include<unordered_set>
#include<unordered_map>
#include<filesystem>
#include<fstream>
#include<queue>
#include<functional>
#include<condition_variable>
#include<mutex>
#include<future>
#include<memory>

struct DirectoryStatistics {
	size_t fileCount = 0;
	size_t lineCount = 0;
	size_t errorCount = 0;
	std::unordered_map<std::string, size_t> lineCountByExtension;
};

struct CountFileLinesResult {
	std::filesystem::path path;
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
	result.path = filePath;
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

class ThreadPool {
private:
	bool stop;
	std::vector<std::thread> workers;
	std::queue<std::function<void()>> tasks;
	std::condition_variable threadCondition;
	std::mutex poolMutex;

public:
	explicit ThreadPool(size_t numThreads) : stop(false) {
		for (int i = 0; i < numThreads; ++i) {
			workers.emplace_back([this] {
				while (true) {
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(poolMutex);
						threadCondition.wait(lock, [this] { return stop || !tasks.empty();});
						if (stop && tasks.empty()) { return; }
						task = std::move(tasks.front());
						tasks.pop();
					}
					task();
				}
			});
		}
	}

	template<typename F, typename... Args>
	auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
		using ReturnType = decltype(f(args...));

		auto boundTask = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

		auto task = std::make_shared<std::packaged_task<ReturnType()>>(boundTask);

		std::future<ReturnType> result = task->get_future();

		{
			std::unique_lock<std::mutex> lock(poolMutex);
			if (stop) throw std::runtime_error("Enqueued after stopping ThreadPool Object");
			tasks.emplace([task]() {(*task)();});
		}
		threadCondition.notify_one();
		return result;
	}

	~ThreadPool() {
		{
			std::unique_lock<std::mutex> lock(poolMutex);
			stop = true;
		}
		threadCondition.notify_all();
		for (std::thread& worker : workers) {
			worker.join();
		}
	}
};
