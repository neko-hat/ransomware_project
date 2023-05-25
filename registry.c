#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    // 현재 시간을 시드값으로 사용하여 srand() 함수 호출
    srand((unsigned int)time(NULL));

    // 8바이트(64비트) 랜덤 키 생성
    unsigned long long key = ((unsigned long long)rand() << 32) | rand();

    printf("Random key: %llx\n", key);

    return 0;
}
