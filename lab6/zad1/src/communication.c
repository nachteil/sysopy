#include "communication.h"
#include "string.h"

char *get_client_id_file(char *client_id) {

    char *unique_id_file = (char *) malloc(strlen(CLIENT_UNIQUE_ID_PREFIX) + 21);
    if (unique_id_file == NULL) {
        mem_error();
    }
    for (int i = 0; i < 21; ++i) unique_id_file[i] = 0;
    strcpy(unique_id_file, CLIENT_UNIQUE_ID_PREFIX);
    strcpy(unique_id_file + strlen(CLIENT_UNIQUE_ID_PREFIX), client_id);

    return unique_id_file;
}


int create_queue(key_t key) {

    int qid;

    if ((qid = msgget(key, IPC_CREAT | IPC_EXCL | 0660)) == -1) {
        return (-1);
    }
    return (qid);
}

int open_queue(key_t key) {
    int qid;

    if ((qid = msgget(key, IPC_CREAT | 0660)) == -1) {
        return (-1);
    }

    return (qid);
}

int send_message(int qid, my_msg_buf *msgbuf) {
    int result;
    if ((result = msgsnd(qid, msgbuf, MSG_SIZE_PARAM, 0)) == -1) {
        return (-1);
    }
    return (result);
}

void receive_message_block(int queue_id, my_msg_buf *msgbuf) {

    msgbuf->msg_type = 0;
    msgrcv(queue_id, msgbuf, MSG_SIZE_PARAM, S2C, 0);
}

int receive_message_nonblock(int queue_id, my_msg_buf *msgbuf) {

    msgbuf->msg_type = 0;
    return msgrcv(queue_id, msgbuf, MSG_SIZE_PARAM, C2S, IPC_NOWAIT);
}

void send_init_msg(int server_queue_id, char *client_id) {

    my_msg_buf buf;
    buf.msg_type = INIT_MSG_TYPE;
    strcpy((char *) &(buf.client_id), client_id);

    if (send_message(server_queue_id, &buf) == -1) {
        error("Message send failed");
    }
}