#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "tidisp.h"
#include "comm.h"
#include "thread.h"

#define VERSION_BASE	(int)1
#define VERSION_MAJOR	(int)0
#define VERSION_MINOR	(int)0

char *warranty =
	"	       Copyright (c) 2016-2022 Sequent Microsystems\n"
		"                                                             \n"
		"		This program is free software; you can redistribute it and/or modify\n"
		"		it under the terms of the GNU Leser General Public License as published\n"
		"		by the Free Software Foundation, either version 3 of the License, or\n"
		"		(at your option) any later version.\n"
		"                                    \n"
		"		This program is distributed in the hope that it will be useful,\n"
		"		but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"		GNU Lesser General Public License for more details.\n"
		"			\n"
		"		You should have received a copy of the GNU Lesser General Public License\n"
		"		along with this program. If not, see <http://www.gnu.org/licenses/>.";

void usage(void)
{
	int i = 0;
	while (gCmdArray[i] != NULL)
	{
		if (gCmdArray[i]->name != NULL)
		{
			if (strlen(gCmdArray[i]->usage1) > 2)
			{
				printf("%s", gCmdArray[i]->usage1);
			}
			if (strlen(gCmdArray[i]->usage2) > 2)
			{
				printf("%s", gCmdArray[i]->usage2);
			}
		}
		i++;
	}
	printf("Type tidisp -h <command> for more help\n");
}

int doBoardInit(void)
{
	int dev = 0;
	int add = 0;
	uint8_t buff[8];

	add = HW_ADD;
	dev = i2cSetup(add);
	if (dev == -1)
	{
		return ERROR;
	}
	if (ERROR == i2cMem8Read(dev, I2C_REVISION_MAJOR_MEM_ADD, buff, 1))
	{
		printf("Thermal Integration display did not detected!\n");
		return ERROR;
	}
	return dev;
}

int boardCheck(void)
{
	int dev = 0;
	int add = 0;
	uint8_t buff[8];

	add = HW_ADD;
	dev = i2cSetup(add);
	if (dev == -1)
	{
		return ERROR;
	}
	if (ERROR == i2cMem8Read(dev, I2C_REVISION_MAJOR_MEM_ADD, buff, 1))
	{

		return ERROR;
	}
	return OK;
}
static void doHelp(int argc, char *argv[]);
const CliCmdType CMD_HELP =
{
	"-h",
	1,
	&doHelp,
	"\t-h		Display the list of command options or one command option details\n",
	"\tUsage:		tidisp -h    Display command options list\n",
	"\tUsage:		tidisp -h <param>   Display help for <param> command option\n",
	"\tExample:		tidisp -h rread    Display help for \"rread\" command option\n"};

static void doHelp(int argc, char *argv[])
{
	int i = 0;
	if (argc == 3)
	{
		while (NULL != gCmdArray[i])
		{
			if (gCmdArray[i]->name != NULL)
			{
				if (strcasecmp(argv[2], gCmdArray[i]->name) == 0)
				{
					printf("%s%s%s%s", gCmdArray[i]->help, gCmdArray[i]->usage1,
						gCmdArray[i]->usage2, gCmdArray[i]->example);
					break;
				}
			}
			i++;
		}
		if (NULL == gCmdArray[i])
		{
			printf("Option \"%s\" not found\n", argv[2]);
			i = 0;
			while (NULL != gCmdArray[i])
			{
				if (gCmdArray[i]->name != NULL)
				{
					printf("%s", gCmdArray[i]->help);
					break;
				}
				i++;
			}
		}
	}
	else
	{
		i = 0;
		while (NULL != gCmdArray[i])
		{
			if (gCmdArray[i]->name != NULL)
			{
				printf("%s", gCmdArray[i]->help);
			}
			i++;
		}
	}
}

static void doVersion(int argc, char *argv[]);
const CliCmdType CMD_VERSION =
{
	"-v",
	1,
	&doVersion,
	"\t-v		Display the tidisp command version number\n",
	"\tUsage:		tidisp -v\n",
	"",
	"\tExample:		tidisp -v  Display the version number\n"};

static void doVersion(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	printf("tidisp v%d.%d.%d Copyright (c) 2016 - 2022 Sequent Microsystems\n",
	VERSION_BASE, VERSION_MAJOR, VERSION_MINOR);
	printf("\nThis is free software with ABSOLUTELY NO WARRANTY.\n");
	printf("For details type: tidisp -warranty\n");

}

