/*
 * bsp_.h
 *
 *  Created on: Apr 17, 2009
 *      Author: KFodor
 */

#ifndef BSP_H_
#define BSP_H_

#define BSP_TICKS_PER_SEC   100

void BSP_init(int argc, char *argv[]);
void BSP_display(char const *msg);
void BSP_display_signal(char const *msg, QEvent const &e);
void BSP_lock(void);
void BSP_unlock(void);
bool BSP_door(void);

#endif /* BSP_H_ */
