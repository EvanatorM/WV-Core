#include <wv/Logger.h>

#ifdef _WIN32
#include <Windows.h>

namespace WillowVox
{
    void Logger::SetColor(int textColor, int bgColor)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, (bgColor << 4) | textColor);
    }
}
#endif