static void doWarranty(int argc, char *argv[]);
const CliCmdType CMD_WAR =
{
	"-warranty",
	1,
	&doWarranty,
	"\t-warranty	Display the warranty\n",
	"\tUsage:		tidisp -warranty\n",
	"",
	"\tExample:		tidisp -warranty  Display the warranty text\n"};

static void doWarranty(int argc UNU, char* argv[] UNU)
{
	printf("%s\n", warranty);
}

static void doList(int argc, char *argv[]);
const CliCmdType CMD_LIST =
	{
		"-list",
		1,
		&doList,
		"\t-list:		List all display boards connected\n\t\t\treturn the # of boards and stack level for every board\n",
		"\tUsage:		tidisp -list\n",
		"",
		"\tExample:		tidisp -list display: 1,0 \n"};

static void doList(int argc, char *argv[])
{

	UNUSED(argc);
	UNUSED(argv);

	if (boardCheck() == OK)
	{
		printf("Display board detected\n");
	}
	else
	{
		printf("NO Dispaly board detected!\n");
	}

}

static void doBoard(int argc, char *argv[]);
const CliCmdType CMD_BOARD =
	{
		"board",
		1,
		&doBoard,
		"\tboard		Display the board status and firmware version number\n",
		"\tUsage:		tidisp board\n",
		"",
		"\tExample:		tidisp board  Display vcc, temperature, firmware version, battery voltage \n"};

static void doBoard(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[5];
	int resp = 0;

	UNUSED(argv);
	if (argc != 2)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}

	resp = i2cMem8Read(dev, I2C_REVISION_MAJOR_MEM_ADD, buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to read board info!\n");
		exit(1);
	}
	printf("Firmware ver %02d.%02d,\n", (int)buff[0], (int)buff[1]);
}

static void doWriteCmd(int argc, char *argv[]);
const CliCmdType CMD_WR_CMD =
{
	"cmdwr",
	1,
	&doWriteCmd,
	"\tcmdwr		Send a command to display\n",
	"\tUsage:		tidisp cmdwr <cmd>\n",
	"",
	"\tExample:		tidisp cmdwr 1 -> Send clear display command\n"};

static void doWriteCmd(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[5];
	int resp = 0;

	if (argc != 3)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	resp = atoi(argv[2]);
	buff[0] = 0xff & resp;
	buff[1] = LCD_CMD_INST_WR;
	resp = i2cMem8Write(dev, I2C_LCD_DATA, buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to write!\n");
		exit(1);
	}
}

static void doWriteData(int argc, char *argv[]);
const CliCmdType CMD_WR_DATA =
{
	"dtwr",
	1,
	&doWriteData,
	"\tdtwr		Send a byte of data to display\n",
	"\tUsage:		tidisp dtwr <cmd>\n",
	"",
	"\tExample:		tidisp dtwr 65 -> Send \"A\" char to display\n"};

static void doWriteData(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[5];
	int resp = 0;

	if (argc != 3)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	resp = atoi(argv[2]);
	buff[0] = 0xff & resp;
	buff[1] = LCD_CMD_DATA_WR;
	resp = i2cMem8Write(dev, I2C_LCD_DATA, buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to write!\n");
		exit(1);
	}
}

static void doWriteText(int argc, char *argv[]);
const CliCmdType CMD_WR_TXT =
{
	"txtwr",
	1,
	&doWriteText,
	"\ttxtwr		Send a text to display\n",
	"\tUsage:		tidisp txtwr <text>\n",
	"",
	"\tExample:		tidisp txtwr Heatweb -> Write \"Heatweb\" text to display\n"};

