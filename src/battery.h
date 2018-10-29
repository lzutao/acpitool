/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Author : David Leemans   -   http://freeunix.dyndns.org:8000/             */
/* Last update : 21-07-2008                                                  */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* battery.h                                                                 */
/*                                                                           */		
/* Copyright (C) 2004,2005,2006,2007,2008  David Leemans		     */	
/* <davidleemans AT scarlet DOT be>	                                     */
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

#ifndef BATTERY_H
#define BATTERY_H

int Get_Battery_Info_from_Proc(const int bat_nr, Battery_Info *bat_info, int verbose);
int Get_Battery_Info_from_Sys(const int bat_nr, Battery_Info *batt_info, int verbose);

int Count_Batteries_ProcFS();
int Count_Batteries_SysFS();

int Do_Battery_Stuff(int show_empty, int bat_info_level, int quiet);

#endif
