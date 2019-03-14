#ifndef YALEXIDE_YALEX_CONSOLE_PANEL_H
#define YALEXIDE_YALEX_CONSOLE_PANEL_H

#include <string>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include "yalex.h"
#include "GUI.h"

#define LOGINFO(x) YalexConsolePanel::instance().info(x)
#define LOGWARN(x) YalexConsolePanel::instance().warn(x)
#define LOGERR(x) YalexConsolePanel::instance().err(x)

template <typename T>
struct reversion_wrapper { T& iterable; };

template <typename T>
auto begin(reversion_wrapper<T> w) { return std::rbegin(w.iterable); }

template <typename T>
auto end(reversion_wrapper<T> w) { return std::rend(w.iterable); }

template <typename T>
reversion_wrapper<T> reverse(T&& iterable) { return { iterable }; }

class YalexConsolePanel : public YalexEditorComponent {
private:
    YalexConsolePanel() : YalexEditorComponent(Docking::YALEX_CONSOLE_PANEL) {

    }
public:
    enum class LogType {
        YALEX,
        INFO,
        WARNING,
        ERROR
    };
    YalexConsolePanel(const YalexConsolePanel&) = delete;
    YalexConsolePanel& operator=(const YalexConsolePanel &) = delete;
    YalexConsolePanel(YalexConsolePanel &&) = delete;
    YalexConsolePanel & operator=(YalexConsolePanel &&) = delete;

    static auto& instance() {
        static YalexConsolePanel test;
        return test;
    }
    void yalex(std::string msg) { messages.push_back({ LogType::YALEX, "YALEX> " + msg }); }
    void info(std::string msg) { messages.push_back({ LogType::INFO,  "LOG I> " + msg }); }
    void warn(std::string msg) { messages.push_back({ LogType::WARNING,"LOG W> " + msg }); }
    void err(std::string msg) { messages.push_back({ LogType::ERROR,"LOG E> " + msg }); }
    void draw() override {
        ImGui::Begin(id.c_str());
        for (auto &[type, msg] : reverse(messages)) {
            switch (type) {
                case LogType::YALEX:    ImGui::TextColored(ImVec4(1, 1, 1, 1), msg.c_str()); break;
                case LogType::INFO:     ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), msg.c_str()); break;
                case LogType::WARNING:  ImGui::TextColored(ImVec4(1, 0, 1, 1), msg.c_str()); break;
                case LogType::ERROR:    ImGui::TextColored(ImVec4(1, 0, 0, 1), msg.c_str()); break;
                default:                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0, 1), msg.c_str()); break;
            }

        }
        ImGui::End();
    }
private:
    std::vector<std::tuple<LogType, std::string>> messages;
};

#endif