#pragma once

class c_menu {
public:
	void frame();
	void init_fonts();

	void cpicker(const char* name, float* color, bool alpha);
	bool keybind(const char* label, int* key, bool show_label);

	void aimbot();
	void visuals();
	void weapons();
	void settings();
};