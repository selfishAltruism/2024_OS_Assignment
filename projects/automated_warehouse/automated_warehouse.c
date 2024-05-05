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

#define MAP_DRAW_DEFAULT_ROW 6
#define MAP_DRAW_DEFAULT_COL 7

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
        case 'U': return map_draw_default[row - 1][col] != 'X' && map_draw_default[row - 1][col] != 'W'; 
        case 'D': return map_draw_default[row + 1][col] != 'X' && map_draw_default[row + 1][col] != 'W'; 
        case 'L': return map_draw_default[row][col - 1] != 'X' && map_draw_default[row][col - 1] != 'W'; 
        case 'R': return map_draw_default[row][col + 1] != 'X' && map_draw_default[row][col + 1] != 'W'; 
        default: return 0; 
    }
}

void findValue(char value, int* row_index, int* col_index) {
    *row_index = -1; 
    *col_index = -1; 

    for (int i = 0; i < MAP_DRAW_DEFAULT_ROW; i++) {
        for (int j = 0; j < MAP_DRAW_DEFAULT_COL; j++) {
            if (map_draw_default[i][j] == value) {
                *row_index = i;
                *col_index = j;
                return;
            }
        }
    }
}

// test code for central control node thread
void test_cnt(void* aux){
        struct purpose *purpose = ((struct cnt_purpose *)aux);
        struct cnt_purpose *cnt_purposes = purpose->cnt_purposes;
        int robotsN = purpose->robotsN;

        int dest_row;
        int dest_col;

        int current_row = 5;
        int current_col = 5;

        char mNum;

        for(int i = 0; i < robotsN; i++){
                
                //move to loading area
                mNum = cnt_purposes[i].mNum + '0';
                findValue(mNum, &dest_row, &dest_col);

                while (current_row != dest_row || current_col != dest_col) {
                        if (current_row > dest_row && is_valid_move(current_row, current_col, 'U')) {
                                current_row--;
                        } else if (current_row < dest_row && is_valid_move(current_row, current_col, 'D')) {
                                current_row++;
                        } else if (current_col > dest_col && is_valid_move(current_row, current_col, 'L')) {
                                current_col--;
                        } else if (current_col < dest_col && is_valid_move(current_row, current_col, 'R')) {
                                current_col++; 
                        } else {
                                printf("Error: Cannot move to destination.\n");
                                break;
                        }

                        struct message move_msg = {
                                row: current_row,
                                col: current_col,
                                current_payload: 0,
                                required_payload: cnt_purposes[i].mNum,
                                cmd: 0
                        };
                        send_message_to_control_node(i, move_msg);

                        unblock_threads();
                        increase_step();

                        while(receive_message_from_robot(i).cmd == -1){
                        }

                        print_map(robots, robotsN);
                }

                //loading
                        struct message move_msg = {
                                row: current_row,
                                col: current_col,
                                current_payload: cnt_purposes[i].mNum,
                                required_payload: cnt_purposes[i].mNum,
                                cmd: 0
                        };
                        send_message_to_control_node(i, move_msg);

                        unblock_threads();
                        increase_step();

                //move to unloading area
                findValue(cnt_purposes[i].loadingDock, &dest_row, &dest_col);

                while (current_row != dest_row || current_col != dest_col) {
                        if (current_row > dest_row && is_valid_move(current_row, current_col, 'U')) {
                                current_row--;
                        } else if (current_row < dest_row && is_valid_move(current_row, current_col, 'D')) {
                                current_row++;
                        } else if (current_col > dest_col && is_valid_move(current_row, current_col, 'L')) {
                                current_col--;
                        } else if (current_col < dest_col && is_valid_move(current_row, current_col, 'R')) {
                                current_col++; 
                        } else {
                                printf("Error: Cannot move to destination.\n");
                                break;
                        }

                        struct message move_msg = {
                                row: current_row,
                                col: current_col,
                                current_payload: cnt_purposes[i].mNum,
                                required_payload: cnt_purposes[i].mNum,
                                cmd: 0
                        };
                        send_message_to_control_node(i, move_msg);

                        unblock_threads();
                        increase_step();

                        while(receive_message_from_robot(i).cmd == -1){
                        }

                        print_map(robots, robotsN);
                }
        }
        printf("\nfinish!!!!!!\n");
}

// test code for robot thread
void test_thread(void* aux){
        struct cnt_purpose info = *((struct cnt_purpose *)aux);

        block_thread();


        struct message cnt_message;

        int message_index = info.robot->name[1] - '0' - 1;

        while(1){
                cnt_message = receive_message_from_control_node(message_index);
                if(cnt_message.cmd > -1){
                        setRobot(info.robot, info.robot->name, cnt_message.row, cnt_message.col, cnt_message.required_payload, cnt_message.current_payload);

                        struct message message = {
                                row: 0,
                                col: 0,
                                current_payload: 0,
                                required_payload: 0,
                                cmd: 1
                        };

                        send_message_to_robot(message_index,message);
                }
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

                setRobot(&robots[i], robotName, 5, 5, 0, mNum);

                cnt_purposes[i].robot = &robots[i];
                cnt_purposes[i].mNum = mNum;
                cnt_purposes[i].loadingDock = loadingDock;
                
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
