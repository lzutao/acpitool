/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Author : David Leemans   -   http://freeunix.dyndns.org:8000/             */
/* Last update : 16-10-2008                                                  */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* acpitool.cpp                                                              */
/*                                                                           */
/* Copyright (C) 2004, 2005, 2006, 2007, 2008                                */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// C stdlib
#include <cstdlib>
#include <cstring>
#include <cassert>

// C++ stdlib
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

// system libs
#include <dirent.h>

#include "acpitool.h"
#include "ac_adapter.h"
#include "thinkpad.h"
#include "toshiba.h"
#include "cpu.h"
#include "battery.h"


using namespace std;

#define App_Version  "AcpiTool v0.5.1"
#define Release_Date "13-Aug-2009"

int Kernel_24 = 0, Kernel_26 = 0;
int Use_Sys = 0, Use_Proc = 0;

/*----------------------------------------------------------------------------------------------------------------------------*/

int Has_ACPI(char *c)
{
    ifstream file_in, file2_in;
    char *filename, *filename2, str[50];
    
    filename = "/proc/acpi/info";
    filename2 = "/sys/module/acpi/parameters/acpica_version";
    
    file_in.open(filename);
    file2_in.open(filename2);
    
    if (!file_in && !file2_in)
    {
    	cout<<"  Could not open any of these files : "<<filename<<", "<<filename2<<endl;
	cout<<"  Make sure your kernel has ACPI support enabled."<<endl;
	strcpy(c, "n.a");
	return 0;
    }	
    else
    {
	if(file_in)
	{
	    file_in.getline(str, 50);
	    file_in.close();
	    strncpy(c, str+25,8);
	    Use_Proc = 1;
	    
	    // cout<<" Got ACPI version from: "<<filename<<endl; //
	    
	    return 1;
	}
	if(file2_in)
	{
	    file2_in.getline(str, 50);
	    file2_in.close();
	    strncpy(c, str,8);
	    Use_Sys = 1;
	    
	    // cout<<" Got ACPI version from: "<<filename2<<endl; //
	    
	    return 1;
	}
    }
}



    
int Print_ACPI_Info(int show_ac, int show_therm, int show_trip, int show_fan, int show_batteries, int show_empty, int show_version, int show_cpu, int show_wake, int e_set, int info_level, int verbose)
{
    if(show_version)
	Do_SysVersion_Info(verbose);
    if(show_batteries)
    	Do_Battery_Stuff(show_empty, info_level, verbose);
    if(e_set) cout<<endl;	
    if(show_ac)
	Do_AC_Info(verbose);
    if(show_fan)
    	Do_Fan_Info(verbose);
    if(e_set) cout<<endl;
    if(show_cpu)
    	Show_CPU_Info();
    if(e_set) cout<<endl;
    if(show_therm)
    	Do_Thermal_Info(show_trip, verbose);
    if(e_set) cout<<endl;
    if(show_wake)
    	Show_WakeUp_Devices(verbose); 
    return 0;
}



int Get_Kernel_Version(char *c, int verbose)
{
    ifstream file_in;
    char *filename, str[20];
    
    filename = "/proc/sys/kernel/osrelease";
    
    file_in.open(filename);
    if (!file_in)
    {
    	if(!verbose)
    	{
    	    strcpy(c, "<n.a>");
    	    return 0;
    	}
    	else
    	{
    	    cout<<"  Could not open file : "<<filename<<endl;
	    return -1;
    	}	
    }
       
    file_in.getline(str, 16);
    file_in.close();
    
    strcpy(c, str);
    
    return 0;
}


int Do_SysVersion_Info(int verbose)
{
    char Acpi_Version[10], Kernel_Version[15];
 
    memset(Acpi_Version, '\0', 10); 
    memset(Kernel_Version, '\0', 15);   

    Get_Kernel_Version(Kernel_Version, verbose);
    Has_ACPI(Acpi_Version);

    cout<<"  Kernel version : "<<Kernel_Version<<"   -    ACPI version : "<<Acpi_Version<<endl;
    cout<<"  -----------------------------------------------------------"<<endl;
    
    return 0;
}  


