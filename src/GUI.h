#ifndef YALEXIDE_GUI_H
#define YALEXIDE_GUI_H

#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

extern "C" {
#define YALEX_DEBUG
#include "yalex.h"
}

class YalexEditorComponent {
public:
    YalexEditorComponent(const char  *_id) : id(_id) {

    }

    virtual void draw() {
        ImGui::Begin(id.c_str());
        ImGui::End();
    }
    std::string &getId() { return id; }
protected:
    std::string id;
};

/*
Brutally copied from https://github.com/SSBMTonberry/emu-jukebox/blob/master/src/managers/ProgramManager.cpp#L115
Modified to suit my needs.

BSD 2-Clause License

Copyright (c) 2019, Robin Berg Pettersen
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
class Docking {
public:
    static constexpr char YALEX_CONSOLE_PANEL[] = "Console";
    static constexpr char YALEX_INFO_PANEL[] = "Info";
    static constexpr char YALEX_SYSTEM_PANEL[] = "System";
    static constexpr char YALEX_STACK_PANEL[] = "Stack";
    static constexpr char YALEX_EDITOR[] = "YalexEditor";
public:
    void drawDock() {
        bool open = true;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin(DOCK_ROOT_ID, &open, window_flags);
        ImGui::PopStyleVar();

        ImGui::PopStyleVar(2);

        ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, { 35, 65, 90, 255 });
        if (ImGui::DockBuilderGetNode(ImGui::GetID(DOCKSPACE_ID)) == nullptr) {
            createDock();
        }
        ImGuiID dockspace_id = ImGui::GetID(DOCKSPACE_ID);
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
        ImGui::PopStyleColor();
        ImGui::End();
    }

    void createDock() {
        ImGuiID dockspace_id = ImGui::GetID(DOCKSPACE_ID);
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout

        //ImGui::DockBuilderAddNode(dockspace_id); //viewport->Size); // Add empty node
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_Dockspace); // Add empty node
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
        ImGuiID system_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, nullptr, &dock_main_id);
        ImGuiID info_id = ImGui::DockBuilderSplitNode(system_id, ImGuiDir_Down, 0.70f, nullptr, &system_id);
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, nullptr, &dock_main_id);
        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, nullptr, &dock_main_id);

        ImGui::DockBuilderDockWindow(YALEX_SYSTEM_PANEL, system_id);
        ImGui::DockBuilderDockWindow(YALEX_INFO_PANEL, info_id);
        ImGui::DockBuilderDockWindow(YALEX_EDITOR, dock_main_id);
        ImGui::DockBuilderDockWindow(YALEX_STACK_PANEL, dock_id_right);
        ImGui::DockBuilderDockWindow(YALEX_CONSOLE_PANEL, dock_id_bottom);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    void resetDock() {
        bool open = true;


        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin(DOCK_ROOT_ID, &open, window_flags);
        ImGui::PopStyleVar();

        ImGui::PopStyleVar(2);

        createDock();

        ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, { 35, 65, 90, 255 });
        ImGuiID dockspace_id = ImGui::GetID(DOCKSPACE_ID);
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
        ImGui::PopStyleColor();
        ImGui::End();
    }
private:
    static constexpr char DOCK_ROOT_ID[] = "YALEXIDE_ROOT";
    static constexpr char DOCKSPACE_ID[] = "YALEX_DOCKSPACE";
};

#endif 
