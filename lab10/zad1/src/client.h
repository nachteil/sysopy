#ifndef SYSOPY_CLIENT_H
#define SYSOPY_CLIENT_H

#define EXPECTED_NUM_PARAMS 4

int create_internet_socket(char *argv[]);
int create_unix_socket(char *, char *);

void *stdin_read_fun(void *);
void lock_data_mutex();
void unlock_data_mutex();
void init_msg_buff();
void enlarge_msg_buff();

void sig_handle(int);
void init_signals();
void sender_receiver_fun(int);

#endif //SYSOPY_CLIENT_H