int Set_Kernel_Version()
{
    ifstream file_in;
    char *filename, str[10];
    
    filename = "/proc/sys/kernel/osrelease";
    
    file_in.open(filename);
    if (!file_in)
    {
    	cout<<"  Could not open file : "<<filename<<endl;
	cout<<"  function Set_Kernel_Version() : can't set kernel version, bailing out"<<endl;
	return -1;
    }	
       
    file_in.getline(str, 9);
    file_in.close();
    
    if(strncmp(str,"2.4",3)==0)
    {
	Kernel_24 = 1;           
	Kernel_26 = 0;
    }
        
    if(strncmp(str,"2.6",3)==0)
    {
	Kernel_24 = 0;           
	Kernel_26 = 1;
    }    
    
    return 0;
}


int Do_Thermal_Info(const int show_trip, int verbose)
{
    ifstream file_in, file_in2;
    char *dirname, str[120];
    vector<string> filename;
    
    DIR *thermal_dir;
    char *name;
    char Temperature[5], State[5];
 
    memset(Temperature, '\0', 5);     
    memset(State, '\0', 5);
    
    dirname = "/proc/acpi/thermal_zone/";   
    thermal_dir = opendir(dirname);
    if(thermal_dir)                    
    {
	struct dirent **namelist;
	int n;

	n = scandir(dirname, &namelist, 0, alphasort);
	if(n<0)
	    perror("scandir");
	else
	{
	    char tmp_fname[256];
	    while(n--)
	    {
		name = namelist[n]->d_name;
		// skip . and .. //
		if (!strcmp (".", name) || !strcmp ("..", name))
		{
		    // do nothing
		}
		else
		{
		    snprintf(tmp_fname, sizeof (tmp_fname) - 1, "/proc/acpi/thermal_zone/%s/state", name);
		    filename.push_back(tmp_fname);

		    snprintf(tmp_fname, sizeof (tmp_fname) - 1, "/proc/acpi/thermal_zone/%s/temperature", name);
		    filename.push_back(tmp_fname);

		    snprintf(tmp_fname, sizeof (tmp_fname) - 1, "/proc/acpi/thermal_zone/%s/trip_points", name);
		    filename.push_back(tmp_fname);
		}
		free(namelist[n]);
	    }
	    free(namelist);
	}
    }      	
    else        
    {
       	if(!verbose)    
    	{
	    cout<<"  Thermal info   : <not available>"<<endl;
      	    return 0;
    	}
    	else            
    	{
    	    cout<<"  Could not open directory : "<<dirname<<endl;
    	    cout<<"  function Do_Thermal_Info() : make sure your kernel has /proc/acpi/thermal_zone support enabled."<<endl;
	    return -1;
    	}	
    }
    closedir(thermal_dir);  

    int tz_count = filename.size();
    if(tz_count>0)     
    {
    	
	tz_count--;
	int tz=0;	
	
    	for(int i=tz_count; i>1; i-=3)     //process entries in reverse order //
    	{
    	    file_in.open(filename[i-2].c_str());
    	    if(file_in)                   
    	    {
    		file_in.getline(str, 120);
    		strncpy(State, str+25, 5); State[4] = '\0';
    		cout<<"  Thermal zone "<<++tz<<" : "<<State<<", ";
    	    }
	    file_in.close();
	    file_in2.open(filename[i-1].c_str());
    	    if(file_in2)                   
    	    {
		file_in2.getline(str, 120);
    		strncpy(Temperature, str+25, 5); Temperature[4] = '\0';
    		cout<<Temperature<<endl;
	    }
	    file_in2.close();
	    if(show_trip)           // show trip_points ? //
	    {
		FILE *fp = fopen(filename[i].c_str(), "r");           
		
		/* using classic FILE and fopen here because ifstream.open fails mysteriously when asigned to multiple files ???? */
		
		if(fp)
		{
		    cout<<"  Trip points : "<<endl;
		    cout<<"  ------------- "<<endl;
		    while(!feof(fp))
		    {
			/* fscanf(fp, "%s", str); */
			memset(str, '\0', 120);
			fgets(str, 120, fp);
			if (strlen(str)!=0)  
			  cout<<"  "<<str;       /* avoid printing empty line */
		    }
		    cout<<endl;	
		}
		else
		   cout<<"  Trip points : <error opening file>"<<endl;
		fclose(fp);	    
	    }
	}
    }
    else   
	cout<<"  Thermal info   : <not available>"<<endl;
    return 0;
}


