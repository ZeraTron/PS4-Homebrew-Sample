#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#define _KERNEL

#include <kernel.h>
#include <systemservice.h>
#include <logdebug.h>
#include <orbis2d.h>
#include <orbisPad.h>

#include <ps4/kernel.h>
#include <sys/syscall.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/mman.h>
#include <sce/kernel.h>
#include <sce/types/kernel.h>

#include "jailbreak.h"
#include "import.h"

int x=1280/2;
int y=720/2;
int w=1280/128;
int h=1280/128;
int step=10;
int scrolpos = 10;
int scrolmov = 2;

int R,G,B;
uint32_t color=0x8000ff00;

#define WIDTH 1280
#define HEIGHT 720

typedef struct {
	int process_id;
	char process_name[100];
} Process;

int64_t flipArg=0;
Orbis2dConfig *conf;
int launch = 1;
int toggle = 0;

void update()
{
	orbisPadUpdate();
}

void loadAndImport() {
	int kernel_module = sceKernelLoadStartModule("libkernel.sprx", 0, NULL, 0, NULL, NULL);

	sceKernelDlsym(kernel_module, "sceKernelGetIdPs", &sceKernelGetIdPs);
	sceKernelDlsym(kernel_module, "sceKernelGetOpenPsIdForSystem", &sceKernelGetOpenPsIdForSystem);
}

// Get all process
void* get_processes(int* process_nbr) {
	int mib[4];
    size_t len;
    int proc_num = 0;

    char *dump = malloc(PAGE_SIZE);

    void* data = malloc(1);

    int i = 1;
    for (i = 1; i < 100; i += 1) {
        size_t mlen = 4;
        sysctlnametomib("kern.proc.pid", mib, &mlen);
        mib[3] = i;
        len = PAGE_SIZE;

        int ret = sysctl(mib, 4, dump, &len, NULL, 0);
        if(ret != -1 && len > 0) {
        	data = realloc(data, sizeof(Process) * (proc_num + 1));
            struct kinfo_proc *proc = (struct kinfo_proc *)dump;

            Process* proc_c = (Process*)(data + (sizeof(Process) * proc_num));

            strcpy(proc_c->process_name, proc->ki_comm);
            proc_c->process_id = i;

            proc_num++;
        }
    }
	free(dump);

	*process_nbr = proc_num;
	return data;
}

// for the cursor sample
void updateController()
{
	int ret;
	ret=orbisPadUpdate();
	if(ret==0)
	{
		if(orbisPadGetButton(ORBISPAD_UP))
		{
			if(y-step>=0)
			{
				y=y-step;
			}
			else
			{
				y=0;
			}
		}
		if(orbisPadGetButton(ORBISPAD_DOWN))
		{
			if(y+step<conf->height-1)
			{
				y=y+step;
			}
			else
			{
				y=conf->height-1-step;
			}
		}						
		if(orbisPadGetButton(ORBISPAD_RIGHT))
		{
			if(x+step<conf->width-1)
			{
				x=x+step;
			}
			else
			{
				x=conf->width-1-step;
			}
		}
		if(orbisPadGetButton(ORBISPAD_LEFT))
		{
			if(x-step>=0)
			{
				x=x-step;
			}
			else
			{
				x=0;
			}
		}
	}
}

