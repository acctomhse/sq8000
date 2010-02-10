#ifndef __GPIO_H
#define __GPIO_H

#include <global.h>
#include "gpio-regs.h"

#undef PA
#undef PB
#undef PC
#undef PD
#undef PE
#undef PF
#undef PG
#undef PH
#undef PI
#undef PJ
#undef PK
#undef PL
#undef PM
#undef PN
#undef PO
#undef PP

// port definition
#define PA	0x0
#define PB	0x1
#define PC	0x2
#define PD	0x3
#define PE	0x4
#define PF	0x5
#define PG	0x6
#define PH	0x7
#define PI	0x8
#define PJ	0x9
#define PK	0xa
#define PL	0xb
#define PM	0xc
#define PN	0xd
#define PO	0xe
#define PP	0xf

extern int socle_gpio_get_value(u8 port);
extern void socle_gpio_set_value(u8 port, u8 value);

extern int socle_gpio_get_direction(u8 port);
	// return: 0:input, 1:output
extern void socle_gpio_set_direction(u8 port, u8 dir);
	// dir:  0:input, 1:output

extern int socle_gpio_get_interrupt_mask(u8 port);
	// return: 0:masked, 1:not masked
extern void socle_gpio_set_interrupt_mask(u8 port, u8 value);
	// value: 0:masked, 1:not masked

extern int socle_gpio_get_interrupt_sense(u8 port);
	// return: 0:edge, 1:level
extern void socle_gpio_set_interrupt_sense(u8 port, u8 value);
	// value: 0:edge, 1:level

extern int socle_gpio_get_interrupt_both_edges(u8 port);
	// return: 0:single, 1:both
extern void socle_gpio_set_interrupt_both_edges(u8 port, u8 value);
	// value: 0:single, 1:both

extern int socle_gpio_get_interrupt_event(u8 port);
	// return: 0:falling edge or low level, 1:rising edge or high level
extern void socle_gpio_set_interrupt_event(u8 port, u8 value);
	// value: 0:falling edge or low level, 1:rising edge or high level

extern void socle_gpio_set_interrupt_clear(u8 port, u8 value);
	// value: 0:no effect, 1:clear

extern int socle_gpio_get_interrupt_status(u8 port);
	// return: 0:not active, 1:asserting interrupt



/////////////////////////////////////////////////////////////////////
//mask: 0:not effect, 1:effect

#define SHIFT_MASK(off)		(0x1 << (off))

extern int socle_gpio_get_value_with_mask(u8 port, u8 mask);
extern void socle_gpio_set_value_with_mask(u8 port, u8 value, u8 mask);
extern void socle_gpio_direction_input_with_mask(u8 port, u8 mask);
extern void socle_gpio_direction_output_with_mask(u8 port, u8 mask);
extern int socle_gpio_get_interrupt_mask_with_mask(u8 port, u8 mask);
extern void socle_gpio_set_interrupt_mask_with_mask(u8 port, u8 value, u8 mask);
extern int socle_gpio_get_interrupt_sense_with_mask(u8 port, u8 mask);
extern void socle_gpio_set_interrupt_sense_with_mask(u8 port, u8 value, u8 mask);
extern int socle_gpio_get_interrupt_both_edges_with_mask(u8 port, u8 mask);
extern void socle_gpio_set_interrupt_both_edges_with_mask(u8 port, u8 value, u8 mask);
extern int socle_gpio_get_interrupt_event_with_mask(u8 port, u8 mask);
extern void socle_gpio_set_interrupt_event_with_mask(u8 port, u8 value, u8 mask);
extern int socle_gpio_get_interrupt_status_with_port(u8 port);


extern void socle_gpio_test_mode_en(u8 port, int en);
	// en: 0: disable, else: enable
extern void socle_gpio_test_mode_ctrl(u8 port, int mode);
	// mode: as follows
#define PB2PA	0x0
#define PA2PB	0x1
#define PD2PC	0x2
#define PC2PD	0x3


struct socle_gpio_irq_s {
	int lock;
	int active;
	void (*sub_routine)(void *pparam);
	void *pparam;
};

struct socle_gpio_s {
	int base;
	int irq;
#ifdef SOCLE_GPIO_WITH_INT
	int irq_num;
	struct socle_gpio_irq_s pin[GPIO_PER_PORT_PIN_NUM * GPIO_PORT_NUM];
#endif
};

///////////////// * GPIO Interrupt Service * /////////////////

extern void socle_init_gpio_irq(void);
extern int socle_gpio_get_irq(u8 port);
	// return: irq number
extern void socle_set_gpio_base(u8 group, u32 base_addr);

#ifdef SOCLE_GPIO_WITH_INT


extern void socle_set_gpio_irq(u8 group, u32 int_num);

#define SET_GPIO_PIN_NUM(port, pin)		(((port) * GPIO_PER_PORT_PIN_NUM) + (pin))

extern int socle_request_gpio_irq(int pin, void (*sub_routine)(void *pparam), int irqflags, void *pparam);
	// irqflags: as follows
#define GPIO_INT_SENSE			(0x1 << 0)
#define GPIO_INT_SENSE_EDGE	(0x0)
#define GPIO_INT_SENSE_LEVEL	(0x1 << 0)

#define GPIO_INT_SINGLE_EDGE	(0x0)
#define GPIO_INT_BOTH_EDGE		(0x1 << 1)

#define GPIO_INT_EVENT			(0x1 << 2)
#define GPIO_INT_EVENT_LO		(0x0)
#define GPIO_INT_EVENT_HI		(0x1 << 2)

extern void socle_free_gpio_irq(int pin);
extern void socle_enable_gpio_irq(int pin);
extern void socle_disable_gpio_irq(int pin);
#endif	//SOCLE_GPIO_WITH_INT

#endif	//__GPIO_H

