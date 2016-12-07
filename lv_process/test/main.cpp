//---------------------------------------------------------------------------------------------------------------------
// LV Process DLL - demo
//---------------------------------------------------------------------------------------------------------------------
// Author: Stanislav Maslan
// E-mail: s.maslan@seznam.cz, smaslan@cmi.cz
// www: https://forums.ni.com/t5/Community-Documents/LV-Process-Windows-pipes-LabVIEW/tac-p/3497843/highlight/true
// Revision: V4.1, 2016-12-06
//
// Basic demo for testing LV process DLL. 
//  1) reads DLL version string
//  2) opens 'cmd.exe' process
//  3) sends command 'data /t' to get date string
//  4) reads and prints response to the command
//  5) sends 'exit' command to leave 'cmd.exe' process
//  6) waits for 'cmd.exe' return code
//---------------------------------------------------------------------------------------------------------------------

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "lv_proc.h"

int main(int argc,char **argv)
{
	char str[256];

    // get DLL version string
    proc_get_dll_version(str,sizeof(str));
    // print it
    printf("DLL version string:\n%s\n\n",str);

    // allocate lv process handle
    TLVPHndl *proc = (TLVPHndl*)malloc(sizeof(TLVPHndl));
    if(!proc)
    {
        printf("Allocation of lv process handle failed!\n");
        return(1);
    }

    // try to create process instance (hide debug console - windows cannot have two consoles for single process!)
    int err = proc_create(proc,"c:\\","cmd.exe",1,1);
    if(err)
    {
        printf("LV process creation failed!");
        return(1);
    }


    // read buffer
    char buf[1024];

    // read initial rubbish from the cmd.exe process stdout
    proc_command(proc,NULL,NULL,0,buf,sizeof(buf),NULL,2000,1000);
            
    printf("Hit any key to issue command \'date /t\' to the process...\n\n");
    _getch();
          
    
    // test command string
    char *cmd = "date /t\n";
    int wrt;

    // issue command string and read response
    err = proc_command(proc,&wrt,cmd,-100,buf,sizeof(buf),NULL,1000,100);
    
    printf("<< %s>> %s\n\n",cmd,buf);

    printf("Hit any key to terminate process (command \'exit\')...\n\n");
    _getch();

    // issue exit command
    proc_write_stdin(proc,"exit\n",-100,NULL);

    // wait for process to return
    int code;
    if(proc_wait_exit(proc,&code,1000))
    {
        // waiting failed - kill it with fire
        printf("Process does not respond - trying kill it...\n");
        err = proc_terminate(proc,1000);
        if(err)
        {
            printf("Process kill failed!\n");
            proc_cleanup(proc);
            return(1);
        }
    }

    // cleanup lv process handle
    proc_cleanup(proc);

    printf("Process terminated with exit code: %d\n\n",code);

    printf("Hit any key to leave...\n",code);
    _getch();
        
    return(0);
}

