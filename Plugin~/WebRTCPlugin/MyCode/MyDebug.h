#include "pch.h"

// Create a callback delegate
typedef void (*OnMyDebugCallback) (const char *message, int color, int size);
static OnMyDebugCallback callbackInstance = nullptr;
void RegisterDebugCallback(OnMyDebugCallback cb);

//Color Enum
enum class Color { Red, Green, Blue, Black, White, Yellow, Orange };

class MyDebug
{
public:
    static void Log(const char* message, Color color = Color::Black);
    static void Log(const std::string message, Color color = Color::Black);
    static void Log(const int message, Color color = Color::Black);
    static void Log(const char message, Color color = Color::Black);
    static void Log(const float message, Color color = Color::Black);
    static void Log(const double message, Color color = Color::Black);
    static void Log(const bool message, Color color = Color::Black);

private:
    static void send_log(const std::stringstream &ss, const Color &color);
};