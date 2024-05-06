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
        int loading_area;
        char unloading_area;
};

struct purpose {
        struct cnt_purpose* cnt_purposes;
        int robotsN;
};

int check_valid_move(int row, int col, char direction, char loading_area, char unloading_area){
        return map_draw_default[row][col] == loading_area || map_draw_default[row][col] == ' ' || map_draw_default[row][col] == unloading_area || map_draw_default[row][col] == 'S';
}

int is_valid_move(int row, int col, char direction, char loading_area, char unloading_area, int is_loading) {
        if(is_loading == 0){
                switch (direction) {
                        case 'U': return check_valid_move(row - 1, col, direction, loading_area, unloading_area) && (map_draw_default[row - 1][col] == loading_area || check_valid_move(row - 2, col, direction, loading_area, unloading_area) || check_valid_move(row - 1, col - 1, direction, loading_area, unloading_area) || check_valid_move(row - 1, col + 1, direction, loading_area, unloading_area));
                        case 'D': return check_valid_move(row + 1, col, direction, loading_area, unloading_area) && (map_draw_default[row + 1][col] == loading_area || check_valid_move(row + 2, col, direction, loading_area, unloading_area) || check_valid_move(row + 1, col - 1, direction, loading_area, unloading_area) || check_valid_move(row + 1, col + 1, direction, loading_area, unloading_area));
                        case 'L': return check_valid_move(row, col - 1, direction, loading_area, unloading_area) && (map_draw_default[row][col - 1] == loading_area || check_valid_move(row, col - 2, direction, loading_area, unloading_area) || check_valid_move(row + 1, col - 1, direction, loading_area, unloading_area) || check_valid_move(row - 1, col - 1, direction, loading_area, unloading_area));
                        case 'R': return check_valid_move(row, col + 1, direction, loading_area, unloading_area) && (map_draw_default[row][col + 1] == loading_area || check_valid_move(row, col + 2, direction, loading_area, unloading_area) || check_valid_move(row + 1, col + 1, direction, loading_area, unloading_area) || check_valid_move(row - 1, col + 1, direction, loading_area, unloading_area));
                        default: return 0; 
                }
        }else{
                switch (direction) {
                        case 'U': return check_valid_move(row - 1, col, direction, loading_area, unloading_area) && (map_draw_default[row - 1][col] == unloading_area || check_valid_move(row - 2, col, direction, loading_area, unloading_area) || check_valid_move(row - 1, col - 1, direction, loading_area, unloading_area) || check_valid_move(row - 1, col + 1, direction, loading_area, unloading_area));
                        case 'D': return check_valid_move(row + 1, col, direction, loading_area, unloading_area) && (map_draw_default[row + 1][col] == unloading_area || check_valid_move(row + 2, col, direction, loading_area, unloading_area) || check_valid_move(row + 1, col - 1, direction, loading_area, unloading_area) || check_valid_move(row + 1, col + 1, direction, loading_area, unloading_area));
                        case 'L': return check_valid_move(row, col - 1, direction, loading_area, unloading_area) && (map_draw_default[row][col - 1] == unloading_area || check_valid_move(row, col - 2, direction, loading_area, unloading_area) || check_valid_move(row + 1, col - 1, direction, loading_area, unloading_area) || check_valid_move(row - 1, col - 1, direction, loading_area, unloading_area));
                        case 'R': return check_valid_move(row, col + 1, direction, loading_area, unloading_area) && (map_draw_default[row][col + 1] == unloading_area || check_valid_move(row, col + 2, direction, loading_area, unloading_area) || check_valid_move(row + 1, col + 1, direction, loading_area, unloading_area) || check_valid_move(row - 1, col + 1, direction, loading_area, unloading_area));
                        default: return 0; 
                }  
        }

}


int check_cycle(char direction, char* record_move) {
        return !(record_move[1] == direction && record_move[0] == record_move[2] && record_move[2] != direction);
}

