#include <iostream>
#include <thread>
#include "console.hpp"
#include "mapper/mapper.hpp"
#include <conio.h>
#include "protect/prot_defines.h"

c_mapper g_mapper;
void mapper_thread() { 
    console << _str_crypt("trauma/skeet project");
    console << _str_crypt(" - fakecri.me/trauma");
    console << "";

    g_mapper.main_thread();
}

void console_thread() {
    while (true) {
        console.tick_title();
    }
}

int APIENTRY main(HINSTANCE, HINSTANCE, LPSTR, int) {
    std::thread console_title(console_thread);
    _getch();
    system("cls");
    std::thread mapper(mapper_thread);
    mapper.join();
    
    return 0;
}
