#include "TerminalColor.h"

std::string TerminalColor::getColorCode(Color color) {
    switch (color) {
        case Color::Black:          return "\033[30m";
        case Color::Red:            return "\033[31m";
        case Color::Green:          return "\033[32m";
        case Color::Yellow:         return "\033[33m";
        case Color::Blue:           return "\033[34m";
        case Color::Magenta:        return "\033[35m";
        case Color::Cyan:           return "\033[36m";
        case Color::White:          return "\033[37m";
        case Color::Gray:           return "\033[90m";
        case Color::LightRed:       return "\033[91m";
        case Color::LightGreen:     return "\033[92m";
        case Color::LightYellow:    return "\033[93m";
        case Color::LightBlue:      return "\033[94m";
        case Color::LightMagenta:   return "\033[95m";
        case Color::LightCyan:      return "\033[96m";
        case Color::LightWhite:     return "\033[97m";
        default:                    return "\033[0m"; 
    }
}

void TerminalColor::SetColor(Color color)
{
    std::cout << getColorCode(color);
}

void TerminalColor::PrintInColor(std::string txt, Color color)
{
    SetColor(color);
    std::cout << txt;
    SetColor(Color::White);
}
