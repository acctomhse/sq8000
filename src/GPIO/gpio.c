#include "dependency.h"
#include "gpio.h"
#include "gpio-regs.h"


//#define CONFIG_GPIO_DEBUG
#ifdef CONFIG_GPIO_DEBUG
	#define GPIO_DBG(fmt, args...) printf("GPIO: %s(): " fmt, __FUNCTION__, ## args)
#else
	#define GPIO_DBG(fmt, args...)
#endif

#define SOCLE_GPIO_MAX_PIN_NUM		(GPIO_PER_PORT_PIN_NUM * GPIO_PORT_NUM * (1 + SOCLE_GPIO_GP1+SOCLE_GPIO_GP2+SOCLE_GPIO_GP3))

#define GET_GPIO_PORT_FROM_PIN_NUM(pin)		((pin) / GPIO_PER_PORT_PIN_NUM)
#define GET_GPIO_GP_PIN_FROM_PIN_NUM(pin)		((pin) % (GPIO_PER_PORT_PIN_NUM * GPIO_PORT_NUM))
#define GET_GPIO_ABS_PIN_FROM_PIN_NUM(pin)		((pin) % GPIO_PER_PORT_PIN_NUM)
#define GET_GPIO_GROUP_FROM_PORT_NUM(port)		((port) / GPIO_PORT_NUM)


static struct socle_gpio_s socle_gpio[1 + SOCLE_GPIO_GP1 + SOCLE_GPIO_GP2 + SOCLE_GPIO_GP3];


#define GPIO_GET_VALUE_WITH_MASK(fun_get, port, mask)	(fun_get((port)) & (mask))
#define GPIO_SET_VALUE_WITH_MASK(fun_get, fun_set, port, value, mask)		\
	u8 data;														\
	data = (fun_get((port)) & (~mask)) | ((value) & (mask));					\
	fun_set((port), data);


static inline int
gpio_read(u32 offset, u32 base)
{
	return (ioread32(base + offset));
}

static inline void
gpio_write(u32 offset, u32 data, u32 base)
{
	iowrite32(data, base + offset);
}

extern int
socle_gpio_get_value(u8 port)
{
	u8 value;
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_PADR;
		break;
	case 1:
		offset = GPIO_PBDR;
		break;
	case 2:
		offset = GPIO_PCDR;
		break;
	case 3:
		offset = GPIO_PDDR;
		break;
	default:
		return -1;
	}

	value = gpio_read(offset, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, value = 0x%x\n", base, offset, value);

	return value;
}



extern void
socle_gpio_set_value(u8 port, u8 value)
{
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_PADR;
		break;
	case 1:
		offset = GPIO_PBDR;
		break;
	case 2:
		offset = GPIO_PCDR;
		break;
	case 3:
		offset = GPIO_PDDR;
		break;
	default:
		return;
	}

	gpio_write(offset, value, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, value = 0x%x\n", base, offset, value);
}


extern int
socle_gpio_get_direction(u8 port)
{
	u8 dir;
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_PACON;
		break;
	case 1:
		offset = GPIO_PBCON;
		break;
	case 2:
		offset = GPIO_PCCON;
		break;
	case 3:
		offset = GPIO_PDCON;
		break;
	default:
		return -1;
	}

	dir = gpio_read(offset, base);
 
	GPIO_DBG("base = 0x%08x, offset = 0x%x, dir = 0x%x\n", base, offset, dir);

	return dir;
}

extern void
socle_gpio_set_direction(u8 port, u8 dir)
{
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_PACON;
		break;
	case 1:
		offset = GPIO_PBCON;
		break;
	case 2:
		offset = GPIO_PCCON;
		break;
	case 3:
		offset = GPIO_PDCON;
		break;
	default:
		return;
	}

	gpio_write(offset, dir, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, dir = 0x%x\n", base, offset, dir);
}

