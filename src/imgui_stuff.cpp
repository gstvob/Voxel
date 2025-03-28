#include "imgui_stuff.h"

void MyGUI::ShowWindows(const Camera& camera, float timeOfDay) {

    ImGuiContext& g = *ImGui::GetCurrentContext();
    ImGuiIO& io = g.IO;
    //ImGuiMetricsConfig* cfg = &g.DebugMetricsConfig;
    float frontX = std::round(camera.m_Front.x);
    float frontZ = std::round(camera.m_Front.z);

    glm::vec3 cameraFront = glm::vec3(frontX, 0.0f, frontZ);

    ImGui::Begin("Metrics");
 
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::Text("%d vertices, %d indices (%d triangles)", io.MetricsRenderVertices, io.MetricsRenderIndices, io.MetricsRenderIndices / 3);
    ImGui::Text("%d visible windows, %d current allocations", io.MetricsRenderWindows, g.DebugAllocInfo.TotalAllocCount - g.DebugAllocInfo.TotalFreeCount);
    ImGui::Text("Camera Pos (%.2f, %.2f, %.2f)", camera.m_Position.x, camera.m_Position.y, camera.m_Position.z);
    ImGui::Text("Camera front (%.2f, %.2f, %.2f)", camera.m_Front.x, camera.m_Front.y, camera.m_Front.z);
    ImGui::Text("Camera front ceiled (%.2f, %.2f, %.2f)", cameraFront.x, cameraFront.y, cameraFront.z);
    ImGui::Text("Chunks %d", world::ChunkCount());
    ImGui::Text("Chunk Queue count %d", world::ChunkQueueCount());
    ImGui::Text("Time of Day %.3f", timeOfDay);
    ImGui::End();
}
