#ifndef YALEXIDE_TOKENIZE_H
#define YALEXIDE_TOKENIZE_H

class Tokenize {
public:
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

private:
    static bool IsAlpha(const char * p) {
        return (*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z');
    }
    static bool IsIdentifier(const char * p, const char * in_end) {
        return (p < in_end) && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9'));
    }

};

#endif
