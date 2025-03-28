#include "game.h"

Game::Game(uint16_t width, uint16_t height) {

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    m_SDLWindow = SDL_CreateWindow("Voxel Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    SDL_GLContext context = SDL_GL_CreateContext(m_SDLWindow);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glViewport(0, 0, width, height);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // INIT IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplSDL2_InitForOpenGL(m_SDLWindow, context);
    ImGui_ImplOpenGL3_Init();

    m_Camera = new Camera();
}

Game::~Game() {
    SDL_DestroyWindow(m_SDLWindow);
    m_SDLWindow = nullptr;
    SDL_Quit();
    delete m_Camera;
}

void Game::ProcessInput() {
    const float cameraSpeed = m_DeltaTime * 0.05f;

    if (m_Keyboard[SDLK_w])
        m_Camera->Move(cameraSpeed * glm::vec3(m_Camera->m_Front.x, 0.0, m_Camera->m_Front.z));
    if (m_Keyboard[SDLK_s])
        m_Camera->Move(-cameraSpeed * glm::vec3(m_Camera->m_Front.x, 0.0, m_Camera->m_Front.z));
    if (m_Keyboard[SDLK_a])
        m_Camera->Move(-glm::normalize(glm::cross(m_Camera->m_Front, m_Camera->m_Up)) * cameraSpeed);
    if (m_Keyboard[SDLK_d])
        m_Camera->Move(glm::normalize(glm::cross(m_Camera->m_Front, m_Camera->m_Up)) * cameraSpeed);
    if (m_Keyboard[SDLK_LCTRL])
        m_Camera->Move(cameraSpeed * glm::vec3(0.0f, 1.0f, 0.0f));
    if (m_Keyboard[SDLK_LSHIFT])
        m_Camera->Move(cameraSpeed * glm::vec3(0.0f, -1.0f, 0.0f));
}

void Game::Draw() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    world::Draw(*m_Camera, m_TimeOfDay);
}

void Game::Run() {
    SDL_Event event;
    bool running = true;
    bool minimized = false;
    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    m_DeltaTime = 0;
    m_FirstMouse = true;
    m_TimeOfDay = 0.0f;

    world::Init();
    while (running) {

        m_TimeOfDay += m_DeltaTime * 0.000003f;  // Progress the time (adjust the multiplier for speed)
        if (m_TimeOfDay >= 1.0f) {
            m_TimeOfDay = 0.0f; // Reset the cycle after 1.0 (24 hours)
        }

        m_MouseEnabled = SDL_GetRelativeMouseMode();
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        m_DeltaTime = (double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());


        glm::vec3 skyColor = glm::mix(glm::vec3(0.3f, 0.75f, 0.96f), glm::vec3(0.01f, 0.01f, 0.07f), m_TimeOfDay);
        glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f); // Set background color

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event); // Forward your event to backend
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                m_Keyboard[event.key.keysym.sym] = true;
                break;
            case SDL_KEYUP:
                m_Keyboard[event.key.keysym.sym] = false;
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == 3) {
                    SDL_SetRelativeMouseMode((SDL_bool) !m_MouseEnabled);
                }
                break;
            case SDL_MOUSEMOTION:
                if (m_FirstMouse) {
                    m_FirstMouse = false;
                    SDL_WarpMouseInWindow(m_SDLWindow, 0, 0);
                    break;
                }
                if (m_MouseEnabled) {
                    m_Camera->Look(event.motion.xrel, event.motion.yrel);
                }
                break;
            }
        }

        if (m_MouseEnabled) {
            ProcessInput();
        }

        Draw();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        MyGUI::ShowWindows(*m_Camera, m_TimeOfDay);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(m_SDLWindow);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}
