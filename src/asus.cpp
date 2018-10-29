/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Authors : David Leemans   -   http://freeunix.dyndns.org:8000/            */
/*           Nicolas Weyland  -  http://ufoalien.bug.ch/                     */
/*									     */
/* Last update : 06-11-2007                                                  */
/*---------------------------------------------------------------------------*/
/*									     */
/* asus.cpp								     */	
/* Support for ASUS ACPI extensions					     */
/*									     */		
/* Copyright (C) 2005	Nicolas Weyland <ufoalien@weyland.ch>		     */
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "asus.h"

using namespace std;

int Be_asus_quiet = 1;

/*----------------------------------------------------------------------------------------------------------------------------*/

int Has_Asus_ACPI()
{
    ifstream file_in;
    char *filename;
    
    filename = "/proc/acpi/asus";
    
    file_in.open(filename);
    if (!file_in)
    	return 0;
    else
    {
	file_in.close();
        return 1;
    }
}

    
int Print_Asus_Info(int show_asus_lcd, int show_asus_video, int show_asus_info, int quiet)
{
    Be_asus_quiet = quiet;
	
    cout<<" Asus ACPI extensions : "<<endl<<endl;
    if(show_asus_lcd)
        Do_Asus_LCD_Info();
    cout<<endl;
    if(show_asus_video)
        Do_Asus_Video_Info();
    cout<<endl;
    if(show_asus_info)
	Do_Asus_Info();
    cout<<endl;

    return 0;
}


int Do_Asus_LCD_Info()
{
    ifstream file_in;
    char *filename, lcdb[40];
    
    filename = "/proc/acpi/asus/brn";
    
    file_in.open(filename);
    if (!file_in)        
    {
    	if(Be_asus_quiet)        
    	{
	    cout<<"LCD : <not available>"<<endl;
    	    return 0;
    	}
    	else              
    	{
    	    cout<<" Could not open file : "<<filename<<endl;
	    cout<<" function Do_Asus_LCD_Info : make sure your kernel has Asus ACPI support enabled."<<endl;
	    return -1;
    	}	
    }
    
    cout<<"   LCD brightness level :  ";
    
    file_in>>lcdb;
    cout<<lcdb;
    cout<<"\n";
    file_in.close();
        
    return 0;
}


int Set_Asus_LCD_Level(int z)
{
    ofstream file_out;
    char *filename;
          
    filename = "/proc/acpi/asus/brn";
    
    file_out.open(filename);
    if (!file_out)        
    {
    	cout<<" Could not open file : "<<filename<<endl;
	cout<<" You must have write access to "<<filename<<" to change the LCD brightness level."<<endl;
	cout<<" Or ensure yourself you are running a kernel with Asus ACPI support enabled."<<endl;
	return -1;
    }
    
    if(z<0) z = 0;
    if(z>15) z = 15;     //brigtness level range is 0..15 //
    
    file_out<<z;         // this it where it happens //
    file_out.close();
    
    cout<<" LCD brightness level set to "<<z<<endl;

    return 0;
}


int Do_Asus_Video_Info()
{
    ifstream file_in;
    char *filename;
    int type;
    filename = "/proc/acpi/asus/disp";
    
    file_in.open(filename);
    if (!file_in)        
    {
    	if(Be_asus_quiet)        
    	{
	    cout<<"<not available>"<<endl;
    	    return 0;
    	}
    	else              
    	{
    	    cout<<" Could not open file : "<<filename<<endl;
	    cout<<" function Do_Asus_Video_Info : make sure your kernel has Asus ACPI support enabled."<<endl;
	    return -1;
    	}	
    }
           
    cout<<"   Video output : ";  
    
    file_in>>type;

    switch(type)
    {
	case 0 : cout<<"no display"<<endl;
		 break;
	case 1 : cout<<"LCD only"<<endl;
	         break;
	case 2 : cout<<"CRT only"<<endl;
	         break;
	case 3 : cout<<"LCD + CRT"<<endl;
	         break;
	case 4 : cout<<"TV-out only"<<endl;
		 break;
	case 5 : cout<<"LCD + TV-out"<<endl;
	         break;
	case 6 : cout<<"CRT + TV-out"<<endl;
	         break;
	case 7 : cout<<"LCD + CRT + TV-out"<<endl;
	         break;
	default : cout<<"Undefined video out option. Check this! Video type = "<<type<<endl;
		  break;
    }	 

    file_in.close();
        
    return 0;
}


