#include "cube.h"

void Chunk::SetChunkPosition(const glm::vec3 pos) {
    m_GlobalPosition = pos;
}

void Chunk::CreateVoxels() {
    m_Cubes.clear();
    for (int x = 0; x < Settings::CHUNK_WIDTH; x++) {
        for (int y = 0; y < Settings::CHUNK_HEIGHT; y++) {
            for (int z = 0; z < Settings::CHUNK_LENGTH; z++) {
                Cube cube = {
                    false,
                    MaterialType::MATERIAL_STONE,
                };
                m_Cubes.push_back(cube);
            }
        }
    }
    m_Mesh = ChunkMesh();
}

void Chunk::SetupTerrain() {
    for (int x = 0; x < Settings::CHUNK_WIDTH; x++) {
        for (int z = 0; z < Settings::CHUNK_LENGTH; z++) {
            float height = noise::getHeight(x + m_GlobalPosition.x, z + m_GlobalPosition.z);
            height = noise::cubicInterpolation(1.0f, Settings::CHUNK_HEIGHT, height);
            if (height > Settings::CHUNK_HEIGHT) {
                height = Settings::CHUNK_HEIGHT;
            } if (height <= 0) {
                height = 1;
            }
            for (int y = 0; y < height; y++) {
                int index = (z * Settings::CHUNK_WIDTH * Settings::CHUNK_HEIGHT) + (y * Settings::CHUNK_WIDTH) + x;
                m_Cubes[index].active = true;
                if (y >= 0 && y < 10){
                    m_Cubes[index].material = MaterialType::MATERIAL_STONE;
                } else if (y >= 10 && y < 30) {
                    m_Cubes[index].material = MaterialType::MATERIAL_DIRT;
                } else if (y >= 30 && y < 100) {
                    m_Cubes[index].material = MaterialType::MATERIAL_GRASS;
                } else {
                    m_Cubes[index].material = MaterialType::MATERIAL_SNOW;
                }
            }
        }
    }
}

