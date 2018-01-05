/*
 * qsafe.h
 *
 *  Created on: Apr 17, 2009
 *      Author: KFodor
 */

#include "qp_port.h"
#include "code.h"
#include "qlock.h"

#ifndef QSAFE_H_
#define QSAFE_H_

class QSafe : public QActive {  // the QSafe container active object

private:

    QLock lock; // An instance of our lock FSM
    Code challenge; // A code entered by the user to secure contents
    Code response; // A code entered by someone accessing the contents

public:
    QSafe():QActive((QStateHandler)&QSafe::initial){}

private:

    // HSM methods
    static QState initial    (QSafe *me, QEvent const *e);
    static QState final      (QSafe *me, QEvent const *e);
    static QState running      (QSafe *me, QEvent const *e);
        static QState closed     (QSafe *me, QEvent const *e);
            static QState program    (QSafe *me, QEvent const *e);
            static QState locked    (QSafe *me, QEvent const *e);
        static QState opened    (QSafe *me, QEvent const *e);
};

#endif /* QSAFE_H_ */
