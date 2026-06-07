// 가상 메모리 페이지 교체 알고리즘 시뮬레이터
// 소스코드 설명은 주석으로 처리
#include <stdio.h>
#include <string.h>

#define MAX_FRAME_SIZE 5
#define MAX_STRING_SIZE 10

// 프레임에 페이지 찾기 - 공용
int in_Frame(char frame[], int frame_size, char page) {
    for (int i = 0; i < frame_size; i++) { // 프레임 수 만큼 반복
        if (frame[i] == page) { // 만약 프레임에 페이지가 있으면
            return i; // 페이지 자리 반환
        }
    }

    return -1; // 없으면 -1 반환
}

// 가장 오래 안 쓴 프레임 찾기 - LRU 함수
int Last_used_Frame(int lastUse[], int frame_size) {
    int min = 0; // 가장 오래 안 쓴 페이지 자리

    for (int i = 1; i < frame_size; i++) { // 프레임 수 만큼 반복
        if (lastUse[i] < lastUse[min]) // i번 프레임이 가장 오래 안 썼다면
            min = i; // 페이지 저장
    }

    return min; // 가장 오래 안 쓴 페이지 자리 반환
}

// 교체 대상 프레임 찾기 - OPT 함수
int find_change_Frame(char frame[], int frame_size, char str[], int curr) {
    int far = -1; // 가장 늦게 다시 사용되는 위치
    int last_pos = 0; // 교체 대상 프레임 위치

    for (int i = 0; i < frame_size; i++) { // i <- 프레임 위치
        int nextUse = -1; // 프레임과 일치하는 페이지 위치

        for (int j = curr + 1; j < MAX_STRING_SIZE; j++) { // 현재 위치 다음부터 탐색
            if (frame[i] == str[j]) { // 프레임과 페이지가 일치하면
                nextUse = j; // 찾은 페이지 위치 저장
                break; // 반복문 빠져 나오기
            }
        }

        if (nextUse == -1) { // 앞으로 다시 사용되지 않는 페이지라면
            return i; // 즉시 교체 대상 선택
        }

        if (nextUse > far) { // 찾은 위치 보다 더 멀다면?
            far = nextUse; // 더 먼 페이지 위치로 변경
            last_pos = i; // 교체할 프레임 위치로 변경
        }
    }

    return last_pos; // 교체할 프레임 위치 반환
}

//FIFO 구현
void FIFO(int frame_size, char str[]) {
    char frame[MAX_FRAME_SIZE];
    int size = 0; // 현재 사용 중인 프레임 개수
    int next = 0; // 다음에 교체될 프레임 위치
    int faults = 0; // 페이지 부재가 일어난 횟수
    int string_size = strlen(str); // 총 페이지 개수

    printf("FIFO\n");

    printf("Result: ");
    for (int i = 0; str[i] != '\0'; i++) {  // 페이지가 공백이 아닐 때까지 실행
        char page = str[i]; // 찾을 페이지 설정

        if (in_Frame(frame, size, page) != -1) { // 프레임 확인
            printf("S "); // 성공하면 S 출력
        }
        else {
            printf("F "); // 실패하면 F 출력
            faults++; // 실패 횟수 추가

            if (size < frame_size) {// 프레임이 비었다면
                frame[size] = page; // 프레임에 페이지 추가하고
                size++; // 프레임 수 증가
            }
            else { // 프레임이 다 찼다면
                frame[next] = page; // 가장 처음에 들어온 페이지를 변경
                next = (next + 1) % frame_size; // 다음에 교체할 프레임 위치로 이동
            }
        }
    }
    printf("\nPage Faults: %d\n", faults);

    double faultRate = (double)faults / string_size * 100; // 페이지 부재 퍼센트 계산
    printf("Fault Rate: %.1f%%\n\n", faultRate);
}

// LRU 구현
void LRU(int frame_size, char str[]) {
    char frame[MAX_FRAME_SIZE];
    int lastUse[MAX_FRAME_SIZE];

    int size = 0;
    int faults = 0;
    int string_size = strlen(str);

    printf("LRU\n");

    printf("Result: ");
    for (int time = 0; str[time] != '\0'; time++) { // 페이지가 공백이 아닐 때까지 실행
        char page = str[time];

        int pos = in_Frame(frame, size, page); // 일치하는 프레임 위치

        if (pos != -1) { // 프레임에 일치한다면
            printf("S ");
            lastUse[pos] = time; // 일치하는 프레임 자리에 사용한 시간대 입력
        }
        else {
            printf("F ");
            faults++;

            if (size < frame_size) { // 프레임이 비었다면
                frame[size] = page; // 프레임에 페이지 추가
                lastUse[size] = time; // 추가한 프레임 자리에 사용한 시간대 입력
                size++; // 프레임 수 증가
            }
            else { // 프레임이 찼다면
                int last_pos = Last_used_Frame(lastUse, frame_size); // 가장 오래 전에 사용된 프레임 위치

                frame[last_pos] = page; // 프레임 자리에 페이지 변경
                lastUse[last_pos] = time; // 변경한 프레임 자리에 사용한 시간대 입력
            }
        }
    }
    printf("\nPage Faults: %d\n", faults);

    double faultRate = (double)faults / string_size * 100;
    printf("Fault Rate: %.f%%\n\n", faultRate);
}

// OPT 구현
void OPT(int frame_size, char str[]) {
    char frame[MAX_FRAME_SIZE];
    int size = 0;
    int faults = 0;

    printf("OPT\n");

    printf("Result: ");
    for (int i = 0; i < MAX_STRING_SIZE; i++) { // 문자열 길이만큼 반복
        char page = str[i]; // 찾을 페이지 설정

        if (in_Frame(frame, size, page) != -1) { // 프레임 확인
            printf("S ");
        }
        else {
            printf("F ");
            faults++;

            if (size < frame_size) {
                frame[size] = page;
                size++;
            }
            else {
                int last_pos; // 변수 지정
                last_pos = find_change_Frame(frame, frame_size, str, i); // 교체 대상 프레임 찾아서 변수에 저장
                frame[last_pos] = page; // 교체 대상 프레임 위치에 변경
            }
        }
    }
    printf("\nPage Faults: %d\n", faults);

    double faultRate = (double)faults / MAX_STRING_SIZE * 100;
    printf("Fault Rate: %.0f%%\n\n", faultRate);
}

int main() {
    char str[MAX_STRING_SIZE + 1];
    int frame_size;

    printf("Frame >> ");
    scanf("%d", &frame_size);

    printf("Page_String >> ");
    for (int i = 0; i < MAX_STRING_SIZE; i++) {
        scanf(" %c", &str[i]);
    }
    str[MAX_STRING_SIZE] = '\0';

    printf("\n");

    FIFO(frame_size, str);
    LRU (frame_size, str);
    OPT (frame_size, str);

    return 0;
}