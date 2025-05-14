#include "../include/Manager.h"
#include <iostream>
#include <Windows.h>

int main() {
    Manager manager;

    //manager.run()하면 config.json에 설정되어 있는 LAMPAD의 모든 피드의 최근 5분 BPS 데이터를 가져와서 저장함.
    while(1){
        manager.run();
        Sleep(10000);
    }

    return 0;
}