// jyeee, 14 Dec 2004
// Adapted from torsius' WinampMagic 
// Originally created: 25 Mar 2004
/*
Usage: WinampMagic.exe [-abe | --help ] file
-a      Enqueue song immediately after currently playing song
-b      Enqueue song before currently playing song
-e      Enqueue song at the end of playlist

NOTE: WINAMP MUST BE ON
*/

#include <stdio.h>
#include <windows.h>
#include <malloc.h>
#include "winamp.h"
#include <string>
using namespace std;

// hides console
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

#define METASIZE 128

// for options
#define AFTER 0
#define BEFORE 1
#define END 2
// /

HWND hwndWinamp;

// returns how many bytes were copied to local buffer
unsigned long ReadWinampToLocal(void *remoteBuf, void *localBuf, unsigned long bufsize)
{
	int isError;
	HANDLE hWinamp;
	unsigned long dWinamp;

	// find the process id
	GetWindowThreadProcessId(hwndWinamp, &dWinamp);

	// open the process object
	hWinamp = OpenProcess(PROCESS_ALL_ACCESS,false,dWinamp);
	if(hWinamp == NULL) return 0;

	isError = ReadProcessMemory(hWinamp, remoteBuf, localBuf, bufsize, NULL);

	CloseHandle(hWinamp);

	if(!isError) return 0;
	else return bufsize;
}

int GetPlayingTrack()
{
	return SendMessage(hwndWinamp,WM_WA_IPC,0,IPC_GETLISTPOS);
}

void enqueueFile(char* filepath)
{
	// perhaps extend this to include playlists :)
	hwndWinamp = FindWindow("Winamp v1.x",NULL);
	COPYDATASTRUCT cds;
	cds.dwData = IPC_PLAYFILE;
	cds.cbData = lstrlen(LPCTSTR(filepath)) + 1;
	cds.lpData = (void *) LPCTSTR(filepath);
	SendMessage(hwndWinamp, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds);
}

int main(int argc, char* argv[])
{
	printf("eat cake");
	int option;
	/* Check for command-line options: */
	if ( argc != 3 || strcmp(argv[1], "--help") == 0 )
	{
		printf("Usage: %s [-abe | --help ] file\n-a\tEnqueue song immediately after currently playing song\n-b\tEnqueue song before currently playing song\n-e\tEnqueue song at the end of playlist\n\nNOTE: WINAMP MUST BE ON\n", argv[0]);
		return 1;
	}
	for (int i = 1; i < argc; i++)
	{
		if ( strcmp(argv[1], "-a") == 0 )
		{
			option = AFTER;
		}
		else if ( strcmp(argv[1], "-b") == 0 )
		{
			option = BEFORE;
		}
		else if ( strcmp(argv[1], "-e") == 0 )
		{
			option = END;
		}
		else
		{
			printf("Unknown option: %s\n", argv[i]);
			printf("Usage: %s [-abe | --help ] file\n-a\tEnqueue song immediately after currently playing song\n-b\tEnqueue song before currently playing song\n-e\tEnqueue song at the end of playlist\n\nNOTE: WINAMP MUST BE ON\n", argv[0]);
			return 1;
		}
	}

	char *returnVal;
	returnVal = (char *)malloc(MAX_PATH*sizeof(char));

	hwndWinamp = FindWindow("Winamp v1.x",NULL);
	if(hwndWinamp == NULL)
	{
		// run winamp
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory( &si, sizeof(si) );
		si.cb = sizeof(si);
		ZeroMemory( &pi, sizeof(pi) );

		// start winamp
		if( !CreateProcess( NULL,   // No module name (use command line). 
			TEXT("C:\\Program Files\\Winamp\\winamp.exe"), // Command line. 
			NULL,             // Process handle not inheritable. 
			NULL,             // Thread handle not inheritable. 
			FALSE,            // Set handle inheritance to FALSE. 
			0,                // No creation flags. 
			NULL,             // Use parent's environment block. 
			NULL,             // Use parent's starting directory. 
			&si,              // Pointer to STARTUPINFO structure.
			&pi )             // Pointer to PROCESS_INFORMATION structure.
		) 
		{
			printf( "CreateProcess failed (%d).\n", GetLastError() );
			return 1;
		}

		// Wait until winamp is idle.
		WaitForInputIdle( pi.hProcess, INFINITE );
		hwndWinamp = FindWindow("Winamp v1.x",NULL);

		int numtracks = SendMessage(hwndWinamp,WM_WA_IPC,0,IPC_GETLISTLENGTH);
		enqueueFile(argv[2]);
		SendMessage(hwndWinamp,WM_WA_IPC,numtracks,IPC_SETPLAYLISTPOS);
		SendMessage(hwndWinamp,WM_COMMAND,40045,0);
		return 1;
	}
	
	int numtracks = SendMessage(hwndWinamp,WM_WA_IPC,0,IPC_GETLISTLENGTH);
	if ( numtracks == 0 )
	{
		enqueueFile(argv[2]); 
		SendMessage(hwndWinamp,WM_WA_IPC,numtracks,IPC_SETPLAYLISTPOS);
		SendMessage(hwndWinamp,WM_COMMAND,40045,0);
		return 0;
	}

	// insert target after everything
	if ( option == END )
	{
		enqueueFile(argv[2]); 
		SendMessage(hwndWinamp,WM_WA_IPC,numtracks,IPC_SETPLAYLISTPOS);
		SendMessage(hwndWinamp,WM_COMMAND,40045,0);
		return 0;
	}
	
	// put the playlist into an array
	char** playlist;
	playlist = new char*[numtracks];
	for ( int i = 0; i < numtracks; i++ )
	{
		ReadWinampToLocal((char *)SendMessage(hwndWinamp, WM_WA_IPC, i, IPC_GETPLAYLISTFILE), returnVal, MAX_PATH);
		playlist[i]= (char *)malloc(MAX_PATH*sizeof(char));
		strcpy(playlist[i],returnVal);
	}

	// get current track and clear playlist
	int currentTrack = GetPlayingTrack();
	SendMessage(hwndWinamp,WM_WA_IPC,0,IPC_DELETE);

	// enqueue tracks back into the playlist
	for ( int j = 0; j < numtracks; j++ )
	{
		//insert target to play before the current track
		if ( j == currentTrack && option == BEFORE)
		{
			enqueueFile(argv[2]); 
		}
		enqueueFile(playlist[j]);
		// insert target after current track
		if ( j == currentTrack && option == AFTER)
		{
			enqueueFile(argv[2]); 
		}
	}
	
	delete[] playlist;

	// set to the correct track
	if ( option == BEFORE )
	{
		SendMessage(hwndWinamp,WM_WA_IPC,currentTrack,IPC_SETPLAYLISTPOS);
	}
	else if ( option == AFTER )
	{
		SendMessage(hwndWinamp,WM_WA_IPC,currentTrack + 1,IPC_SETPLAYLISTPOS);
	}
	
	// hit the play button
	SendMessage(hwndWinamp,WM_COMMAND,40045,0); 

	free(returnVal);
	// thanks, torsius & Joonas

	return 0;
}
