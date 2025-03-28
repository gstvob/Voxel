#include "chunkmanager.h"

namespace world {
    bool isRunning;
    std::unordered_map<glm::vec3, std::unique_ptr<Chunk>, ChunkKeyFuncs, ChunkKeyFuncs> chunks;
    std::queue<glm::vec3> chunkQueue;

    std::mutex chunkQueueMutex;
    std::mutex chunkMutex;
}

void world::Init() {
    isRunning = false;
    std::thread executeFromQueueThread([]() {
        while (!isRunning) {
            if (!chunkQueue.empty()) {
                chunkQueueMutex.lock();
                glm::vec3 chunkPosition = chunkQueue.back();
                chunkQueue.pop();
                chunkQueueMutex.unlock();

                if (chunks.find(chunkPosition) == chunks.end() || chunks[chunkPosition] == nullptr) {
                    Chunk chunk = Chunk();
                    chunk.SetChunkPosition(chunkPosition);
                    chunk.CreateVoxels();
                    chunk.SetupTerrain();
                    chunkMutex.lock();
                    chunks[chunkPosition] = std::make_unique<Chunk>(chunk);
                    chunkMutex.unlock();
                }
            }
        }
    });
    executeFromQueueThread.detach();
}

void world::UpdateDrawQueue(const Camera& camera) {
    float chunkX = camera.m_Position.x > 0 ? std::floor(camera.m_Position.x / Settings::CHUNK_WIDTH) : std::ceilf(camera.m_Position.x / Settings::CHUNK_WIDTH);
    float chunkY = 0.0f;
    float chunkZ = camera.m_Position.z > 0 ? std::floor(camera.m_Position.z / Settings::CHUNK_LENGTH) : std::ceilf(camera.m_Position.z / Settings::CHUNK_LENGTH);
    glm::vec3 cameraChunkPosition = glm::vec3(chunkX * Settings::CHUNK_WIDTH, chunkY, -chunkZ * Settings::CHUNK_LENGTH);

    for (float x = 0.f; x < 14.f; x++) {
        for (float z = 0.f; z < 14.f; z++) {
            glm::vec3 chunkOne = glm::vec3(x * Settings::CHUNK_WIDTH, 0.0f, z * Settings::CHUNK_LENGTH) + cameraChunkPosition;
            glm::vec3 chunkTwo = glm::vec3(-x * Settings::CHUNK_WIDTH, 0.0f, z * Settings::CHUNK_LENGTH) + cameraChunkPosition;
            glm::vec3 chunkThree = glm::vec3(x * Settings::CHUNK_WIDTH, 0.0f, -z * Settings::CHUNK_LENGTH) + cameraChunkPosition;
            glm::vec3 chunkFour = glm::vec3(-x * Settings::CHUNK_WIDTH, 0.0f, -z * Settings::CHUNK_LENGTH) + cameraChunkPosition;

            chunkMutex.lock();
            if (std::abs(glm::distance(cameraChunkPosition, chunkOne)) < Settings::CHUNK_WIDTH * 14
                && (chunks.find(chunkOne) == chunks.end() || chunks[chunkOne] == nullptr)) {
                chunkQueueMutex.lock();
                chunkQueue.push(chunkOne);
                chunkQueueMutex.unlock();
            }

            if (std::abs(glm::distance(cameraChunkPosition, chunkTwo)) < Settings::CHUNK_WIDTH * 14
                && (chunks.find(chunkTwo) == chunks.end() || chunks[chunkTwo] == nullptr)) {
                chunkQueueMutex.lock();
                chunkQueue.push(chunkTwo);
                chunkQueueMutex.unlock();
            }

            if (std::abs(glm::distance(cameraChunkPosition, chunkThree)) < Settings::CHUNK_WIDTH * 14
                && (chunks.find(chunkThree) == chunks.end() || chunks[chunkThree] == nullptr)) {
                chunkQueueMutex.lock();
                chunkQueue.push(chunkThree);
                chunkQueueMutex.unlock();
            }

            if (std::abs(glm::distance(cameraChunkPosition, chunkFour)) < Settings::CHUNK_WIDTH * 14
                && (chunks.find(chunkFour) == chunks.end() || chunks[chunkFour] == nullptr)) {
                chunkQueueMutex.lock();
                chunkQueue.push(chunkFour);
                chunkQueueMutex.unlock();
            }
            chunkMutex.unlock();
        }
    }


    for (auto& [position, chunk] : chunks) {
        if (chunk != nullptr) {
            if (std::abs(glm::distance(cameraChunkPosition, position) > Settings::CHUNK_WIDTH * 14)) {
                chunkMutex.lock();
                chunks[position] = nullptr;
                chunkMutex.unlock();
                continue;
            }
        }
    }
}

void world::Draw(const Camera& camera, float timeOfDay) {
    world::UpdateDrawQueue(camera);
    chunkMutex.lock();
    for (auto& [position, chunk] : chunks) {
        if (chunk == nullptr) {
            continue;
        }
        chunk->Draw(camera, timeOfDay);
    }
    chunkMutex.unlock();
}

void world::Clear() {
    isRunning = false;
    chunks.clear();
    for (int i = 0; i < chunkQueue.size(); i++) {
        chunkQueue.pop();
    }
}

int world::ChunkCount() {
    return chunks.size();
}

int world::ChunkQueueCount() {
    return chunkQueue.size();
}
