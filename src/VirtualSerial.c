/*
  Copyright 2015 Mike McMahon

             LUFA Library
     Copyright (C) Dean Camera, 2014.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2014  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the VirtualSerial demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "VirtualSerial.h"

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
  {
    .Config =
      {
        .ControlInterfaceNumber   = INTERFACE_ID_CDC_CCI,
        .DataINEndpoint           =
          {
            .Address          = CDC_TX_EPADDR,
            .Size             = CDC_TXRX_EPSIZE,
            .Banks            = 1,
          },
        .DataOUTEndpoint =
          {
            .Address          = CDC_RX_EPADDR,
            .Size             = CDC_TXRX_EPSIZE,
            .Banks            = 1,
          },
        .NotificationEndpoint =
          {
            .Address          = CDC_NOTIFICATION_EPADDR,
            .Size             = CDC_NOTIFICATION_EPSIZE,
            .Banks            = 1,
          },
      },
  };

#define CHAR_PORT PORTB
#define CHAR_PIN PINB

#ifndef CHAR_MASK
#define CHAR_MASK 0x7F
#endif

#ifndef PARITY_CHECK
#define PARITY_CHECK PARITY_NONE
#endif
#define PARITY_NONE -1
#define PARITY_EVEN 0
#define PARITY_ODD 1

#ifndef CHAR_PULLUP_MASK
#if PARITY_CHECK == PARITY_NONE
#define CHAR_PULLUP_MASK CHAR_MASK
#else
#define CHAR_PULLUP_MASK 0xFF
#endif
#endif

#define CONTROL_PORT PORTD
#define CONTROL_PIN PIND
#define CONTROL_DDR DDRD
#define CONTROL_STROBE (1 << 0)
#define CONTROL_STROBE_INTERRUPT (1 << INT0)
#ifndef CONTROL_STROBE_TRIGGER
#define CONTROL_STROBE_TRIGGER TRIGGER_FALLING
#endif
#define TRIGGER_FALLING (1 << ISC01)
#define TRIGGER_RISING ((1 << ISC01) | (1 << ISC00))
#ifndef CONTROL_NSHIFTS
#define CONTROL_NSHIFTS 0
#endif
#if CONTROL_NSHIFTS > 0
#define CONTROL_SHIFTS_SHIFT 1
#define CONTROL_SHIFTS_MASK (((1<<CONTROL_SHIFTS_SHIFT) - 1) << CONTROL_SHIFTS_SHIFT)
#endif

#ifdef READY_STATE
#define READY_PORT PORTC
#define READY_DDR DDRC
#define READY_MASK (1 << 7)
#define READY_HIGH true
#define READY_LOW false
#if READY_STATE == READY_HIGH
#define READY_ON READY_PORT |= READY_MASK
#else
#define READY_ON READY_PORT &= ~READY_MASK
#endif
#endif

/*** Character Queue ***/

typedef struct {
  uint8_t charCode;
#if CONTROL_NSHIFTS > 0
  uint8_t shifts;
#endif
} queue_entry_t;
#define QUEUE_SIZE 16
static queue_entry_t CharQueue[QUEUE_SIZE];
static uint8_t CharQueueIn, CharQueueOut;

static inline void QueueClear(void)
{
  CharQueueIn = CharQueueOut = 0;
}

static inline bool QueueIsEmpty(void)
{
  return (CharQueueIn == CharQueueOut);
}

static inline bool QueueIsFull(void)
{
  // One entry wasted to be able to check this easily.
  return (((CharQueueIn + 1) % QUEUE_SIZE) == CharQueueOut);
}

static inline queue_entry_t QueueRemove(void)
{
  queue_entry_t entry = CharQueue[CharQueueOut];
  CharQueueOut = (CharQueueOut + 1) % QUEUE_SIZE;
  return entry;
}

static inline void QueueAdd(queue_entry_t entry)
{
  CharQueue[CharQueueIn] = entry;
  CharQueueIn = (CharQueueIn + 1) % QUEUE_SIZE;
}

/*** Keyboard Interface ***/

static void Parallel_Kbd_Init(void)
{
  QueueClear();

  // Enable pullups.
  CHAR_PORT |= CHAR_PULLUP_MASK;
  CONTROL_PORT |= CONTROL_STROBE
#if CONTROL_NSHIFTS > 0
    | CONTROL_SHIFTS_MASK
#endif
    ;

  // Interrupt 0 on trigger edge of STROBE
  EIMSK |= CONTROL_STROBE_INTERRUPT;
  EICRA |= CONTROL_STROBE_TRIGGER;

#ifdef READY_STATE
  READY_DDR |= READY_MASK;
  READY_ON;
#endif
}

static void Parallel_Kbd_Task(void)
{
  queue_entry_t entry;
  
  // NOTE: If simulating a full keyboard and not a 7-bit serial port, it
  // would be possible to use the shift and ctrl bits to reconstruct more
  // of the key state.

  while (!QueueIsEmpty()) {
    entry = QueueRemove();

    uint8_t charCode = entry.charCode;
#if PARITY_CHECK != PARITY_NONE
    uint8_t parity = 0;
    for (uint8_t i = 0; i < 8; i++) {
      if ((charCode & (1 << i)) != 0) {
        parity ^= 1;
      }
    }
    if (parity != PARITY_CHECK) {
      continue;
    }
#endif
#ifdef CHAR_INVERT
    charCode = ~charCode;
#endif
    charCode &= CHAR_MASK;
    CDC_Device_SendByte(&VirtualSerial_CDC_Interface, charCode);
  }
}

/*** Interrupt Handler ***/

ISR(INT0_vect)
{
  queue_entry_t entry;

  entry.charCode = CHAR_PIN;
#if CONTROL_NSHIFTS > 0
  entry.shifts = (~CONTROL_PIN & CONTROL_SHIFTS_MASK) >> CONTROL_SHIFTS_SHIFT;
#endif

  if (!QueueIsFull()) {
    QueueAdd(entry);
  }
}

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
  SetupHardware();

  LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
  GlobalInterruptEnable();

  for (;;)
  {
    Parallel_Kbd_Task();

    /* Must throw away unused bytes from the host, or it will lock up while waiting for the device */
    CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);

    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    USB_USBTask();
  }
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
  /* Disable watchdog if enabled by bootloader/fuses */
  MCUSR &= ~(1 << WDRF);
  wdt_disable();

  /* Disable clock division */
  clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
  /* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
  XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
  XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

  /* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
  XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
  XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

  PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

  /* Hardware Initialization */
  Parallel_Kbd_Init();
  LEDs_Init();
  USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
  LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
  LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
  bool ConfigSuccess = true;

  ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);

  LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
  CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}
