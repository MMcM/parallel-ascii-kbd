/*
  Copyright 2015 Mike McMahon

             LUFA Library
     Copyright (C) Dean Camera, 2014.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

extern USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface;

/*** Parallel input on B0-B7 */

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

/*** Strobe on D0 ***/

#define CONTROL_PORT PORTD
#define CONTROL_DDR DDRD
#define CONTROL_STROBE (1 << 0)
#define CONTROL_STROBE_INTERRUPT (1 << INT0)
#ifndef CONTROL_STROBE_TRIGGER
#define CONTROL_STROBE_TRIGGER TRIGGER_FALLING
#endif
#define TRIGGER_FALLING (1 << ISC01)
#define TRIGGER_RISING ((1 << ISC01) | (1 << ISC00))

/*** Direct switches on D1-D7 (ignoring LED), F0-F7 (if needed) ***/

#ifndef DIRECT_KEYS
#define DIRECT_KEYS 0
#endif
#if DIRECT_KEYS > 0

#define DIRECT_PORT PORTD
#define DIRECT_PIN PIND
#define DIRECT_DDR DDRD

#if (BOARD == BOARD_TEENSY) || (BOARD == BOARD_TEENSY2)
#define DIRECT_PORT_LEDS LEDS_LED1
#elif BOARD == BOARD_LEONARDO
#define DIRECT_PORT_LEDS LEDS_LED1  
#else
#define DIRECT_PORT_LEDS 0
#endif

#ifndef DIRECT_PORT_UNUSED
#define DIRECT_PORT_UNUSED DIRECT_PORT_LEDS
#endif

#define DIRECT_PORT_SHIFT 1

#if DIRECT_KEYS < 8

#define DIRECT_PORT_MASK ((((1<<DIRECT_KEYS) - 1) << DIRECT_PORT_SHIFT) & ~DIRECT_PORT_UNUSED)
typedef uint8_t direct_keys_t;

#else

#define DIRECT_PORT_MASK ((0xFF << DIRECT_PORT_SHIFT) & ~DIRECT_PORT_UNUSED)
typedef uint16_t direct_keys_t;

#define DIRECT_PORT_2 PORTF
#define DIRECT_PIN_2 PINF
#define DIRECT_DDR_2 DDRF
#define DIRECT_PORT_2_MASK ((1<<(DIRECT_KEYS-7)) - 1)

#endif

#ifndef DIRECT_INVERT_MASK
#define DIRECT_INVERT_MASK 0
#endif

#ifndef DIRECT_DEBOUNCE
#define DIRECT_DEBOUNCE 0
#endif

// Return value normalized to 1 for pressed key.
static inline direct_keys_t ReadDirectKeys(void)
{
  direct_keys_t result = (DIRECT_PIN & DIRECT_PORT_MASK & ~DIRECT_PORT_UNUSED) >> DIRECT_PORT_SHIFT;
#if DIRECT_KEYS > 7
  result |= (DIRECT_PIN_2 & DIRECT_PORT_2_MASK) << 7;
#endif
#if DIRECT_INVERT_MASK
  result ^= DIRECT_INVERT_MASK;
#endif
  return result;
}

#endif

/*** Bell / buzzer / speaker on C6 for BEL ***/

#define BELL_PORT PORTC
#define BELL_DDR DDRC
#define BELL_MASK (1 << 6)

#define BELL_MODE_NONE -1
#define BELL_MODE_LOW 0
#define BELL_MODE_HIGH 1
#define BELL_MODE_TONE 2

#ifndef BELL_MODE
#define BELL_MODE BELL_MODE_NONE
#endif

#ifndef BELL_DURATION_USEC
#define BELL_DURATION_USEC 5
#endif

#if BELL_MODE == BELL_MODE_NONE
#define BELL_OFF {}
#define BELL_ON {}
#elif BELL_MODE == BELL_MODE_LOW
#define BELL_OFF BELL_PORT |= BELL_MASK
#define BELL_ON BELL_PORT &= ~BELL_MASK
#elif BELL_MODE == BELL_MODE_HIGH
#define BELL_OFF BELL_PORT &= ~BELL_MASK
#define BELL_ON BELL_PORT |= BELL_MASK
#elif BELL_MODE == BELL_MODE_TONE
#error NIY
#endif

/*** Ready / ack / repeat signal on C7 ***/

