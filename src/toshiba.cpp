/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Author : David Leemans   -   http://freeunix.dyndns.org:8000/             */
/* Last update : 16-10-2008                                                  */
/*---------------------------------------------------------------------------*/
/*									     */
/* toshiba.cpp								     */	
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include "toshiba.h"

using namespace std;

int Be_quiet = 1;

/*----------------------------------------------------------------------------------------------------------------------------*/

int Has_Toshiba_ACPI()
{
    ifstream file_in;
    char *filename;
    
    filename = "/proc/acpi/toshiba";
    
    file_in.open(filename);
    if (!file_in)
    	return 0;
    else
    {
	file_in.close();
        return 1;
    }
}

    
int Print_Toshiba_Info(int show_lcd, int show_video, int show_fan, int quiet)
{
    Be_quiet = quiet;
	
    cout<<" Toshiba ACPI extensions : "<<endl<<endl;
    if(show_lcd)
        Do_LCD_Info();
    cout<<endl;
    if(show_video)
        Do_Video_Info();
    cout<<endl;
    if(show_fan)
        Do_Toshiba_Fan_Info();
    cout<<endl;
    	     
    return 0;
}


int Do_Toshiba_Fan_Info()
{
    ifstream file_in;
    char *filename, str[40];
    
    filename = "/proc/acpi/toshiba/fan";
    
    file_in.open(filename);
    if (!file_in)        
    {
    	if(Be_quiet)        
    	{
    	    cout<<"<not available>"<<endl;
    	    return 0;
    	}
    	else              
    	{
    	    cout<<" Could not open file : "<<filename<<endl;
	    cout<<" function Do_Toshiba_Fan_Info : make sure your kernel has Toshiba ACPI support enabled."<<endl;
	    return -1;
    	}	
    }
    
    cout<<"   Fan : ";
    
    file_in.getline(str, 40);
    
    if(strncmp(str+25,"0",1)==0)
       cout<<" off "<<endl;
    else
    {
    	if(strncmp(str+25,"1",1)==0)
    	    cout<<" on, ";
	
	file_in.getline(str, 40);
	if(strncmp(str+25,"0",1)==0)
        cout<<"not forced "<<endl;
	else
	{
	    if(strncmp(str+25,"1",1)==0)
		cout<<"forced on "<<endl;
	}
    }     // "forced running" state is shown only if the fan IS running, makes no sense imho if it's not running anyway // 

    file_in.close();
        
    return 0;
}   


int Do_LCD_Info()
{
    ifstream file_in;
    char *filename, str[40];
    
    filename = "/proc/acpi/toshiba/lcd";
    
    file_in.open(filename);
    if (!file_in)        
    {
    	if(Be_quiet)        
    	{
	    cout<<"LCD : <not available>"<<endl;
    	    return 0;
    	}
    	else              
    	{
    	    cout<<" Could not open file : "<<filename<<endl;
	    cout<<" function Do_LCD_Info : make sure your kernel has Toshiba ACPI support enabled."<<endl;
	    return -1;
    	}	
    }
    
    cout<<"   LCD brightness level :  ";       
    file_in.getline(str, 40);
    cout<<str+25<<"  ";
    file_in.getline(str, 40);
    cout<<"["<<str+25<<" levels]"<<endl;
    file_in.close();
        
    return 0;
}


int Force_Fan(int s)
{
    ofstream file_out;
    char *filename, str[15];
    
    filename = "/proc/acpi/toshiba/fan";
    
    file_out.open(filename);
    if (!file_out)        
    {
    	cout<<" Could not open file : "<<filename<<endl;
	cout<<" You must have write access to "<<filename<<" to stop or start the fan."<<endl;
	cout<<" Or ensure yourself you are running a kernel with Toshiba ACPI support enabled."<<endl;
	return -1;
    }
    
    if(s<=0) s = 0;
    if(s>=1) s = 1;     // ensure it is 0 or 1, and nothing else //
    
    sprintf(str, "force_on:%d", s);
    
    file_out<<str;         
    file_out.close();
        
    cout<<" Fan is forced ";
    if(s) 
	cout<<"to run."<<endl;
    else
	cout<<"off."<<endl;

    return 0;
}


int Set_LCD_Level(int l)
{
    ofstream file_out;
    char *filename, str[15];
    
    filename = "/proc/acpi/toshiba/lcd";
    
    file_out.open(filename);
    if (!file_out)        
    {
    	cout<<" Could not open file : "<<filename<<endl;
	cout<<" You must have write access to "<<filename<<" to change the LCD brightness level."<<endl;
	cout<<" Or ensure yourself you are running a kernel with Toshiba ACPI support enabled."<<endl;
	return -1;
    }
    
    if(l<0) l = 0;
    if(l>7) l = 7;     //brigtness level range is 0..7, well afaik //
    
    sprintf(str, "brightness:%d", l);
    
    file_out<<str;         // this it where it happens //
    file_out.close();
    
    cout<<" LCD brightness level set to "<<l<<endl;

    return 0;
}


int Do_Video_Info()
{
    ifstream file_in;
    char *filename, str[40];
    int lcd, crt;
    
    filename = "/proc/acpi/toshiba/video";
    
    file_in.open(filename);
    if (!file_in)        
    {
    	if(Be_quiet)        
    	{
	    cout<<"<not available>"<<endl;
    	    return 0;
    	}
    	else              
    	{
    	    cout<<" Could not open file : "<<filename<<endl;
	    cout<<" function Do_Video_Info : make sure your kernel has Toshiba ACPI support enabled."<<endl;
	    return -1;
    	}	
    }
           
    cout<<"   Video output : ";  
    
    file_in.getline(str, 40);
    if(strncmp(str+25,"1",1)==0)
    {
        cout<<" LCD ";
        lcd = 1;
    }
    else
	lcd = 0;
    
    file_in.getline(str, 40);
    if(strncmp(str+25,"1",1)==0)
    {
        if(lcd)
    	    cout<<"+";
	cout<<" CRT ";
	crt = 1;  
    }
    else
	crt = 0;	
    
    file_in.getline(str, 40);
    if(strncmp(str+25,"1",1)==0)
    {
        if(lcd || crt)
    	    cout<<"+";
	cout<<" TV ";
    }	
    cout<<endl;
    
    file_in.close();
        
    return 0;
}

