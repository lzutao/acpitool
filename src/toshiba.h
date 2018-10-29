/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Author : David Leemans   -   http://freeunix.dyndns.org:8088/             */
/* Last update : 21-08-2004                                                  */
/*---------------------------------------------------------------------------*/
/*									     */
/* toshiba.h								     */	
/* Support for Toshiba ACPI extensions					     */
/*									     */		
/* Copyright (C) 2004  David Leemans <davidleemans AT tiscali DOT be>	     */
/*									     */
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

#ifndef TOSHIBA_H
#define TOSHIBA_H

int Has_Toshiba_ACPI();

int Print_Toshiba_Info(int show_lcd, int show_video, int show_fan, int quiet);

int Do_LCD_Info();

int Set_LCD_Level(int l);

int Do_Video_Info();

int Force_Fan(int s);

int Do_Toshiba_Fan_Info();
    	     
#endif
