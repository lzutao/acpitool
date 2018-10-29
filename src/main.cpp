/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Author : David Leemans   -   http://freeunix.dyndns.org:8000/             */
/* Last update : 24-03-2008                                                  */
/*---------------------------------------------------------------------------*/
/*					                       		     */
/* main.cpp                             				     */
/*				        				     */
/* Copyright (C) 2004  David Leemans <davidleemans AT tiscali DOT be>	     */
/*						                             */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <unistd.h>
#include <getopt.h>
#include "acpitool.h"
#include "toshiba.h"
#include "asus.h"
#include "thinkpad.h"

using namespace std;

int main(int argc, char *argv[])
{
    int show_empty = 0;
    int show_version = 0;
    int show_batteries = 0;
    int show_ac = 0;
    int show_fan = 0;
    int show_tfan = 0;
    int show_cpu = 0;
    int show_wake = 0;
    int show_lcd = 0;
    int show_video = 0;
    int show_asus_lcd = 0;
    int show_asus_video = 0;
    int show_asus_info = 0;
    int show_therm = 0;
    int e_set = 0;
    int show_trip_points = 0;
    int info_level = 0;       // verbosity level //
    int verbose = 0;            // do not print error messages , just show "not available" //
    int rc = 0;
    char Acpi_Version[10];
    
    if(!Has_ACPI(Acpi_Version))
    {
    	Show_App_Version();
	return -1;
    }
    
    
    int c;
    
    struct option longopts[] =
    {
	{"version", 0, 0, 'V'},
	{"suspend", 0, 0, 's'},
	{"Toshiba", 0, 0, 'T'},
	{"battery", 0, 0, 'B'},
	{"ac_adapter", 0, 0, 'a'},
	{"cpu", 0, 0, 'c'},
	{"fan", 0, 0, 'f'},
	{"Asus", 0, 0, 'A'},	
	{"help", 0, 0, 'h'},
	{"wakeup", 0, 0, 'w'},
	{"thermal", 0, 0, 't'},
	{NULL, 0, 0, 0}
    };
    extern int opterr;

    opterr = 0;

    if(argc>1)    
    {
    	while ((c = getopt_long(argc, argv, "aAbcBefF:hjl:m:n:o:sStTvVwW:z:", longopts, NULL)) != -1)
	{
	    switch (c)
	    {
		case 'a': 
		    show_ac = 1;
		    break;
		case 'A': 
		    if(Has_Asus_ACPI())
		    {
			show_asus_lcd = 1;
			show_asus_video = 1;
			show_asus_info = 1;
			Print_Asus_Info(show_asus_lcd, show_asus_video, show_asus_info, verbose);
		    }
		    else
			cout<<"Sorry, but no Asus ACPI extensions were found on this system. "<<endl;
		    break;
		case 'b': 
		    show_batteries = 1;
		    break;
		case 'c': 
		    show_cpu = 1;
		    break;
		case 'B': 
		    show_empty = 1;
		    show_batteries = 1;
		    info_level = 1;
		    break;
		case 'e': 
		    show_version = 1;       
		    show_batteries = 1;
		    show_ac = 1;
		    show_empty = 1;
		    show_fan = 1;
		    show_therm = 1;
		    info_level = 1;
		    show_cpu = 1;
		    show_wake = 1;
		    show_trip_points = 1;
		    e_set = 1;
		    break;
		case 'f':
		    show_fan = 1;
		    break;
		case 'F': 
		    if(Has_Toshiba_ACPI())
			Force_Fan(atoi(optarg));
		    else
			cout<<"Forcing the fan of/off is only supported on Toshiba laptops. \n"
			"No Toshiba ACPI extensions were found. "<<endl; 
		    break;
		case 'h': 
		    Show_Help();
		    break;
		case 'j': 
		    Eject_Thinkpad_Bay();
		    break;
		case 'l': 
		    if(Has_Toshiba_ACPI())
			Set_LCD_Level(atoi(optarg)); 
		    else
			if (Has_Thinkpad_ACPI())
			    Set_Thinkpad_LCD_Level(atoi(optarg));
			else
			    cout<<"Changing LCD brightness level is only supported on Toshiba or Thinkpad laptops." << endl;
		    break;	      
		case 'o': 
		    if(Has_Asus_ACPI())
			Set_Asus_LCD_State(atoi(optarg));
		    else
			cout<<"Sorry, but no Asus ACPI extensions were found on this system. "<<endl;
		    break;
		case 'm': 
		    if(Has_Asus_ACPI())
			Set_MLed(atoi(optarg));
		    else
			cout<<"Switching the mail led on/off is only supported on Asus laptops. \n"
			"No Asus ACPI extensions were found. "<<endl; 
		    break;
		case 'n': 
		    if(Has_Asus_ACPI())
			Set_WLed(atoi(optarg));
		    else
			cout<<"Switching the wireless led on/off is only supported on Asus laptops. \n"
			"No Asus ACPI extensions were found. "<<endl;
		    break;	      		  
		case 't': 
		    show_therm = 1;
		    show_trip_points = 1;
		    break;
		case 'T': 
		    if(Has_Toshiba_ACPI())
		    {
			show_tfan = 1;
			show_lcd = 1;
			show_video = 1;
			Print_Toshiba_Info(show_lcd, show_video, show_tfan, verbose);
		    }
		    else
			cout<<"Sorry, but no Toshiba ACPI extensions were found on this system. "<<endl;
		    break;
		case 'V':
		    Show_App_Version();
		    break;
		case 'v': 
		    verbose = 1;
		    break;
		case 's': 
		    Do_Suspend(3);
		    break;      
		case 'S': 
		    Do_Suspend(4);
		    break;
		case 'w': 
		    show_wake = 1;
		    break;
		case 'W': 
		    rc = Toggle_WakeUp_Device(atoi(optarg), verbose);
		    if(rc==0) Show_WakeUp_Devices(verbose);         //show wakeup devices if status change succeeded //
		    break;
		case 'z': 
		    if(Has_Asus_ACPI())
			Set_Asus_LCD_Level(atoi(optarg));    // convert option to int //
		    else
			cout<<"Changing Asus LCD brightness level is only supported on Asus laptops. \n"
			"No Asus ACPI extensions were found. "<<endl;
		    break;	      
		default:
		    Usage();
		    break;
	    }
	}
	Print_ACPI_Info(show_ac, show_therm, show_trip_points, show_fan, show_batteries, show_empty, show_version, show_cpu, show_wake, e_set, info_level, verbose); 
    }
    else
    {          
   	show_batteries = 1;              // default values, acpitool called without options //
   	show_ac = 1;
   	show_therm = 1;
	verbose = 0;
  	Print_ACPI_Info(show_ac, show_therm, show_trip_points, show_fan, show_batteries, show_empty, show_version, show_cpu, show_wake, e_set, info_level, verbose); 
    }
    return 0;
}
