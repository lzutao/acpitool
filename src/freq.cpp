/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Author : David Leemans   -   http://freeunix.dyndns.org:8000/             */
/* Last update : 13-08-2009                                                  */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* freq.cpp                                                                  */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <dirent.h>
#include <string.h>
#include "acpitool.h"
#include "freq.h"


using namespace std;


int Has_Freq()     // I suppose we have a kernel with freq scaling if we can read this file ? //
{
	int result = 0;

	FILE *freqinfo_fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");

	if(freqinfo_fp)
	{
	    result = 1;
	    fclose(freqinfo_fp);
	}
	return result;
}


int Show_Freq_Info()
{
	int min, max, cur;
	char str[300];
	
	min = 0;
	max = 0;
	cur = 0;
	
	if( Get_Min_Freq()>0)
	    min = Get_Min_Freq();
	if( Get_Max_Freq()>0)
	    max = Get_Max_Freq();
	if( Get_Current_Freq()>0)
	    cur = Get_Current_Freq();
	  
	     
	printf("  Min/Max frequency      : %d/%d MHz\n", min/1000, max/1000);
	printf("  Current frequency      : %d MHz\n", cur/1000);
	if(Get_Governor(str)==0)
	    printf("  Frequency governor     : %s \n", str);
	if(Get_Driver(str)==0)
	    printf("  Freq. scaling driver   : %s \n", str);
	return 0;
}



int Get_Min_Freq()
{
	char str[300];
	int min_freq = 0;

	FILE *freqinfo_fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq", "r");

	if(freqinfo_fp)                   
	{
		memset(str, '\0', 300);
		fscanf(freqinfo_fp, "%s", str);
		min_freq = atoi(str);
		fclose(freqinfo_fp);
                return min_freq;
	}
	else
	{
		printf("  Error : Could not open file /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq for reading.\n");
		return -1;
	}	
}



int Get_Max_Freq()
{
	char str[300];
	int max_freq = 0;

	FILE *freqinfo_fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", "r");

	if(freqinfo_fp)                   
	{
		memset(str, '\0', 300);
		fscanf(freqinfo_fp, "%s", str);
		max_freq = atoi(str);
		fclose(freqinfo_fp);
                return max_freq;
	}
	else
	{
		printf("  Error : Could not open file /sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq for reading.\n");
		return -1;
	}	
}


int Get_Current_Freq()
{
	char str[300];
	int cur_freq = 0;

	FILE *freqinfo_fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");

	if(freqinfo_fp)                   
	{
		memset(str, '\0', 300);
		fscanf(freqinfo_fp, "%s", str);
		cur_freq = atoi(str);
		fclose(freqinfo_fp);
                return cur_freq;
	}
	else
	{
		printf("  Error : Could not open file /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq for reading.\n");
		return -1;
	}	
}


int Get_Governor(char *c)
{
	char str[300];

	FILE *freqinfo_fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", "r");

	if(freqinfo_fp)                   
	{
		memset(str, '\0', 300);
		fscanf(freqinfo_fp, "%s", str);
		if (strlen(str)>0)
		    strcpy(c, str);
		fclose(freqinfo_fp);
                return 0;
	}
	else
	{
		printf("  Error : Could not open file /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor for reading.\n");
		return -1;
	}	
}


int Get_Driver(char *c)
{
	char str[300];

	FILE *freqinfo_fp = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_driver", "r");

	if(freqinfo_fp)                   
	{
		memset(str, '\0', 300);
		fscanf(freqinfo_fp, "%s", str);
		if (strlen(str)>0)
		    strcpy(c, str);
		fclose(freqinfo_fp);
                return 0;
	}
	else
	{
		printf("  Error : Could not open file /sys/devices/system/cpu/cpu0/cpufreq/scaling_driver for reading.\n");
		return -1;
	}	
}

