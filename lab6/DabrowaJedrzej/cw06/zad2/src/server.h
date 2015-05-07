#ifndef SYSOPY_SERVER_H
#define SYSOPY_SERVER_H

#define MAX_CLIENTS 15
#define EMPTY_CLIENT_SLOT -1

void cleanup(void);
void enter_communicator_loop();
int enter_command_loop(void *);

#endif //SYSOPY_SERVER_H
