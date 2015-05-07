#ifndef SYSOPY_CLIENT_H
#define SYSOPY_CLIENT_H

void cleanup_fn(void);
int receiver_loop(void *);
int sender_loop(char *, int);

#endif //SYSOPY_CLIENT_H
