// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef EnableScreenSaver
#include "PSCAN.H"

#define sUP		0
#define sDOWN	1
#define sRIGHT	2
#define sLEFT	3

#define SCR_TOP		1
#define SCR_BOTTOM	24
#define SCR_LEFT	0
#define SCR_RIGHT	79

#define MAXWORMS 50
#define MAXWORMLEN	30

typedef struct {
	int d,p,i,len;
	int x[MAXWORMLEN];
	int y[MAXWORMLEN];
} worm;

void ClearStatusPortal();

int numWhite=0,numBlack=0;
int scrScreenHan=0,callingScreen=0,numWorms=0,wormLen=0,liveWorms=0;
char ScreenSaveRunning,ScreenSaverDead;
#define GotoXY(x,y) gotoxy(x%(SCR_RIGHT+1),y%(SCR_BOTTOM+1))
extern HKEY hScrKey;
extern time_t lastKeyTime;
extern int languageID;

/************************************************************************************/
void TraceLines(worm *w) {

	int next = (rand()%4);
	char seg,ii,cx,cy;
	BYTE screenchar[10];

	if (!w->p) {	// don't draw a dead worm
		GotoXY (0,0);
		return;
	}

	cx=w->x[0];
	cy=w->y[0];
	for (ii=w->len+1 ; ii>0 ; ii--) { // shift the position array to the right
		w->x[ii]=w->x[ii-1];
		w->y[ii]=w->y[ii-1];
	}
	w->x[0]=cx;
	w->y[0]=cy;
	if (w->x[ii]>=0) { 		// erase the last piece of the tail
		GotoXY ((w->x[w->len]),(w->y[w->len]));
		printf (" ");
	}

	cx=w->x[0];
	cy=w->y[0];

	GotoXY (cx,cy);
	switch (w->d) { // Find the direction we are going 
		case sUP:
			switch (next) { // now react appropriately for the next direction
				case sUP: case sDOWN: next=sUP;
					(w->y[0])--;
					if ((w->y[0])<SCR_TOP) (w->y[0])=SCR_BOTTOM;
					seg = 186;
					break;
				case sRIGHT:
					(w->x[0])++;
					if ((w->x[0])>SCR_RIGHT-1) (w->x[0])=SCR_LEFT;
					seg = 201;
					break;
				case sLEFT:					
					(w->x[0])--;
					if ((w->x[0])<SCR_LEFT) (w->x[0])=SCR_RIGHT;
					seg = 187;
					break;
			}
			break;
		case sDOWN:
			switch (next) {
				case sUP: case sDOWN: next=sDOWN;
					(w->y[0])++;
					if ((w->y[0])>SCR_BOTTOM) (w->y[0])=SCR_TOP;
					seg = 186;
					break;
				case sRIGHT:
					(w->x[0])++;
					if ((w->x[0])>SCR_RIGHT+1) (w->x[0])=SCR_LEFT;
					seg = 200;
					break;
				case sLEFT:
					(w->x[0])--;
					if ((w->x[0])<SCR_LEFT) (w->x[0])=SCR_RIGHT;
					seg = 188;
					break;
			}
			break;
		case sRIGHT:
			switch (next) {
				case sUP:
					(w->y[0])--;
					if ((w->y[0])<SCR_TOP) (w->y[0])=SCR_BOTTOM;
					seg = 188;
					break;
				case sDOWN:
					(w->y[0])++;
					if ((w->y[0])>SCR_BOTTOM) (w->y[0])=SCR_TOP;
					seg = 187;
					break;
				case sRIGHT: case sLEFT: next=sRIGHT;
					(w->x[0])++;
					if ((w->x[0])>SCR_RIGHT) (w->x[0])=SCR_LEFT;
					seg = 205;
					break;
			}
			break;
		case sLEFT:
			switch (next) {
				case sUP:
					(w->y[0])--;
					if ((w->y[0])<SCR_TOP) (w->y[0])=SCR_BOTTOM;
					seg = 200;
					break;
				case sDOWN:
					(w->y[0])++;
					if ((w->y[0])>SCR_BOTTOM) (w->y[0])=SCR_TOP;
					seg = 201;
					break;
				case sRIGHT: case sLEFT: next=sLEFT;
					(w->x[0])--;
					if ((w->x[0])<SCR_LEFT) (w->x[0])=SCR_RIGHT;
					seg = 205;
					break;
			}
			break;
	}

	CopyFromScreenMemory (1,1,screenchar,(w->x[0]),(w->y[0]));	// see what character is where we will print the head
	
	if (screenchar[0]>'z' || screenchar[0]==1 || screenchar[0]==2 || screenchar[0]==15) {	// we ran into another worm
		next++;
		w->x[0]=cx;		// reset the position
		w->y[0]=cy;
		printf ("%c",w->p); // redraw the head
		GotoXY (0,0);
		return;
	}
	else {
		printf ("%c",seg); // Draw the next body segment
		GotoXY ((w->x[0]),(w->y[0]));
		printf ("%c",w->p); // draw the head
		(w->d)=next;
	}

	if (screenchar[0]=='*')  {
		if (w->p==1) numWhite--;
		else numBlack--;
		w->p=0; // we hit pesticide so die
		liveWorms--;
		for (ii=MAXWORMLEN-1 ; ii>0 && ScreenSaveRunning && !kbhit() ; ii--) // erase the worm body
			if (w->x[ii]>=0){
				NTxSleep(1);
				GotoXY(w->x[ii],w->y[ii]);
				printf (" ");
				GotoXY(0,0);
			}
		GotoXY(w->x[0],w->y[0]);
		printf ("%c",15);
	}
	GotoXY (0,0);
}
/************************************************************************************/
void RunWorms(void*args){

	char *str;
	int strLen;
	int i=0,j=0,SpeedDelay=0;
	extern HKEY hScrKey;
	worm wyrm[MAXWORMS];
	time_t ltime=time(NULL);
	time_t ktime=time(NULL);
	time_t ctime=time(NULL);

	srand(time(NULL));

	SetCurrentScreen(scrScreenHan);
	DisplayScreen (scrScreenHan);
	SetAutoScreenDestructionMode(TRUE);
	
	wormLen = GetVal(hScrKey,"WormLength",20);
	if (wormLen>MAXWORMLEN)
		wormLen=MAXWORMLEN;

	numWhite=numBlack=0;
		
	for (i=0 ; i<numWorms ; i++) {	// Initialize the worms
		for (j=0;j<MAXWORMLEN;j++) wyrm[i].x[j]=wyrm[i].y[j]=-1;
		wyrm[i].x[0] = wyrm[i].x[1] = rand()%SCR_RIGHT+1;
		wyrm[i].y[0] = wyrm[i].y[1] = rand()%SCR_BOTTOM+1;
		wyrm[i].d = rand()%4;
		wyrm[i].len = wormLen;
		wyrm[i].p = (i%2)+1;
		if (wyrm[i].p==1) numWhite++;
		else numBlack++;
	}
	liveWorms=numWorms;
	str=LS(IDS_MAIN_TITLE);
	strLen=NumBytes(str)+2;
	ScreenSaveRunning=TRUE;
	ScreenSaverDead=FALSE;

	SpeedDelay=GetVal(hScrKey,"Delay",150);

	dprintf ("SpeedDelay:%d\n",SpeedDelay);
	dprintf ("WormLen:%d\n",wormLen);

	while (!kbhit() && ScreenSaveRunning && CheckIfScreenDisplayed(scrScreenHan,FALSE) && SystemRunning) {

		for (i=0 ; i<numWorms && ScreenSaveRunning && SystemRunning ; i++) {
			ThreadSwitchWithDelay();
			if (wyrm[i].p)
				TraceLines(&wyrm[i]);
		}
		if (!SystemRunning || !ScreenSaveRunning) break;
		NTxSleep (SpeedDelay);
		if (time(NULL)>ltime+1+(SpeedDelay/50)) { // print the string
			GotoXY (rand()%(SCR_RIGHT-strLen),(rand()%SCR_BOTTOM)+1);
			printf (" %s ",str);
			GotoXY (0,0);
			ltime=time(NULL);
		}
		if (time(NULL)>ktime) { // pesticide distribution
			GotoXY (rand()%SCR_RIGHT,rand()%SCR_BOTTOM);
			printf ("*");
			GotoXY (0,0);
			ktime=time(NULL);
		}
		if (time(NULL)>ctime+1200) { // clear the screen occasionally
			clrscr();
			GotoXY (0,0);
			ctime=time(NULL);
		}
		if (liveWorms<=0) {					// all the worms are dead
			for (i=0 ; i<numWorms ; i++) {	// so start over
				wyrm[i].p = (i%2)+1;
				if (wyrm[i].p==1) numWhite++;
				else numBlack++;
			}
			liveWorms=numWorms;
			ctime=time(NULL);
			clrscr();
		}
		GotoXY (1+liveWorms,0);
		printf ("Light Worms: %2d   Dark Worms: %2d     ",numBlack,numWhite,time(NULL)-ctime);
	}

	DestroyScreen(scrScreenHan);
	scrScreenHan=0;
	SetCurrentScreen (callingScreen);
	DisplayScreen (callingScreen);
	ScreenSaveRunning=FALSE;
	ThreadSwitchWithDelay();
	ScreenSaverDead=TRUE;
	ClearStatusPortal();
	lastKeyTime = time(NULL);
	NTxExitThread (EXIT_THREAD,0);
}
/************************************************************************************/
void StartScreenSaver(int CallingScreen,int nWorms) {

	if (ScreenSaveRunning || NWversion < 4 || languageID != 4)
		return;
		
	dprintf ("Screen Saver\n");
	numWorms = (nWorms>0 ? (nWorms<MAXWORMS ? nWorms : MAXWORMS) : GetVal(hScrKey,"NumWorms",15));
	callingScreen=CallingScreen;
	scrScreenHan = CreateScreen("RTVScan Screen Saver",AUTO_DESTROY_SCREEN);//callingScreen;
	if (scrScreenHan==EFAILURE || scrScreenHan==NULL) return;
	NTxBeginThreadSize(RunWorms,NULL,0);
	NTxSleep(100);
}
/************************************************************************************/
#endif
