#ifndef YALEXIDE_YALEX_SYSTEM_PANEL_H
#define YALEXIDE_YALEX_SYSTEM_PANEL_H

#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

extern "C" {
#define YALEX_DEBUG
#include "yalex.h"
}
#include "yalex_globals.h"
#include "yalex_system.h"
#include "GUI.h"

#include <iostream>
using namespace std;

class YalexSystemPanel : public YalexEditorComponent {
public:
    YalexSystemPanel(yalex_world *_world) : YalexEditorComponent(Docking::YALEX_SYSTEM_PANEL), world(_world) {
        previousConfig.stackSize = YALEX_SIZE_STACK;
        previousConfig.tokenStrSize = YALEX_SIZE_TOKEN_STR;
        previousConfig.maxDependableStack = YALEX_SIZE_MAX_DEPENDABLE_STACK;
        previousConfig.lambdaStackStr = YALEX_SIZE_LAMBDA_STACK_STR;
        previousConfig.lambdasStack = YALEX_SIZE_LAMBDAS_STACK;
        previousConfig.registerCount = YALEX_SIZE_REGISTERS;
        previousConfig.sysLambdasStack = YALEX_SIZE_SYS_LAMBDAS_STACK;
        previousConfig.tokens = YALEX_SIZE_TOKENS;
        previousConfig.userTokens = 20; //YALEX_SIZE_USER_TOKENS;
    }
    void configure() {
        userTokens = 64; // init to 64, better than whatever
        /// DELETE STACK
        if (world->stack) {
            cout << "Deleting previous stack" << endl;
            for (size_t i = 0; i < previousConfig.stackSize; i++) {
                YALEX_FREE(world->stack[i].data.text);
            }
            YALEX_FREE(world->stack);
            world->stack = 0;
        }

        cout << "Making new stack and prepare it" << endl;
        /// MAKE STACK
        world->stack = (stack_item*) YALEX_MALLOC(sizeof(stack_item) * YALEX_SIZE_STACK);

        /// PREPARE STACK
        for (size_t i = 0; i < YALEX_SIZE_STACK; i++) {
            world->stack[i].data.number = 0;
            world->stack[i].data.text = (char *) YALEX_MALLOC(YALEX_SIZE_TOKEN_STR);
            world->stack[i].data.text[0] = 0;
            world->stack[i].meta = YALEX_TOKEN_UNDEFINED;
        }
        cout << "Accessing stack 2 meta get " << world->stack[2].meta << endl;

        /// DELETE LAMBDAS
        if (world->lambdas) {
            for (size_t i = 0; i < previousConfig.lambdasStack; i++) {
                YALEX_FREE(world->lambdas[i].name);
                YALEX_FREE(world->lambdas[i].requirements);
                YALEX_FREE(world->lambdas[i].stack);
            }
            YALEX_FREE(world->lambdas);
            world->lambdas = 0;
        }

        /// MAKE LAMBDAS
        world->lambdas = (lambda*) YALEX_MALLOC(sizeof(lambda) * YALEX_SIZE_LAMBDAS_STACK);

        /// PREPARE LAMBDAS
        for (size_t i = 0; i < YALEX_SIZE_LAMBDAS_STACK; i++) {
            world->lambdas[i].requirementCount = 0;
            world->lambdas[i].name = (char*) YALEX_MALLOC(YALEX_SIZE_TOKEN_STR);
            world->lambdas[i].name[0] = 0;
            world->lambdas[i].requirements = (char*) YALEX_MALLOC(YALEX_SIZE_MAX_DEPENDABLE_STACK);
            world->lambdas[i].requirements[0] = 0;
            world->lambdas[i].stack = (char*) YALEX_MALLOC(YALEX_SIZE_LAMBDA_STACK_STR);
            world->lambdas[i].stack[0] = 0;
        }

        /// DELETE REGISTERS
        if (world->registers) {
            YALEX_FREE(world->registers);
        }
        /// MAKE REGISTERS
        world->registers = new numeric_type[YALEX_SIZE_REGISTERS];
        for (size_t i = 0; i < YALEX_SIZE_REGISTERS; i++) {
            world->registers[i] = 0;
        }

        /// DELETE SYS LAMBDAS
        if (yalex_system()->lambdas) {
            for (size_t i = 0; i < previousConfig.sysLambdasStack; i++) {
                 YALEX_FREE(yalex_system()->lambdas[i].name);
                 YALEX_FREE(yalex_system()->lambdas[i].requirements);
                 YALEX_FREE(yalex_system()->lambdas[i].stack);
            }
            YALEX_FREE(yalex_system()->lambdas);
            yalex_system()->lambdas = 0;
        }

        /// MAKE SYS LAMBDAS
        yalex_system()->lambdas = (lambda*)YALEX_MALLOC(sizeof(lambda) * YALEX_SIZE_SYS_LAMBDAS_STACK);

        /// PREPARE SYS LAMDAS
        for (size_t i = 0; i < YALEX_SIZE_SYS_LAMBDAS_STACK; i++) {
            yalex_system()->lambdas[i].requirementCount = 0;
            yalex_system()->lambdas[i].name = (char*) YALEX_MALLOC(YALEX_SIZE_TOKEN_STR);
            yalex_system()->lambdas[i].name[0] = 0;
            yalex_system()->lambdas[i].requirements = (char*) YALEX_MALLOC(YALEX_SIZE_MAX_DEPENDABLE_STACK);
            yalex_system()->lambdas[i].requirements[0] = 0;
            yalex_system()->lambdas[i].stack = (char*) YALEX_MALLOC(YALEX_SIZE_LAMBDA_STACK_STR);
            yalex_system()->lambdas[i].stack[0] = 0;
        }

        /// DELETE SYS TOKENS
        if (yalex_system()->tokens) {
            for (size_t i = 0; i < previousConfig.tokens; i++) {
                YALEX_FREE(yalex_system()->tokens[i].requirements);
            }
            YALEX_FREE(yalex_system()->tokens);
            yalex_system()->tokens = 0;
        }

        /// MAKE SYS TOKENS
        yalex_system()->tokens = (token_built_in*) YALEX_MALLOC(sizeof(token_built_in) * (YALEX_SIZE_TOKENS + userTokens));

        yalex_system()->tokenCount = 0;
        /// PREPARE SYS TOKENS
        for (size_t i = 0; i < YALEX_SIZE_TOKENS + userTokens; i++) {
            yalex_system()->tokens[i].exec = 0;
            yalex_system()->tokens[i].requirementCount = 0;
            yalex_system()->tokens[i].token = (char*) YALEX_MALLOC(YALEX_SIZE_TOKEN_STR);
            yalex_system()->tokens[i].token[0] = 0;
            yalex_system()->tokens[i].requirements = (char*) YALEX_MALLOC(YALEX_SIZE_MAX_DEPENDABLE_STACK);
            for (size_t j = 0; j < YALEX_SIZE_MAX_DEPENDABLE_STACK; j++) {
                yalex_system()->tokens[i].requirements[j] = 0;
            }
        }

        previousConfig.stackSize = YALEX_SIZE_STACK;
        previousConfig.tokenStrSize = YALEX_SIZE_TOKEN_STR;
        previousConfig.maxDependableStack = YALEX_SIZE_MAX_DEPENDABLE_STACK;
        previousConfig.lambdaStackStr = YALEX_SIZE_LAMBDA_STACK_STR;
        previousConfig.lambdasStack = YALEX_SIZE_LAMBDAS_STACK;
        previousConfig.registerCount = YALEX_SIZE_REGISTERS;
        previousConfig.sysLambdasStack = 5 + YALEX_SIZE_SYS_LAMBDAS_STACK;
        previousConfig.tokens = YALEX_SIZE_TOKENS + userTokens;
    }

