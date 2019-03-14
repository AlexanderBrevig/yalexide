#ifndef YALEXIDE_YALEX_DEF_H
#define YALEXIDE_YALEX_DEF_H

#include "ImGuiColorTextEdit\TextEditor.h"
#include "yalex.h"

class YalexDefinition {
public:
    static const TextEditor::LanguageDefinition& YalexLanguage() {
        static bool inited = false;
        static TextEditor::LanguageDefinition langDef;
        if (!inited) {
            static const char* const yalexKeywords[] = {
                "true", "false",
            };
            for (auto& k : yalexKeywords)
                langDef.mKeywords.insert(k);

            for (size_t i = 0; i < YALEX_SIZE_REGISTERS; i++) {
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
                } else if (Tokenize::TokenizeYalexStyleLambdaDef(in_begin, in_end, out_begin, out_end))
                    paletteIndex = TextEditor::PaletteIndex::CharLiteral;
                else if (Tokenize::TokenizeYalexStyleLambdaDeferred(in_begin, in_end, out_begin, out_end))
                    paletteIndex = TextEditor::PaletteIndex::Preprocessor;
                else if (Tokenize::TokenizeYalexStyleString(in_begin, in_end, out_begin, out_end))
                    paletteIndex = TextEditor::PaletteIndex::String;
                else if (Tokenize::TokenizeYalexStyleIdentifier(in_begin, in_end, out_begin, out_end))
                    paletteIndex = TextEditor::PaletteIndex::Identifier;
                else if (Tokenize::TokenizeYalexStyleNumber(in_begin, in_end, out_begin, out_end))
                    paletteIndex = TextEditor::PaletteIndex::Number;
                else if (Tokenize::TokenizeYalexStylePunctuation(in_begin, in_end, out_begin, out_end))
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
};

#endif
