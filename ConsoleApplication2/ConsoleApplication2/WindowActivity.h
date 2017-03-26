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
	int prevWidth = 0;
	int prevHeight = 0;

	void keyPress(WORD);
	void keyRelease(WORD);
	void getResolution(int &height, int &width);

public:
	WindowActivity();
	bool performAction(std::string);
	void overrideMouse(float x, float y,int height, int width);
	~WindowActivity();
};

