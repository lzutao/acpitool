/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Author : David Leemans   -   http://freeunix.dyndns.org:8000/             */
/* Last update : 16-10-2008                                                  */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/* battery.cpp                                                               */
/* Get battery info from /proc/acpi/battery or /sys/class/power_supply       */
/*                                                                           */
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
#include "thinkpad.h"
#include "toshiba.h"
#include "cpu.h"
#include "battery.h"

using namespace std;

// remaining battery capacity (in %) = (remaining capacity / last full battery capacity) x 100 //
// remaining battery life (in hh:mm:ss) = (remaining capacity / present battery rate ) //
   
int Do_Battery_Stuff(int show_empty, int info_level, int verbose)
{
    Battery_Info *Batt_Info[2];
    int Bat_Nr;
    float Remaining_Percentage, Remaining_Time, Battery_Left_Percent;
    int Time_In_Seconds = 0;
    int Hours = 0;
    int Minutes = 0;
    int Show_Time = 0;
    int Precision = 0;
    int Is_Charging = 0;
    int Is_Discharging = 0;
    int AC_Online = 0;
    int Present_Batteries = 0;
    int Use_Proc = 0, Use_Sys = 0;
    
    int Nr_Batteries = Count_Batteries_ProcFS();          /* check in /proc/acpi first   */
    /* printf("Number of battery entries found in /proc/acpi : %d \n", Nr_Batteries); */
    
    if(Nr_Batteries > 0) 
    {
	Use_Proc = 1;
	Use_Sys  = 0;
    }
    else                                               /*  nothing in /proc/acpi, try /sys/class    */
    {
	Nr_Batteries = Count_Batteries_SysFS();
	/* printf("Number of battery entries found in /sys/class/power_supply : %d \n", Nr_Batteries); */
	if(Nr_Batteries > 0) 
	{
	    Use_Sys  = 1;
	    Use_Proc = 0;
	}
    }
    
    // printf("\n Number of battery entries found: %d \n\n", Nr_Batteries); 

    if(Nr_Batteries>0)
    {
        for(int i=0; i<Nr_Batteries; i++)
    	{
	    Bat_Nr = i+1;
	    Batt_Info[i] = new Battery_Info;
	    		
	    Batt_Info[i]->Battery_Present = 0;
	    memset(Batt_Info[i]->Remaining_Cap, '\0', 10);
	    memset(Batt_Info[i]->Design_Cap, '\0', 10);
	    memset(Batt_Info[i]->LastFull_Cap, '\0', 10);
	    memset(Batt_Info[i]->Present_Rate, '\0', 10);
	    memset(Batt_Info[i]->Charging_State, '\0', 12);
	    memset(Batt_Info[i]->Technology, '\0', 13);
	    memset(Batt_Info[i]->Model, '\0', 13);
	    memset(Batt_Info[i]->Serial, '\0', 13);
	    memset(Batt_Info[i]->Bat_Type, '\0', 13); 
	    memset(Batt_Info[i]->Voltage_Now, '\0', 13); 
	    
	    // initialize all struct members to blanks --> avoid rubbish in output //
			
	    if(Use_Proc==1)
		Get_Battery_Info_from_Proc(Bat_Nr, Batt_Info[i], verbose);
	    else
	    	Get_Battery_Info_from_Sys(Bat_Nr, Batt_Info[i], verbose);
	
	    /* printf("\n -------------------------------------------\n");
	    printf("\n Remaining_Cap: %s \n", Batt_Info[i]->Remaining_Cap);
	    printf("\n Design_Cap: %s \n",Batt_Info[i]->Design_Cap);
	    printf("\n LastFull_Cap: %s \n",Batt_Info[i]->LastFull_Cap);
	    printf("\n Present_Rate: %s \n",Batt_Info[i]->Present_Rate);
	    printf("\n Charging_State: %s \n",Batt_Info[i]->Charging_State);
	    printf("\n Technology: %s \n",Batt_Info[i]->Technology);
	    printf("\n Model: %s \n",Batt_Info[i]->Model);
	    printf("\n Serial: %s \n",Batt_Info[i]->Serial);
	    printf("\n Bat_Type: %s \n",Batt_Info[i]->Bat_Type); 
	    printf("\n Voltage_Now: %s \n",Batt_Info[i]->Voltage_Now);  */
	
	
	    	
	    Show_Time = atoi(Batt_Info[i]->Present_Rate);   
	    // avoid division by 0 if system is on AC power and battery is full and thus not charging //

	    Is_Charging = 0;      // determine whether battery is charging or not //
	    
	    switch(Batt_Info[i]->Battery_Present)
	    {
		case 1 : 
		{	
	    	    Present_Batteries++;
		    Remaining_Percentage = float(atoi(Batt_Info[i]->Remaining_Cap)) / float(atoi(Batt_Info[i]->LastFull_Cap)) * 100.0;
		    
		    /* from Alan Pope : some broken Dell batteries report a remaining capacity bigger
		       than their last full capacity or their design capacity. This led acpitool to report
		       stuff like 107% battery percentage. To avoid this silliness, I added next if statement */  
		      
		    if( Remaining_Percentage > 100.0)
			Remaining_Percentage = 100.0;
                        	    
		    if( int(Remaining_Percentage) < 10)
            		Precision = 3;
            	    else
            		Precision = 4;
            	    
		    if( strncmp(Batt_Info[i]->Charging_State,"char",4)==0 ) 
		    {
			Is_Charging = 1;
		    }
		    else
		    {
			if(strncmp(Batt_Info[i]->Charging_State,"disch",5)==0) Is_Discharging = 1;
		    }
		    		    
	    	    if(Show_Time)      // calculate remaining or charging time only if present battery rate != 0 //
	    	    {
			if(Is_Charging)
			  Remaining_Time = (float(atoi(Batt_Info[i]->LastFull_Cap)) - float(atoi(Batt_Info[i]->Remaining_Cap))) / float(atoi(Batt_Info[i]->Present_Rate)); 
			else
			  Remaining_Time = float(atoi(Batt_Info[i]->Remaining_Cap)) / float(atoi(Batt_Info[i]->Present_Rate)); 
			// this represents hours //
			
			/* same Dell bug as above */
			if (Remaining_Time < 0)
                            Remaining_Time = 0.0;
		
			Time_In_Seconds = int(Remaining_Time * 3600.0);
			Hours = Time_In_Seconds / 3600;
			Time_In_Seconds = Time_In_Seconds - (Hours * 3600);
			Minutes = Time_In_Seconds / 60;
			Time_In_Seconds = Time_In_Seconds - (Minutes * 60);
	    	    }
		    
		    if(atoi(Batt_Info[i]->Design_Cap) > 0)
		      Battery_Left_Percent = float(atoi(Batt_Info[i]->LastFull_Cap)) / float(atoi(Batt_Info[i]->Design_Cap)) * 100.0;
		    else
		      Battery_Left_Percent = -1.0;
		      			  
	    
	    	    switch(info_level)
	    	    {
			case 0 : cout.precision(Precision);
			         cout<<"  Battery #"<<Bat_Nr<<"     : "<<Batt_Info[i]->Charging_State<<", "<<showpoint<<Remaining_Percentage<<"%";
			         if(Show_Time)
			    	    cout<<", "<<setfill('0')<<setw(2)<<Hours<<":"<<setfill('0')<<setw(2)<<Minutes<<":"\
				    <<setfill('0')<<setw(2)<<Time_In_Seconds;
			         cout<<endl; 
			         break;
				 
			case 1 : cout<<"  Battery #"<<Bat_Nr<<"     : present"<<endl;
			         cout<<"    Remaining capacity : "<<Batt_Info[i]->Remaining_Cap<<", ";
			         cout.precision(Precision);
			         cout<<showpoint<<Remaining_Percentage<<"%";
				 
				 if(Show_Time)
				    cout<<", "<<setfill('0')<<setw(2)<<Hours<<":"<<setfill('0')<<setw(2)<<Minutes<<":"\
				    <<setfill('0')<<setw(2)<<Time_In_Seconds;
				 cout<<endl;
				 
				 cout<<"    Design capacity    : "<<Batt_Info[i]->Design_Cap<<endl;
				 cout<<"    Last full capacity : "<<Batt_Info[i]->LastFull_Cap;
				 
				 if(Battery_Left_Percent<100.0)
				 {
				    cout<<", "<<Battery_Left_Percent<<"% of design capacity"<<endl;
				    cout<<"    Capacity loss      : "<<(100.0 - Battery_Left_Percent)<<"%"<<endl;
				 }    
				 else
				   cout<<endl;
				 
				 cout<<"    Present rate       : "<<Batt_Info[i]->Present_Rate<<endl;
			         cout<<"    Charging state     : "<<Batt_Info[i]->Charging_State<<endl;
			         printf("    Battery type       : %s \n", Batt_Info[i]->Technology);
			         
			         //<<", "<<Batt_Info[i]->Bat_Type<<endl;//
				 
				 if (strlen(Batt_Info[i]->Model)!=0)
			            cout<<"    Model number       : "<<Batt_Info[i]->Model<<endl;
			         if (strlen(Batt_Info[i]->Serial)!=0)
				    cout<<"    Serial number      : "<<Batt_Info[i]->Serial<<endl;
				 /* on some batteries, these values are empty */     
				    
			         break;
				 
			default : cout<<"  Undefined info level, fix this. Info level : "<<info_level<<endl;
			          break;
	    	    }
	    	    break;	
		}
	    case 0 : if(show_empty) cout<<"  Battery #"<<Bat_Nr<<"     : slot empty"<<endl;
		     break;
	    case 2 : cout<<"  Battery         : <not available>"<<endl;
		     break;
	    case 3 : cout<<"  Battery         : <not available>"<<endl;
		     break;
	    default : cout<<"  Undefined present value, fix this. Value : "<<Batt_Info[i]->Battery_Present<<endl;
	              break;
	    }    
	
	}
	
	if(Is_Discharging)
	    AC_Online = 0;    // assume AC must be  offline if at least 1 battery is discharging //
	else
	    AC_Online = 1;    // assume (possibly dangerous) AC is online if all batteries report being charging or charged //
	
	/* printf(" Number of batteries present : %d \n", Present_Batteries); 
	printf(" AC_Online =  %d \n", AC_Online); */
	
	// calculate total battery time left if more than 1 battery is present and AC is offline //
	if(Present_Batteries>1 && AC_Online==0)  
	{
	    int Total_Remaining_Cap, Total_LastFull_Cap, Total_Rate;
	    
	    Total_LastFull_Cap = 0;
	    Total_Remaining_Cap = 0;
	    Total_Rate = 0;
	    
	    for(int t=0; t<Present_Batteries; t++)
	    {
		Total_Remaining_Cap = Total_Remaining_Cap + (atoi(Batt_Info[t]->Remaining_Cap));
		Total_LastFull_Cap = Total_LastFull_Cap + (atoi(Batt_Info[t]->LastFull_Cap));
		Total_Rate = Total_Rate + (atoi(Batt_Info[t]->Present_Rate));
	    }  
	    
	    if(Total_Rate > 0)    // unlikely to happen if we got here but avoid possible division by 0 //
	    {
		Remaining_Percentage = float(Total_Remaining_Cap) / float(Total_LastFull_Cap) * 100.0;
		
		if( Remaining_Percentage > 100.0) Remaining_Percentage = 100.0;
                        	    
		if( int(Remaining_Percentage) < 10)
            	    Precision = 3;
		else
		    Precision = 4;
		
		Remaining_Time = float(Total_Remaining_Cap) / float(Total_Rate); 
		Time_In_Seconds = int(Remaining_Time * 3600.0);
		Hours = Time_In_Seconds / 3600;
		Time_In_Seconds = Time_In_Seconds - (Hours * 3600);
		Minutes = Time_In_Seconds / 60;
		Time_In_Seconds = Time_In_Seconds - (Minutes * 60);
		
		cout.precision(Precision);
		cout<<"  All batteries  : "<<showpoint<<Remaining_Percentage<<"%";
		cout<<", "<<setfill('0')<<setw(2)<<Hours<<":"<<setfill('0')<<setw(2)<<Minutes<<":"
		<<setfill('0')<<setw(2)<<Time_In_Seconds<<endl; 
	    }
	}
	for(int t=0; t<Nr_Batteries; t++) delete Batt_Info[t];
    }
    else
    {
	if(Nr_Batteries==0)   //this would apply to a regular desktop with acpi support //
	{
	    cout<<"  Battery status : <not available>"<<endl;
	    return 0;
	}
	if(Nr_Batteries<0)
	{
	    if(!verbose)
	    {
		cout<<"  Battery status : <error reading info>"<<endl;
		return 0;
	    }
	    else
	    {
		cout<<"  Battery status : <error reading info>"<<endl;
		cout<<"  Function Count_Batteries returned : "<<Nr_Batteries<<endl;
		return -1;
	    }
	}
    }
    return 0;
}




