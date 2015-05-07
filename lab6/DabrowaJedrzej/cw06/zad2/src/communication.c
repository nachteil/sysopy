#include "communication.h"
#include "string.h"

mqd_t create_queue(char *queue_name) {

    mqd_t qid;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 8;
    attr.mq_msgsize = MSG_SIZE_PARAM;
    attr.mq_curmsgs = 0;

    return (qid = mq_open(queue_name, O_CREAT | O_EXCL | O_RDWR, 0660, &attr));
}

mqd_t open_queue(char *name) {
    mqd_t qid;
    struct mq_attr attr;

    attr.mq_maxmsg = 25;
    attr.mq_msgsize = MSG_SIZE_PARAM;

    return (qid = mq_open(name, O_RDWR, 0660, &attr));
}

int send_message(mqd_t qid, my_msg_buf *msgbuf) {
    int result;
    if ((result = mq_send(qid, (const char *) msgbuf, MSG_SIZE_PARAM, 0)) == -1) {
        return (-1);
    }
    return (result);
}

int receive_message_block(mqd_t queue_id, my_msg_buf *msgbuf) {
    return (int) mq_receive(queue_id, (char *) msgbuf, MSG_SIZE_PARAM, NULL);
}

void send_init_msg(mqd_t server_queue_id, char *client_id) {

    my_msg_buf buf;
    buf.msg_type = MSG_TYPE_INIT;
    strcpy((char *) &(buf.client_id), client_id);

    if (send_message(server_queue_id, &buf) == -1) {
        error("Message send failed");
    }
}