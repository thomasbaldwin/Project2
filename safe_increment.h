#ifndef SAFE_INCREMENT_H
#define SAFE_INCREMENT_H

void enter_region(int, int, int);
void increment(char *);
void leave_region();
int get_other_flag(int);
int get_turn(int, int, int);

#endif