extern int
socle_gpio_get_interrupt_mask(u8 port)
{
	u8 value;
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_IEA;
		break;
	case 1:
		offset = GPIO_IEB;
		break;
	case 2:
		offset = GPIO_IEC;
		break;
	case 3:
		offset = GPIO_IED;
		break;
	default:
		return -1;
	}

	value = gpio_read(offset, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, value = 0x%x\n", base, offset, value);

	return value;
}

extern void
socle_gpio_set_interrupt_mask(u8 port, u8 value)
{
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_IEA;
		break;
	case 1:
		offset = GPIO_IEB;
		break;
	case 2:
		offset = GPIO_IEC;
		break;
	case 3:
		offset = GPIO_IED;
		break;
	default:
		return;
	}

	gpio_write(offset, value, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, value = 0x%x\n", base, offset, value);
}

extern int
socle_gpio_get_interrupt_sense(u8 port)
{
	u8 value;
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_ISA;
		break;
	case 1:
		offset = GPIO_ISB;
		break;
	case 2:
		offset = GPIO_ISC;
		break;
	case 3:
		offset = GPIO_ISD;
		break;
	default:
		return -1;
	}

	value = gpio_read(offset, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, value = 0x%x\n", base, offset, value);

	return value;
}

extern void
socle_gpio_set_interrupt_sense(u8 port, u8 value)
{
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_ISA;
		break;
	case 1:
		offset = GPIO_ISB;
		break;
	case 2:
		offset = GPIO_ISC;
		break;
	case 3:
		offset = GPIO_ISD;
		break;
	default:
		return;
	}

	gpio_write(offset, value, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, value = 0x%x\n", base, offset, value);
}

extern int
socle_gpio_get_interrupt_both_edges(u8 port)
{
	u8 value;
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_IBEA;
		break;
	case 1:
		offset = GPIO_IBEB;
		break;
	case 2:
		offset = GPIO_IBEC;
		break;
	case 3:
		offset = GPIO_IBED;
		break;
	default:
		return -1;
	}

	value = gpio_read(offset, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, value = 0x%x\n", base, offset, value);

	return value;
}

extern void
socle_gpio_set_interrupt_both_edges(u8 port, u8 value)
{
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_IBEA;
		break;
	case 1:
		offset = GPIO_IBEB;
		break;
	case 2:
		offset = GPIO_IBEC;
		break;
	case 3:
		offset = GPIO_IBED;
		break;
	default:
		return;
	}

	gpio_write(offset, value, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, value = 0x%x\n", base, offset, value);
}

extern int
socle_gpio_get_interrupt_event(u8 port)
{
	u8 value;
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_IEVA;
		break;
	case 1:
		offset = GPIO_IEVB;
		break;
	case 2:
		offset = GPIO_IEVC;
		break;
	case 3:
		offset = GPIO_IEVD;
		break;
	default:
		return -1;
	}

	value = gpio_read(offset, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, value = 0x%x\n", base, offset, value);

	return value;
}

extern void
socle_gpio_set_interrupt_event(u8 port, u8 value)
{
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_IEVA;
		break;
	case 1:
		offset = GPIO_IEVB;
		break;
	case 2:
		offset = GPIO_IEVC;
		break;
	case 3:
		offset = GPIO_IEVD;
		break;
	default:
		return;
	}

	gpio_write(offset, value, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, value = 0x%x\n", base, offset, value);
}

extern void
socle_gpio_set_interrupt_clear(u8 port, u8 value)
{
	u16 offset;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = GPIO_ICA;
		break;
	case 1:
		offset = GPIO_ICB;
		break;
	case 2:
		offset = GPIO_ICC;
		break;
	case 3:
		offset = GPIO_ICD;
		break;
	default:
		return;
	}

	gpio_write(offset, value, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, value = 0x%x\n", base, offset, value);
}


extern int
socle_gpio_get_interrupt_status(u8 port)
{
	u32 value;
	u16 offset = GPIO_ISR;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	value = gpio_read(offset, base);

	GPIO_DBG("base = 0x%08x, offset = 0x%x, value = 0x%x\n", base, offset, value);

	return value;
}