int save_direction(char direction, char* record_move){
        record_move[0] = record_move[1];
        record_move[1] = record_move[2];
        record_move[2] = direction;
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
void control_cnt(void* aux){
        struct purpose *purpose = ((struct cnt_purpose *)aux);
        struct cnt_purpose *cnt_purposes = purpose->cnt_purposes;
        int robotsN = purpose->robotsN;

        struct message robot_message;

        int dest_row;
        int dest_col;

        int current_row;
        int current_col;

        char loading_area;

        printf("\nautomated warehouse starts operating!\n");

        printf("\n===========================\n");
        print_map(robots, robotsN);

        for(int i = 0; i < robotsN; i++){
                findValue('W', &current_row, &current_col);
                
                //move to loading area phase
                loading_area = cnt_purposes[i].loading_area + '0';
                findValue(loading_area, &dest_row, &dest_col);

                char record_move[3] = "   ";

                while (current_row != dest_row || current_col != dest_col) {
                        if (current_row > dest_row && is_valid_move(current_row, current_col, 'U', loading_area, cnt_purposes[i].unloading_area, 0) && check_cycle('U', record_move)) {
                                save_direction('U', record_move);
                                current_row--;
                        } else if (current_row < dest_row && is_valid_move(current_row, current_col, 'D', loading_area, cnt_purposes[i].unloading_area, 0) && check_cycle('D', record_move)) {
                                save_direction('D', record_move);
                                current_row++;
                        } else if (current_col > dest_col && is_valid_move(current_row, current_col, 'L', loading_area, cnt_purposes[i].unloading_area, 0) && check_cycle('L', record_move)) {
                                save_direction('L', record_move);
                                current_col--;
                        } else if (current_col < dest_col && is_valid_move(current_row, current_col, 'R', loading_area,cnt_purposes[i].unloading_area, 0) && check_cycle('R', record_move)) {
                                save_direction('R', record_move);
                                current_col++; 
                        } else {
                                if (is_valid_move(current_row, current_col, 'U', loading_area, cnt_purposes[i].unloading_area, 0) && check_cycle('D', record_move)) {
                                        save_direction('U', record_move);
                                        current_row--;
                                } else if (is_valid_move(current_row, current_col, 'D', loading_area, cnt_purposes[i].unloading_area, 0) && check_cycle('D', record_move)) {
                                        save_direction('D', record_move);
                                        current_row++;
                                } else if (is_valid_move(current_row, current_col, 'L', loading_area, cnt_purposes[i].unloading_area, 0) && check_cycle('D', record_move)) {
                                        save_direction('L', record_move);
                                        current_col--;
                                } else if (is_valid_move(current_row, current_col, 'R', loading_area,cnt_purposes[i].unloading_area, 0) && check_cycle('D', record_move)) {
                                        save_direction('R', record_move);
                                        current_col++; 
                                } else {
                                        printf("Error: Cannot move to destination.\n");
                                        break;
                                }
                        }

                        for(int j = 0; j < robotsN; j++){
                                if(j == i){
                                        struct message move_msg = {
                                                row: current_row,
                                                col: current_col,
                                                current_payload: 0,
                                                required_payload: cnt_purposes[i].loading_area,
                                                cmd: 0
                                        };
                                        send_message_to_control_node(i, move_msg);
                                }else{
                                        struct message standby_msg = {
                                                row: -1,
                                                col: -1,
                                                current_payload: -1,
                                                required_payload: -1,
                                                cmd: 0
                                        };
                                        send_message_to_control_node(j, standby_msg);
                                }                        
                        }

                        unblock_threads();
                        increase_step();

                        //stand by message
                        for(int j = 0; j < robotsN; j++){
                                robot_message = receive_message_from_robot(j);
                                while(robot_message.cmd < 0){
                                        robot_message = receive_message_from_robot(j);
                                }
                                if(robot_message.row >= 0){
                                        current_row = robot_message.row;
                                        current_col = robot_message.col;
                                }
                        }

                        printf("\n===========================\n");
                        print_map(robots, robotsN);
                }

                //loading phase
                for(int j = 0; j < robotsN; j++){
                        if(j == i){
                                struct message load_msg = {
                                        row: current_row,
                                        col: current_col,
                                        current_payload: cnt_purposes[i].loading_area,
                                        required_payload: cnt_purposes[i].loading_area,
                                        cmd: 0
                                };
                                send_message_to_control_node(i, load_msg);
                        }else{
                                struct message standby_msg = {
                                        row: -1,
                                        col: -1,
                                        current_payload: -1,
                                        required_payload: -1,
                                        cmd: 0
                                };
                                send_message_to_control_node(j, standby_msg);
                        }                        
                }

                unblock_threads();
                increase_step();

                //stand by message
                for(int j = 0; j < robotsN; j++){
                        robot_message = receive_message_from_robot(j);
                        while(robot_message.cmd < 0){
                                robot_message = receive_message_from_robot(j);
                        }
                        if(robot_message.row >= 0){
                                current_row = robot_message.row;
                                current_col = robot_message.col;
                        }
                }

                printf("\n===========================\n");
                print_map(robots, robotsN);

                //move to unloading area phase
                findValue(cnt_purposes[i].unloading_area, &dest_row, &dest_col);

                save_direction(' ', record_move);

                while (current_row != dest_row || current_col != dest_col) {
                        if (current_row > dest_row && is_valid_move(current_row, current_col, 'U', loading_area, cnt_purposes[i].unloading_area, 1) && check_cycle('U', record_move)) {
                                save_direction('U', record_move);
                                current_row--;
                        } else if (current_row < dest_row && is_valid_move(current_row, current_col, 'D', loading_area, cnt_purposes[i].unloading_area, 1) && check_cycle('D', record_move)) {
                                save_direction('D', record_move);
                                current_row++;
                        } else if (current_col > dest_col && is_valid_move(current_row, current_col, 'L', loading_area, cnt_purposes[i].unloading_area, 1) && check_cycle('L', record_move)) {
                                save_direction('L', record_move);
                                current_col--;
                        } else if (current_col < dest_col && is_valid_move(current_row, current_col, 'R', loading_area,cnt_purposes[i].unloading_area, 1) && check_cycle('R', record_move)) {
                                save_direction('R', record_move);
                                current_col++; 
                        } else {
                                if (is_valid_move(current_row, current_col, 'U', loading_area, cnt_purposes[i].unloading_area, 1) && check_cycle('D', record_move)) {
                                        save_direction('U', record_move);
                                        current_row--;
                                } else if (is_valid_move(current_row, current_col, 'D', loading_area, cnt_purposes[i].unloading_area, 1) && check_cycle('D', record_move)) {
                                        save_direction('D', record_move);
                                        current_row++;
                                } else if (is_valid_move(current_row, current_col, 'L', loading_area, cnt_purposes[i].unloading_area, 1) && check_cycle('D', record_move)) {
                                        save_direction('L', record_move);
                                        current_col--;
                                } else if (is_valid_move(current_row, current_col, 'R', loading_area,cnt_purposes[i].unloading_area, 1) && check_cycle('D', record_move)) {
                                        save_direction('R', record_move);
                                        current_col++; 
                                } else {
                                        printf("Error: Cannot move to destination.\n");
                                        break;
                                }
                        }

                        for(int j = 0; j < robotsN; j++){
                                if(j == i){
                                        struct message move_msg = {
                                                row: current_row,
                                                col: current_col,
                                                current_payload: cnt_purposes[i].loading_area,
                                                required_payload: cnt_purposes[i].loading_area,
                                                cmd: 0
                                        };
                                        send_message_to_control_node(i, move_msg);
                                }else{
                                        struct message standby_msg = {
                                                row: -1,
                                                col: -1,
                                                current_payload: -1,
                                                required_payload: -1,
                                                cmd: 0
                                        };
                                        send_message_to_control_node(j, standby_msg);
                                }                        
                        }

                        unblock_threads();
                        increase_step();

                        //stand by message
                        for(int j = 0; j < robotsN; j++){
                                robot_message = receive_message_from_robot(j);
                                while(robot_message.cmd < 0){
                                        robot_message = receive_message_from_robot(j);
                                }
                                if(robot_message.row >= 0){
                                        current_row = robot_message.row;
                                        current_col = robot_message.col;
                                }
                        }

                        printf("\n===========================\n");
                        print_map(robots, robotsN);
                }
        }

        printf("\nautomated warehouse operational complete!\n");
}

// test code for robot thread
void control_thread(void* aux){
        struct cnt_purpose info = *((struct cnt_purpose *)aux);
        struct message cnt_message;
        int message_index = info.robot->name[1] - '0' - 1;

        //first block
        block_thread();

        while(1){
                //stand by massage
                cnt_message = receive_message_from_control_node(message_index);
                while(cnt_message.cmd < 0){
                        cnt_message = receive_message_from_control_node(message_index);
                }

                //moving or loading order
                if(cnt_message.row >= 0){
                        setRobot(info.robot, info.robot->name, cnt_message.row, cnt_message.col, cnt_message.required_payload, cnt_message.current_payload);

                        struct message message = {
                                row: cnt_message.row,
                                col: cnt_message.col,
                                current_payload: cnt_message.current_payload,
                                required_payload: cnt_message.required_payload,
                                cmd: 1
                        };

                        send_message_to_robot(message_index, message);
                }else{ 
                        //stand by order

                        struct message message = {
                                row: -1,
                                col: -1,
                                current_payload: -1,
                                required_payload: -1,
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
                int loading_area = atoi(token);
                char* robotName;
                robotName = malloc(sizeof(char) + sizeof(int));

                snprintf(robotName, 4, "R%d", i+1);

                char unloading_area = token[strlen(token)-1];

                setRobot(&robots[i], robotName, 5, 5, 0, loading_area);

                cnt_purposes[i].robot = &robots[i];
                cnt_purposes[i].loading_area = loading_area;
                cnt_purposes[i].unloading_area = unloading_area;
                
                threads[i + 1] = thread_create(robotName, 0, control_thread, &cnt_purposes[i]);

                i++;
                token = strtok_r(NULL, ":", &robotsSet);
        }

        struct purpose purpose = {
                cnt_purposes: cnt_purposes,
                robotsN: robotsN,
        };

        threads[0] = thread_create("CNT", 0, &control_cnt, &purpose);

        free(str);  
}
