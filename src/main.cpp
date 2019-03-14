#include "../yalexide_config.h"

#include <sstream>
#include <string>
#include <vector>

#include "imgui/imgui.h"
#include "imgui/imgui-SFML.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include "ImGuiColorTextEdit\TextEditor.h"

static bool TokenizeYalexStyleString(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end) {
    const char * p = in_begin;

    if (*p == '"') {
        p++;

        while (p < in_end) {
            // handle end of string
            if (*p == '"') {
                out_begin = in_begin;
                out_end = p + 1;
                return true;
            }

            // handle escape character for "
            if (*p == '\\' && p + 1 < in_end && p[1] == '"')
                p++;

            p++;
        }
    }

    return false;
}
static bool IsAlpha(const char * p) {
    return (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z');
}
static bool IsIdentifier(const char * p, const char * in_end) {
    return (p < in_end) && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9'));
}
static bool TokenizeYalexStyleIdentifier(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end) {
    const char * p = in_begin;

    if (IsAlpha(p) || *p == '\'' || *p == '_') {
        p++;

        while (IsIdentifier(p, in_end))
            p++;

        out_begin = in_begin;
        out_end = p;
        return true;
    }

    return false;
}

static bool TokenizeYalexStyleLambdaDef(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end) {
    const char * p = in_begin;

    if (*p == ':') {
        p++;

        while (IsIdentifier(p, in_end))
            p++;

        out_begin = in_begin;
        out_end = p;
        return true;
    }

    return false;
}

static bool TokenizeYalexStyleLambdaDeferred(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end) {
    const char * p = in_begin;

    if (*p == '\'') {
        p++;

        while (IsIdentifier(p, in_end))
            p++;

        out_begin = in_begin;
        out_end = p;
        return true;
    }

    return false;
}

static bool TokenizeYalexStyleNumber(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end) {
    const char * p = in_begin;

    const bool startsWithNumber = *p >= '0' && *p <= '9';

    if (*p != '+' && *p != '-' && !startsWithNumber)
        return false;

    p++;

    bool hasNumber = startsWithNumber;

    while (p < in_end && (*p >= '0' && *p <= '9')) {
        hasNumber = true;

        p++;
    }

    if (hasNumber == false)
        return false;

    bool isFloat = false;
    bool isHex = false;
    bool isBinary = false;

    if (p < in_end) {
        if (*p == '.') {
            isFloat = true;

            p++;

            while (p < in_end && (*p >= '0' && *p <= '9'))
                p++;
        } else if (*p == 'x' || *p == 'X') {
            // hex formatted integer of the type 0xef80

            isHex = true;

            p++;

            while (p < in_end && ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')))
                p++;
        } else if (*p == 'b' || *p == 'B') {
            // binary formatted integer of the type 0b01011101

            isBinary = true;

            p++;

            while (p < in_end && (*p >= '0' && *p <= '1'))
                p++;
        }
    }

    if (isHex == false && isBinary == false) {
        // floating point exponent
        if (p < in_end && (*p == 'e' || *p == 'E')) {
            isFloat = true;

            p++;

            if (p < in_end && (*p == '+' || *p == '-'))
                p++;

            bool hasDigits = false;

            while (p < in_end && (*p >= '0' && *p <= '9')) {
                hasDigits = true;

                p++;
            }

            if (hasDigits == false)
                return false;
        }

        // single precision floating point type
        if (p < in_end && *p == 'f')
            p++;
    }

    if (isFloat == false) {
        // integer size type
        while (p < in_end && (*p == 'u' || *p == 'U' || *p == 'l' || *p == 'L'))
            p++;
    }

    out_begin = in_begin;
    out_end = p;
    return true;
}

static bool TokenizeYalexStylePunctuation(const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end) {
    (void) in_end;

    switch (*in_begin) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '=':
        case '!':
        case '|':
        case '&':
        case '<':
        case '>':
        case '(':
        case ')':
            out_begin = in_begin;
            out_end = in_begin + 1;
            return true;
    }

    return false;
}

const TextEditor::LanguageDefinition& LangYalex() {
    static bool inited = false;
    static TextEditor::LanguageDefinition langDef;
    if (!inited) {
        static const char* const yalexKeywords[] = {
            "true", "false",
        };
        for (auto& k : yalexKeywords)
            langDef.mKeywords.insert(k);

        for (size_t i = 0; i < 16; i++) { //TODO: get register def from yalex
            std::stringstream regset;
            std::stringstream regread;
            regset << "R" << i << "S";
            regread << "R" << i << "R";
            langDef.mKeywords.insert(regset.str());
            langDef.mKeywords.insert(regread.str());
        }

        std::vector<std::string> identifiers = {
            "push", "pop", "dup", "clr",
            "select", "resolve", "run",
            "&&", "||", "==", "!=",
            "nop", "_",
            "print","dump",
        };

        for (auto& k : identifiers) {
            TextEditor::Identifier id;
            id.mDeclaration = "Built-in function";
            langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
        }

        langDef.mTokenize = [](const char * in_begin, const char * in_end, const char *& out_begin, const char *& out_end, TextEditor::PaletteIndex & paletteIndex) -> bool {
            paletteIndex = TextEditor::PaletteIndex::Max;

            while (in_begin < in_end && isblank(*in_begin))
                in_begin++;

            if (in_begin == in_end) {
                out_begin = in_end;
                out_end = in_end;
                paletteIndex = TextEditor::PaletteIndex::Default;
            } else if (TokenizeYalexStyleLambdaDef(in_begin, in_end, out_begin, out_end))
                paletteIndex = TextEditor::PaletteIndex::CharLiteral;
            else if (TokenizeYalexStyleLambdaDeferred(in_begin, in_end, out_begin, out_end))
                paletteIndex = TextEditor::PaletteIndex::Preprocessor; 
            else if (TokenizeYalexStyleString(in_begin, in_end, out_begin, out_end))
                paletteIndex = TextEditor::PaletteIndex::String;
            else if (TokenizeYalexStyleIdentifier(in_begin, in_end, out_begin, out_end))
                paletteIndex = TextEditor::PaletteIndex::Identifier;
            else if (TokenizeYalexStyleNumber(in_begin, in_end, out_begin, out_end))
                paletteIndex = TextEditor::PaletteIndex::Number;
            else if (TokenizeYalexStylePunctuation(in_begin, in_end, out_begin, out_end))
                paletteIndex = TextEditor::PaletteIndex::Punctuation;

            return paletteIndex != TextEditor::PaletteIndex::Max;
        };

        langDef.mCommentStart = "/*";
        langDef.mCommentEnd = "*/";
        langDef.mSingleLineComment = "//";

        langDef.mCaseSensitive = true;
        langDef.mAutoIndentation = true;

        langDef.mName = "C++";

        inited = true;
    }
    return langDef;
}
int main() {
    sf::RenderWindow window(sf::VideoMode(900, 900), "YalexIDE", sf::Style::Default);
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    TextEditor editor;
    editor.SetLanguageDefinition(LangYalex());

    std::vector<std::string> lines;
    lines.push_back(":fibstep (R1R R2R + R3S pop R2R R1S pop R3R R2S pop R4R 1 + R4S pop rec)");
    lines.push_back(":rec (R4R R0R 1 + < 'fibstep _ select)");
    lines.push_back(":start (R0R 1 - R0S pop rec pop pop pop pop pop pop R3R)");
    lines.push_back(":fib (R0S 0 R1S 1 R2S 0 R3S 1 R4S R0R 3 < 1 'start select)");
    lines.push_back("0x1 false ||");
    lines.push_back("dump");
    lines.push_back("\"0x1 2 3 + +\" run");
    editor.SetTextLines(lines);

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        //ImGui::ShowTestWindow();
        editor.Render("Yalex", window.getSize());
        //ImGui::Begin("Hello, world!");
        //ImGui::Button("Look at this pretty button");
        //ImGui::End();

        window.clear();
        //window.draw(shape);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();

    return 0;
}