extern int
socle_gpio_get_value_with_mask(u8 port, u8 mask)
{
	socle_gpio_direction_input_with_mask(port, mask);
	return socle_gpio_get_value(port) & mask;
}

extern void
socle_gpio_set_value_with_mask(u8 port, u8 value, u8 mask)
{
 	u8 data;

	socle_gpio_direction_output_with_mask(port, mask);
	data = (socle_gpio_get_value(port) & ~mask) | (value & mask);
	socle_gpio_set_value(port, data);
}

extern void
socle_gpio_direction_input_with_mask(u8 port, u8 mask)
{
	u8 dir;

	dir = socle_gpio_get_direction(port) & ~mask;
	socle_gpio_set_direction(port, dir);
}

extern void
socle_gpio_direction_output_with_mask(u8 port, u8 mask)
{
	u8 dir;

	dir = socle_gpio_get_direction(port) | mask;
	socle_gpio_set_direction(port, dir);
}

extern int
socle_gpio_get_interrupt_mask_with_mask(u8 port, u8 mask)
{
	return GPIO_GET_VALUE_WITH_MASK(socle_gpio_get_interrupt_mask, port, mask);
}

extern void
socle_gpio_set_interrupt_mask_with_mask(u8 port, u8 value, u8 mask)
{
	GPIO_SET_VALUE_WITH_MASK(socle_gpio_get_interrupt_mask, socle_gpio_set_interrupt_mask, port, value, mask)
}

extern int
socle_gpio_get_interrupt_sense_with_mask(u8 port, u8 mask)
{
	return GPIO_GET_VALUE_WITH_MASK(socle_gpio_get_interrupt_sense, port, mask);
}

extern void
socle_gpio_set_interrupt_sense_with_mask(u8 port, u8 value, u8 mask)
{
	GPIO_SET_VALUE_WITH_MASK(socle_gpio_get_interrupt_sense, socle_gpio_set_interrupt_sense, port, value, mask)
}

extern int
socle_gpio_get_interrupt_both_edges_with_mask(u8 port, u8 mask)
{
	return GPIO_GET_VALUE_WITH_MASK(socle_gpio_get_interrupt_both_edges, port, mask);
}

extern void
socle_gpio_set_interrupt_both_edges_with_mask(u8 port, u8 value, u8 mask)
{
	GPIO_SET_VALUE_WITH_MASK(socle_gpio_get_interrupt_both_edges, socle_gpio_set_interrupt_both_edges, port, value, mask)
}

extern int
socle_gpio_get_interrupt_event_with_mask(u8 port, u8 mask)
{
	return GPIO_GET_VALUE_WITH_MASK(socle_gpio_get_interrupt_event, port, mask);
}

extern void
socle_gpio_set_interrupt_event_with_mask(u8 port, u8 value, u8 mask)
{
	GPIO_SET_VALUE_WITH_MASK(socle_gpio_get_interrupt_event, socle_gpio_set_interrupt_event, port, value, mask)
}

extern int
socle_gpio_get_interrupt_status_with_port(u8 port)
{
	u32 value;
	u16 offset;

	value = socle_gpio_get_interrupt_status(port);

	switch (port % GPIO_PORT_NUM) {
	case 0:
		offset = 0;
		break;
	case 1:
		offset = 8;
		break;
	case 2:
		offset = 16;
		break;
	case 3:
		offset = 24;
		break;
	default:
		return -1;
	}

	value = value >> offset;

	return (value & 0xFF);
}


extern void
socle_gpio_test_mode_en(u8 port, int en)
{
	int data;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	data = gpio_read(GPIO_TEST, base);

	if (en)
		data |= 0x1;
	else
		data &= ~0x1;

	gpio_write(GPIO_TEST, data, base);
}