int Get_Battery_Info_from_Proc(const int bat_nr, Battery_Info *bat_info, int verbose)
{
    ifstream file_in;
    char filename[4][65], str[100], temp[100];
    int bat_count = 0, start = 0, findex = 0;
    DIR *battery_dir;
    char *name, *dirname;
       
    dirname = "/proc/acpi/battery/";    //find all entries in this dir 
    // dirname = "/home/david/dropzone/data/devel/acpi/battery/";    // keep this for testing //    

    battery_dir = opendir(dirname);
    if(battery_dir)                     // we can read this dir //
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
		if (!strcmp (".", name) || !strcmp ("..", name))
		{
		    // do nothing
		}
		else
		{
		    assert (findex < 4);
		    sprintf(filename[findex], "/proc/acpi/battery/%s/info", name);
		    findex++;
		
		    assert (findex < 4);
		    sprintf(filename[findex], "/proc/acpi/battery/%s/state", name);
		    findex++;
		
		    bat_count++;
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
   	    bat_info->Battery_Present = 2;     // 2 represents error value //
   	    return 0;
   	}
   	else
   	{
   	    cout<<" Function Get_Battery_Info : Could not read directory : /proc/acpi/battery/"<<endl;
   	    cout<<" Make sure your kernel has ACPI battery support enabled."<<endl;
   	    return -1;
   	}
    }
    closedir(battery_dir);  
    
    //we found all dir entries, now process them //
    switch(bat_nr)       // select battery first //
    {
    	case 1 : if(bat_count==1)
		    start = 0;
		 if(bat_count==2)
		    start = 2;    
    		 break;
    	case 2 : start = 0;
    		 break;      
    	default : cout<<"Invalid battery number, fix this"<<endl;
    		  return -1;
    		  break;
    } //NOTE : scandir returns entries in reverse order //
  
    // first get battery presence from 1st file//
            
    
    if(bat_count>0)
    {
	file_in.open(filename[start]);
	
    	if (!file_in)
    	{
	    cout<<" Function Get_Battery_Info : could not open file "<<filename[start]<<endl;
	    cout<<" Make sure your kernel has ACPI battery support enabled or check presence of a battery."<<endl;
	    return -1;
    	}
    	
    	file_in.getline(str, 100);
    	strncpy(temp, str+25, 4);
    	if(strncmp(temp,"yes",3)==0)
    	    bat_info->Battery_Present = 1;               //yes, we have a battery //
    	else
    	{
    	    bat_info->Battery_Present = 0;
    	    return 0;                  //bail out if battery is not present //
    	}
    	
	// then get the design capacity //
    	file_in.getline(str, 100);
    	strncpy(bat_info->Design_Cap, str+25, 9);
	
    	// then get the last full capacity //
    	file_in.getline(str, 100);
    	strncpy(bat_info->LastFull_Cap, str+25, 9);
	
	if (strncmp(bat_info->LastFull_Cap,"unknown",7)==0)
	{
    	    bat_info->Battery_Present = 0;
    	    return 0;                  //bail out if battery is not present //
    	}
	/* some Dell laptops seem to report a 2nd battery as being present, while it is NOT, but then report the 
	   last full capacity and other values as "unknown". This sucks, Mr Dell ! If the last full capacity
	   is unknown, the battery is considered as being unavailable. */  
	
    
    	// then get the technology //
    	file_in.getline(str, 100);
    	strncpy(bat_info->Technology, str+25, 12);
    
    	// then get the model number //
    	for(int t=0; t<5; t++)
	file_in.getline(str, 100);            //skip 5 lines //
    	file_in.getline(str, 100);
    	strncpy(bat_info->Model, str+25, 12);
    
    	// then get the serial number //
    	file_in.getline(str, 100);
    	strncpy(bat_info->Serial, str+25, 12);
    
    	// then get the battery type //
    	file_in.getline(str, 100);
    	strncpy(bat_info->Bat_Type, str+25, 12);
    
    	file_in.close();
    	
    	// then open 2nd file = /proc/acpi/.../state //

    	file_in.open(filename[start+1]);
	if (!file_in)
    	{
	    cout<<" Function Get_Battery_Info : could not open file "<<filename[start+1]<<endl;
	    cout<<" Make sure your kernel has ACPI battery support enabled or check presence of a battery."<<endl;
	    return -1;
    	}
	
    	// then get the charging state //
    	file_in.getline(str, 100); file_in.getline(str, 100);     // skip first 2 lines //
    	file_in.getline(str, 100);
    	strncpy(bat_info->Charging_State, str+25, 12);
	if (strncmp(bat_info->Charging_State,"unknown",7)==0) strncpy(bat_info->Charging_State, "charged",7);
	/* on older kernels, like 2.4.22, the charging state is reported as "unknown", whereas in recent kernels
	   this was changed to "charged". */  

    	// then get the charging rate //
    	file_in.getline(str, 100);
    	strncpy(bat_info->Present_Rate, str+25, 9);
	if (strncmp(bat_info->Charging_State,"charged",7)==0)
	{
	    if (strncmp(bat_info->Present_Rate, "unknown",7)==0) strncpy(bat_info->Present_Rate, "0      ",7);
	}    
	/* some batteries report the present rate as "unknown", even when they report the battery as being charged.
	   If the battery is charged, the rate should be 0 */     
	  

    	// then get the remaining capacity //
    	file_in.getline(str, 100);
    	strncpy(bat_info->Remaining_Cap, str+25, 9);
    
    	file_in.close();
	}
	else      // battery dir is readable but empty : only . and .. at most //
	    bat_info->Battery_Present = 3;   
	
	return 0;
}



