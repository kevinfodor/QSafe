/*
 * Lock.h
 *
 *  Created on: Apr 20, 2009
 *      Author: KFodor
 */

#include "qp_port.h"

#ifndef QLOCK_H_
#define QLOCK_H_

class QLock : public QFsm
{
public:
	QLock():QFsm((QStateHandler)&QLock::initial) {}
	void unlock(void);
	void lock(void);

private:
	// FSM states
	static QState initial(QLock *me, QEvent const *e);
	static QState locked(QLock *me, QEvent const *e);
	static QState unlocked(QLock *me, QEvent const *e);
};

#endif /* QLOCK_H_ */