int Do_Fan_Info(int verbose)
{
	ifstream file_in;
	char *dirname, filename[4][50], str[40];
	int fan_count = 0;
	DIR *fan_dir;
	struct dirent *fan_dirent;
	char *name;
	char FAN_Status[9];

	if (Has_Thinkpad_ACPI())
	{
		Do_Thinkpad_Fan_Info();
		return 0;
	}

	if (Has_Toshiba_ACPI())
	{
		Do_Toshiba_Fan_Info();
		return 0;
	}

	memset(FAN_Status, '\0', 9);         // avoid rubbish in output //

	dirname = "/proc/acpi/fan/";    
	fan_dir = opendir(dirname);
	if(fan_dir)                             
	{
		while((fan_dirent = readdir(fan_dir)))   
		{
			name = fan_dirent->d_name;
			// skip . and .. // 
			if (!strcmp (".", name) || !strcmp ("..", name)) continue;
			sprintf(filename[fan_count], "/proc/acpi/fan/%s/state", name);
			fan_count++;
		}
	}
	else        
	{
		if(!verbose)   
		{
			cout<<"  Fan            : <not available>"<<endl;
			return 0;
		}
		else              
		{
			cout<<"  Could not open directory : "<<dirname<<endl;
			cout<<"  function Do_Fan_Info() : make sure your kernel has ACPI fan support enabled."<<endl;
			return -1;
		}	
	}
	closedir(fan_dir);  

	if(fan_count>0)
	{
		for(int i=0; i<fan_count; i++)
		{
			file_in.open(filename[i]);
			if(file_in)                   
			{
				file_in.getline(str, 40);
				strncpy(FAN_Status, str+25, 8);
				file_in.close();
				cout<<"  Fan            : "<<FAN_Status<<endl;
			}
		}
	}
	else
		cout<<"  Fan            : <not available>"<<endl;
	return 0;
}




int Show_WakeUp_Devices(int verbose)
{
    ifstream file_in;
    char *filename, str[40];
    
    filename = "/proc/acpi/wakeup";
    
    file_in.open(filename);
    if (!file_in)        
    {
    	if(!verbose)        
    	{
	    cout<<"  wakeup devices : <not available>"<<endl;
    	    return 0;
    	}
    	else              
    	{
    	    cout<<"  Function Show_WakeUp_Devices : could not open file "<<filename<<endl;
	    cout<<"  Make sure your kernel has /proc/acpi/wakeup support enabled."<<endl;
	    return -1;
    	}	
    }
    else
    {
	file_in.getline(str, 40);           // first line are just headers //
	cout<<"   "<<str<<endl;
	cout<<"  ---------------------------------------"<<endl;
        int t = 1;
	while(!file_in.eof())
	{
	    file_in.getline(str, 40);
	    if (strlen(str)!=0)                     // avoid printing last empty line //
	    {
		cout<<"  "<<t<<". "<<str<<endl;
		t++;
	    }
	}	
	file_in.close();
    }
    
    return 0;
}


int Toggle_WakeUp_Device(const int Device, int verbose)
{
    ifstream file_in;
    ofstream file_out;
    char *filename, str[50];
    int index = 1;
    char Name[25][5];            // 25 should be enough I guess, I have only 9 so far //
    
    filename = "/proc/acpi/wakeup";
    
    file_in.open(filename);
    if (!file_in)        
    {
    	if(!verbose)        
    	{
	    cout<<"  wakeup devices : <not available>"<<endl;
    	    return 0;
    	}
    	else              
    	{
    	    cout<<"  Function Toggle_WakeUp_Device : could not open file "<<filename<<endl;
	    cout<<"  Make sure your kernel has /proc/acpi/wakeup support enabled."<<endl;
	    return -1;
    	}	
    }
    
    file_in.getline(str, 50);             // first line are just headers //
    while(!file_in.eof())                 // count all devices and store their names//
    {
        file_in.getline(str, 50);
        if(strlen(str)!=0)                // avoid empty last line //
        {
	    memset(Name[index], '\0', 5);
	    strncpy(Name[index], str, 4);
	    index++;
	}
    }	
    file_in.close();
	
    index--;                                      // last device = index - 1 //
	
    if( (Device>=1) && (Device<=index) )          // check this first !! //
    {
        file_out.open(filename);
        if (!file_out)
        {
	    cout<<"  Function Toggle_WakeUp_Device : could not open file : "<<filename<<". \n"
	    "  You must have write access to "<<filename<<" to enable or disable a wakeup device. \n"
	    "  Check the permissions on "<<filename<<" or run acpitool as root."<<endl;
	    return -1;
	}
	else file_out<<Name[Device];     // change status //
	cout<<"  Changed status for wakeup device #"<<Device<<" ("<<Name[Device]<<")"<<endl<<endl;
	file_out.close();
    }
    else
    {
        cout<<" Function Toggle_Wakeup_Device : invalid device number "<<Device<<". \n"
	      " Run 'acpitool -w' to get valid device numbers ."<<endl;
	return -1;
    }
    
    return 0;
}



