#pragma once
#include <string>
#include <iostream>

enum Color {
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    Gray,
    LightRed,
    LightGreen,
    LightYellow,
    LightBlue,
    LightMagenta,
    LightCyan,
    LightWhite
};

class TerminalColor
{
private:
	static std::string getColorCode(Color color);
public:
	static void SetColor(Color color);
	static void PrintInColor(std::string txt, Color color);
};

