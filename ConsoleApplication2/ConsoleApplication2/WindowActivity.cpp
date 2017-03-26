#include "WindowActivity.h"
#include <iostream>



WindowActivity::WindowActivity() {
	inputEvent.type = INPUT_KEYBOARD;
	inputEvent.ki.wScan = 0;
	inputEvent.ki.time = 0;
	inputEvent.ki.dwExtraInfo = 0;
}

void WindowActivity::keyPress(WORD key) {
	inputEvent.ki.wVk = key;
	inputEvent.ki.dwFlags = 0;
	SendInput(1, &inputEvent, sizeof(INPUT));
}

void WindowActivity::keyRelease(WORD key) {
	inputEvent.ki.wVk = key;
	inputEvent.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &inputEvent, sizeof(INPUT));
}

void WindowActivity::getResolution(int & height, int & width) {
	RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (width, height)
   height = desktop.bottom;
   width = desktop.right;
}

bool WindowActivity::performAction(std::string action) {
	if (action == "c" || action == "C") {
		keyPress(VK_CONTROL);
		keyPress(0x43);
		keyRelease(0x43);
		keyRelease(VK_CONTROL);
		return true;
	} else if (action == "v" || action == "V") {
		std::cout << "Paste worked!" << std::endl;
		keyPress(VK_CONTROL);
		keyPress(0x56);
		keyRelease(0x56);
		keyRelease(VK_CONTROL);
		return true;
	}
	return false;
}

// x is width, y is height
void WindowActivity::overrideMouse(float x, float y, int height, int width) {
	if (x != -1 && y != -1) {
			double factor = 1;

			int screenHeight, screenWidth;
			//get Resolution
			getResolution(screenHeight, screenWidth);


			POINT point;
			GetCursorPos(&point);

			//int newx = point.x - prevWidth + x * screenWidth * factor;
			//int newy = point.y - prevHeight + y * screenHeight * factor;
			//

			SetCursorPos(point.x - x * screenWidth, point.y - y * screenHeight);

			//
			//GetCursorPos(&point);

			//prevHeight = point.y;
			//prevWidth = point.x;

	}

}



WindowActivity::~WindowActivity() {}