int Do_Suspend(int State)                     //put system in sleep state 3 or 4 (standby) //
{
    ofstream file_out;
    char *filename, *str;
    
    Set_Kernel_Version();
    
    filename = "/proc/acpi/sleep";   // nice default values //
    str = "3";
    
    switch(State)
    {
    	case 3 : if(Kernel_26)
		 {
		    filename = "/sys/power/state";
		    str = "mem";
		 }
		 break;
	case 4 : if(Kernel_24) str = "4";
		 if(Kernel_26)
		 {
		    filename = "/sys/power/state";
		    str = "disk";
		 }
		 break;
	default : cout<<" Function Do_Suspend : unsupported sleep state, fix this"<<endl;
		  return -1; break;
    }
    
    file_out.open(filename);
    if (!file_out)
    {
    	cout<<" Function Do_Suspend : could not open file : "<<filename<<". \n"
	      " You must have write access to "<<filename<<" to suspend your computer."<<endl;
	return -1;
    }	
       
    file_out<<str;     // sleep tight //
    
    return 0;
}


int Show_Help()
{
    cout<<" Usage: acpitool [option] . . . \n"
    " Shows ACPI information from the /proc/acpi filesystem, like battery status,\n"
    " temperature, or ac power. Can also suspend your machine (if supported).\n"
    "\n"
    "   -a, --ac_adapter   AC adapter information\n"    //Added by ufoalien:
    "   -A, --Asus         show supported Asus ACPI extensions (LCD brightness level, video out routing DSTD/acpi4asus info)\n"
    "   -b                 battery status, available batteries only\n"
    "   -B, --battery      battery status, all info on all battery entries\n"
    "   -c, --cpu          CPU information (type, speed, cache size, frequency scaling, c-states, . . .)\n"
    "   -e                 show just about everything\n"
    "   -f, --fan          show fan status\n"
    "   -F x               force fan on (x=1) or switch back to auto mode (x=0). (Toshiba only)\n"
    "   -h, --help         show this help screen\n"
    "   -j                 eject ultrabay device (Thinkpad only)\n"
    "   -l x               set LCD brightness level to x, where x is 0..7 (Toshiba and Thinkpad only)\n"
    "   -m x               switch the mail led on (x=1) or off (x=0) (Asus only)\n"
    "   -n x               switch the wireless led on (x=1) or off (x=0). (Asus only)\n"
    "   -o x               set LCD on (x=1) or off (x=0). (Asus only)\n"       //added by ufoalien 
    "   -s, --suspend      suspend to memory (sleep state S3), if supported\n"
    "   -S                 suspend to disk (sleep state S4), if supported\n"
    "   -t, --thermal      thermal information, including trip_points\n"
    "   -T, --Toshiba      show supported Toshiba ACPI extensions (LCD brightness level, video out routing, fan status)\n"
    "   -v                 be more verbose (more detailed error messages, only usefull combined with other options)\n"
    "   -V, --version      show application version number and release date\n"
    "   -w, --wakeup       show wakeup capable devices\n"
    "   -W x               enable/disable wakeup capable device x. The x can be seen when invoking -w first.\n"
    "   -z x               set Asus LCD brightness level to x, where x is 0..15 (Asus only).\n"       //added by ufoalien 
    "\n"
    " If invoked without options, acpitool displays information about available batteries,\n"
    " AC adapter and thermal information.\n\n"
    " For more info, type man acpitool at the prompt.\n\n";
    
    cout<<" "<<App_Version<<", "<<"released "<<Release_Date<<endl;
    cout<<" Homepage: http://freeunix.dyndns.org:8000/site2/acpitool.shtml \n\n";
    return 0;
}


int Show_App_Version()
{
    cout<<" "<<App_Version<<", "<<"released "<<Release_Date<<endl;
    return 0;
}

int Usage()
{
    cout<<" You invoked acpitool with an unknown option. Type 'acpitool -h' or 'acpitool --help' for info. "<<endl;
    return 0;
}
