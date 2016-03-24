#ifndef SAFE_INCREMENT_H
#define SAFE_INCREMENT_H

void increment(char*);
int enter_region(void);
void leave_region(int);
int get_set_flag(pid_t);
int get_turn(pid_t);

#endif
