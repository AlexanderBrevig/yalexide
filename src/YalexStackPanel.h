#ifndef YALEXIDE_YALEX_STACK_PANEL_H
#define YALEXIDE_YALEX_STACK_PANEL_H

#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "yalex.h"
#include "GUI.h"
#include<iostream>
class YalexStackPanel : public YalexEditorComponent {
public:
    YalexStackPanel(yalex_world *_world) : YalexEditorComponent(Docking::YALEX_STACK_PANEL), world(_world) {
        std::cout << world->stack[2].meta << std::endl;
    }
    void draw() override {
        ImGui::Begin(id.c_str());
        ImGui::Text("STACK (%d/%d):", world->sp, YALEX_SIZE_STACK);
        for (size_t i = world->sp; i > 0; i--) {
            std::string meta;
            std::string content;
            switch (world->stack[i].meta) {
                case YALEX_TOKEN_NAN:
                    meta = "TEXT";
                    content = world->stack[i].data.text;
                    break;
                case YALEX_TOKEN_NUM:
                    meta = " NUM";
                    content = std::to_string(world->stack[i].data.number);
                    break;
                case YALEX_TOKEN_UNDEFINED:
                    meta = "UNDF";
                    content = "<undefined>";
                    break;
                case YALEX_TOKEN_LAMBDA:
                    meta = " LAM";
                    content = world->stack[i].data.text;
                    break;
                case YALEX_TOKEN_EVAL:
                    meta = "EVAL";
                    content = world->stack[i].data.text;
                    break;
                case YALEX_TOKEN_LAMBDA_DEFERRED:
                    meta = "LAMD";
                    content = world->stack[i].data.text;
                    break;
            }
            if (world->sp - i == 0) {
                ImGui::Text("sp:\t%s\t%s", meta.c_str(), content.c_str());
            } else {
                ImGui::Text("%02d:\t%s\t%s", world->sp - i, meta.c_str(), content.c_str());
            }
        }
        ImGui::End();
    }
private:
    yalex_world *world;
};

#endif
