#ifndef SAFE_INCREMENT_H
#define SAFE_INCREMENT_H

void increment(char*);
void enter_region(void);
void leave_region(void);
int get_set_flag(pid_t);
int get_turn(pid_t);

#endif