static void doWriteText(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[5];
	int resp = 0;
	unsigned int i = 0;
	int j = 0;

	if (argc < 3)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	for (j = 2; j < argc; j++)
	{
		for (i = 0; i < strlen(argv[j]); i++)
		{
			//resp = argv[2]);
			buff[0] = 0xff & argv[j][i];
			buff[1] = LCD_CMD_DATA_WR;
			resp = i2cMem8Write(dev, I2C_LCD_DATA, buff, 2);
			if (FAIL == resp)
			{
				printf("Fail to write!\n");
				exit(1);
			}
		}
		buff[0] = ' ';
		resp = i2cMem8Write(dev, I2C_LCD_DATA, buff, 2);
		if (FAIL == resp)
		{
			printf("Fail to write!\n");
			exit(1);
		}
	}
}
void setCursor(int dev, int line, int column)
{
	u8 buff[5];
	int resp = 0;
	int address = 0;

	if (line < 1 || line > LCD_LINES_NO)
	{
		printf("Invald line number [1..%d]\n", LCD_LINES_NO);
		exit(1);
	}
	if (column < 1 || column > LCD_COL_NO)
	{
		printf("Invald column number [1..%d]\n", LCD_COL_NO);
		exit(1);
	}
	if (line == 2) //swap lines 3 and 2
	{
		line = 3;
	}
	else if (line == 3)
	{
		line = 2;
	}
	address = (line - 1) * LCD_COL_NO + (column - 1);
	if (line > 2)
	{
		address = LCD_SECOND_LINE_ADD_OFFSET + (line - 3) * LCD_COL_NO
			+ (column - 1);
	}
	buff[0] = 0xff & (address + LCD_INST_SET_DDR);
	buff[1] = LCD_CMD_INST_WR;
	resp = i2cMem8Write(dev, I2C_LCD_DATA, buff, 2);
	if (FAIL == resp)
	{
		printf("Fail to write!\n");
		exit(1);
	}
}

static void doWriteTextAt(int argc, char *argv[]);
const CliCmdType CMD_WR_TXT_AT =
	{
		"txtwrat",
		1,
		&doWriteTextAt,
		"\ttxtwrat		Send a text to display at certain position\n",
		"\tUsage:		tidisp txtwrat <ine> <column> <text>\n",
		"",
		"\tExample:		tidisp txtwr 1 1 Heatweb -> Write \"Heatweb\" text to display at line 1 column 1\n"};

static void doWriteTextAt(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[5];
	int resp = 0;
	unsigned int i = 0;
	int j = 0;
	int line = 0;
	int column = 0;
	int writePos = 0;

	if (argc < 5)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	line = atoi(argv[2]);
	column = atoi(argv[3]);
	setCursor(dev, line, column);
	writePos = column;
	for (j = 4; j < argc; j++)
	{
		for (i = 0; i < strlen(argv[j]); i++)
		{
			if (writePos <= LCD_COL_NO)
			{
				//resp = argv[2]);
				buff[0] = 0xff & argv[j][i];
				buff[1] = LCD_CMD_DATA_WR;
				resp = i2cMem8Write(dev, I2C_LCD_DATA, buff, 2);
				if (FAIL == resp)
				{
					printf("Fail to write!\n");
					exit(1);
				}
				writePos++;
			}
		}
		if (writePos <= LCD_COL_NO)
		{
			buff[0] = ' ';
			resp = i2cMem8Write(dev, I2C_LCD_DATA, buff, 2);
			if (FAIL == resp)
			{
				printf("Fail to write!\n");
				exit(1);
			}
			writePos++;
		}
	}
}

static void doSetCursor(int argc, char *argv[]);
const CliCmdType CMD_SET_CURSOR =
{
	"crs",
	1,
	&doSetCursor,
	"\tcrs		Set cursor position for 4x20 character display\n",
	"\tUsage:		tidisp crs <line> <column>\n",
	"",
	"\tExample:		tidisp crs 1 10 -> Set cursor position at line 1 column 10\n"};

static void doSetCursor(int argc, char *argv[])
{
	int dev = -1;
	int line = 0;
	int column = 0;

	if (argc != 4)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	line = atoi(argv[2]);
	column = atoi(argv[3]);
	setCursor(dev, line, column);
}

static void doWriteBacklight(int argc, char *argv[]);
const CliCmdType CMD_WR_BL =
{
	"blwr",
	1,
	&doWriteBacklight,
	"\tblwr		Set the backlight intensity [0..100]\n",
	"\tUsage:		tidisp blwr <bl>\n",
	"",
	"\tExample:		tidisp blwr 50 -> Set backlight half intensity\n"};

static void doWriteBacklight(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[5];
	int resp = 0;

	if (argc != 3)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	resp = atoi(argv[2]);
	if (resp < 0 || resp > 100)
	{
		printf("Invalid backligt intensity value [0..100]\n");
		exit(1);
	}
	buff[0] = 0xff & resp;

	resp = i2cMem8Write(dev, I2C_LCD_BL, buff, 1);
	if (FAIL == resp)
	{
		printf("Fail to write!\n");
		exit(1);
	}
}

