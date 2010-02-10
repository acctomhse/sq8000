//*****************************************************************************
//
// File Name    : 'buffer.c'
// Title        : Multipurpose byte buffer structure and methods
// Author       : Pascal Stang - Copyright (C) 2001-2002
// Created      : 9/23/2001
// Revised      : 9/23/2001
// Version      : 1.0
// Target MCU   : any
// Editor Tabs  : 3
//
// This code is distributed under the GNU Public License
//              which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <buffer.h>
#include <global.h>

// global variables

// access routines
void bufferInit(cBuffer* buffer, unsigned char *start, unsigned short size)
{
	// set start pointer of the buffer
	buffer->dataptr    = start;
	buffer->size       = size;
	// initialize index and length
	buffer->dataindex  = 0;
	buffer->datalength = 0;
}

unsigned char  bufferGetFromFront(cBuffer* buffer)
{
	unsigned char data = 0;

	// check to see if there's data in the buffer
	if (buffer->datalength)
	{
		// enter critical section
//		disable_interrupts();
		// get the first character from buffer
		data = buffer->dataptr[buffer->dataindex];
		// move index down and decrement length
		buffer->dataindex++;
		if (buffer->dataindex >= buffer->size)
		{
			buffer->dataindex %= buffer->size;
		}
		buffer->datalength--;
		// leave critical section
//		enable_irq();
	}
	// return
	return data;
}

unsigned char bufferGetAtIndex(cBuffer* buffer, unsigned short index)
{
	// return character at index in buffer
	return buffer->dataptr[(buffer->dataindex+index)%(buffer->size)];
}

unsigned char bufferAddToEnd(cBuffer* buffer, unsigned char data)
{
	// make sure the buffer has room
	if (buffer->datalength < buffer->size)
	{
		// enter critical section
//		disable_interrupts();
		// save data byte at end of buffer
		buffer->dataptr[(buffer->dataindex + buffer->datalength) % buffer->size] = data;
		// increment the length
		buffer->datalength++;
		// leave critical section
//		enable_irq();
		// return success
		return -1;
	}
	else return 0;
}

unsigned char bufferIsNotFull(cBuffer* buffer)
{
	// check to see if the buffer has room
	// return true if there is room
	return (buffer->datalength < buffer->size);
}

void bufferFlush(cBuffer* buffer)
{
	// enter critical section
//	disable_interrupts();
	// flush contents of the buffer
	buffer->datalength = 0;
	// leave critical section
//	enable_irq();
}
