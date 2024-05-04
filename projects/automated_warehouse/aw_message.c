#include "projects/automated_warehouse/aw_message.h"

// 메시지 함 배열을 동적으로 할당하는 함수
void allocate_message_boxes(int num_robots) {
    // 물류 로봇들이 중앙 관제 노드로부터 받는 메시지 함 배열 할당
    boxes_from_central_control_node = (struct message_box *)malloc(num_robots * sizeof(struct message_box));
    if (boxes_from_central_control_node == 0) {
        exit(1);
    }
    
    // 중앙 관제 노드가 각 물류 로봇들에게 받는 메시지 함 배열 할당
    boxes_from_robots = (struct message_box *)malloc(num_robots * sizeof(struct message_box));
    if (boxes_from_robots == 0) {
        free(boxes_from_central_control_node);
        exit(1);
    }
}

// 할당된 메시지 함 배열을 해제하는 함수
void deallocate_message_boxes() {
    free(boxes_from_central_control_node);
    free(boxes_from_robots);
}


void send_message_to_control_node(int tid, struct message msg) {
    boxes_from_central_control_node[tid].msg = msg;
    boxes_from_central_control_node[tid].dirtyBit = 1;
}

void receive_message_from_control_node(int tid) {
    if (boxes_from_central_control_node[tid].dirtyBit) {
        struct message received_msg = boxes_from_central_control_node[tid].msg;
        boxes_from_central_control_node[tid].dirtyBit = 0;

        return received_msg;
    }

    return 0;
}

void send_message_to_robot(int tid, struct message msg) {
    boxes_from_robots[tid].msg = msg;
    boxes_from_robots[tid].dirtyBit = 1;
}

void receive_message_from_robot(int tid) {
    if (boxes_from_robots[tid].dirtyBit) {
        struct message received_msg = boxes_from_robots[tid].msg;
        boxes_from_robots[tid].dirtyBit = 0;

        return received_msg;
    }
    
    return 0;
}