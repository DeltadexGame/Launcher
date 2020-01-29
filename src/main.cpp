#include "Launcher.h"

#include <windows.h>
#include <tchar.h>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib,"wininet.lib")
using namespace std;

int main() {
  Launcher launcher;
  // Run the launcher
  launcher.Run();
  return 0;
}