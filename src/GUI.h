#ifndef YALEXIDE_GUI_H
#define YALEXIDE_GUI_H

#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "yalex.h"

class YalexEditorComponent {
public:
    YalexEditorComponent(std::string _id) : id(_id) {

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
    inline static const std::string Docking::YALEX_CONSOLE_PANEL = "Console";
    inline static const std::string Docking::YALEX_INFO_PANEL = "Info";
    inline static const std::string Docking::YALEX_STACK_PANEL = "Stack";
    inline static const std::string Docking::YALEX_EDITOR = "YalexEditor";
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
        ImGui::Begin(Docking::DOCK_ROOT_ID.c_str(), &open, window_flags);
        ImGui::PopStyleVar();

        ImGui::PopStyleVar(2);

        if (ImGui::DockBuilderGetNode(ImGui::GetID(Docking::DOCKSPACE_ID.c_str())) == nullptr) {
            createDock();
        }

        ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, { 35, 65, 90, 255 });
        ImGuiID dockspace_id = ImGui::GetID(DOCKSPACE_ID.c_str());
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
        ImGui::PopStyleColor();
        ImGui::End();
    }

    void createDock() {
        ImGuiID dockspace_id = ImGui::GetID(Docking::DOCKSPACE_ID.c_str());
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout

        //ImGui::DockBuilderAddNode(dockspace_id); //viewport->Size); // Add empty node
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_Dockspace); // Add empty node
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
        ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, nullptr, &dock_main_id);
        ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, nullptr, &dock_main_id);
        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, nullptr, &dock_main_id);

        ImGui::DockBuilderDockWindow(YALEX_INFO_PANEL.c_str(), dock_id_left);
        ImGui::DockBuilderDockWindow(YALEX_EDITOR.c_str(), dock_main_id);
        ImGui::DockBuilderDockWindow(YALEX_STACK_PANEL.c_str(), dock_id_right);
        ImGui::DockBuilderDockWindow(YALEX_CONSOLE_PANEL.c_str(), dock_id_bottom);
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
        ImGui::Begin(Docking::DOCK_ROOT_ID.c_str(), &open, window_flags);
        ImGui::PopStyleVar();

        ImGui::PopStyleVar(2);

        createDock();

        ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, { 35, 65, 90, 255 });
        ImGuiID dockspace_id = ImGui::GetID(Docking::DOCKSPACE_ID.c_str());
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);
        ImGui::PopStyleColor();
        ImGui::End();
    }
private:
    inline static const std::string Docking::DOCK_ROOT_ID = "YALEXIDE_ROOT";
    inline static const std::string Docking::DOCKSPACE_ID = "YALEX_DOCKSPACE";
};

#endif 