#define READY_ACK_PORT PORTC
#define READY_ACK_DDR DDRC
#define READY_ACK_MASK (1 << 7)

#define READY_ACK_MODE_NONE 0
#define READY_ACK_MODE_DTR 2
#define READY_ACK_MODE_KEY_ACK 3

#ifndef READY_ACK_MODE
#define READY_ACK_MODE READY_ACK_MODE_NONE
#endif

#define READY_ACK_ON_LOW false
#define READY_ACK_ON_HIGH true

#ifndef READY_ACK_DURATION_USEC
#define READY_ACK_DURATION_USEC 5
#endif
#ifndef READY_ACK_DELAY_MSEC
#define READY_ACK_DELAY_MSEC 0
#endif

#ifndef READY_ACK_ON_STATE
#define READY_ACK_ON_STATE READY_ACK_ON_HIGH
#endif

#if READY_ACK_ON_STATE == READY_ACK_ON_LOW
#define READY_ACK_OFF READY_ACK_PORT |= READY_ACK_MASK
#define READY_ACK_ON READY_ACK_PORT &= ~READY_ACK_MASK
#else
#define READY_ACK_OFF READY_ACK_PORT &= ~READY_ACK_MASK
#define READY_ACK_ON READY_ACK_PORT |= READY_ACK_MASK
#endif

/*** Serial input commands ***/

#define ASCII_ENQ 0x05
#define ASCII_BEL 0x07

#ifndef ANSWERBACK
#define ANSWERBACK "Hello\r\n"
#endif

static const char answerback[] PROGMEM = ANSWERBACK;

/*** Character Queue ***/

