#include "../yalexide_config.h"

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <imgui/imgui.h>
#include <imgui/imgui-SFML.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>

extern "C" {
#define YALEX_DEBUG
#include "yalex.h"
}

#include "YalexEditor.h"
#include "YalexStackPanel.h"
#include "YalexSystemPanel.h"
#include "YalexInfoPanel.h"
#include "YalexConsolePanel.h"

yalex_world world;


void replMessageCallback(const char* ptr) {
    YalexConsolePanel::instance().yalex(ptr);
}

int main() {
    world.lambdas = 0;
    world.stack = 0;
    world.registers = 0;

    Docking dock;
    YalexSystemPanel configure(&world);
    configure.configure();
    yalex_init(&world, replMessageCallback);

    YalexEditor editor(&world);
    YalexStackPanel stack(&world);
    YalexInfoPanel info(&world);

    sf::VideoMode mode = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(mode, "YalexIDE", sf::Style::Default);
    window.setView(sf::View(sf::FloatRect(0.f, 0.f, mode.width, mode.height)));
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    window.resetGLStates(); // call it if you only process ImGui. Otherwise not needed.

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::SFML::Init(window);

    auto io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
                                                                 // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    std::vector<std::string> lines;
    lines.push_back(":fibstep (R1R R2R + R3S pop R2R R1S pop R3R R2S pop R4R 1 + R4S pop rec)");
    lines.push_back(":rec (R4R R0R 1 + < 'fibstep _ select)");
    lines.push_back(":start (R0R 1 - R0S pop rec pop pop pop pop pop pop R3R)");
    lines.push_back(":fib (R0S 0 R1S 1 R2S 0 R3S 1 R4S R0R 3 < 1 'start select)");
    lines.push_back("10 fib");
    lines.push_back("1 2 +");
    editor.SetTextLines(lines);

    LOGINFO("Info");
    LOGWARN("Warning");
    LOGERR("Error");

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::KeyReleased) {
                editor.doShortcuts();
            }
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        dock.drawDock();
        editor.draw();
        configure.draw();
        info.draw();
        stack.draw();
        YalexConsolePanel::instance().draw();
        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}
