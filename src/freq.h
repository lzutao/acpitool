/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Author : David Leemans   -   http://freeunix.dyndns.org:8000/             */
/* Last update : 06-02-2007                                                  */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* freq.h                                                                    */
/*                                                                           */		
/* Copyright (C) 2007  David Leemans <davidleemans AT scarlet DOT be>	     */
/*                                                                           */
/* This program is free software; you can redistribute it and/or modify      */
/* it under the terms of the GNU General Public License as published by      */
/* the Free Software Foundation; either version 2 of the License, or         */
/* (at your option) any later version.                                       */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*---------------------------------------------------------------------------*/

#ifndef FREQ_H
#define FREQ_H


/* check for presence of freq scaling  */
int Has_Freq();


/* returns the lowest supported frequency */
int Get_Min_Freq();


/* returns the highest supported frequency */
int Get_Max_Freq();


/* returns the current cpu frequency */
int Get_Current_Freq();


/* prints out all information, wrapper around the other functions */
int Show_Freq_Info();


/* returns the frequency governor in use */
int Get_Governor(char *c);


/* returns the frequency driver */
int Get_Driver(char *c);

#endif
