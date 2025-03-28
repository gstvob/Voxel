#ifndef CHUNK_MANAGER_H
#define CHUNK_MANAGER_H

#include <unordered_map>
#include <queue>
#include <thread>
#include <mutex>
#include "cube.h"

namespace world {
	void Init();
	static void UpdateDrawQueue(const Camera& camera);
	void Draw(const Camera& camera, float timeOfDay);
	void Clear();

	extern int ChunkCount();
	extern int ChunkQueueCount();
}


#endif