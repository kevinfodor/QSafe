/*
 * QLock.cpp
 *
 *  Created on: Apr 20, 2009
 *      Author: KFodor
 */

#include "qp_port.h"
#include "bsp.h"
#include "qlock.h"

// This creates a constant string out of the macro argument provided
#define MACRO_TO_STRING(x)  # x

enum Signals {

    // dispatched signals (internal)
    LOCK_SIG = Q_USER_SIG,
    UNLOCK_SIG,

    MAX_SIG // the last signal
};

void display_signal(char const *msg, QEvent const &e)
{
    if(&e == 0)
    {
        return;
    }

    if(e.sig < Q_USER_SIG)
    {
        BSP_display_signal(msg, e);
    }
    else
    {
        char *string;

        switch(e.sig)
        {
            case LOCK_SIG:
                string = MACRO_TO_STRING(LOCK_SIG);
            break;

            case UNLOCK_SIG:
                string = MACRO_TO_STRING(UNLOCK_SIG);
            break;

            default:
                string = "Unknown signal";
            break;
        }

        BSP_display(msg);
        BSP_display("[");
        BSP_display(string);
        BSP_display("]\n");
    }

    return;
}

/*
 * Command interface (lock/unlock)
 */

void QLock::lock(void)
{
    // Lock!
    QEvent e;
    e.sig = LOCK_SIG;
    dispatch(&e);
}

void QLock::unlock(void)
{
    // Unlock!
    QEvent e;
    e.sig = UNLOCK_SIG;
    dispatch(&e);
}

/*
 * FSM implementation
 */

QState QLock::initial(QLock *me, QEvent const *e)
{
    display_signal("QLock::initial", *e);

    // Initial transition to the 'unlocked' state
	return Q_TRAN(&QLock::unlocked);
};

QState QLock::locked(QLock *me, QEvent const *e)
{
    display_signal("QLock::locked", *e);

    switch(e->sig)
    {
        case Q_ENTRY_SIG:
        {
            BSP_lock(); // Set lock
            return Q_HANDLED();
        }

        case UNLOCK_SIG:
            return Q_TRAN(unlocked);

    }

    return Q_IGNORED();
};

QState QLock::unlocked(QLock *me, QEvent const *e)
{
    display_signal("QLock::unlocked", *e);

    switch(e->sig)
    {
        case Q_ENTRY_SIG:
        {
            BSP_unlock(); // Clear lock
            return Q_HANDLED();
        }

        case LOCK_SIG:
            return Q_TRAN(locked);
    }

    return Q_IGNORED();
};

