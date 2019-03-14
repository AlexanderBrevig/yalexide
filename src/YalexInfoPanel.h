#ifndef YALEXIDE_YALEX_INFO_PANEL_H
#define YALEXIDE_YALEX_INFO_PANEL_H

#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "yalex.h"
#include "GUI.h"

class YalexInfoPanel : public YalexEditorComponent {
public:
    YalexInfoPanel(yalex_world &_world) : YalexEditorComponent(Docking::YALEX_INFO_PANEL), world(_world) {

    }
    void draw() override {
        ImGui::Begin(id.c_str());
        ImGui::Text("LAMBDAS (%d/%d):", world.lm, YALEX_SIZE_LAMBDAS_STACK);
        for (size_t i = 0; i < world.lm; i++) {
            ImGui::Text(":%s (%s)", world.lambdas[i].name, world.lambdas[i].stack);
        }
        ImGui::Separator();
        ImGui::Text("REGISTERS (%d):", YALEX_SIZE_REGISTERS);
        for (size_t i = 0; i < YALEX_SIZE_REGISTERS; i++) {
            ImGui::Text("%02d:\t%d", i, world.registers[i]);
        }
        ImGui::End();
    }
private:
    yalex_world &world;
};

#endif