int Get_Battery_Info_from_Sys(const int bat_nr, Battery_Info *batt_info, int verbose)
{
    ifstream file_in;
    char filename[6][65], str[100], temp[100];
    int bat_count = 0, start = 0, findex = 0;
    DIR *battery_dir;
    char *name, *dirname;
       
    dirname = "/sys/class/power_supply/";        //find all entries in this dir 

    battery_dir = opendir(dirname);
    if(battery_dir)                     // we can read this dir //
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
		if (!strcmp (".", name) || !strcmp ("..", name))
		{
		    free(namelist[n]);
		    continue;
		}

		memset(temp, '\0', 100);
		sprintf(temp, "/sys/class/power_supply/%s/type", name);
		
		
		FILE *temp_fp = fopen(temp, "r");
	
	        if(temp_fp)
		{
		    memset(str, '\0', 100);
		    fscanf(temp_fp, "%s", str);
		    
		    fclose(temp_fp);
		    if(strncmp("Batt", str, 4)==0)
		    {
			 bat_count++;
			 assert (findex < 6);
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
	if(!verbose)
   	{
   	    batt_info->Battery_Present = 2;     // 2 represents error value //
   	    return 0;
   	}
   	else
   	{
   	    printf(" Function Get_Battery_Info_from_Sys: could not read directory %s \n", dirname);
   	    printf(" Make sure your kernel has ACPI battery support enabled.\n");
   	    return -1;
   	}
    }
    closedir(battery_dir);  
    
    /*  let's see if this messy indexing with files actually gives us the right files and numbers  */
    
    //we found all dir entries, now process them //
    switch(bat_nr)       // select battery first //
    {
    	case 1 : if(bat_count==1)
		    start = 0;
		 if(bat_count==2)
		    start = 1;    
    		 break;
    	case 2 : start = 0;
    		 break;      
    	default : cout<<"Invalid battery number, fix this"<<endl;
    		  return -1;
    		  break;
    } //NOTE : scandir returns entries in reverse order //
  
  
    // printf("From Get_Bat_Info, start= %d. \n ", start);  //
    
    if(bat_count>0)
    {
	FILE *power_fp = fopen(filename[start], "r");
	if(!power_fp)
	{
	    printf(" Function Get_Battery_Info_from_Sys : could not open file %s . \n", filename[start]);
	    printf(" Make sure your kernel has ACPI battery support enabled or check presence of a battery. \n");
	    return -1;
    	}
    	
    	memset(str, '\0', 100);
	for(int t=0; t<5; t++)
	    fgets(str, 100, power_fp);            /* skip first 5 lines */
    	
    	/* get battery status (full, charging, ...) */
    	memset(str, '\0', 100);
    	fgets(str, 100, power_fp);
    	if (strlen(str)>0)
    	{
    	    memset(temp, '\0', 100);
    	    sscanf(str, "%*[^=] %*c %s %[^\n]",temp); 
    	    strncpy(batt_info->Charging_State, temp, 12);
    	}   
	
	    
	/* get battery presence (0 or 1) */    
	memset(str, '\0', 100);
	fgets(str, 100, power_fp);   
	if (strlen(str)>0)
	{
	    memset(temp, '\0', 100);
	    strncpy(temp, str+21, 1);
    	    if(strncmp(temp,"1",1)==0)
    	        batt_info->Battery_Present = 1;               /* yes, we have a battery */
    	    else
    	    {
    		batt_info->Battery_Present = 0;
    		printf(" Battery is not present, bailing out. \n");
    		return 0;                                    /* bail out if battery is not present */
    	    }
    	}
    	    
    	    
    	/* get technology */    
    	memset(str, '\0', 100);
	fgets(str, 100, power_fp);   
	if (strlen(str)>0)
    	{
    	    memset(temp, '\0', 100);
    	    sscanf(str, "%*[^=] %*c %s %[^\n]",temp); 
    	    strncpy(batt_info->Technology, temp, 12);
    	} 
    	else
    	    strncpy(batt_info->Technology, "unknown", 7);
    	    
    	    
    	    
    	//printf(" \n bat_info_tech = %s \n\n ",  batt_info->Technology);    
    	
    	

	fgets(str, 100, power_fp);    	/* skip 1 line */	


	/* get voltage_now */    
	memset(str, '\0', 100);
	fgets(str, 100, power_fp);
	if (strlen(str)>0)
    	{
    	    memset(temp, '\0', 100);
    	    sscanf(str, "%*[^=] %*c %s %[^\n]",temp); 
    	    strncpy(batt_info->Voltage_Now, temp, 12);
    	}    
    	else
    	    strncpy(batt_info->Voltage_Now, "unknown", 7);

    	
	/* get current_now, which I believe is the charging rate ? */    
	memset(str, '\0', 100);
	fgets(str, 100, power_fp);
	if (strlen(str)>0)
    	{
    	    memset(temp, '\0', 100);
    	    sscanf(str, "%*[^=] %*c %s %[^\n]",temp); 
    	    strncpy(batt_info->Present_Rate, temp, 12);
    	}       
    	else
    	    strncpy(batt_info->Present_Rate, "unknown", 7);

	
	/* get charge_full_design */    
	memset(str, '\0', 100);
	fgets(str, 100, power_fp);
	if (strlen(str)>0)
    	{
    	    memset(temp, '\0', 100);
    	    sscanf(str, "%*[^=] %*c %s %[^\n]",temp); 
    	    strncpy(batt_info->Design_Cap, temp, 12);
    	}          
    	else
    	    strncpy(batt_info->Design_Cap, "unknown", 7);
    	    
    	    
    	//printf(" \n bat_info_design_cap = %s \n ",  batt_info->Design_Cap);    


	/* get charge_full, which is the last full capacity I guess ? */    
	memset(str, '\0', 100);
	fgets(str, 100, power_fp);   
	if (strlen(str)>0)
    	{
    	    memset(temp, '\0', 100);
    	    sscanf(str, "%*[^=] %*c %s %[^\n]",temp); 
    	    strncpy(batt_info->LastFull_Cap, temp, 12);
    	}       
    	else
    	    strncpy(batt_info->LastFull_Cap, "unknown", 7);


	//printf(" \n bat_info_lastfull_cap = %s \n\n ",  batt_info->LastFull_Cap); 


	/* get charge_now */    
	memset(str, '\0', 100);
	fgets(str, 100, power_fp);   
	if (strlen(str)>0)
    	{
    	    memset(temp, '\0', 100);
    	    sscanf(str, "%*[^=] %*c %s %[^\n]",temp); 
    	    strncpy(batt_info->Remaining_Cap, temp, 12);
    	}       
    	else
    	    strncpy(batt_info->Remaining_Cap, "unknown", 7);

	//printf(" \n bat_info_remaining_cap = %s \n\n ",  batt_info->Remaining_Cap); 


	/* get model_name */  
	memset(str, '\0', 100);  
	fgets(str, 100, power_fp);   
	if (strlen(str)>0)
    	{
    	    memset(temp, '\0', 100);
	    strncpy(temp, str+24, 12);         // use strncpy here because sscanf chokes on blanks in this one ? //

    	    memset(str, '\0', 100);
    	    sscanf(temp, "%[^\n]", str);       // strip trailing \n, fucks up output //
    	    
    	    strncpy(batt_info->Model, str, 12);
    	}       
    	else
    	    strncpy(batt_info->Model, "unknown", 7);

	fgets(str, 100, power_fp);   
	
	/* get serial */    
	memset(str, '\0', 100);
	fgets(str, 100, power_fp);   
	if (strlen(str)!=0)
    	{
    	    memset(temp, '\0', 100);
    	    sscanf(str, "%*[^=] %*c %s %[^\n]",temp); 
    	    strncpy(batt_info->Serial, temp, 12);
    	}       
    	else
    	    strncpy(batt_info->Serial, "unknown", 7);
    
    	fclose(power_fp);
	}
    else      // battery dir is readable but empty : only . and .. at most //
        batt_info->Battery_Present = 3;   
    return 0;
}



int Count_Batteries_ProcFS()
{
    DIR *battery_dir;
    char *name, *dirname;
    int t = 0;
       
    dirname = "/proc/acpi/battery/";    
    battery_dir = opendir(dirname);
    if(battery_dir)                  
    {
	struct dirent **namelist;
	int n = 0;

	n = scandir(dirname, &namelist, 0, alphasort);
	if(n<0)
	  t = -1;
	else
	{
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
		    t++;
		}
		free(namelist[n]);
	    }
	    free(namelist);
	}
    }
    else
    {
	t = 0;
    }
    closedir(battery_dir);
    return t;
}



