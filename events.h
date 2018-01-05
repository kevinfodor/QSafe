/*
 * events.h
 *
 *  Created on: Apr 22, 2009
 *      Author: KFodor
 */

#ifndef EVENTS_H_
#define EVENTS_H_

enum QSafeSignals {

    // publish/subscribe signals
    TERMINATE_SIG = Q_USER_SIG, // published by BSP to terminate the application
    MAX_PUB_SIG, // the last published signal

    // posted signals
    OPENED_SIG,
    CLOSED_SIG,
    KEY_SIG,

    MAX_SIG // the last signal

};

// KEY - Event
struct KeyEvt : public QEvent {
    char key;
};

#endif /* EVENTS_H_ */