void setLed(int dev, int color, int led, int val)
{
	u8 buff[2];
	int resp;

	if (led < 1 || led > 6)
	{
		printf("Invalid LED no! [1..6]\n");
		exit(1);
	}
	if (color < RED || color > BLUE)
	{
		printf("Invalid color!\n");
		exit(1);
	}
	resp = i2cMem8Read(dev, color, buff, 1);
	if (FAIL == resp)
	{
		printf("Fail to read!\n");
		exit(1);
	}

	if (val != 0)
	{
		buff[0] |= 1 << (led - 1);
	}
	else
	{
		buff[0] &= ~ (1 << (led - 1));
	}
	resp = i2cMem8Write(dev, color, buff, 1);
	if (FAIL == resp)
	{
		printf("Fail to write!\n");
		exit(1);
	}
}

void setAllLeds(int dev, int color, int val)
{
	u8 buff[2];
	int resp;

	if (color < RED || color > BLUE)
	{
		printf("Invalid color!\n");
		exit(1);
	}
	buff[0] = 0x3f & val;
	resp = i2cMem8Write(dev, color, buff, 1);
	if (FAIL == resp)
	{
		printf("Fail to write!\n");
		exit(1);
	}
}

static void doWriteRedLed(int argc, char *argv[]);
const CliCmdType CMD_WR_REDL =
{
	"red",
	1,
	&doWriteRedLed,
	"\tred		Turn on or off one or all the red  LED's\n",
	"\tUsage:		tidisp red <led> <0/1>\n",
	"\tUsage:		tidisp red <led_bitmap>\n",
	"\tExample:		tidisp red 1 1 -> Turn led 1 red color ON\n"};

static void doWriteRedLed(int argc, char *argv[])
{
	int dev = -1;

	if (argc != 3 && argc != 4)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	if (argc == 4)
	{
		setLed(dev, RED, atoi(argv[2]), atoi(argv[3]));
	}
	else
	{
		setAllLeds(dev, RED, atoi(argv[2]));
	}
}

static void doWriteGreenLed(int argc, char *argv[]);
const CliCmdType CMD_WR_GREENL =
{
	"green",
	1,
	&doWriteGreenLed,
	"\tgreen		Turn on or off one or all the green LED's\n",
	"\tUsage:		tidisp green <led> <0/1>\n",
	"\tUsage:		tidisp green <led_bitmap>\n",
	"\tExample:		tidisp green 1 1 -> Turn led 1 green color ON\n"};

static void doWriteGreenLed(int argc, char *argv[])
{
	int dev = -1;

	if (argc != 3 && argc != 4)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	if (argc == 4)
	{
		setLed(dev, GREEN, atoi(argv[2]), atoi(argv[3]));
	}
	else
	{
		setAllLeds(dev, GREEN, atoi(argv[2]));
	}
}

static void doWriteBlueLed(int argc, char *argv[]);
const CliCmdType CMD_WR_BLUEL =
{
	"blue",
	1,
	&doWriteBlueLed,
	"\tblue		Turn on or off one or all the blue LED's\n",
	"\tUsage:		tidisp blue <led> <0/1>\n",
	"\tUsage:		tidisp blue <led_bitmap>\n",
	"\tExample:		tidisp blue 1 1 -> Turn led 1 blue color ON\n"};

static void doWriteBlueLed(int argc, char *argv[])
{
	int dev = -1;

	if (argc != 3 && argc != 4)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	if (argc == 4)
	{
		setLed(dev, BLUE, atoi(argv[2]), atoi(argv[3]));
	}
	else
	{
		setAllLeds(dev, BLUE, atoi(argv[2]));
	}
}
#define LED_DELAY_MS 100
static void doTestLeds(int argc, char *argv[]);
const CliCmdType CMD_TEST_LED =
{
	"testl",
	1,
	&doTestLeds,
	"\ttestl		Test all LED's\n",
	"\tUsage:		tidisp testl\n",
	"",
	"\tExample:		tidisp testl -> test all LED's\n"};

static void doTestLeds(int argc, char *argv[])
{
	int dev = -1;
	int i = 0;
	UNUSED(argv);
	if (argc != 2)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	for (i = 0; i < 6; i++)
	{
		setLed(dev, RED, i + 1, 1);
		busyWait(LED_DELAY_MS);
	}
	for (i = 0; i < 6; i++)
	{
		setLed(dev, RED, i + 1, 0);
		busyWait(LED_DELAY_MS);
	}
	for (i = 0; i < 6; i++)
	{
		setLed(dev, GREEN, i + 1, 1);
		busyWait(LED_DELAY_MS);
	}
	for (i = 0; i < 6; i++)
	{
		setLed(dev, GREEN, i + 1, 0);
		busyWait(LED_DELAY_MS);
	}
	for (i = 0; i < 6; i++)
	{
		setLed(dev, BLUE, i + 1, 1);
		busyWait(LED_DELAY_MS);
	}
	for (i = 0; i < 6; i++)
	{
		setLed(dev, BLUE, i + 1, 0);
		busyWait(LED_DELAY_MS);
	}
}