extern void
socle_gpio_test_mode_ctrl(u8 port, int mode)
{
	int  test_mode, data;
	u32 base;

	base = socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].base;

	switch (mode) {
	case PB2PA:
		test_mode = PB2PA;
		break;
	case PA2PB:
		test_mode = PA2PB;
		break;
	case PD2PC:
		test_mode = PD2PC;
		break;
	case PC2PD:
		test_mode = PC2PD;
		break;
	default:
		printf("Invalid argument! mode = %d\n", mode);
		return;
	}

	data = gpio_read(GPIO_TEST, base) & ~0x6;
	gpio_write(GPIO_TEST, (test_mode << 1) | data, base);
}


///////////////// * GPIO Interrupt Service * /////////////////

#ifdef SOCLE_GPIO_WITH_INT

#define xchg(ptr, x)				\
	((__typeof__(*(ptr)))__xchg((u32)(x), (ptr), sizeof(*(ptr))))

static inline u32
__xchg(u32 x, volatile void *ptr, u32 size)
{
	u32 ret;

	switch (size) {
	case 1:
		asm volatile ("@  __xchg1\n"
			      " swp %0, %1, [%2]"
			      : "=&r" (ret)
			      : "r" (x), "r" (ptr)
			      : "memory", "cc");
		break;
	case 4:
		asm volatile ("@ __xchg4\n"
			      " swp %0, %1, [%2]"
			      : "=&r" (ret)
			      : "r" (x), "r" (ptr)
			      : "memory", "cc");
		break;
	}
	return ret;
}

extern int
socle_gpio_get_irq(u8 port)
{
	return socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].irq;
}

extern int
socle_request_gpio_irq(int pin, void (*sub_routine)(void *pparam), int irqflags, void *pparam)
{
	int port = GET_GPIO_PORT_FROM_PIN_NUM(pin), val = 1 << GET_GPIO_ABS_PIN_FROM_PIN_NUM(pin);
	struct socle_gpio_irq_s *irq = &socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].pin[GET_GPIO_GP_PIN_FROM_PIN_NUM(pin)];

	GPIO_DBG("SOCLE_GPIO_MAX_PIN_NUM=%d pin[%d] isr@0x%x\n", SOCLE_GPIO_MAX_PIN_NUM, pin, sub_routine);

	if ((pin >= SOCLE_GPIO_MAX_PIN_NUM) || !sub_routine)
		goto bad;
	if (xchg(&irq->lock, 1) != 0)
		goto busy;

	irq->active = 1;
	irq->sub_routine = sub_routine;
	irq->pparam = pparam;
	socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].irq_num++;

	// set as normal mode
	socle_gpio_test_mode_en(port, 0);

	// set as input
	socle_gpio_get_value_with_mask(port, val);

	// set interrupt trigger type
	printf("Request GPIO[%d] IRQ, Triggle with ", pin);
	if (GPIO_INT_SENSE_EDGE == (irqflags & GPIO_INT_SENSE)) {
		socle_gpio_set_interrupt_sense_with_mask(port,  0x0, val);

		if (GPIO_INT_SINGLE_EDGE == (irqflags & GPIO_INT_BOTH_EDGE)) {
			socle_gpio_set_interrupt_both_edges_with_mask(port, 0x0, val);

			if (GPIO_INT_EVENT_LO == (irqflags & GPIO_INT_EVENT)) {
				printf("Falling Edge\n");
				socle_gpio_set_interrupt_event_with_mask(port, 0x0, val);
			} else if (GPIO_INT_EVENT_HI == (irqflags & GPIO_INT_EVENT))  {
				printf("Rising Edge\n");
				socle_gpio_set_interrupt_event_with_mask(port, val, val);
			}

		} else if (GPIO_INT_BOTH_EDGE == (irqflags & GPIO_INT_BOTH_EDGE))  {
			printf("Both Edge\n");
			socle_gpio_set_interrupt_both_edges_with_mask(port, val, val);
		}

	} else if (GPIO_INT_SENSE_LEVEL == (irqflags & GPIO_INT_SENSE))  {
		socle_gpio_set_interrupt_sense_with_mask(port,  val, val);

		if (GPIO_INT_EVENT_LO == (irqflags & GPIO_INT_EVENT)) {
			printf("Low Level\n");
			socle_gpio_set_interrupt_event_with_mask(port, 0x0, val);
		} else if (GPIO_INT_EVENT_HI == (irqflags & GPIO_INT_EVENT))  {
			printf("High Level\n");
			socle_gpio_set_interrupt_event_with_mask(port, val, val);
		}
	}

	// enable interrupt
	socle_gpio_set_interrupt_mask_with_mask(port, val, val);

	return 0;

