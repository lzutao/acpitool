/*---------------------------------------------------------------------------*/
/* Program to get/set some ACPI settings                                     */
/* Author : David Leemans   -   http://freeunix.dyndns.org:8000/             */
/* Last update : 13-08-2009                                                  */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* cpu.cpp                                                                   */
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
#include <cassert>

#include <iostream>
#include <iomanip>
#include <fstream>

#include <dirent.h>

#include "acpitool.h"
#include "cpu.h"
#include "freq.h"


using namespace std;


int Show_CPU_Info()
{
	ifstream file_in, file2_in;
	char *dirname, filename[25][70], str[300], temp[130];
	int cpu_count = 0, findex = 0, thrott = 0;
	long int max_cstate = 0, cst_cnt = 0, cst_usage_abs[8],
		 cst_usage_notC0 = 0;
	long long int bm_activity;
	float cst_usage_rel[8];
	DIR *cpu_dir;
	char *name;

	FILE *cpuinfo_fp = fopen("/proc/cpuinfo", "r");

	if(cpuinfo_fp)                   
	{
		for(int t=0; t<4; t++) fgets(str, 299, cpuinfo_fp);          // skip 4 lines //
		fgets(str, 299, cpuinfo_fp);
		memset(temp, '\0', 130);			      // use sscanf to extract what we need :  //
		sscanf(str, "%*[^:] %*s %[^\n]",temp);                // ignore all up till :,  then ignore the :, then use what's left //
		printf("  CPU type               : %s \n", temp);     // till the newline char //
		fgets(str, 299, cpuinfo_fp);           
		fgets(str, 299, cpuinfo_fp);
		
		if(Has_Freq())
		    Show_Freq_Info();          // show freq scaling info if we have it //
		else
		{
		    memset(temp, '\0', 130);           
		    sscanf(str, "%*[^:] %*s %s",temp);                    // ignore all up till :,  then ignore the :, then use what's left //
		    printf("  CPU speed              : %s MHz \n", temp);
		}
		
		fgets(str, 299, cpuinfo_fp);
		memset(temp, '\0', 130);
		sscanf(str, "%*[^:] %*s %s",temp);  
		printf("  Cache size             : %s KB\n", temp);
		
		for(int t=0; t<5; t++) fgets(str, 299, cpuinfo_fp);    // we can skip at least 5 lines to reach Bogomips line//
		
		while(!feof(cpuinfo_fp))                      // Run loop until Bogomips line is found //
		{                                             // Need this generic aproach: the bogomips location is //
		    fgets(str, 299, cpuinfo_fp);              // different for Amd64, Opteron, Core Duo, P4 with Ht, ... //
		    memset(temp, '\0', 130);
		    if(strncmp(str,"bogo",4)==0)
		    {
			sscanf(str, "%*[^:] %*s %s",temp);  
			printf("  Bogomips               : %s \n", temp);
		    }
		}    
		fclose(cpuinfo_fp);
	}
	else
	{
		printf("  Error : Could not open file /proc/cpuinfo.\n");
		return -1;
	}	


	dirname = "/proc/acpi/processor/";    //find all entries in this dir //
	cpu_dir = opendir(dirname);
	if(cpu_dir)                     // we can read this dir //
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
				    assert (findex < 25);
				    sprintf(filename[findex], "/proc/acpi/processor/%s/info", name);
				    findex++;

				    assert (findex < 25);
				    sprintf(filename[findex], "/proc/acpi/processor/%s/power", name);
				    findex++;

				    assert (findex < 25);
				    sprintf(filename[findex], "/proc/acpi/processor/%s/throttling", name);
				    findex++;
				    cpu_count++;
				}
				free(namelist[n]);
			}
			free(namelist);
		}
	}      	
	else
	{
		cout<<"  Function Show_CPU_Info : could not read directory "<<dirname<<endl;
		cout<<"  Make sure your kernel has ACPI processor support enabled."<<endl;
		return -1;
	}
	closedir(cpu_dir);  


	if(cpu_count>0)     
	{
		findex--;

		if(cpu_count>1) cout<<endl<<"  # of CPU's found       : "<<cpu_count<<endl<<endl;

		for(int i=findex; i>0; i-=3)     //process entries in reverse order //
		{
			file_in.open(filename[i-2]);      // acpi/processor/%s/info
			
			//file_in.open("/home/david/dropzone/devel/acpi/processor/CPU0/info");//
			
			if(file_in)                   
			{
				file_in.getline(str, 90); // processor id
				memset(temp, '\0', 130);
				strncpy(temp, str+25, 5);
				cout<<"  Processor ID           : "<<temp<<endl;

				file_in.getline(str, 100); // acpi id
				file_in.getline(str, 100); // bus mastering control
				memset(temp, '\0', 130);
				strncpy(temp, str+25, 5);
				cout<<"  Bus mastering control  : "<<temp<<endl;

				file_in.getline(str, 100); // power management
				memset(temp, '\0', 130);
				strncpy(temp, str+25, 5);
				cout<<"  Power management       : "<<temp<<endl;

				file_in.getline(str, 100); // throttling control
				memset(temp, '\0', 130);
				strncpy(temp, str+25, 5);
				cout<<"  Throttling control     : "<<temp<<endl;
				if(strncmp(temp,"yes",3)==0)
					thrott = 1;		//yes, this CPU reports it can do power throttling //

				file_in.getline(str, 100);	// limit interface (beware: on linux2.4
											// this line yields "perf mgmt")
				memset(temp, '\0', 130);
				strncpy(temp, str+25, 5);
				cout<<"  Limit interface        : "<<temp<<endl;

				file_in.close();
			}
			else
			{
				cout<<"  could not open file "<<filename[i-2]<<endl;
			}	

			FILE *power_fp = fopen(filename[i-1], "r");   // acpi/processor/%s/power         
		
			/* Aug 4, 2006: using classic FILE and fopen here because ifstream.open fails mysteriously 
			   when assigned to multiple files ???? WTF? */
		
			if(power_fp)                   
			{
				fgets(str, 100, power_fp); // active state
				memset(temp, '\0', 80);
				strncpy(temp, str+25, 5);
				cout<<"  Active C-state         : "<<temp;

		  	   // next code added by Niko Ehrenfeuchter (University of  Freiburg, Germany) //
			
				fgets(str, 100, power_fp); // max_cstate
				memset(&temp, 0, sizeof(temp));
				strncpy(temp, str+26, 2);
				max_cstate = strtol(temp, NULL, 10);
				// no output since cstate-count isn't set properly by kernel,
				// maybe this will be fixed at some later time...
				// cout << "  Supported C-states     : " << max_cstate << endl;


				fgets(str, 100, power_fp); // bus master activity
				memset(&temp, 0, sizeof(temp));
				strncpy(temp, str+25, 8);
				bm_activity = strtoll(temp, NULL, 16);
				// cout << "  Bus Master Activity hx : 0x" << temp << endl;
				// cout << "  Bus Master Activity dc : " << bm_activity << endl;

				fgets(str, 100, power_fp); // states (line contains no information)
				while(!feof(power_fp))
				{
					fgets(str, 130, power_fp);         // cstate Cx
								     // reading up to 130 char here for AMD64 cpu's  //
					if (strlen(str)!=0)
					{
						cst_cnt++; // NOTE: we have no usage cnter for C0!!!
						memset(&temp, 0, sizeof(temp));
						strncpy(temp, str+80, 8);
						cst_usage_abs[cst_cnt] = strtol(temp, NULL, 10);
						cst_usage_notC0 += cst_usage_abs[cst_cnt];
					}
				}
				cst_usage_notC0 = cst_usage_notC0 -  cst_usage_abs[cst_cnt];	
				// now we can set max_cstate to the correct value
				max_cstate = cst_cnt - 1;
				cout << "  C-states (incl. C0)    : " << max_cstate << endl;
				//cout << "  C-usage not C0         : " << cst_usage_notC0 << endl;//

				// calculate usage etc.
				if(cst_usage_notC0!=0)  // avoid possible division by 0, happens if usage is 00000000 //
				{
				    for (int cst=1; cst <= (max_cstate-1); cst++)
				    {
					cst_usage_rel[cst] = float(cst_usage_abs[cst+1]) / cst_usage_notC0;
					fprintf(stdout, "  Usage of state C%d      : %li (%.1f %%)\n", cst,
							cst_usage_abs[cst+1], (cst_usage_rel[cst] * 100));
				    }
				}

				fclose(power_fp);
				
				max_cstate = 0;  cst_cnt = 0;  cst_usage_notC0 = 0;  // reset for the next loop //
				
			    // end code added by Niko Ehrenfeuchter (University of  Freiburg, Germany) //
			}
			else
			{
				cout<<"  Could not open file "<<filename[i-1]<<endl;
			}


			if(thrott)
			{
				FILE *throt_fp = fopen(filename[i], "r");   // acpi/processor/%s/throttling         
				
				if(throt_fp)                   
				{
				    fgets(str, 100, throt_fp); // active state
				    memset(temp, '\0', 80);
				    strncpy(temp, str+25, 5);
				    cout<<"  T-state count          : "<<temp;	// number of throttling states //
				    fgets(str, 100, throt_fp);
				    memset(temp, '\0', 80);
				    strncpy(temp, str+25, 5);
				    cout<<"  Active T-state         : "<<temp<<endl;
				    fclose(throt_fp);
				}
				else cout<<" Could not open file "<<filename[i]<<endl;
			}

			if(cpu_count>1) cout<<endl;
		}
	}
	else   
		cout<<"  CPU info       : <not available>"<<endl;
	return 0;
}

