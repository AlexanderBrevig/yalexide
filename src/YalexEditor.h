#ifndef YALEXIDE_YALEX_EDITOR_H
#define YALEXIDE_YALEX_EDITOR_H

#include <filesystem>
#include <fstream>

#include "Tokenize.h"
#include "YalexDefinition.h"
#include "ImGuiColorTextEdit\TextEditor.h"

#include "GUI.h"
#include "YalexConsolePanel.h"

#include "yalex.h"

class YalexEditor : public TextEditor, public YalexEditorComponent {
public:
    YalexEditor(yalex_world &_world) : YalexEditorComponent(Docking::YALEX_EDITOR), world(_world) {
        SetLanguageDefinition(YalexDefinition::YalexLanguage());
        isDirty = this->IsTextChanged();
        currentPath = std::filesystem::current_path();
        strcpy(inputPathBuffer, currentPath.u8string().c_str());
        fileSelectOpen = false;
        canDoShortcuts = true;
    }
    void draw() override {
        if (this->IsTextChanged()) isDirty = true;

        ImGui::Begin(id.c_str());
        ImGui::BeginChild("action_panel", { -1, 35 }, true, 0);
        ImGui::SameLine();
        bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
        bool newHotkey = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N);
        bool openHotkey = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::O);
        bool saveHotkey = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
        bool runHotkey = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R);
        bool runLineHotkey = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::L);
        bool resetHotkey = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C);
        auto hotkey = [&ctrl, this](bool btn) {
            bool doShortkey = ctrl && btn; 
            if (canDoShortcuts && doShortkey) {
                canDoShortcuts = false;
                return true;
            }
            return false;
        };
        if (ImGui::Button("[N]ew") || hotkey(newHotkey)) {
            if (isDirty) {
                //TODO: ask for save
                //YalexConsolePanel::print("must save, file is dirty");
            }
            filePath = "";  
            this->SetText("");
        }
        ImGui::SameLine();
        if (ImGui::Button("[O]pen") || hotkey(openHotkey)) {
            if (isDirty) {
                //TODO: ask for save
                //YalexConsolePanel::print("must save, file is dirty");
            }
            ImGui::OpenPopup("fileopen");
            fileSelectOpen = true;
        }
        if (ImGui::BeginPopupModal("fileopen", &fileSelectOpen, ImGuiWindowFlags_NoResize)) {
            ImGui::Text("Select a file");
            if (ImGui::InputText("URI", inputPathBuffer, inputPathBufferSize)) {
                currentPath = std::string(inputPathBuffer);
            }
            ImGui::Separator();
            
            ImGui::BeginChild("listing", ImVec2(1200, 700), true);
            if (std::filesystem::is_directory(currentPath)) {
                bool selected = false;
                if (ImGui::Selectable("..", &selected, ImGuiSelectableFlags_DontClosePopups)) {
                    currentPath = currentPath.parent_path();
                }
                for (auto& dirpath : std::filesystem::directory_iterator(currentPath)) {
                    if ((std::filesystem::is_directory(dirpath) && dirpath.path().stem().string().at(0) != '.')
                        || 
                        (dirpath.path().has_extension()
                            && (
                                dirpath.path().extension().string().compare(".yal") == 0
                                ||
                                dirpath.path().extension().string().compare(".txt") == 0
                            ))) {
                        if (ImGui::Selectable(dirpath.path().generic_u8string().c_str(), &selected, ImGuiSelectableFlags_DontClosePopups)) {
                            if (!std::filesystem::is_directory(dirpath)) {
                                std::ifstream filestream(dirpath, std::ios::in);
                                std::string line;
                                this->SetText("");
                                while (std::getline(filestream, line)) {
                                    this->InsertText(line.c_str());
                                    this->InsertText("\n");
                                }
                                filestream.close();
                                ImGui::CloseCurrentPopup();
                            } else {
                                currentPath = dirpath;
                            }
                        }
                    }
                }
            }
            ImGui::EndChild();
            ImGui::EndPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("[S]ave") || hotkey(saveHotkey)) {

        }
        ImGui::SameLine();
        if (!this->GetText().empty() && ImGui::Button("[R]un File") || hotkey(runHotkey)) {
            LOGINFO("Running file");
            std::vector<std::string> lines = this->GetTextLines();
            for (auto line : lines) {
                yalex_repl(&world, line.c_str());
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Run [L]ine") || hotkey(runLineHotkey)) {

            std::string code = this->GetCurrentLineText();
            LOGINFO("Running line '" + code + "'");
            yalex_repl(&world, code.c_str());
        }
        ImGui::SameLine();
        if (ImGui::Button("[C] Reset yalex") || hotkey(runHotkey)) {
            yalex_init(&world, world.onResultCallback);
        }
        ImGui::EndChild();


        Render("actualYalexEditor");
        ImGui::End();
    }
    void doShortcuts() {
        canDoShortcuts = true;
    }
private:
    std::filesystem::path filePath;
    bool isDirty;
    yalex_world &world;
    bool fileSelectOpen;
    std::filesystem::path currentPath;
    char inputPathBuffer[256];
    const size_t inputPathBufferSize = 256;
    bool canDoShortcuts;
};

#endif
