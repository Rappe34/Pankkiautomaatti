#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

/* This wrapper is just for making sure that the едц letters show up when running the program in the terminal. */

int main(void) {
    system("chcp 65001 >nul"); // With this console command precisely
    system("pankkiautomaatti_core.exe");

    Sleep(1000);

    return 0;
}