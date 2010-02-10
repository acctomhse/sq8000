#include <test_item.h>
#include <global.h>
#include <ctype.h>

extern char cpu_type[];

static int shell_layer = -1;
static char *test_item_name[20];

static void test_item_get_act_items(struct test_item_container *container, int act_test_item[], int *item_num);
static void test_item_show_menu(struct test_item_container *container, int act_test_item[], int item_num);
static void socle_development_board_information(void);
static void test_item_shell(struct test_item_container *container, char **item_name, u32 layer);

extern int
test_item_ctrl(struct test_item_container *container, int autotest)
{
	int i, err_flag, input_flag, item_index = 0;
	int act_test_item_array[30] = {0}, act_test_item_num;
	char input[3];		// for 2 digital

	test_item_get_act_items(container, act_test_item_array, &act_test_item_num);

	if (!autotest)
		test_item_show_menu(container, act_test_item_array, act_test_item_num);

	shell_layer++;

	while (1) {
		err_flag = 0;
		input_flag = 0;
	
		/* Show prompt string */
		if (!autotest)
			test_item_shell(container, test_item_name, shell_layer);

		if (autotest) {
			input[0] = 'z';
		} else {
			input_flag = 1;
			scanf("%s", input);
		}
		
		/* Execute the option */
		switch (input[0]) {
		case '?':
			test_item_show_menu(container, act_test_item_array, act_test_item_num);
			break;

		case 'i':
		case 'I':
			socle_development_board_information();
			break;

		case 'z':
		case 'Z':
			for (i = 0; i < act_test_item_num; i++) {
				if (container->items[act_test_item_array[i]].allow_autotest) {
					if (1 == input_flag) {
						printf("\n");
						input_flag = 0;
					}

					test_item_shell(container, test_item_name, shell_layer);
					printf("%d\n", i);

					test_item_name[shell_layer] = container->items[act_test_item_array[i]].item_name;

					if (container->items[act_test_item_array[i]].item_func(1)) {
						printf("%s... [Fail]\n\n", container->items[act_test_item_array[i]].item_name);
						err_flag |= -1;
					} else {
						printf("%s... [Pass]\n\n", container->items[act_test_item_array[i]].item_name);
					}
					
					MSDELAY(10);	// to show message slowly

				}
			}
	
		case 'x':
		case 'X':
			shell_layer--;
			return err_flag;
				
		default:
			if ('\0' == input[1]) {		// for only 1 digit
				if (!isdigit(input[0]))
					goto default_break;
			} else {					// for 2 digit
				if (!isdigit(input[0]) || !isdigit(input[1]))
					goto default_break;
			}

			item_index = atoi(input);

default_break:
			if (item_index >= act_test_item_num)
				goto error_break;
			else {
				test_item_name[shell_layer] = container->items[act_test_item_array[item_index]].item_name;

				if (container->items[act_test_item_array[item_index]].item_func(0))
					printf("%s... [Fail]\n", container->items[act_test_item_array[item_index]].item_name);
				else
					printf("%s... [Pass]\n", container->items[act_test_item_array[item_index]].item_name);
			}
		}
		continue;

error_break:
		printf("Invalid Option!\n");
	}
}

static void
test_item_get_act_items(struct test_item_container *container, int act_test_item[], int *item_num)
{
	int i;
	int test_item_num = container->test_item_size / sizeof(struct test_item);

	*item_num = 0;
	for (i = 0; i < test_item_num; i++)
		if (container->items[i].enable)
			act_test_item[(*item_num)++] = i;
}

static void
test_item_show_menu(struct test_item_container *container, int act_test_item[], int item_num)
{
	int i;

	printf("\n====== (C) 2003-2007 Socle Technology Corp. ======\n");
	printf("< %s >\n", container->menu_name);
	printf("--------------------------------------------------\n");
	
	for (i = 0; i < item_num; i++)
		printf("%2d. %s\n", i, container->items[act_test_item[i]].item_name);

	printf("--------------------------------------------------\n");
	printf(" I. Development Board Information\n");
	printf(" Z. Fully Items Auto-Test\n");
	printf(" ?. Display This Menu\n");
	printf(" X. Exit \n");
	printf("==================================================\n");
}

static void
socle_development_board_information(void)
{
	printf("CPU Type --- %s\n", cpu_type);
	printf("CPU     Clock Frequency : %4dM Hz\n", socle_get_cpu_clock() / (1000 * 1000));
	printf("AHB Bus Clock Frequency : %4dM Hz\n", socle_get_ahb_clock() / (1000 * 1000));
	printf("APB Bus Clock Frequency : %4dM Hz\n", socle_get_apb_clock() / (1000 * 1000));
#if defined(CONFIG_PC7210) || defined(CONFIG_PDK) || defined(CONFIG_PC9220)
	printf("UART Frequency 		: %4dM Hz\n", socle_get_uart_clock() / (1000 * 1000));
#endif
	printf("--------------------------------------------------\n");
	printf("Code End Address = 0x%08x, TEST_PATTERN_START = 0x%08x\n", _end, TEST_PATTERN_START);
}


static void
test_item_shell(struct test_item_container *container, char **item_name, u32 layer)
{
	u32 i;

	printf("[%s@Socle_Diag. /", container->shell_name);

	for (i = 0; i < layer; i++) {
		if ((i != 0) && (0 == i % 3))
			printf("\n");
 		printf("%s/", item_name[i]);
	}

	printf("]$ ");
}

