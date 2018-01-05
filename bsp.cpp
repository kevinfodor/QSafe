/*
 * bsp.cpp
 *
 *  Created on: Apr 17, 2009
 *      Author: KFodor
 */

#include "qp_port.h"
#include "bsp.h"
#include "events.h"

#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

Q_DEFINE_THIS_FILE

// This creates a constant string out of the macro argument provided
#define MACRO_TO_STRING(x)  # x

// local variables -----------------------------------------------------------
static uint8_t l_running;

// Global-scope objects (opaque pointer to the QSafe container) ---------
extern QActive * const APP_safe;     // QSafe active object

//............................................................................
static void onConsoleInput(char key)
{
    switch (key)
    {
        case 'o': // Door open event?
        {
            APP_safe->postFIFO(Q_NEW(QEvent, OPENED_SIG));
            break;
        }
        case 'c': // Door closed event?
        {
            APP_safe->postFIFO(Q_NEW(QEvent, CLOSED_SIG));
            break;
        }
        case '0': // Key event?
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'x': // CLEAR
        case 'e': // ENTER
        {
            KeyEvt *e = Q_NEW(KeyEvt, KEY_SIG);
            e->key = key;
            APP_safe->postFIFO(e);
            break;
        }
        case 'q': // Quit pressed?
        {
            QF::publish(Q_NEW(QEvent, TERMINATE_SIG));
            break;
        }
    }
}

//............................................................................
static const char *signalString(QSignal s)
{
    const char *string;

    switch(s)
    {
        case 0: // Q_EMPTY_SIG
            string = "Q_EMPTY_SIG";
        break;

        case Q_ENTRY_SIG:
            string = MACRO_TO_STRING(Q_ENTRY_SIG);
        break;

        case Q_EXIT_SIG:
            string = MACRO_TO_STRING(Q_EXIT_SIG);
        break;

        case Q_INIT_SIG:
            string = MACRO_TO_STRING(Q_INIT_SIG);
        break;

        case OPENED_SIG:
            string = MACRO_TO_STRING(OPENED_SIG);
        break;

        case CLOSED_SIG:
            string = MACRO_TO_STRING(CLOSED_SIG);
        break;

        case KEY_SIG:
            string = MACRO_TO_STRING(KEY_SIG);
        break;

        case TERMINATE_SIG:
            string = MACRO_TO_STRING(TERMINATE_SIG);
        break;

        case MAX_PUB_SIG:
            string = MACRO_TO_STRING(MAX_PUB_SIG);
        break;

        case MAX_SIG:
            string = MACRO_TO_STRING(MAX_SIG);
        break;

        default:
            string = "Unknown Signal";
        break;
    }

    return string;
}

//............................................................................
static DWORD WINAPI idleThread(LPVOID par) {   // signature for CreateThread()

    (void)par;
    l_running = (uint8_t)1;

    // Fix console problem
    setvbuf(stdout, NULL, _IONBF, 0);

    while (l_running) {
//        if (_kbhit())
        {                                        // any key pressed?
            char c = getc(stdin);
            onConsoleInput(c);
        }
    }
    return 0;                                              // return success
}

//............................................................................
void BSP_init(int argc, char *argv[])
{
    HANDLE hIdle;

    hIdle = CreateThread(NULL, 1024, &idleThread, (void *)0, 0, NULL);
    Q_ASSERT(hIdle != (HANDLE)0);                    // thread must be created
    SetThreadPriority(hIdle, THREAD_PRIORITY_IDLE);

    QF_setTickRate(BSP_TICKS_PER_SEC); // set the desired tick rate

    printf("Quantum Safe (QSafe) Problem example"
           "\nQEP %s\nQF  %s\n"
           "Press 'o' to open the door\n"
           "Press 'c' to close the door\n"
           "Press '0'..'9' to enter a code\n"
           "Press 'x' to clear entry\n"
           "Press 'e' for enter\n"
           "Press 'q' to quit...\n"
           "\n",
           QEP::getVersion(),
           QF::getVersion());
}

//............................................................................
void BSP_display(char const *msg)
{
    printf("%s", msg);
    return;
}

//............................................................................
void BSP_display_signal(char const *msg, QEvent const &e)
{
    if(&e != 0)
    {
        if(e.sig != 0)
        {
            printf("%s[%s]\n", msg, signalString(e.sig));
        }
    }
    else
    {
        printf("%s\n", msg);
    }
    return;
}

//............................................................................
void BSP_lock(void)
{
    printf("LOCKED\n");
    return;
}

//............................................................................
void BSP_unlock(void)
{
    printf("UNLOCKED\n");
    return;
}

//............................................................................
bool BSP_door(void)
{
    // default is door closed
    return true;
}

//----------------------------------------------------------------------------
//............................................................................
void QF::onStartup(void)
{
    BSP_display("QF::onStartup\n");
    return;
}

//............................................................................
void QF::onIdle(void)
{
    QF_INT_UNLOCK(dummy); // callback is invoked with interrupts locked
                          // therefore we must enable them now.

    BSP_display("QF::onIdle\n");

    QF_WAIT_FOR_EVENTS(); // yield the CPU until new event(s) arrive

    return;
}

//............................................................................
void QF::onCleanup(void)
{
    BSP_display("QF::onCleanup\n");
    l_running = 0;
    exit(0);
}

//............................................................................
void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    fprintf(stderr, "Assertion failed in %s, line %d", file, line);
    exit(0);
}

