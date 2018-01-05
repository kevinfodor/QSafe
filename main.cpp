/*
 * main.cpp
 *
 *  Created on: Apr 17, 2009
 *      Author: KFodor
 */

#include "qp_port.h"
#include "bsp.h"

#include "events.h"
#include "qsafe.h"

// Local-scope objects -------------------------------------------------------
static QSubscrList   subscrStore[MAX_PUB_SIG];
static QEvent const *mySafeQueue[4]; // queue storage for QSafe
static QSafe mySafe; // the QSafe active object

static union Events {

    void *min_size;
    KeyEvt te;
    // other event types to go into this pool

} smlPoolStore[4]; // storage for the event pool

// Global-scope objects (opaque pointer to the QSafe container) ---------
extern QActive * const APP_safe = &mySafe; // QSafe active object

//............................................................................
int main(int argc, char *argv[]) {

    // initialize the BSP
    BSP_init(argc, argv);

    // initialize the Quantum Framework
    QF::init();

    // initialize publish-subscribe
    QF::psInit(subscrStore, Q_DIM(subscrStore));

    // initialize event pools
    QF::poolInit(smlPoolStore, sizeof(smlPoolStore), sizeof(smlPoolStore[0]));

    // start the active objects
    mySafe.start(1, mySafeQueue, Q_DIM(mySafeQueue),
        (void *)0, 1024, (QEvent *)0);

    // run the QF application
    QF::run();

    return 0;
}
