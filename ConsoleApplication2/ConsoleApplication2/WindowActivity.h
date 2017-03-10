#pragma once
#include <windows.h>
#include <string>
#define WINVER 0x0500

enum Action {
	Copy, Paste
};

class WindowActivity {
private:
	INPUT inputEvent;

	void keyPress(WORD);
	void keyRelease(WORD);
public:
	WindowActivity();
	bool performAction(std::string);
	~WindowActivity();
};

