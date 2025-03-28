#ifndef CUBE_H
#define CUBE_H

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "settings.h"
#include "shader.h"
#include "glad/glad.h"
#include "camera.h"
#include "noise.h"

enum class MaterialType : uint8_t {
	MATERIAL_STONE = 0,
	MATERIAL_DIRT,
	MATERIAL_GRASS,
	MATERIAL_SNOW,
};

struct Cube {
	bool active = false;
	MaterialType material = MaterialType::MATERIAL_STONE;
};

struct CubeData {
	uint8_t x;
	uint8_t y;
	uint8_t z;
	uint8_t direction;
	uint8_t colorIndex;
};

struct ChunkKeyFuncs {

	size_t operator()(const glm::vec3& k)const {
		return std::hash<int>()(k.x) ^ std::hash<int>()(k.y) ^ std::hash<int>()(k.z);
	}

	bool operator()(const glm::vec3& a, const glm::vec3& b)const {
		return a.x == b.x && a.y == b.y && a.z == b.z;
	}
};

class Chunk;
class ChunkMesh;

class ChunkMesh {
public:
	ChunkMesh() = default;
	~ChunkMesh();
	void GenerateChunkVertices(const Chunk& chunk);
	void CreateChunkMesh();
	
	size_t IndicesNumber() {
		return m_SolidIndices.size();
	}
public:	
	GLuint vbo = -1, vao = -1, ebo = -1, shader = -1;
	bool wasGenerated = false;
private:
	std::vector<CubeData> m_SolidVertices;
	std::vector<unsigned int> m_SolidIndices;
};

class Chunk {
public:
	Chunk() = default;
	~Chunk() {
		m_Cubes.clear();
	};

	void SetChunkPosition(const glm::vec3 pos);
	void CreateVoxels();
	void SetupTerrain();
	void Draw(const Camera& camera, float timeOfDay);

	const Cube& CubeAt(int x, int y, int z) const {
		int index = z * Settings::CHUNK_WIDTH * Settings::CHUNK_HEIGHT + y * Settings::CHUNK_WIDTH + x;
		if (index < m_Cubes.size())
			return m_Cubes[index];
		else
			return Cube();
	}
	glm::vec3 Position() const {
		return m_GlobalPosition;
	}
private:
	glm::vec3 m_GlobalPosition;
	ChunkMesh m_Mesh;
	std::vector<Cube> m_Cubes;
};
#endif
