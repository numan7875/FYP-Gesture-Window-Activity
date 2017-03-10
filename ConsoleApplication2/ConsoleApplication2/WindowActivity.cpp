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

WindowActivity::~WindowActivity() {}
