#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    // ���� �ð��� �õ尪���� ����Ͽ� srand() �Լ� ȣ��
    srand((unsigned int)time(NULL));

    // 8����Ʈ(64��Ʈ) ���� Ű ����
    unsigned long long key = ((unsigned long long)rand() << 32) | rand();

    printf("Random key: %llx\n", key);

    return 0;
}
