#include "imgui/imgui.h"

ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
    return ImVec2(a.x + b.x, a.y + b.y);
}

ImVec2 operator-(const ImVec2& a, const ImVec2& b) {
    return ImVec2(a.x - b.x, a.y - b.y);
}

ImVec2 operator*(const ImVec2& a, const double b) {
    return ImVec2(a.x * b, a.y * b);
}

ImVec2 operator*(const ImVec2& a, const float b) {
    return ImVec2(a.x * b, a.y * b);
}