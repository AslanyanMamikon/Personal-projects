#include "RegexStream.h"

int RegexStream::Read() {
    if (!str || str[pos] == '\0') {
        return -1;
    }
    return str[pos++];
}

int RegexStream::Peek() {
    if (!str || str[pos] == '\0') {
        return -1;
    }
    return str[pos];
}

void RegexStream::Back() {
    if (pos > 0) {
        --pos;
    }
} 