int main(uint64_t stackbase, uint64_t othervalue) 
{
	//hide playroom splash
	sceSystemServiceHideSplashScreen();

	//perform jailbreak
	if (getuid() != 0) {
		int64_t ret;
		ps4KernelExecute((void*)jailbreak, NULL, &ret, NULL);
	}

	loadAndImport();
	orbisPadInit();
	orbis2dInit();
	
	conf=orbis2dGetConf();
	orbis2dSetBackgroundColor( orbis2dGetRGB(0, 0, 0) );
	orbis2dSetBackFontColor( orbis2dGetRGB(0, 0, 0) );

	char* idps = malloc(64);
	memset(idps, 0, 64);
	sceKernelGetIdPs(idps);

	char* psid = malloc(16);
	memset(psid, 0, 16);
	sceKernelGetOpenPsIdForSystem(psid);

	char idps_buf[255];
	for (int i = 0; i<64; i++) {
		sprintf(idps_buf+i, "%x", idps[i]);
	}

	char psid_buf[255];
	for (int i = 0; i<16; i++) {
		sprintf(psid_buf+i, "%x", psid[i]);
	}

	int process_nbr = 0;
	void* process_list = NULL;

	while(launch)
	{
		update();

		orbis2dStartDrawing();
		orbis2dClearBuffer();

		orbis2dDrawRectColor(0, 1280, 0, 30, orbis2dGetRGB(25, 25, 25)); // draw back rectangle at top
		orbis2dSetBackFontColor( orbis2dGetRGB(25, 25, 25) ); // set backfont color to same as rectangle
		orbis2dSetFontColor( orbis2dGetRGB(200, 60, 250) ); // set font color
		orbis2dDrawString(10, 10, "PS4 Homebrew Sample"); // text 
		orbis2dSetBackFontColor( orbis2dGetRGB(0, 0, 0) ); // reset backfont color			
			
			// sliding text
			if(scrolpos+scrolmov<conf->width-1)
			{
				orbis2dSetFontColor( orbis2dGetRGB(100, 100, 100) );
				orbis2dDrawString(10 + scrolpos, HEIGHT - 40, "This is a sliding text");
				scrolpos=scrolpos+scrolmov;
				if(scrolpos == 1278)
				{
					scrolpos = 0;
				}
			}
				// rainbow ZT
				R=rand()%256;
				G=rand()%256;
				B=rand()%256;
				orbis2dSetFontColor(orbis2dGetRGB(R,G,B));
				orbis2dDrawString(WIDTH - 120, 60, " ___________ ");
				orbis2dDrawString(WIDTH - 120, 70, "(___  (_   _)");
				orbis2dDrawString(WIDTH - 120, 80, "   / /  | |  ");
				orbis2dDrawString(WIDTH - 120, 90, "  / /   | |  ");
				orbis2dDrawString(WIDTH - 120, 100, " / /__  | |  ");
				orbis2dDrawString(WIDTH - 120, 110, "/_____) |_|  ");

		//menu commands
		orbis2dSetFontColor( orbis2dGetRGB(100, 100, 100) );
		orbis2dDrawString(20, 60, "Press [TRIANGLE] to access IDPS and PSID menu");
		orbis2dDrawString(20, 80, "Press [SQUARE] to access Process list");
		orbis2dDrawString(20, 100, "Press [CROSS] to access Playground");
		orbis2dDrawString(20, 120, "Press [CIRCLE] & [R2] to exit");
		orbis2dSetFontColor( orbis2dGetRGB(255, 0, 0) );
		orbis2dDrawString(20, 485, "/!\\WIP STUFFS /!\\");
		orbis2dDrawString(20, 500, "Press [OPTIONS] for backup launcher");
		orbis2dDrawString(20, 520, "Press [R3] for test listmenu");
		orbis2dSetFontColor( orbis2dGetRGB(0, 255, 0) );
		orbis2dDrawString(10, HEIGHT - 20, "By ZeraTron");

			if(orbisPadGetButton(ORBISPAD_CIRCLE)) 
			{
				if(orbisPadGetButton(ORBISPAD_R2)) // if circle + R2 is pressed, exit the app
				{
					launch = 0;
				}
				else // else if circle is pressed, return to main menu
				{
					toggle = 0;
				}
			}
			
			if(orbisPadGetButton(ORBISPAD_TRIANGLE))
			{
				if(toggle == 0) // if triangle is pressed and you're in main menu, move in the menu associated
				{
				toggle = 1;
				}
			}
			
			if(orbisPadGetButton(ORBISPAD_SQUARE))
			{
				if(toggle == 0) // if square is pressed and you're in main menu, move in the menu associated
				{
					toggle = 2;
				}
			}
			
			if(orbisPadGetButton(ORBISPAD_CROSS))
			{
				if(toggle == 0) // if cross is pressed and you're in main menu, move in the menu associated
				{
					toggle = 3;
				}
			}
			
			if(orbisPadGetButton(ORBISPAD_OPTIONS)) //WIP stuff, probably wont work
			{
				if(toggle == 0) // if options is pressed and you're in main menu, move in the menu associated
				{
					toggle = 4;
				}
			}
			
			if(orbisPadGetButton(ORBISPAD_R3)) //WIP stuff, probably wont work
			{
				if(toggle == 0) // if R3 is pressed and you're in main menu, move in the menu associated
				{
					toggle = 5;
				}
			}
			
			if(toggle == 1) // Triangle menu
			{
				orbis2dClearBuffer();
				orbis2dDrawRectColor(0, 1280, 0, 30, orbis2dGetRGB(25, 25, 25));
				orbis2dSetBackFontColor( orbis2dGetRGB(25, 25, 25) );
				orbis2dSetFontColor( orbis2dGetRGB(200, 60, 250) );
				orbis2dDrawString(10, 10, "IDPS & PSID");
				orbis2dSetBackFontColor( orbis2dGetRGB(0, 0, 0) );
				
				orbis2dSetFontColor( orbis2dGetRGB(100, 100, 100) );
				
				if(orbisPadGetButton(ORBISPAD_SQUARE))
				{
					orbis2dSetFontColor( orbis2dGetRGB(100, 100, 100) );
					char buffer[1024];
					sprintf(buffer, "IDPS: %s", idps_buf);
					orbis2dDrawString(10, 60, buffer);
					sprintf(buffer, "PSID: %s", psid_buf);
					orbis2dDrawString(10, 80, buffer);
				}else{
					orbis2dSetFontColor( orbis2dGetRGB(100, 100, 100) );
					orbis2dDrawString(10, 60, "IDPS: [ H I D D E N ]    ");
					orbis2dDrawString(10, 80, "PSID: [ H I D D E N ]    ");
				}
				
				orbis2dDrawString(10, HEIGHT - 60, "Press [SQUARE] to displays IDs");
				orbis2dDrawString(10, HEIGHT - 40, "Press [CIRCLE] to return main menu");
				orbis2dSetFontColor( orbis2dGetRGB(0, 255, 0) );
				orbis2dDrawString(10, HEIGHT - 20, "By ZeraTron");
			}
			
			if(toggle == 2) // Square menu
			{
				orbis2dClearBuffer();
				orbis2dDrawRectColor(0, 1280, 0, 30, orbis2dGetRGB(25, 25, 25));
				orbis2dSetBackFontColor( orbis2dGetRGB(25, 25, 25) );
				orbis2dSetFontColor( orbis2dGetRGB(200, 60, 250) );
				orbis2dDrawString(10, 10, "Process list");
				orbis2dSetBackFontColor( orbis2dGetRGB(0, 0, 0) );
				
				orbis2dSetFontColor( orbis2dGetRGB(100, 100, 100) );
				
				process_list = get_processes(&process_nbr);

				int scroll_y = 0;
				for (int i = 0; i < process_nbr; i++) {
					orbis2dDrawString(10, 60 + (scroll_y*10), ((Process*)process_list)[i].process_name);
					scroll_y++;
				}

				free(process_list);

				orbis2dDrawString(10, HEIGHT - 40, "Press [CIRCLE] to return main menu");
				orbis2dSetFontColor( orbis2dGetRGB(0, 255, 0) );
				orbis2dDrawString(10, HEIGHT - 20, "By ZeraTron");
			}
			
			if(toggle == 3) // Cross menu
			{
				orbis2dClearBuffer();
				orbis2dDrawRectColor(0, 1280, 0, 30, orbis2dGetRGB(25, 25, 25));
				orbis2dSetBackFontColor( orbis2dGetRGB(25, 25, 25) );
				orbis2dSetFontColor( orbis2dGetRGB(200, 60, 250) );
				orbis2dDrawString(10, 10, "Playground");
				orbis2dSetBackFontColor( orbis2dGetRGB(0, 0, 0) );
				orbis2dSetFontColor( orbis2dGetRGB(100, 100, 100) );
				orbis2dDrawString(10, HEIGHT - 60, "Press [R2] to change the pointer color");
				orbis2dDrawString(10, HEIGHT - 40, "Press [CIRCLE] to return main menu");
				orbis2dSetFontColor( orbis2dGetRGB(0, 255, 0) );
				orbis2dDrawString(10, HEIGHT - 20, "By ZeraTron");
				updateController();
				orbis2dDrawRectColor(x,w,y,h,color);
				if(orbisPadGetButton(ORBISPAD_R2))
				{
					R=rand()%256;
					G=rand()%256;
					B=rand()%256;
					color=0x80000000|R<<16|G<<8|B;
				}
			}
			
			if(toggle == 4) //Options menu
			{
				orbis2dClearBuffer();
				orbis2dDrawRectColor(0, 1280, 0, 30, orbis2dGetRGB(25, 25, 25));
				orbis2dSetBackFontColor( orbis2dGetRGB(25, 25, 25) );
				orbis2dSetFontColor( orbis2dGetRGB(200, 60, 250) );
				orbis2dDrawString(10, 10, "Backup Launcher");
				orbis2dSetBackFontColor( orbis2dGetRGB(0, 0, 0) );
				orbis2dSetFontColor( orbis2dGetRGB(100, 100, 100) );
				orbis2dDrawString(10, HEIGHT - 40, "Press [CIRCLE] to return main menu");
				orbis2dSetFontColor( orbis2dGetRGB(0, 255, 0) );
				orbis2dDrawString(10, HEIGHT - 20, "By ZeraTron");
				
					char cwd[1024];
					char printer[1300];
					if(getcwd(cwd, sizeof(cwd)) != NULL)
					{
						sprintf(printer, "dir: %s", cwd);
						orbis2dDrawString(20, 80, printer);	//printing cur working dir
					}										// code working well
				
				DIR * rep; //supposed to open a directory but don't work
				struct dirent *dir;
				rep = opendir(cwd);
				if (!rep)
				{
					orbis2dDrawString(30, 50, "failed :(");
				}
				else
				{
					char bxl[50];
					sprintf(bxl, "path: %p", rep);
					orbis2dDrawString(30, 50, "opened :)");
					orbis2dDrawString(30, 60, bxl);
					while((dir = readdir(rep)) != NULL) {
						for (int i = 0; i > 10; i++)
						{
							char buffer[1024];
							sprintf(buffer, "%s\n", dir->d_name);
							orbis2dDrawString(10, 60+i, buffer);
						}
					}
				}
			}
			
			if(toggle == 5) //R3 menu
			{
				orbis2dClearBuffer();
				orbis2dDrawRectColor(0, 1280, 0, 30, orbis2dGetRGB(25, 25, 25));
				orbis2dSetBackFontColor( orbis2dGetRGB(25, 25, 25) );
				orbis2dSetFontColor( orbis2dGetRGB(200, 60, 250) );
				orbis2dDrawString(10, 10, "test listmenu");
				orbis2dSetBackFontColor( orbis2dGetRGB(0, 0, 0) );
				orbis2dSetFontColor( orbis2dGetRGB(100, 100, 100) );
				orbis2dDrawString(10, HEIGHT - 40, "Press [CIRCLE] to return main menu");
				orbis2dSetFontColor( orbis2dGetRGB(0, 255, 0) );
				orbis2dDrawString(10, HEIGHT - 20, "By ZeraTron");

					int i = 1; //was just using this menu to experiment stuffs, nothing useful here, put what you want
					char str[10];
					sprintf(str, "i value: %d", i);
					orbis2dDrawString(20, 60, str);	
			}
		
		orbis2dFinishDrawing(flipArg);
		orbis2dSwapBuffers();
		flipArg++;
	}
			
	orbis2dFinish();
	orbisPadFinish();
	exit(0);

	return 0;
}