typedef struct {
  uint8_t charCode;
#if DIRECT_KEYS > 0
  direct_keys_t directKeys;
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

/*** Actions ***/

#ifdef DEBUG_ACTIONS

static inline char HexDigit(uint8_t i) {
  return (i < 10) ? '0' + i : 'A' + (i - 10);
}

static void CharAction(uint8_t charCode)
{
  char str[] = "00 ?\r\n";
  str[0] = HexDigit(charCode >> 4);
  str[1] = HexDigit(charCode & 0x0F);
  str[3] = charCode >= ' ' && charCode <= '~' ? charCode : ' ';
  CDC_Device_SendString(&VirtualSerial_CDC_Interface, str);
}

#else

static inline void CharAction(uint8_t charCode)
{
  CDC_Device_SendByte(&VirtualSerial_CDC_Interface, charCode);
}

#endif

#if DIRECT_KEYS > 0

#ifdef DEBUG_ACTIONS

static void DirectKeyAction(uint8_t key, bool pressed)
{
  char str[] = "D-00:?\r\n";
  str[2] = HexDigit(key / 10);
  str[3] = HexDigit(key % 10);
  str[5] = pressed ? 'D' : 'U';
  CDC_Device_SendString(&VirtualSerial_CDC_Interface, str);
}

#ifdef DIRECT_ESC_PREFIX_MASK
static void CharEscPrefixAction(uint8_t charCode)
{
  char str[] = "00 ESC ?\r\n";
  str[0] = HexDigit(charCode >> 4);
  str[1] = HexDigit(charCode & 0x0F);
  str[7] = charCode;
  CDC_Device_SendString(&VirtualSerial_CDC_Interface, str);
}
#endif

#else

typedef void (*direct_action_t)(uint8_t key, bool pressed);

static void DirectBreakAction(uint8_t key, bool pressed)
{
  // There is no CDC_Device_SendBreak and the OS driver does not really do anything with this state notification.
  if (pressed) {
    LEDs_SetAllLEDs(LEDS_ALL_LEDS);
    VirtualSerial_CDC_Interface.State.ControlLineStates.DeviceToHost |= CDC_CONTROL_LINE_IN_BREAK;
  } else {
    VirtualSerial_CDC_Interface.State.ControlLineStates.DeviceToHost &= ~CDC_CONTROL_LINE_IN_BREAK;
    LEDs_SetAllLEDs(LEDS_NO_LEDS);
  }
  CDC_Device_SendControlLineStateChange(&VirtualSerial_CDC_Interface);
}

#define DIRECT_BREAK DirectBreakAction

static void DirectAnswerbackAction(uint8_t key, bool pressed)
{
  if (pressed) {
    CDC_Device_SendString_P(&VirtualSerial_CDC_Interface, answerback);
  }
}

#define DIRECT_HERE_IS DirectAnswerbackAction

#ifdef ANSWERBACK_2
static void DirectAnswerback2Action(uint8_t key, bool pressed)
{
  static const char answerback_2[] PROGMEM = ANSWERBACK_2;
  if (pressed) {
    CDC_Device_SendString_P(&VirtualSerial_CDC_Interface, answerback_2);
  }
}
#define DIRECT_ANSWERBACK_2 DirectAnswerback2Action
#endif

#ifdef ANSWERBACK_3
static void DirectAnswerback3Action(uint8_t key, bool pressed)
{
  static const char answerback_3[] PROGMEM = ANSWERBACK_3;
  if (pressed) {
    CDC_Device_SendString_P(&VirtualSerial_CDC_Interface, answerback_3);
  }
}
#define DIRECT_ANSWERBACK_3 DirectAnswerback3Action
#endif

#ifdef DIRECT_ESC_PREFIX_MASK
static void CharEscPrefixAction(uint8_t charCode)
{
  uint8_t esc[3];
  uint8_t i = 0;
  esc[i++] = '\e';
#ifdef DIRECT_ESC_PREFIX_VT100
  esc[i++] = '[';
#endif
  esc[i++] = charCode;
  CDC_Device_SendData(&VirtualSerial_CDC_Interface, esc, i);
}
#endif

static const direct_action_t direct_actions[DIRECT_KEYS+1] PROGMEM = {
  NULL,
#ifdef DIRECT_KEY_1
  [1] = DIRECT_KEY_1,
#endif
#ifdef DIRECT_KEY_2
  [2] = DIRECT_KEY_2,
#endif
#ifdef DIRECT_KEY_3
  [3] = DIRECT_KEY_3,
#endif
#ifdef DIRECT_KEY_4
  [4] = DIRECT_KEY_4,
#endif
#ifdef DIRECT_KEY_5
  [5] = DIRECT_KEY_5,
#endif
#ifdef DIRECT_KEY_6
  [6] = DIRECT_KEY_6,
#endif
#ifdef DIRECT_KEY_7
  [7] = DIRECT_KEY_7,
#endif
#ifdef DIRECT_KEY_8
  [8] = DIRECT_KEY_8,
#endif
#ifdef DIRECT_KEY_9
  [9] = DIRECT_KEY_9,
#endif
#ifdef DIRECT_KEY_10
  [10] = DIRECT_KEY_10,
#endif
#ifdef DIRECT_KEY_11
  [11] = DIRECT_KEY_11,
#endif
#ifdef DIRECT_KEY_12
  [12] = DIRECT_KEY_12,
#endif
#ifdef DIRECT_KEY_13
  [13] = DIRECT_KEY_13,
#endif
#ifdef DIRECT_KEY_14
  [14] = DIRECT_KEY_14,
#endif
#ifdef DIRECT_KEY_15
  [15] = DIRECT_KEY_15,
#endif
};

static void DirectKeyAction(uint8_t key, bool pressed)
{
  direct_action_t action = (direct_action_t)pgm_read_ptr(direct_actions + key);
  if (action != NULL) {
    (*action)(key, pressed);
  }
}

#endif
#endif

/*** Interrupt Handler ***/

ISR(INT0_vect)
{
  queue_entry_t entry;

  entry.charCode = CHAR_PIN;
#if DIRECT_KEYS > 0
  entry.directKeys = ReadDirectKeys();
#endif

  if (!QueueIsFull()) {
    QueueAdd(entry);
  }
}

#if (DIRECT_DEBOUNCE > 0) || (READY_ACK_DELAY_MSEC > 0)
#ifndef ENABLE_SOF_EVENTS
#error ENABLE_SOF_EVENTS must be turned on as well
#endif
#endif

#ifdef ENABLE_SOF_EVENTS
static uint16_t millisCounter = 0;

void EVENT_USB_Device_StartOfFrame(void)
{
  millisCounter++;
}
#endif

#if READY_ACK_MODE == READY_ACK_MODE_DTR
void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
  if (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR) {
    READY_ACK_ON;
  } else {
    READY_ACK_OFF;
  }
}
#endif

