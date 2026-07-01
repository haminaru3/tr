#pragma once
#include <string>
#include <iostream>

#include <random>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <thread>
#include <conio.h>

#include "protect/prot_defines.h"

class c_console {
public:
    void print_line(const std::string& text) const {
        if (text.empty()) { std::cout << "\n"; return; }
        std::cout << _str_crypt("\033[37m[\033[38;2;125;255;125m-\033[37m]\033[38;2;220;220;220m ") + text + _str_crypt("\033[0m") << std::endl;
    }

    void tick_title()
    {
        static const std::vector<std::wstring> titles = {
            _wstr_crypt(L":wave:"),
            _wstr_crypt(L"fakecri.me/trauma"),
            _wstr_crypt(L"for yougame.biz"),
            _wstr_crypt(L"14.08.24 Умер матвей мин.дамаг, я так и не успела признатся в любви."),
            _wstr_crypt(L"привет хочешь убить свою мать камнями тупой пидр")
        };

        static size_t current_index = 0;
        static size_t char_pos = 0;
        static int phase = 0;
        static DWORD last_tick = GetTickCount();
        static DWORD delay = 100;

        if (titles.empty()) return;

        DWORD current_tick = GetTickCount();
        if (current_tick - last_tick < delay) return;
        last_tick = current_tick;

        const std::wstring& current_title = titles[current_index];

        switch (phase) {
        case 0: {
            std::wstring temp = current_title.substr(0, char_pos);
            SetConsoleTitleW(temp.c_str());
            char_pos++;
            if (char_pos > current_title.length()) {
                phase = 1;
                delay = 2000;
            }
            break;
        }
        case 1: {
            phase = 2;
            delay = 50;
            break;
        }
        case 2: {
            std::wstring temp = current_title.substr(0, char_pos);
            SetConsoleTitleW(temp.c_str());
            char_pos--;
            if (char_pos == 0) {
                phase = 0;
                delay = 100;
                current_index = (current_index + 1) % titles.size();
            }
            break;
        }
        }
    }

    c_console() {
        str_crypt_start;
        DWORD _console = 0;
        HWND hwnd = GetConsoleWindow();
        RECT r; GetWindowRect(hwnd, &r);
        MoveWindow(hwnd,
            (GetSystemMetrics(SM_CXSCREEN) - 600) / 2,
            (GetSystemMetrics(SM_CYSCREEN) - 500) / 2,
            600, 500, TRUE);

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        GetConsoleMode(hOut, &_console);
        SetConsoleMode(hOut, _console | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(hIn, &_console);
        SetConsoleMode(hIn, _console & ~ENABLE_QUICK_EDIT_MODE);

        ShowScrollBar(hwnd, SB_VERT, 0);
        SetWindowLongA(hwnd, -16, GetWindowLongA(hwnd, -16) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);

        static const std::string warning = _str_crypt(" - \033[38;2;255;125;125mWARNING!\033[0m");
        static const std::string copyright = _str_crypt(
            "  |  THIS PROJECT IS COMPLETELY FREE!\n  |  MAKE SURE YOU DOWNLOADED IT FROM TRUSTED SOURCES.\n  |  LEGIT SOURCE: fakecri.me/trauma | AVOID SCAMS."
        );

        print_line("trauma/skeet project\0\0\0");
        print_line("build: public-release");
        print_line("");

        std::cout << warning << std::endl;
        std::cout << copyright << std::endl;

        print_line("");
        print_line("press any key to start...");
        str_crypt_end;
    }

    c_console& operator<<(const std::string& text) {
        print_line(text);
        return *this;
    }
};

inline c_console console;

#define add_log(x) console.print_line(x)