static void doReadButton(int argc, char *argv[]);
const CliCmdType CMD_READ_BUTTON =
{
	"swrd",
	1,
	&doReadButton,
	"\tswrd		Read one push button or all as a bitmap\n",
	"\tUsage:		tidisp swrd <sw>\n",
	"\tUsage:		tidisp swrd\n",
	"\tExample:		tidisp swrd 1 -> Read button no 1\n"};

static void doReadButton(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[2];
	int resp = 0;
	int sw = 0;

	if (argc != 2 && argc != 3)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	resp = i2cMem8Read(dev, I2C_SW_MOM_ADD, buff, 1);
	if (FAIL == resp)
	{
		printf("Fail to read!\n");
		exit(1);
	}
	if (argc == 3)
	{
		sw = atoi(argv[2]);
		if (sw < 1 || sw > 6)
		{
			printf("Invalid sw no! [1..6]\n");
			exit(1);
		}
		if (buff[0] & (1 << (sw - 1)))
		{
			printf("1\n");
		}
		else
		{
			printf("0\n");
		}
	}
	else
	{
		printf("%d\n", (int)buff[0]);
	}
}

static void doReadButtonInt(int argc, char *argv[]);
const CliCmdType CMD_READ_BUTTON_INT =
	{
		"swird",
		1,
		&doReadButtonInt,
		"\tswird		Read one push button interrupt flag or all as a bitmap and clear them\n",
		"\tUsage:		tidisp swird <sw>\n",
		"\tUsage:		tidisp swird\n",
		"\tExample:		tidisp swird 1 -> Read button no 1 interrupt flag\n"};

static void doReadButtonInt(int argc, char *argv[])
{
	int dev = -1;
	u8 buff[2];
	int resp = 0;
	int sw = 0;

	if (argc != 2 && argc != 3)
	{
		printf("Invalid arguments number type \"tidisp -h\" for details\n");
		exit(1);
	}
	dev = doBoardInit();
	if (dev <= 0)
	{
		exit(1);
	}
	resp = i2cMem8Read(dev, I2C_SW_INT_ADD, buff, 1);
	if (FAIL == resp)
	{
		printf("Fail to read!\n");
		exit(1);
	}
	if (argc == 3)
	{
		sw = atoi(argv[2]);
		if (sw < 1 || sw > 6)
		{
			printf("Invalid sw no! [1..6]\n");
			exit(1);
		}
		if (buff[0] & (1 << (sw - 1)))
		{
			printf("1\n");
		}
		else
		{
			printf("0\n");
		}
		buff[0] &= ~ (1 << (sw - 1));
	}
	else
	{
		printf("%d\n", (int)buff[0]);
		buff[0] = 0;
	}
	resp = i2cMem8Write(dev, I2C_SW_INT_ADD, buff, 1);
	if (FAIL == resp)
	{
		printf("Fail to write!\n");
		exit(1);
	}
}

const CliCmdType *gCmdArray[] =
{
	&CMD_VERSION,
	&CMD_HELP,
	&CMD_WAR,
	&CMD_LIST,
	&CMD_BOARD,
	&CMD_WR_CMD,
	&CMD_WR_DATA,
	&CMD_WR_TXT,
	&CMD_WR_TXT_AT,
	&CMD_SET_CURSOR,
	&CMD_WR_BL,
	&CMD_WR_REDL,
	&CMD_WR_GREENL,
	&CMD_WR_BLUEL,
	&CMD_TEST_LED,
	&CMD_READ_BUTTON,
	&CMD_READ_BUTTON_INT,
	NULL}; //null terminated array of cli structure pointers

int main(int argc, char *argv[])
{
	int i = 0;

	if (argc == 1)
	{
		usage();
		return -1;
	}
	while (NULL != gCmdArray[i])
	{
		if ( (gCmdArray[i]->name != NULL) && (gCmdArray[i]->namePos < argc))
		{
			if (strcasecmp(argv[gCmdArray[i]->namePos], gCmdArray[i]->name) == 0)
			{
				gCmdArray[i]->pFunc(argc, argv);
				return 0;
			}
		}
		i++;
	}
	printf("Invalid command option\n");
	usage();

	return -1;
}