#if DIRECT_KEYS > 0
#if DIRECT_DEBOUNCE == 0
static inline bool ReadDirectKeysDebounce(direct_keys_t *ret)
{
  *ret = ReadDirectKeys();
  return true;
}
#else
static bool ReadDirectKeysDebounce(direct_keys_t *ret)
{
  static bool debounceInProgress = false;
  static direct_keys_t debounceKeys = 0;
  static uint16_t debounceStart;
  direct_keys_t current = ReadDirectKeys();
  if (debounceInProgress) {
    if (debounceKeys == current) {
      *ret = current;
      return true;
    }
  } else {
    if (debounceKeys == current &&
        millisCounter - debounceStart > DIRECT_DEBOUNCE) {
      debounceInProgress = false;
      *ret = current;
      return true;
    }
  }
  debounceKeys = current;
  debounceInProgress = true;
  debounceStart = millisCounter;
  return false;
}
#endif

static void UpdateDirectKeys(direct_keys_t directKeysNext)
{
  static direct_keys_t directKeysPrev = 0;
  if (directKeysPrev != directKeysNext) {
    direct_keys_t directKeysDiff = directKeysPrev ^ directKeysNext;
    for (uint8_t i = 0; i < DIRECT_KEYS; i++) {
      if (directKeysDiff & (1 << i)) {
        DirectKeyAction(i + 1, (directKeysNext & (1 << i)) != 0);
      }
    }
    directKeysPrev = directKeysNext;
  }
}
#endif

/*** Keyboard Interface ***/

void Parallel_Kbd_Init(void)
{
  QueueClear();

  // Enable pullups.
  CHAR_PORT |= CHAR_PULLUP_MASK;
  CONTROL_PORT |= CONTROL_STROBE;

#if DIRECT_KEYS > 0
  DIRECT_PORT |= DIRECT_PORT_MASK;
#if DIRECT_KEYS > 7
  DIRECT_PORT_2 |= DIRECT_PORT_2_MASK;
#endif
#endif

  // Interrupt 0 on trigger edge of STROBE
  EIMSK |= CONTROL_STROBE_INTERRUPT;
  EICRA |= CONTROL_STROBE_TRIGGER;

#if BELL_MODE != BELL_MODE_NONE
  BELL_DDR |= BELL_MASK;
  BELL_OFF;
#endif

#if READY_ACK_MODE != READY_ACK_MODE_NONE
  READY_ACK_DDR |= READY_ACK_MASK;
  READY_ACK_OFF;
#endif
}

void Parallel_Kbd_Task(void)
{
  // Read from serial input.
  int16_t in = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
  if (in > 0) {
    if (in == ASCII_ENQ) {
      CDC_Device_SendString_P(&VirtualSerial_CDC_Interface, answerback);
    } else if (in == ASCII_BEL) {
      BELL_ON;
      _delay_us(BELL_DURATION_USEC);
      BELL_OFF;
    }
  }

  // Check interrupt queue.
  bool sent = false;
  while (!QueueIsEmpty()) {
    queue_entry_t entry = QueueRemove();
#if DIRECT_KEYS > 0
    UpdateDirectKeys(entry.directKeys);
#endif
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
#ifdef DIRECT_ESC_PREFIX_MASK
    if ((entry.directKeys & DIRECT_ESC_PREFIX_MASK) != 0)
      CharEscPrefixAction(charCode);
    else
#endif
    CharAction(charCode);
    sent = true;
  }
#if READY_ACK_MODE == READY_ACK_MODE_KEY_ACK
#if READY_ACK_DELAY_MSEC == 0
  if (sent) {
      READY_ACK_ON;
      _delay_us(READY_ACK_DURATION_USEC);
      READY_ACK_OFF;
  }
#else
  static uint16_t lastSentMillis;
  static bool readyAckPending;
  if (sent) {
    lastSentMillis = millisCounter;
    readyAckPending = true;
  } else if (readyAckPending && millisCounter - lastSentMillis > READY_ACK_DELAY_MSEC) {
    READY_ACK_ON;
    _delay_us(READY_ACK_DURATION_USEC);
    READY_ACK_OFF;
    readyAckPending = false;
  }
#endif
#endif

#if DIRECT_KEYS > 0
  // Check direct keys
  direct_keys_t directKeysNext;
  if (ReadDirectKeysDebounce(&directKeysNext)) {
    UpdateDirectKeys(directKeysNext);
  }
#endif
}
