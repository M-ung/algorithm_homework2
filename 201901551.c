#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FAMILY 16           // 최대 가족 수를 정의한다.
#define EARLY_ARRIVAL 6         // 초기 도착한 가족 수를 정의한다.
#define MAX_TICKET_BOOTH 4      // 매표소의 총 개수를 정의한다.
#define MAX_ENDTIME 18          // 최대 스케줄링 시간을 정의한다.

typedef struct {                // 가족 정보를 담는 구조체를 정의한다.
    char id;                    // 가족의 알파벳
    int familySize;             // 가족의 구성원 수
} Family;

typedef struct {               // 매표소 상태를 담는 구조체를 정의한다.
    int nextAvailableTime;     // 다음 작업을 처리할 수 있는 가장 빠른 시간
    char tasks[MAX_ENDTIME];   // 각 매표소에 할당된 작업을 저장하는 배열
} TicketBooth;

Family families[MAX_FAMILY];   // 가족 정보 배열을 선언한다.
TicketBooth ticketBooths[MAX_TICKET_BOOTH];  // 매표소 상태 배열을 선언한다.

// 함수 선언
void init();                   // 파일 입출력으로 txt 파일을 읽어오는 함수   
void run();                    // 알고리즘을 실행해주는 함수로, 최종적으로 출력형태가 나올 수 있도록 한다
void arrange();                // 가족 정보 배열 정렬, 매표소 상태 배열 초기화를 해주는 함수이다.
void scheduleTasks();          // 각 가족을 매표소에 스케줄링하는 함수로, 구성원 수와 도착 시간을 기준으로 최적의 매표소에 할당한다.
void printSchedule();          // 스케줄링 결과를 화면에 출력하는 함수로, 각 매표소에서 어떤 가족 구성원이 언제 처리되는지를 나타낸다.
int compareFamily(const void* a, const void* b); // qsort를 위한 비교 함수로, 가족의 구성원 수와 가족 알파벳을 기준으로 정렬한다.

int main() {
    init();                    // 파일 입출력으로 txt 파일을 읽어오는 함수
    run();                     // 스케줄링과 결과 출력 함수 호출
    return 0;                  // 메인 함수 종료
}

// 파일 입출력으로 txt 파일을 읽어오는 함수 
void init() {
    FILE* file = fopen("customer.txt", "r");    // customer.txt 파일을 읽기 모드로 열기
    if (!file) {
        printf("파일을 열 수 없다.\n");
        exit(1);
    }

    for (int i = 0; i < MAX_FAMILY; i++) {
        // 파일에서 문자와 숫자를 읽어와서 가족 정보 배열에 저장한다.
        if (fscanf(file, " %c %d", &families[i].id, &families[i].familySize) != 2) {
            printf("파일 형식이 잘못되었다.\n");
            exit(1);
        }
    }
    fclose(file);   // 파일 닫기
}

// 알고리즘을 실행해주는 함수로, 최종적으로 출력형태가 나올 수 있도록 한다
void run() {
    printf("201901551 김정목\n");
    scheduleTasks();   // 스케줄링 함수 호출
    printSchedule();   // 스케줄 결과 출력 함수 호출
}

// 가족 정보 배열 정렬, 매표소 상태 배열 초기화를 해주는 함수이다.
void arrange() {
    // 가족들을 두 그룹으로 나눠 각각 정렬한다.
    qsort(families, EARLY_ARRIVAL, sizeof(Family), compareFamily);
    qsort(&families[EARLY_ARRIVAL], MAX_FAMILY - EARLY_ARRIVAL, sizeof(Family), compareFamily);

    // 각 매표소의 초기 nextAvailableTime을 설정한다.
    for (int i = 0; i < MAX_TICKET_BOOTH; i++) {
        if (i < 2) { 
            // 1번, 2번 매표소의 경우 18시(0)부터 시작한다.
            ticketBooths[i].nextAvailableTime = 0;
        } else { 
            // 3번, 4번 매표소의 경우 19시 30분(9)부터 시작한다.
            ticketBooths[i].nextAvailableTime = 9;
        }
        // 매표소의 작업 스케줄을 'X'로 초기화한다.
        memset(ticketBooths[i].tasks, 'X', sizeof(ticketBooths[i].tasks));
    }
}

