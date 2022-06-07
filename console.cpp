#include<Windows.h>
#include<iostream>
#include "test.h"

using namespace std;

int changeConsole() {
    char buf[0xFF];
    GetConsoleTitleA(buf, 0xFF); //获取原标题
    cout << buf << endl;

    SetConsoleTitleA("test"); //设置一个新标题

    GetConsoleTitleA(buf, 0xFF);//获取新标题
    cout << buf << endl;
}