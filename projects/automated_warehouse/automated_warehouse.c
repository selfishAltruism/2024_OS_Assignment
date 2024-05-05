#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "threads/init.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"

#include "devices/timer.h"

#include "projects/automated_warehouse/aw_message.h"
#include "projects/automated_warehouse/aw_manager.h"

struct robot* robots;

struct cnt_purpose {
        struct robot *robot;
        int mNum;
        char loadingDock;
};

struct purpose {
        struct cnt_purpose* cnt_purposes;
        int robotsN;
};

// 현재 위치에서 이동 가능한지 확인하는 함수
int is_valid_move(int row, int col, char direction) {
    switch (direction) {
        case 'U': return map_draw_default[row - 1][col] != 'X'; 
        case 'D': return map_draw_default[row + 1][col] != 'X'; 
        case 'L': return map_draw_default[row][col - 1] != 'X'; 
        case 'R': return map_draw_default[row][col + 1] != 'X'; 
        default: return 0; 
    }
}

// test code for central control node thread
void test_cnt(void* aux){
        struct purpose *purpose = ((struct cnt_purpose *)aux);
        struct cnt_purpose *cnt_purposes = purpose->cnt_purposes;
        int robotsN = purpose->robotsN;

        printf("\n%s\n", cnt_purposes->robot->name);

        int dest_row = 2;
        int dest_col = 0;

        int current_row = 1;
        int current_col = 1;

        while (current_row != dest_row || current_col != dest_col) {
                // 이동할 방향 결정

                if (current_row > dest_row && is_valid_move(current_row, current_col, 'U')) {
                        current_row--;
                } else if (current_row < dest_row && is_valid_move(current_row, current_col, 'D')) {
                        current_row++;
                } else if (current_col > dest_col && is_valid_move(current_row, current_col, 'L')) {
                        current_col--;
                } else if (current_col < dest_col && is_valid_move(current_row, current_col, 'R')) {
                        current_col++; 
                } else {
                        // 이동할 수 없는 경우, 에러 처리 등 필요한 작업 수행
                        printf("Error: Cannot move to destination.\n");
                        break;
                }

                // 이동 메시지 보내기
                struct message move_msg = {
                row: current_row,
                col: current_col,
                current_payload: 0,
                required_payload: 0,
                cmd: 0
                };
                send_message_to_control_node(0, move_msg);

                unblock_threads();

                while(receive_message_from_robot(0).cmd == -1){
                }
                        
                printf("\nreceive!\n");

                print_map(robots, robotsN);
        }
        printf("\n %d %d", current_row, current_col);
}

// test code for robot thread
void test_thread(void* aux){
        struct cnt_purpose info = *((struct cnt_purpose *)aux);
        printf("%d thread %s : %d\n", thread_current ()-> tid, info.robot->name);

        block_thread();

        struct message cnt_message;

        if(strcmp(info.robot->name, "R1") == 0){

                while(1){
                        cnt_message = receive_message_from_control_node(0);
                        while(cnt_message.cmd == -1){
                                cnt_message = receive_message_from_control_node(0);
                                printf("\n%d", cnt_message.cmd);
                        }

                        printf("\nok");
                        
                        setRobot(info.robot, info.robot->name, cnt_message.row, cnt_message.col, cnt_message.required_payload, cnt_message.current_payload);

                        struct message message = {
                                row: 0,
                                col: 0,
                                current_payload: 0,
                                required_payload: 0,
                                cmd: 1
                        };

                        send_message_to_robot(0,message);

                        block_thread();
                }

        }else{
                block_thread();
        }
        
}

// entry point of simulator
void run_automated_warehouse(char **argv)
{
        init_automated_warehouse(argv); // do not remove this

        printf("implement automated warehouse!\n");

        //init blocked_threads
        list_init(&blocked_threads);

        //robot define
        int robotsN = atoi(argv[1]);
        robots = malloc(sizeof(struct robot) * robotsN);
        

        //thread define
        tid_t* threads = malloc(sizeof(tid_t) * (robotsN + 1));

        //define cnt_purposes
        struct cnt_purpose* cnt_purposes;
        cnt_purposes = malloc(sizeof(struct cnt_purpose) * (robotsN));

        //define message_boxes
        allocate_message_boxes(robotsN);


        int i = 0;
        char *robotsSet;
        size_t len = strlen(argv[2]) + 1;
        char *str = malloc(len);

        if (str != NULL) {
                memcpy(str, argv[2], len);
        }
        char *token = strtok_r(str, ":", &robotsSet);

        while (token != NULL) {
                int mNum = atoi(token);
                char* robotName;
                robotName = malloc(sizeof(char) + sizeof(int));

                snprintf(robotName, 4, "R%d", i+1);

                char loadingDock = token[strlen(token)-1];

                //will be delete part
                printf("%s", robotName);
                printf(": %d + %c\n", mNum, loadingDock);

                cnt_purposes[i].robot = &robots[i];
                cnt_purposes[i].mNum = mNum;
                cnt_purposes[i].loadingDock = loadingDock;
                
                //set robot & thread
                setRobot(&robots[i], robotName, 5, 5, 0, 0);
                threads[i + 1] = thread_create(robotName, 0, &test_thread, &cnt_purposes[i]);

                i++;
                token = strtok_r(NULL, ":", &robotsSet);
        }

        struct purpose purpose = {
                cnt_purposes: cnt_purposes,
                robotsN: robotsN,
        };

        threads[0] = thread_create("CNT", 0, &test_cnt, &purpose);

        free(str);  
}
