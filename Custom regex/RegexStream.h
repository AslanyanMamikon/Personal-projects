#ifndef REGEX_STREAM_H__
#define REGEX_STREAM_H__

class RegexStream {
private:
    const char* str;
    int pos;

public:
    RegexStream(const char* str) : str(str), pos(0) { }
    ~RegexStream() { }

    int Read();
    int Peek();
    void Back();
};

#endif // REGEX_STREAM_H__