bad:
	printf("Socle GPIO: trying to allocate pin[%d] fail!\n", pin);
busy:
	return -1;
}

extern void
socle_free_gpio_irq(int pin)
{
	int port = GET_GPIO_PORT_FROM_PIN_NUM(pin), val = 1 << GET_GPIO_ABS_PIN_FROM_PIN_NUM(pin);
	struct socle_gpio_irq_s *irq = &socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].pin[GET_GPIO_GP_PIN_FROM_PIN_NUM(pin)];

	if (pin >= SOCLE_GPIO_MAX_PIN_NUM)
		goto bad;

	if (irq->active) {
		printf("Socle GPIO: freeing active pin[%d]\n", pin);
		irq->active = 0;
		socle_gpio_set_interrupt_mask_with_mask(port, 0x0, val);
	}

	if (xchg(&irq->lock, 0) != 0) {
		irq->sub_routine = NULL;
		socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].irq_num--;
		return;
	}

bad:
	printf("Socle GPIO: trying to free pin[%d] fail!\n", pin);
}

extern void 
socle_enable_gpio_irq(int pin)
{
	int port = GET_GPIO_PORT_FROM_PIN_NUM(pin), val = 1 << GET_GPIO_ABS_PIN_FROM_PIN_NUM(pin);
	struct socle_gpio_irq_s *irq = &socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].pin[GET_GPIO_GP_PIN_FROM_PIN_NUM(pin)];

	if ((pin >= SOCLE_GPIO_MAX_PIN_NUM) || !irq->lock)
		goto bad;

	if (0 == irq->active) {
		irq->active = 1;
		socle_gpio_set_interrupt_mask_with_mask(port, val, val);
	}

	return;

bad:
	printf("Socle GPIO: trying to enable free pin[%d] fail!\n", pin);
}

extern void
socle_disable_gpio_irq(int pin)
{
	int port = GET_GPIO_PORT_FROM_PIN_NUM(pin), val = 1 << GET_GPIO_ABS_PIN_FROM_PIN_NUM(pin);
	struct socle_gpio_irq_s *irq = &socle_gpio[GET_GPIO_GROUP_FROM_PORT_NUM(port)].pin[GET_GPIO_GP_PIN_FROM_PIN_NUM(pin)];

	if ((pin >= SOCLE_GPIO_MAX_PIN_NUM) || !irq->lock)
		goto bad;

	if (1 == irq->active) {
		irq->active = 0;
		socle_gpio_set_interrupt_mask_with_mask(port, 0x0, val);
	}

	return;

bad:
	printf("Socle GPIO: trying to disable free pin[%d] fail!\n", pin);
}


static void
socle_gpio_com_isr(int group)
{
	int status, i, idx, irq_num = 0;

	if (!socle_gpio[group].irq_num)
		return;

	// read interrupt status
	status = socle_gpio_get_interrupt_status(GPIO_PORT_NUM * group);

	for (i = 0, idx = 0x1; i < (GPIO_PER_PORT_PIN_NUM * GPIO_PORT_NUM); i++, idx <<= 1) {
		if (idx & status) {
			int pin = i + GPIO_PER_PORT_PIN_NUM * GPIO_PORT_NUM * group;
			int port = GET_GPIO_PORT_FROM_PIN_NUM(pin), val = 1 << GET_GPIO_ABS_PIN_FROM_PIN_NUM(GET_GPIO_GP_PIN_FROM_PIN_NUM(pin));
			struct socle_gpio_irq_s *irq = &socle_gpio[group].pin[GET_GPIO_GP_PIN_FROM_PIN_NUM(pin)];

			GPIO_DBG("i = %d, pin[%d]\n", i, pin);

			if (irq->sub_routine) {
				// execute sub_routine
				irq->sub_routine(irq->pparam);
				// clear interrupt
				socle_gpio_set_interrupt_clear(port, val);

				irq_num++;
			} else {
				printf("Error! socle_gpio_isr_gp%d: No socle_gpio_irq_s[%d].sub_routine!\n", group, pin);
				continue;
			}

			if (irq_num >= socle_gpio[group].irq_num)
				return;
		}
	}
}

