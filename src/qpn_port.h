/*****************************************************************************
* Microblaze Qnano port qpn_port.h
* Last modified July 15, 2010 at 1:44 PM
*****************************************************************************/
#ifndef qpn_port_h
#define qpn_port_h

#include "stm32l476xx.h"

#define Q_PARAM_SIZE            4
#define QF_TIMEEVT_CTR_SIZE     1

/* maximum # active objects--must match EXACTLY the QF_active[] definition  */
#define QF_MAX_ACTIVE	1

                              /* interrupt locking policy for XPS compiler */
#define QF_INT_LOCK()           __disable_irq();
#define QF_INT_UNLOCK()         __enable_irq();
#define Q_ROM_PTR(rom_var_)     rom_var_
#define Q_ROM_BYTE(rom_var_)    rom_var_
                                  
#define QF_ISR_NEST
#define Q_ROM
#define Q_ROM_VAR
#define Q_REENTRANT
#define Q_ISR_NEST

#include <stdint.h>       /* IAR provides C99-standard exact-width integers */
#include "qepn.h"         /* QEP-nano platform-independent public interface */
#include "qfn.h"           /* QF-nano platform-independent public interface */


#endif                                                        /* qpn_port_h */