    void draw() override {
        ImGui::Begin(id.c_str());
        ImGui::Text("SYSTEM");

        SettingField("STACK", YALEX_SIZE_STACK,
                     "The max number of symbols on the stack");
        SettingField("TOKEN_STR", YALEX_SIZE_TOKEN_STR,
                     "The max number of characters for a token");
        SettingField("DEPENDABLE_STACK", YALEX_SIZE_MAX_DEPENDABLE_STACK,
                     "The max number of symbols required for a token");
        SettingField("USER_TOKENS", userTokens,
                     "The number of user defined tokens");
        SettingField("LAMBDA_STACK_STR", YALEX_SIZE_LAMBDA_STACK_STR,
                     "The string length of a lambda body");
        SettingField("LAMBDAS_STACK", YALEX_SIZE_LAMBDAS_STACK,
                     "The max number of lambdas");
        SettingField("REGISTERS", YALEX_SIZE_REGISTERS,
                     "The number of registers");
        SettingField("SYS_LAMBDAS_STACK", YALEX_SIZE_SYS_LAMBDAS_STACK,
                     "The number of system lambdas");

        if (ImGui::Button("Configure")) {
            configure();
        }

        ImGui::End();
    }
private:
    static void SettingField(const char *field, int &setting, const char *tooltip) {
        HelpMarker(tooltip);
        ImGui::SameLine();
        ImGui::InputInt(field, &setting, 1, 10);
    }
    static void HelpMarker(const char* desc) {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
    yalex_world *world;
    int userTokens;
    typedef struct _yalex_config {
        int stackSize;
        int tokenStrSize;
        int maxDependableStack;
        int userTokens;
        int lambdaStackStr;
        int lambdasStack;
        int registerCount;
        int sysLambdasStack;
        int tokens;
    } yalex_config;
    yalex_config previousConfig;
};

#endif