static void
socle_gpio_isr_gp0(void *pparam)
{
	socle_gpio_com_isr(0);
}

static void
socle_gpio_isr_gp1(void *pparam)
{
	socle_gpio_com_isr(1);
}

static void
socle_gpio_isr_gp2(void *pparam)
{
	socle_gpio_com_isr(2);
}

static void
socle_gpio_isr_gp3(void *pparam)
{
	socle_gpio_com_isr(3);
}

extern void
socle_set_gpio_irq(u8 group, u32 int_num)
{
	if (group == 0) {
		socle_gpio[group].irq = int_num;
		request_irq(socle_gpio[group].irq, socle_gpio_isr_gp0, NULL);
	} else if (group == 1) {
		socle_gpio[group].irq = int_num;
		request_irq(socle_gpio[group].irq, socle_gpio_isr_gp1, NULL);
	} else if (group == 2) {
		socle_gpio[group].irq = int_num;
		request_irq(socle_gpio[group].irq, socle_gpio_isr_gp2, NULL);
	} else if (group == 3) {
		socle_gpio[group].irq = int_num;
		request_irq(socle_gpio[group].irq, socle_gpio_isr_gp3, NULL);
	} else
		printf("set gpio irq fail\n");
}

#else
extern int
socle_gpio_get_irq(u8 port)
{
	return 0;
}
#endif	//SOCLE_GPIO_WITH_INT


extern void
socle_init_gpio_irq(void)
{
	memset((char *)socle_gpio, 0, sizeof(socle_gpio));

	socle_gpio[0].base = SOCLE_APB0_GPIO0;

#if SOCLE_GPIO_GP1 == 1
	socle_gpio[1].base = SOCLE_APB0_GPIO1;
#endif

#if SOCLE_GPIO_GP2 == 1
	socle_gpio[2].base = SOCLE_APB0_GPIO2;
#endif

#if SOCLE_GPIO_GP2 == 1
	socle_gpio[3].base = SOCLE_APB0_GPIO3;
#endif



#ifdef SOCLE_GPIO_WITH_INT
	socle_gpio[0].irq = SOCLE_INTC_GPIO0;
	socle_gpio[0].irq_num = 0;
	request_irq(socle_gpio[0].irq, socle_gpio_isr_gp0, NULL);

#if SOCLE_GPIO_GP1 == 1
	socle_gpio[1].irq = SOCLE_INTC_GPIO1;
	socle_gpio[1].irq_num = 0;
	request_irq(socle_gpio[1].irq, socle_gpio_isr_gp1, NULL);
#endif

#if SOCLE_GPIO_GP2 == 1
	socle_gpio[2].irq = SOCLE_INTC_GPIO2;
	socle_gpio[2].irq_num = 0;
	request_irq(socle_gpio[2].irq, socle_gpio_isr_gp2, NULL);
#endif

#if SOCLE_GPIO_GP3 == 1
	socle_gpio[3].irq = SOCLE_INTC_GPIO3;
	socle_gpio[3].irq_num = 0;
	request_irq(socle_gpio[3].irq, socle_gpio_isr_gp3, NULL);
#endif

#endif	//SOCLE_GPIO_WITH_INT

}

extern void
socle_set_gpio_base(u8 group, u32 base_addr)
{
	if (group > 3) {
		printf("set gpio base fail\n");
		return;
	}
	socle_gpio[group].base = base_addr;
}

