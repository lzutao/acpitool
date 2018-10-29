/* support for IBM Thinkpad acpi driver */

#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <fstream>

using namespace std;
int IBM_be_quiet = 1;

int Has_Thinkpad_ACPI()
{
 ifstream file_in;
 char *filename;

 filename = "/proc/acpi/ibm";

 file_in.open(filename);
 if (!file_in)
   return 0;
 file_in.close();
 return 1;
}


int Eject_Thinkpad_Bay()
{
 ofstream bay;
 char *filename;

 if (!Has_Thinkpad_ACPI())
   {
     cout << "Ultrabay eject is only available on Thinkpads." << endl;
     return -1;
   }

 filename = "/proc/acpi/ibm/bay";

 bay.open(filename);
 if (!bay)
   {
     cout << "Could not open file : " << filename << " for writing." << endl;
     return -1;
   }
 
 bay << "eject" << endl;
 bay.close();

 return 0;
}


int Set_Thinkpad_LCD_Level(int z)
{
 ofstream bright;
 char *filename;

 filename = "/proc/acpi/ibm/brightness";

 bright.open(filename);
 if (!bright)
   {
     cout << "Could not open file: " << filename << " for writing." << endl;
     return -1;
   }

 /* brightness ranges from 0 .. 7 */
 if (z<0) z=0;
 if (z>7) z=7;

 bright << "level " << z;
 bright.close();
 return 0;
}


int Do_Thinkpad_Fan_Info()
{
 ifstream fan;
 char *filename, *status, *speed;
 char line[32];
 int i;

 filename = "/proc/acpi/ibm/fan";
 speed = NULL;
 status = NULL;

 fan.open(filename);

 if (!fan)
   {
     cout << "Failed to open file: " << filename << endl;
     return -1;
   }

 for (i=0; i<2; i++)
   {
     fan.getline(line, 32);
     if (!strncmp(line, "status:", 7))
    status = strdup(line + 9);
     if (!strncmp(line, "speed:", 6))
    speed = strdup(line + 8);
   }
 fan.close();
 cout<<"  Fan            : "<< status <<endl;
 cout<<"  Fan Speed      : "<< speed << " RPM" << endl;
 free(status);
 free(speed);
 return 0;
} 
