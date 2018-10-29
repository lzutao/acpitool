/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Authors : David Leemans   -   http://freeunix.dyndns.org:8000/            */
/*           Nicolas Weyland  -  http://ufoalien.bug.ch/                     */
/*									     */
/* Last update : 23-10-2005                                                  */
/*---------------------------------------------------------------------------*/
/*									     */
/* asus.h								     */	
/* Support for Asus ACPI extensions					     */
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

#ifndef ASUS_H
#define ASUS_H

int Has_Asus_ACPI();

int Print_Asus_Info(const int show_asus_lcd, const int show_asus_video, const int show_asus_info, const int quiet);

int Do_Asus_LCD_Info();

int Set_Asus_LCD_Level(int z);

int Set_Asus_LCD_State(int o);

int Set_MLed(int state);

int Set_WLed(int state);

int Do_Asus_Video_Info();

int Do_Asus_Info();

#endif
