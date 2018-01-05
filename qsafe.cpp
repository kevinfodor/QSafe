/*
 * qsafe.cpp
 *
 *  Created on: Apr 22, 2009
 *      Author: KFodor
 */

#include "qp_port.h"
#include "bsp.h"
#include "events.h"
#include "code.h"
#include "qlock.h" // QF based lock FSM
#include "qsafe.h"

QState QSafe::initial(QSafe *me, QEvent const *e)
{
    BSP_display_signal("QSafe::initial", *e);

    // Subscribe to signals...
    me->subscribe(TERMINATE_SIG);

    // initial transition for lock-FSM
    me->lock.init();

    // handle initial transition
    return Q_TRAN(&QSafe::running);
}

QState QSafe::final(QSafe *me, QEvent const *e)
{
    BSP_display_signal("QSafe::final", *e);

    switch (e->sig)
    {
        case Q_ENTRY_SIG:
        {
            BSP_display("\nBye!Bye!\n");
            QF::stop(); // stop QF and cleanup
            return Q_HANDLED();
        }
    }

    // Return top level pseudostate
    return Q_SUPER(&QHsm::top);
}

QState QSafe::running(QSafe *me, QEvent const *e)
{
    BSP_display_signal("QSafe::running", *e);

    switch (e->sig)
    {
        case Q_INIT_SIG:
        {
            // Initial transition in qsafe composite state
            // door is initially closed or opened.
            if(BSP_door() == true) // Door is initially closed
            {
                return Q_TRAN(&QSafe::closed);
            }
            else
            {
                // Door is initially open
                return Q_TRAN(&QSafe::opened);
            }
        }

        // Respond to any subscribed signals...

        case TERMINATE_SIG:
            return Q_TRAN(&QSafe::final);
    }

    // Return top level pseudostate
    return Q_SUPER(&QHsm::top);
}

/*
 * 'running' composite states
 */

QState QSafe::closed(QSafe *me, QEvent const *e)
{
    BSP_display_signal("QSafe::closed", *e);

    switch(e->sig)
    {
        case Q_INIT_SIG:
            return Q_TRAN(&QSafe::program);

        case OPENED_SIG:
            return Q_TRAN(&QSafe::opened);
    }

    // Allow super-state to handle signal
    return Q_SUPER(&QSafe::running);
}

QState QSafe::opened(QSafe *me, QEvent const *e)
{
    BSP_display_signal("QSafe::opened", *e);

    switch (e->sig)
    {
        case CLOSED_SIG:
            return Q_TRAN(&QSafe::closed);
    }

    // Allow super-state to handle signal
    return Q_SUPER(&QSafe::running);
}

/*
 * 'closed' composite states
 */

QState QSafe::program (QSafe *me, QEvent const *e)
{
    BSP_display_signal("QSafe::program", *e);

    switch (e->sig)
    {
        case Q_ENTRY_SIG:
        {
            // Clear challenge input(reset)
            me->challenge.clear();
        }

        case KEY_SIG:
        {
            if(((KeyEvt*)e)->key == 'e') // ENTER
            {
                // Check if entry is good (complete)
                if(me->challenge.good() == true)
                {
                    return Q_TRAN(&QSafe::locked);
                }
                return Q_HANDLED();
            }
            else if(((KeyEvt*)e)->key == 'x') // CLEAR
            {
                return Q_TRAN(&QSafe::program);
            }
            // NUMERIC
            else if((((KeyEvt*)e)->key >= '0') && (((KeyEvt*)e)->key <= '9'))
            {
                // Handle numeric keys
                me->challenge += ((KeyEvt*)e)->key;
                return Q_HANDLED();
            }
        }
    }

    // Allow super-state to handle signal
    return Q_SUPER(&QSafe::closed);
}

QState QSafe::locked (QSafe *me, QEvent const *e)
{
    BSP_display_signal("QSafe::locked", *e);

    switch (e->sig)
    {
        case Q_ENTRY_SIG:
        {
            me->lock.lock(); // Lock safe

            // Clear response input(reset)
            me->response.clear();
            return Q_HANDLED();
        }

        case Q_EXIT_SIG:
        {
            me->lock.unlock(); // Unlock safe
            return Q_HANDLED();
        }

        case KEY_SIG:
        {
            if(((KeyEvt*)e)->key == 'e') // ENTER
            {
                // Does response match the challenge
                if(me->challenge == me->response)
                {
                    // Transition to super-state
                    return Q_TRAN(&QSafe::closed);
                }
                return Q_HANDLED();
            }
            else if(((KeyEvt*)e)->key == 'x') // CLEAR
            {
                // Clear response input(reset)
                me->response.clear();
                return Q_HANDLED();
            }
            // NUMERIC
            else if((((KeyEvt*)e)->key >= '0') && (((KeyEvt*)e)->key <= '9'))
            {
                // Handle numeric keys
                me->response += ((KeyEvt*)e)->key;
                return Q_HANDLED();
            }
        }

        // Note: I am not handling this signal, but if it occurs
        // in this state that means someone forced the door open
        // with a pry-bar and the safe is probably broke!
        case OPENED_SIG:
        {
            BSP_display("ALERT! Safe cracked open.\n");
        }
    }

    // Allow super-state to handle signal
    return Q_SUPER(&QSafe::closed);
}