int Count_Batteries_SysFS()
{
    DIR *battery_dir;
    char filename[4][65], str[100];
    char *name, *dirname;
    int findex = 0, batcount = 0;
    
    /* Since kernel 2.6.24, the acpi interface is gradually being moved from /proc/acpi/... to the sys filesystem. */
    /* So, if we find no batteries in the proc fs, look here before bailing out.                                   */
       
    dirname = "/sys/class/power_supply/";    
    battery_dir = opendir(dirname);
    if(battery_dir)                 
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
		
		assert (findex < 4);
		sprintf(filename[findex], "/sys/class/power_supply/%s/type", name);
		findex++;
		
		free(namelist[n]);
	    }
	    free(namelist);
	}
    }      	
    else
    {
   	return -1;             /* can't read dir, so nothing in here */
    }
    closedir(battery_dir);  

    /* We enumerated all entries in /sys/class/powersupply/, but the AC adapter lives here too. */
    /* So, run em trough a loop to find out how many of these entries really are batteries.     */ 
    
    
    if(findex>0)
    {
	for(int t=0; t<findex; t++)
	{
	    FILE *powertype_fp = fopen(filename[t], "r");
	    
	    if(powertype_fp)
	    {
		memset(str, '\0', 100);
		fscanf(powertype_fp, "%s", str);
		fclose(powertype_fp);
		if(strncmp("Batt", str, 4)==0) batcount++;
	    }
        }
    }
    
    return batcount;
}

