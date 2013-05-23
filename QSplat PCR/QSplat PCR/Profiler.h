#pragma once

#include <chrono>
#include <mutex>

using namespace std::chrono;

class Profiler {
private:
	time_point<system_clock> frameStart;
	float frameTime;

	time_point<system_clock> updateStart;
	float updateTime;

	int depth;
	int maxDepth;
	std::mutex depthMutex;

public:

	///<summary>
	/// Construct a Profiler object
	///</summary>
	Profiler() : depth(0), maxDepth(0), frameTime(0), updateTime(0) {
	}
	
	///<summary>
	/// Frame start tick
	///</summary>
	void FrameStart() {
		frameStart = system_clock::now();
	}

	///<summary>
	/// Frame end toc
	///</summary>
	void FrameEnd() {
		frameTime = duration_cast<milliseconds>(system_clock::now()-frameStart).count();
	}

	void ResponseStartTime() {
	}

	void ResponseEndTime() {
	}

	///<summary>
	/// Update start tick
	///</summary>
	void UpdateStart() {
		updateStart = system_clock::now();
	}

	///<summary>
	/// Update end toc
	///</summary>
	void UpdateEnd() {
		updateTime = duration_cast<microseconds>(system_clock::now()-updateStart).count();
	}

	///<summary>
	/// Set Current Depth of traversal
	///</summary>
	///<param name="depth">Current depth</param>
	void SetCurrentDepth(int depth) {
		std::lock_guard<std::mutex> lock(depthMutex);

		this->depth = depth;
		if (depth > maxDepth)
			maxDepth = depth;
	}

	///<summary>
	/// Get Current Depth of traversal
	///</summary>
	float GetDepth() {
		std::lock_guard<std::mutex> lock(depthMutex);

		return depth;
	}

	///<summary>
	/// Get Current Render Time
	///</summary>
	float GetCurrentFrameRate() {
		//return frameTime;
		//if (frameTime == 0) return 999.9f;
		return 1000 / frameTime;
	}

	///<summary>
	/// Get Current Swap Time
	///</summary>
	float GetCurrentUpdateRate() {
		return updateTime;
		if (updateTime == 0) return 999.9f;
		return 1000 / updateTime;
	}

	int GetResponseInputLag() {
		return 0;
	}

	int GetResponseCompleteLag() {
		return 0;
	}

	///<summary>
	/// Get Maximum Depth of traversal
	///</summary>
	float GetMaxDepth() {
		std::lock_guard<std::mutex> lock(depthMutex);

		return maxDepth;
	}
};