int Do_Asus_Info()
{
    ifstream file_in;
    char *filename, info[255];
    
    filename = "/proc/acpi/asus/info";
    
    file_in.open(filename);
    if (!file_in)        
    {
    	if(Be_asus_quiet)        
    	{
	    cout<<"Info : <not available>"<<endl;
    	    return 0;
    	}
    	else              
    	{
    	    cout<<" Could not open file : "<<filename<<endl;
	    cout<<" function Do_Asus_Info : make sure your kernel has Asus ACPI support enabled."<<endl;
	    return -1;
    	}	
    }

    cout<<"   DSTD Version : \n";  
    
	file_in.getline(info, 100);
	cout<<"   "<<info;
	cout<<"\n";
	file_in.getline(info, 100);
	cout<<"   "<<info;
	cout<<"\n";
	file_in.getline(info, 100);
	cout<<"   "<<info;
	cout<<"\n";
	file_in.getline(info, 100);
	cout<<"   "<<info;
	cout<<"\n";
	file_in.getline(info, 100);
	cout<<"   "<<info;
	cout<<"\n";
	file_in.getline(info, 100);
	cout<<"   "<<info;
	cout<<"\n";
	file_in.getline(info, 100);
	cout<<"   "<<info;
	cout<<"\n";
	file_in.getline(info, 100);
	cout<<"   "<<info;
	cout<<"\n";
	file_in.getline(info, 100);
	cout<<"   "<<info;
	cout<<"\n";
	file_in.getline(info, 100);
	cout<<"   "<<info;
	cout<<"\n";
	file_in.getline(info, 100);
	cout<<"   "<<info;
	cout<<"\n"<<endl;
	
    file_in.close();
        
    return 0;
}


int Set_Asus_LCD_State(int o)
{
    ofstream file_out;
    char *filename;
    filename = "/proc/acpi/asus/lcd";
    
    file_out.open(filename);
    if (!file_out)        
    {
    	cout<<" Could not open file : "<<filename<<endl;
	cout<<" You must have write access to "<<filename<<" to switch the LCD display on/off."<<endl;
	cout<<" Or ensure yourself you are running a kernel with Asus ACPI support enabled."<<endl;
	return -1;
    }
    
    if(o<0) o = 0;
    if(o>1) o = 1;    
    
    file_out<<o;
    file_out.close();

    return 0;
}


int Set_MLed(int state)
{
    ofstream file_out;
    char *filename;
    filename = "/proc/acpi/asus/mled";
    
    file_out.open(filename);
    if (!file_out)        
    {
    	cout<<" Could not open file : "<<filename<<endl;
	cout<<" You must have write access to "<<filename<<" to switch the mail led on/off."<<endl;
	cout<<" Or ensure yourself you are running a kernel with Asus ACPI support enabled."<<endl;
	return -1;
    }
    
    if(state<0) state = 0;
    if(state>1) state = 1;    
    
    file_out<<state;
    file_out.close();

    return 0;
}


int Set_WLed(int state)
{
    ofstream file_out;
    char *filename;
    filename = "/proc/acpi/asus/wled";
    
    file_out.open(filename);
    if (!file_out)        
    {
    	cout<<" Could not open file : "<<filename<<endl;
	cout<<" You must have write access to "<<filename<<" to switch the wireless led on/off."<<endl;
	cout<<" Or ensure yourself you are running a kernel with Asus ACPI support enabled."<<endl;
	return -1;
    }
    
    if(state<0) state = 0;
    if(state>1) state = 1;    
    
    file_out<<state;
    file_out.close();

    return 0;
}