void Chunk::Draw(const Camera& camera, float timeOfDay) {

    if (!m_Mesh.wasGenerated) {
        m_Mesh.GenerateChunkVertices(*this);
        m_Mesh.CreateChunkMesh();
        m_Mesh.wasGenerated = true;
    }

    glUseProgram(m_Mesh.shader);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
    model = glm::translate(model, glm::vec3(m_GlobalPosition.x, m_GlobalPosition.y, -m_GlobalPosition.z));
    glm::mat4 view = camera.GetView();

    float angle = timeOfDay * 360.f;
    glm::vec3 sunDirection = glm::vec3(sin(glm::radians(angle)), cos(glm::radians(angle)), 0.0f);
    glm::vec3 lightColor = glm::mix(glm::vec3(1.0f, 0.9f, 0.8f), glm::vec3(0.1f, 0.1f, 0.5f), timeOfDay);

    glUniform3fv(glGetUniformLocation(m_Mesh.shader, "lightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(m_Mesh.shader, "sunDirection"), 1, glm::value_ptr(sunDirection));
    glUniformMatrix4fv(glGetUniformLocation(m_Mesh.shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(m_Mesh.shader, "view"), 1, GL_FALSE, glm::value_ptr(view));

    glBindVertexArray(m_Mesh.vao);
    glDrawElements(GL_TRIANGLES, m_Mesh.IndicesNumber(), GL_UNSIGNED_INT, 0);
}

ChunkMesh::~ChunkMesh() {

    if (shader != -1) {
        glDeleteProgram(shader);
    }
    if (ebo != -1) {
        glDeleteBuffers(1, &ebo);
    } 
    if (vbo != -1) {
        glDeleteBuffers(1, &vbo);
    }
    if (vao != -1) {
        glDeleteVertexArrays(1, &vao);
    }
}

void ChunkMesh::GenerateChunkVertices(const Chunk& chunk) {
    int cubeIndex = 0;

    for (int x = 0; x < Settings::CHUNK_WIDTH; x++) {
        for (int y = 0; y < Settings::CHUNK_HEIGHT; y++) {
            for (int z = 0; z < Settings::CHUNK_LENGTH; z++) {
                if (chunk.CubeAt(x,y,z).active) {
                    bool generateUp = true, 
                        generateFront = true, 
                        generateLeft = true, 
                        generateRight = true, 
                        generateBack = true, 
                        generateBottom = true;

                    if (x - 1 >= 0 && chunk.CubeAt(x - 1, y, z).active) generateLeft = false;
                    if (x + 1 < Settings::CHUNK_WIDTH && chunk.CubeAt(x + 1, y, z).active) generateRight = false;
                    if (y - 1 >= 0 && chunk.CubeAt(x, y - 1, z).active) generateBottom = false;
                    if (y + 1 < Settings::CHUNK_HEIGHT && chunk.CubeAt(x, y + 1, z).active) generateUp = false;
                    if (z - 1 >= 0 && chunk.CubeAt(x, y, z - 1).active) generateBack = false;
                    if (z + 1 < Settings::CHUNK_LENGTH && chunk.CubeAt(x, y, z + 1).active) generateFront = false;

                    uint8_t color = (uint8_t)chunk.CubeAt(x, y, z).material;

                    if (generateFront) {
                        m_SolidIndices.push_back(cubeIndex);
                        m_SolidIndices.push_back(cubeIndex + 1);
                        m_SolidIndices.push_back(cubeIndex + 2);
                        m_SolidIndices.push_back(cubeIndex + 2);
                        m_SolidIndices.push_back(cubeIndex + 3);
                        m_SolidIndices.push_back(cubeIndex);

                        m_SolidVertices.push_back(CubeData{ uint8_t(0 + x), uint8_t(0 + y), uint8_t(0 - z), 0, color });
                        m_SolidVertices.push_back(CubeData{ uint8_t(1 + x), uint8_t(0 + y), uint8_t(0 - z), 0, color });
                        m_SolidVertices.push_back(CubeData{ uint8_t(1 + x), uint8_t(1 + y), uint8_t(0 - z), 0, color });
                        m_SolidVertices.push_back(CubeData{ uint8_t(0 + x), uint8_t(1 + y), uint8_t(0 - z), 0, color });
                        cubeIndex += 4;
                    }
                    if (generateUp) {
                        m_SolidIndices.push_back(cubeIndex);
                        m_SolidIndices.push_back(cubeIndex + 1);
                        m_SolidIndices.push_back(cubeIndex + 2);
                        m_SolidIndices.push_back(cubeIndex + 2);
                        m_SolidIndices.push_back(cubeIndex + 3);
                        m_SolidIndices.push_back(cubeIndex);

                        m_SolidVertices.push_back(CubeData{ uint8_t(0 + x), uint8_t(1 + y), uint8_t(0 - z), 1, color }); // 3
                        m_SolidVertices.push_back(CubeData{ uint8_t(1 + x), uint8_t(1 + y), uint8_t(0 - z), 1, color }); // 2
                        m_SolidVertices.push_back(CubeData{ uint8_t(1 + x), uint8_t(1 + y), uint8_t(1 - z), 1, color }); // 4
                        m_SolidVertices.push_back(CubeData{ uint8_t(0 + x), uint8_t(1 + y), uint8_t(1 - z), 1, color }); // 5
                        cubeIndex += 4;
                    }
                    if (generateLeft) {
                        m_SolidIndices.push_back(cubeIndex);
                        m_SolidIndices.push_back(cubeIndex + 1);
                        m_SolidIndices.push_back(cubeIndex + 2);
                        m_SolidIndices.push_back(cubeIndex + 2);
                        m_SolidIndices.push_back(cubeIndex + 3);
                        m_SolidIndices.push_back(cubeIndex);

                        m_SolidVertices.push_back(CubeData{ uint8_t(0 + x), uint8_t(0 + y), uint8_t(0 - z), 2, color }); // 0
                        m_SolidVertices.push_back(CubeData{ uint8_t(0 + x), uint8_t(1 + y), uint8_t(0 - z), 2, color }); // 3
                        m_SolidVertices.push_back(CubeData{ uint8_t(0 + x), uint8_t(1 + y), uint8_t(1 - z), 2, color }); // 5
                        m_SolidVertices.push_back(CubeData{ uint8_t(0 + x), uint8_t(0 + y), uint8_t(1 - z), 2, color }); // 6
                        cubeIndex += 4;
                    }
                    if (generateRight) {
                        m_SolidIndices.push_back(cubeIndex);
                        m_SolidIndices.push_back(cubeIndex + 1);
                        m_SolidIndices.push_back(cubeIndex + 2);
                        m_SolidIndices.push_back(cubeIndex + 2);
                        m_SolidIndices.push_back(cubeIndex + 3);
                        m_SolidIndices.push_back(cubeIndex);

                        m_SolidVertices.push_back(CubeData{ uint8_t(1 + x), uint8_t(1 + y), uint8_t(1 - z), 3, color }); // 4
                        m_SolidVertices.push_back(CubeData{ uint8_t(1 + x), uint8_t(1 + y), uint8_t(0 - z), 3, color }); // 2
                        m_SolidVertices.push_back(CubeData{ uint8_t(1 + x), uint8_t(0 + y), uint8_t(0 - z), 3, color }); // 1
                        m_SolidVertices.push_back(CubeData{ uint8_t(1 + x), uint8_t(0 + y), uint8_t(1 - z), 3, color }); // 7
                        cubeIndex += 4;
                    }
                    if (generateBack) {
                        m_SolidIndices.push_back(cubeIndex);
                        m_SolidIndices.push_back(cubeIndex + 1);
                        m_SolidIndices.push_back(cubeIndex + 2);
                        m_SolidIndices.push_back(cubeIndex + 2);
                        m_SolidIndices.push_back(cubeIndex + 3);
                        m_SolidIndices.push_back(cubeIndex);

                        m_SolidVertices.push_back(CubeData{ uint8_t(1 + x), uint8_t(1 + y), uint8_t(1 - z), 4, color }); // 4
                        m_SolidVertices.push_back(CubeData{ uint8_t(1 + x), uint8_t(0 + y), uint8_t(1 - z), 4, color }); // 7
                        m_SolidVertices.push_back(CubeData{ uint8_t(0 + x), uint8_t(0 + y), uint8_t(1 - z), 4, color }); // 6
                        m_SolidVertices.push_back(CubeData{ uint8_t(0 + x), uint8_t(1 + y), uint8_t(1 - z), 4, color }); // 5
                        cubeIndex += 4;
                    }
                    if (generateBottom) {
                        m_SolidIndices.push_back(cubeIndex);
                        m_SolidIndices.push_back(cubeIndex + 1);
                        m_SolidIndices.push_back(cubeIndex + 2);
                        m_SolidIndices.push_back(cubeIndex + 2);
                        m_SolidIndices.push_back(cubeIndex + 3);
                        m_SolidIndices.push_back(cubeIndex);

                        m_SolidVertices.push_back(CubeData{ uint8_t(0 + x), uint8_t(0 + y), uint8_t(1 - z), 5, color }); // 6
                        m_SolidVertices.push_back(CubeData{ uint8_t(1 + x), uint8_t(0 + y), uint8_t(1 - z), 5, color }); // 7
                        m_SolidVertices.push_back(CubeData{ uint8_t(1 + x), uint8_t(0 + y), uint8_t(0 - z), 5, color }); // 1
                        m_SolidVertices.push_back(CubeData{ uint8_t(0 + x), uint8_t(0 + y), uint8_t(0 - z), 5, color }); // 0
                        cubeIndex += 4;
                    }
                }
            }
        }
    }
}

void ChunkMesh::CreateChunkMesh() {
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);
    glCreateVertexArrays(1, &vao);

    glNamedBufferData(ebo, m_SolidIndices.size() * sizeof(unsigned int), m_SolidIndices.data(), GL_STATIC_DRAW);
    glNamedBufferData(vbo, m_SolidVertices.size() * sizeof(CubeData), m_SolidVertices.data(), GL_STATIC_DRAW);

    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(CubeData));
    glVertexArrayElementBuffer(vao, ebo);
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    glEnableVertexArrayAttrib(vao, 2);

    glVertexArrayAttribFormat(vao, 0, 3, GL_BYTE, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribFormat(vao, 1, 1, GL_BYTE, GL_FALSE, offsetof(CubeData, direction));
    glVertexArrayAttribBinding(vao, 1, 0);
    glVertexArrayAttribFormat(vao, 2, 1, GL_BYTE, GL_FALSE, offsetof(CubeData, colorIndex));
    glVertexArrayAttribBinding(vao, 2, 0);

    shader = ShaderLoader().LoadShader("src/shaders/basic_vertex.glsl", "src/shaders/basic_fragment.glsl");
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), (float)(Settings::WINDOW_WIDTH / Settings::WINDOW_HEIGHT), 0.1f, 1000.f);

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));

    m_SolidVertices.clear();

}
