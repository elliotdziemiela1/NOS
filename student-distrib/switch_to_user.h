#ifndef _SWITCH_TO_USER_H
#define _SWITCH_TO_USER_H

#ifndef ASM

extern void user_switch();
extern uint8_t user_eip[4];

#endif /* ASM */
#endif /* _SWITCH_TO_USER_H */
