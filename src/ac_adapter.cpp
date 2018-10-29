/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Author : David Leemans   -   http://freeunix.dyndns.org:8000/             */
/* Last update : 24-07-2008                                                  */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* ac_adapter.cpp                                                            */
/*                                                                           */
/* Copyright (C) 2008                                                        */
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

#include <cstdlib>
#include <cstring>
#include <cassert>

#include <iostream>
#include <iomanip>
#include <fstream>

#include <dirent.h>

#include "acpitool.h"
#include "ac_adapter.h"


using namespace std;


/*----------------------------------------------------------------------------------------------------------------------------*/

int Do_AC_Info(int verbose)            // wrapper around the other 2 functions //
{
    ifstream file_in;
    char *dirname, filename[4][50], str[40];
    int ac_count = 0;
    DIR *ac_dir;
    struct dirent *ac_dirent;
    char *name;
    char AC_Status[9];
 
    memset(AC_Status, '\0', 9);         // avoid rubbish in output //
      
    dirname = "/proc/acpi/ac_adapter/";    
    ac_dir = opendir(dirname);
    if(ac_dir)                             
    {
    	while((ac_dirent = readdir(ac_dir)))   
  	{
	    name = ac_dirent->d_name;
      	    // skip . and .. // 
    	    if (!strcmp (".", name) || !strcmp ("..", name)) continue;
       	    sprintf(filename[ac_count], "/proc/acpi/ac_adapter/%s/state", name);
       	    ac_count++;
       	}
    }
    closedir(ac_dir);  
    
    if(ac_count>0)                 /* if we found something usefull in /proc, use it, otherwise try /sys/...  */
    	Do_AC_Info_Proc(verbose);
    else
	Do_AC_Info_Sys();

    return 0;
}



int Do_AC_Info_Proc(int verbose)
{
    ifstream file_in;
    char *dirname, filename[4][50], str[40];
    int ac_count = 0;
    DIR *ac_dir;
    struct dirent *ac_dirent;
    char *name;
    char AC_Status[9];
 
    memset(AC_Status, '\0', 9);         // avoid rubbish in output //
      
    dirname = "/proc/acpi/ac_adapter/";    
    ac_dir = opendir(dirname);
    if(ac_dir)                             
    {
    	while((ac_dirent = readdir(ac_dir)))   
  	{
	    name = ac_dirent->d_name;
      	    // skip . and .. // 
    	    if (!strcmp (".", name) || !strcmp ("..", name)) continue;
       	    sprintf(filename[ac_count], "/proc/acpi/ac_adapter/%s/state", name);
       	    ac_count++;
       	}
    }
    else        
    {
       	if(!verbose)   
    	{
    	    printf("  AC adapter     : <not available> \n");
      	    return -1;
    	}
    	else              
    	{
    	    printf("  Could not open directory : %s \n", dirname);
    	    printf("  function Do_AC_Info() : make sure your kernel has ACPI ac_adapter support enabled. \n");
	    return -1;
    	}	
    }
    closedir(ac_dir);  
    
    if(ac_count>0)
    {
    	for(int i=0; i<ac_count; i++)
    	{
    	    file_in.open(filename[i]);
    	    if(file_in)                   
    	    {
		file_in.getline(str, 40);
    		strncpy(AC_Status, str+25, 8);
    		file_in.close();
    		printf("  AC adapter     : %s \n", AC_Status);
    	    }
    	}
    }
    else
    	printf("  AC adapter     : <not available> \n");
    return 0;
}



int Do_AC_Info_Sys()
{
    ifstream file_in;
    char filename[2][65], str[100], temp[100];
    int ac_count = 0, start = 0, findex = 0;
    DIR *ac_dir;
    char *name, *dirname;
       
    dirname = "/sys/class/power_supply/";        //find all entries in this dir 

    ac_dir = opendir(dirname);
    if(ac_dir)                     // we can read this dir //
    {
	struct dirent **namelist;
	int n;

	n = scandir(dirname, &namelist, 0, alphasort);
	if(n<0)
	    perror("scandir");
	else
	{
	    while(n--)
	    {
		name = namelist[n]->d_name;
		// skip . and .. //
		if (!strcmp (".", name) || !strcmp ("..", name)) continue;
		memset(temp, '\0', 100);
		sprintf(temp, "/sys/class/power_supply/%s/type", name);
		
		FILE *temp_fp = fopen(temp, "r");
	
	        if(temp_fp)
		{
		    memset(str, '\0', 100);
		    fscanf(temp_fp, "%s", str);
		    fclose(temp_fp);
		    if(strncmp("Mains", str, 5)==0)         /* find the one that deals with the AC adapter */
		    {
			 assert (findex < 2);
			 ac_count++;
			 sprintf(filename[findex], "/sys/class/power_supply/%s/uevent", name);
			 findex++;
		    }
		free(namelist[n]);
		}
	    }
	    free(namelist);
	}
    }      	
    else
    {
   	printf(" Function Do_AC_Info_Sys: could not read directory %s \n", dirname);
   	printf(" Make sure your kernel has ACPI AC adapter support enabled.\n");
   	return -1;
    }
    closedir(ac_dir);  
    

    if(ac_count>0)
    {
    	for(int i=0; i<ac_count; i++)            /* I don't expect to find > 1, but you never know */
    	{
    	    FILE *power_fp = fopen(filename[i], "r");
    	    if(power_fp)                   
    	    {
		for(int t=0; t<5; t++) 
		    fgets(str, 100, power_fp);              /* just skip the first 5 lines */

		memset(str, '\0', 100);
		fgets(str, 100, power_fp);  
		
		if (strlen(str)!=0)
    		{
    		    memset(temp, '\0', 100);
    		    sscanf(str, "%*[^=] %*c %s %[^\n]",temp); 
    		    
    		    /* keep this for debugging */
    		    /* printf(" from Do_AC_SYS: temp = %s \n", temp);*/
    		    
    		    if(strncmp(temp,"1",1)==0)
    			printf("  AC adapter     : online \n");
    		    else
    			printf("  AC adapter     : off-line \n");
    		}
    		else
    		    printf("  AC adapter     : <info not available> \n");
    	    }	    	
    	    else
    		printf(" Error opening file: %s \n ", filename[i]);
    	}
    }
    else
    	printf("  AC adapter     : <info not available or off-line> \n");
    
    return 0;
}
    
