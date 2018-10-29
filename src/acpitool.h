/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Author : David Leemans   -   http://freeunix.dyndns.org:8000/             */
/* Last update : 22-07-2008                                                  */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* acpitool.h                                                                */
/*                                                                           */		
/* Copyright (C) 2004,2005,2006,2007,2008                                    */
/* David Leemans <davidleemans AT scarlet DOT be>	                     */
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

#ifndef ACPITOOL_H
#define APCITOOL_H

struct Battery_Info {
    int  Battery_Present;
    char Charging_State[12];
    char Remaining_Cap[10];
    char Design_Cap[10];
    char LastFull_Cap[10];
    char Present_Rate[10];
    char Technology[13];
    char Model[13];
    char Serial[13];
    char Bat_Type[13];
    char Voltage_Now[13];
};



int Print_ACPI_Info(int show_ac, int show_therm, int show_trip, int show_fan, int show_batteries, int show_empty, int show_version, int show_cpu, int show_wake, int e_set, int info_level, int quiet);

int Show_CPU_Info();

int Get_Kernel_Version(char *c, int verbose);
int Set_Kernel_Version();

int Do_Thermal_Info(const int show_trip, int verbose);

int Do_AC_Info(int verbose);

int Do_Fan_Info(int verbose);

int Do_Battery_Stuff(int show_empty, int bat_info_level, int quiet);

int Has_ACPI(char *c);

int Do_Suspend(int State);

int Show_Help();

int Show_App_Version();

int Do_SysVersion_Info(int verbose);

int Show_WakeUp_Devices(int verbose);

int Toggle_WakeUp_Device(const int Device, int verbose);

int Usage();

#endif