// 각 가족을 매표소에 스케줄링하는 함수로, 구성원 수와 도착 시간을 기준으로 최적의 매표소에 할당한다.
void scheduleTasks() {
    arrange(); // arrage 함수 호출

    // 18시 00분
    for (int i = 0; i < EARLY_ARRIVAL; i++) {
        int earliestTime = MAX_ENDTIME + 1;  // 최소 시작 시간을 기록하는 변수. 초기값은 최대값으로 설정한다.
        int selectedBooth = -1;  // 선택된 매표소의 인덱스를 저장하는 변수. 초기값은 -1(선택 안됨)으로 설정한다.

        // 모든 매표소를 순회하며 가장 빠르게 가족을 수용할 수 있는 매표소를 찾는다.
        // j의 반복 범위를 0 ~ 2(1, 2 매표소)가 아닌 0 ~ MAX_TICKET_BOOTH로 둔 이유는 19시 30분이 지나도 들어가지 못 한 가족이 있다면, 19시 30분 이후에는 모든 매표소에서 입장시키기 위해서이다.
        for (int j = 0; j < MAX_TICKET_BOOTH; j++) { 
            if (ticketBooths[j].nextAvailableTime < earliestTime) { // 현재 탐색 중인 매표소의 다음 가능 시간이 기록된 최소 시작 시간보다 빠르면
                earliestTime = ticketBooths[j].nextAvailableTime; // 최소 시작 시간을 업데이트하고
                selectedBooth = j; // 해당 매표소의 인덱스를 저장한다.
            }
        }

        // 선택된 매표소가 없는 경우 스케줄링에 실패했다고 출력하고 프로그램을 종료한다.
        if (selectedBooth == -1) {
            printf("스케줄링에 실패했다.\n");
            exit(1);
        }

        // 선택된 매표소의 작업 배열에 현재 가족의 작업을 배정한다.
        for (int k = 0; k < families[i].familySize; k++) {
            ticketBooths[selectedBooth].tasks[earliestTime + k] = families[i].id;
        }

        ticketBooths[selectedBooth].nextAvailableTime += families[i].familySize; // 해당 매표소의 다음 가능 시간을 가족의 구성원 수만큼 증가시켜 업데이트한다.
    }

    // 19시 30분 전에 남은 가족이 없어서 빈 시간을 가진 매표소가 있는지 확인. 만약 있다면 nextAvailableTime을 9로 한다.
    if(ticketBooths[0].nextAvailableTime < 9) {
        ticketBooths[0].nextAvailableTime = 9;
    }
    if(ticketBooths[1].nextAvailableTime < 9) {
        ticketBooths[1].nextAvailableTime = 9;
    }

    // 19시 30분 
    for (int i = EARLY_ARRIVAL; i < MAX_FAMILY; i++) {
        int earliestTime = MAX_ENDTIME + 1;  // 최소 시작 시간을 기록하는 변수. 초기값은 최대값으로 설정한다.
        int selectedBooth = -1;  // 선택된 매표소의 인덱스를 저장하는 변수. 초기값은 -1(선택 안됨)으로 설정한다.

        for (int j = 0; j < MAX_TICKET_BOOTH; j++) { // 모든 매표소를 순회하며 가장 빠르게 가족을 수용할 수 있는 매표소를 찾는다.
            if (ticketBooths[j].nextAvailableTime < earliestTime) { // 현재 탐색 중인 매표소의 다음 가능 시간이 기록된 최소 시작 시간보다 빠르면
                earliestTime = ticketBooths[j].nextAvailableTime; // 최소 시작 시간을 업데이트하고
                selectedBooth = j; // 해당 매표소의 인덱스를 저장한다.
            }
        }

        // 선택된 매표소가 없는 경우 스케줄링에 실패했다고 출력하고 프로그램을 종료한다.
        if (selectedBooth == -1) {
            printf("스케줄링에 실패했다.\n");
            exit(1);
        }

        // 선택된 매표소의 작업 배열에 현재 가족의 작업을 배정한다.
        for (int k = 0; k < families[i].familySize; k++) {
            ticketBooths[selectedBooth].tasks[earliestTime + k] = families[i].id;
        }

        ticketBooths[selectedBooth].nextAvailableTime += families[i].familySize; // 해당 매표소의 다음 가능 시간을 가족의 구성원 수만큼 증가시켜 업데이트한다.
    }
}


// 스케줄링 결과를 화면에 출력하는 함수로, 각 매표소에서 어떤 가족 구성원이 언제 처리되는지를 나타낸다.
void printSchedule() {
    for (int i = 0; i < MAX_TICKET_BOOTH; i++) {
        // 각 매표소의 스케줄 결과를 출력한다.
        printf("%d번 매표소: ", i + 1);
        for (int j = 0; j < MAX_ENDTIME; j++) {
            printf("%c", ticketBooths[i].tasks[j]);
        }
        printf("\n");
    }
}

// qsort를 위한 비교 함수로, 가족의 구성원 수와 가족 알파벳을 기준으로 정렬한다.
int compareFamily(const void* a, const void* b) {
    // 가족 정보를 비교하는 함수이다.
    Family family1 = *(Family*)a;
    Family family2 = *(Family*)b;

    if (family1.familySize == family2.familySize) {        // 구성원 수가 같다면,
        return (family1.id - family2.id);                  // 알파벳 순서로 비교한다.
    } else {                                               // 구성원 수가 다르다면,
        return (family1.familySize - family2.familySize);  // 구성원 수로 비교한다.
    }
}