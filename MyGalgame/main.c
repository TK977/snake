#include "StartUI.h"
#include "Settings.h"
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char* argv[]) {
#ifdef _WIN32
    // Windows 控制台设置 UTF-8 编码
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "zh_CN.UTF-8");
#endif

    // 初始化SDL子系统

    if (!initSDLsubsystem()) {
        return 1;
    }
    // Initialize Settings
    InitSettings();

    // 运行启动界面
    StartUICreate();

    // 清理资源
    uninitSDLsubsystem();

    return 0;
}