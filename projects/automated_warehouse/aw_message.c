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


// 중앙 관제 노드로 메시지를 보내는 함수
void send_message_to_control_node(int robot_id, struct message msg) {
    // 메시지 박스에 메시지 저장
    boxes_from_central_control_node[robot_id].msg = msg;
    // dirtyBit 설정
    boxes_from_central_control_node[robot_id].dirtyBit = 1;
}

// 중앙 관제 노드로부터 메시지를 받는 함수
void receive_message_from_control_node(int robot_id) {
    // 메시지 박스가 dirty 상태인지 확인
    if (boxes_from_central_control_node[robot_id].dirtyBit) {
        // 메시지를 받아옴
        struct message received_msg = boxes_from_central_control_node[robot_id].msg;
        // dirtyBit 초기화
        boxes_from_central_control_node[robot_id].dirtyBit = 0;

        // 받은 메시지를 처리한다.
        // 예를 들어, 메시지를 어딘가에 저장하거나 처리하는 작업을 수행한다.
        // 여기서는 간단히 출력만 하도록 하겠습니다.
        printf("Message received from central control node by robot %d: Row=%d, Col=%d, Current Payload=%d, Required Payload=%d, Command=%d\n",
               robot_id, received_msg.row, received_msg.col, received_msg.current_payload, received_msg.required_payload, received_msg.cmd);
    }
}