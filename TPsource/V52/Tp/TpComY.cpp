// Pekka Pirila's sports timekeeping program (Finnish: tulospalveluohjelma)
// Copyright (C) 2015 Pekka Pirila 

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#if defined(__BORLANDC__)
#pragma option -K
#endif
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <process.h>
#include <sys/timeb.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
//#include <bvideo.h>
#include <bkeybrd.h>
#include <bstrings.h>
#include <butil.h>
#pragma hdrstop
#include <sys/stat.h>

#ifdef MAXOSUUSLUKU
#include "VDeclare.h"
#else
#include "HkDeclare.h"
#endif

#define UDPYHTEYS

#ifndef DEBUG1
#define DEBUG1 1
#endif
#ifndef DEBUG2
#define DEBUG2 0
#endif
//#define DBGXX
//#define PING

#ifndef IN_Q_EMPTY
#define IN_Q_EMPTY 10
#endif

#define FILESEND  10

#define LUEBUF 3000
#define LUECLIBUF 100
int UDPviive_lue = 20;
int UDPviive_lah = 10;
int UDPviive_ts = 20;
int UDPCliWait;
int TCPviive_lah[MAX_PORTTI];
int TCPviive_nak[MAX_PORTTI];
int TCPviive_pak[MAX_PORTTI];
int UDPvarmistamaton[MAX_PORTTI];
static int lah_tied_seur[MAX_LAHPORTTI];
static bool tiedostosanoma[MAX_LAHPORTTI];
static bool com_keskeytt[MAX_PORTTI];

void naytaTestRivi(void);
void sendTestLopetus(void);
void lahetaKopiot(int cn, combufrec obuf);
sendtestparamtp sendtestparam;

#define MAXMONITORI 10
HANDLE hcMon[MAXMONITORI];
ipparamtp ipParamMon[MAXMONITORI];
int monitoriVali = 10000;
int monportbase = 14770;

void alkusanoma(combufrec *obuf, int cn, int flags);
bool tark_alku(const combufrec * const cbuf, int cn);
void DSPIQ(int nj);
//void tall_ec(UINT32 badge, int piste, int t, int kielto, int portti);
static INT kirjaahyv(int cn, char *buf);
static INT kirjaahyvStream(int cn);
static int lahetapaketti(int cn);
static void kirjaa_tiedhyv(int cn);
static int get_tied_sanoma(int cn, combufrec *obuf);
void closec(int cn);
void lueemit(LPVOID lpCn);
#ifndef __linux__
void tiedonsiirtoUDP(LPVOID lpCn);
#else
void *tiedonsiirtoUDP(LPVOID lpCn);
#endif
void init_class_len(void);
void WRITEYHT(wchar_t *st, INT cn);
void DSPQ(INT r, INT cn);

#define MAX_LAHPORTTIY MAX_LAHPORTTI
#define MAX_PORTTIY MAX_PORTTI
#ifndef MAXCOMDATA
#define  MAXCOMDATA 250
#endif

extern int alkut;
extern HANDLE hComm[];
extern int ackreq[];              /* != 0 Ilmaisee, ett� yhteys toimii */
extern int yhtfl[];              /* != 0 Ilmaisee, ett� yhteys toimii */
extern int keyclose[];                   /* K�ytt�j� sulkenut portin */
static int autoclose;               /* Ohjelma sulkenut vastaanoton */
extern int  initid[];                    /* inpakid initialisoimatta */
extern int intv[];                      /* Tiedonsiirron odotusv�li     */
//int intv[2];                      /* Tiedonsiirron odotusv�li     */
extern combufrec *inbuf;      /* Saapuvien jono */
extern combufrec *outbuf[];     /* L�htevien jono */
extern int  combufsize;
extern char *combuf[];          /* IO-portin puskuri */
extern int  lahfl[];            /* L�hetys k�ynniss� */
extern int  vkesken[];              /* Vastaanotto kesken */
extern int  chcomkesto[];           /* Yhden merkin l�hett�miskesko */
extern int  hyvkesken[];            /* Hyv�ksynt� kesken */
extern char outpakid[];    /* seuraava l�hetett�v� id */
extern int vastcom0[];
extern int lahcomserver[];
int varaserver;
extern int class_len[];     // L�hetett�v�n tiedon pituus mukaanlukien
                                   // checksum ja itse tieto
extern int timerfl, tbase, trate;
extern INT vpiste[];
extern int comtype[];
unsigned int TCP_on[6];
extern INT yhteysalku;
static INT luepaketteja = 1;
extern int tcpautoalku;
extern INT ts_close[];
extern int acn[];


#ifdef PING
HTHREAD hPingThread;
#endif

#define R_BUFLEN_C 900

extern comparamtp comparam[];

typedef union {
   struct {
	  USHORT alku;
      char badge[3];
      char fill1;
      char week;
      char year;
      char fill2;
      char chksum;
	  ctrltype c[50];
      char fill3[40];
      char chs;
      char cs[4];
	  char chp;
	  char cp[4];
      char chl;
      char cl[4];
      char fill4;
      char chksum2;
      } r12;
   char bytes[R_BUFLEN_C+1];
   } Y_san_type;

#ifdef DBGXX
    FILE *dfl;
    char dflb[20000];

//    dfl = fopen("comdump", "wb");
//    memset(dfbl, 0, sizeof(dflb));
#endif

#ifdef PROFILOI

#define PROFARRSIZE 10000

typedef struct {
   int time;
   int cn;
	wchar_t msg[30];
   } proftp;

proftp *profarr;
int profptr;
FILE *proffile;
CRITICAL_SECTION prof_CriticalSection;

void open_profile(void)
   { 
   if (!profiloi)
      return;
   InitializeCriticalSection(&prof_CriticalSection);
   profarr = (proftp *) calloc(PROFARRSIZE, sizeof(proftp));
   if (profiloi == 2)
		proffile = fopen("profdata.txt", "wt");
   }

void close_profile(void)
   {
   int i;

   if (!profiloi)
	  return;
   if (profiloi == 1 && !proffile) {
      proffile = _wfopen(L"profdata.txt", L"wt");
      for (i = profptr; i < PROFARRSIZE; i++)
         if (profarr[i].time)
            fwprintf(proffile, L"%10d %3d %-29.29s\n",
               profarr[i].time, profarr[i].cn, profarr[i].msg);
      }
   if (!proffile)
	   return;
   for (i = 0; i < profptr; i++)
      if (profarr[i].time) {
         fwprintf(proffile, L"%10d %3d %-29.29s\n",
            profarr[i].time, profarr[i].cn, profarr[i].msg);
         }
	if (proffile)
		fclose(proffile);
   proffile = NULL;
   profiloi = 0;
   DeleteCriticalSection(&prof_CriticalSection);
   }

void put_profile(wchar_t *msg, int cn)
   {
	int i;

   if (!profiloi)
      return;
   EnterCriticalSection(&prof_CriticalSection);
   profarr[profptr].time = mstimer();
   profarr[profptr].cn = cn;
   wcsncpy(profarr[profptr].msg, msg, 29);
   profptr++;
   if (profptr >= PROFARRSIZE) {
      if (profiloi == 2) {
         for (i = 0; i < PROFARRSIZE; i++)
            fwprintf(proffile, L"%10d %3d %-29.29s\n",
			   profarr[i].time, profarr[i].cn, profarr[i].msg);
         }
      profptr = 0;
      }
   LeaveCriticalSection(&prof_CriticalSection);
   }
#else
void close_profile(void)
   {
	}
#endif

int writeerrorkysy(wchar_t *msg)
{
   int vast;

   erbeep();
//   wchar_t wmsg[200] = L"";
//   mbstowcs(wmsg, msg, sizeof(wmsg)-1);
	vast = MessageBoxW(NULL, msg, L"", MB_YESNO);
   return(vast != IDYES);
}

int closeport_x(int cn)
	 {
	if (comtype[cn] == comtpRS)
		return (closeport(hComm+cn));
	else {
	   if (comtype[cn] & comtpUDP)
		  return (closeportUDP(hComm+cn));
	   if (comtype[cn] & comtpTCP)
		  return (closeportTCP(hComm+cn));
	   }
	return(0);
	}

int openport_x(int cn, int portti, int baud, char parity, int bits, int sbits, int xonoff)
	{
	if (inLopetus)
		return(0);
	if (comtype[cn] == comtpRS)
		 return (openport(hComm+cn, portti, baud, parity, bits, sbits, xonoff));
   if (comtype[cn] & comtpUDP)
		 return (openportUDP(hComm+cn, ipparam+cn));
   if (comtype[cn] & comtpTCP)
		 return (openportTCP(hComm+cn, ipparam+cn));
	return(0);
	}

void setcomtimeouts_x(int cn, int rdconst, int rdmult, int rdintvl,
					int wrtconst, int wrtmult)
	{
	if (comtype[cn] == comtpRS)
		 setcomtimeouts(hComm[cn], rdconst, rdmult, rdintvl, wrtconst, wrtmult);
    }

int quesize_x(int cn, int *que)
    {
	if (comtype[cn] == comtpRS)
		return(quesize(hComm[cn], que));
	 else  if (comtype[cn] & comtpUDP) {
		*que = UDPstreambuffered(hComm[cn]);
		return(0);
		}
	 else if (comtype[cn] & comtpTCP) {
		*que = TCPbuffered(hComm[cn]);
		return(0);
		}
	 else {
		 *que = 1;
	   return(0);
		 }
	}

void outquesize_x(int cn, int *que)
	{
	if (comtype[cn] == comtpRS)
		 outquesize(hComm[cn], que);
	 else
		 *que = 0;
	}

void o_flush_x(int cn)
	{
	if (comtype[cn] == comtpRS) {
		o_flush(hComm[cn]);
		}
	}

void i_flush_x(int cn)
   {
   if (comtype[cn] == comtpRS)
	  i_flush(hComm[cn]);
	else if (comtype[cn] == comtpUDPSTREAM)
		i_flush_UDPstream(hComm[cn]);
	else if (comtype[cn] & comtpTCP)
		i_flush_TCP(hComm[cn]);
	}

int read_ch_x(int cn, char *ch, int *que)
	{
	if (comtype[cn] == comtpRS)
	  return(read_ch(hComm[cn], ch, que));
	else if (comtype[cn] == comtpUDPSTREAM)
		return(read_UDPstream(hComm[cn], 1, ch, que));
	else if (comtype[cn] & comtpTCP)
	  return(read_TCP(hComm[cn], 1, ch, que));
	else
	  return(0);
   }

int wrt_ch_x(int cn, char ch)
	{
	int retval;

	if (comtype[cn] == comtpRS) {
		retval = wrt_ch(hComm[cn], ch);
		}
	if (comtype[cn] & comtpUDP)
		 retval = wrt_ch_UDP(hComm[cn], ch);
	if (comtype[cn] & comtpTCP)
		 retval = wrt_ch_TCP(hComm[cn], ch);
	return(retval);
	}

int read_st_x(int cn, int len, char *buf, int *nread, int *que)
	{
	if (comtype[cn] == comtpRS)
		 return(read_st(hComm[cn], len, buf, nread, que));
	else {
		*que = 0;
		if (comtype[cn] ==  comtpUDPSTREAM)
			return(read_UDPstream(hComm[cn], len, buf, nread));
		if (comtype[cn] & comtpTCP)
			return(read_TCP(hComm[cn], len, buf, nread));
		}
	return(0);
	}

int wrt_st_x(int cn, int len, char *buf, int *nsent)
	{
	int retval;

	if (comtype[cn] == comtpRS) {
        retval = wrt_st(hComm[cn], len, buf, nsent);
        }
	if (comtype[cn] & comtpUDP)
		 retval = wrt_st_UDP(hComm[cn], len, buf, nsent);
	if (comtype[cn] & comtpTCP)
		 retval = wrt_st_TCP(hComm[cn], len, buf, nsent);
	return(retval);
	}

#ifdef UDPYHTEYS
int wrt_st_x_srv(int cn, int len, char *buf, int *nsent)
	{
	int retval = 0;

	if (comtype[cn] & comtpUDP)
         retval = wrt_st_UDPsrv(hComm[cn], len, buf, nsent);
	return(retval);
	}
#endif

void addMonitori(wchar_t *str)
{
	wchar_t *p;
	int k;

	p = str;
	if (monitoriLkm >= MAXMONITORI)
		return;
	ipParamMon[monitoriLkm].iptype = ipUDPBOTH;
	wcscpy(ipParamMon[monitoriLkm].destaddr, L"255.255.255.255");
	ipParamMon[monitoriLkm].destport = monportbase + monitoriLkm;
	if (*p && wcswcind(*p, L":,/=") >= 0) {
		p = wcstok(p+1, L":,/=");
		if (p) {
			if (wcslen(p) > 3)
				wcsncpy(ipParamMon[monitoriLkm].destaddr, p, 63);
			p = wcstok(NULL, L":,/=");
			if (p && (k = _wtoi(p)) > 0) {
				ipParamMon[monitoriLkm].destport = (USHORT) k;
				}
			}
		}
	monitoriLkm++;
}

void lahetaMonitorille(int yhtlkm, int yhtavattu, int jonossa, int jonoja)
{
	static int edLahetys = 0, inLahMon;
	char buf[1000];
	int t, err2, nsent;
	static bool lahMonOpen[MAXMONITORI];
	static HANDLE hCMon[MAXMONITORI];

	if (inLahMon || monitoriLkm == 0 || (t = mstimer()) < edLahetys + monitoriVali)
		return;
	inLahMon = 1;
	edLahetys = t;
	sprintf(buf, "Kone:%s;YhtLkm:%d;AvLkm:%d;Jonossa:%d;JonoLkm:%d",
		konetunn, yhtlkm, yhtavattu, jonossa, jonoja);
	buf[512] = 0;
	for (int i = 0; i < monitoriLkm && !inLopetus; i++) {
		if (!lahMonOpen[i]) {
			if (openportUDP(&hCMon[i], &ipParamMon[i]))
				continue;
			UDPcliAllowBroadcast(hCMon[i]);
			lahMonOpen[i] = 1;
			}
		if (i)
			Sleep(50);
		err2 = wrt_st_UDP_0(hCMon[i], strlen(buf), buf, &nsent);
		}
	inLahMon = 0;
}

int lahettamatta(int cn)
{
	return(cjseur[cn] - cjens[cn] + lah_tied_seur[cn]);
}

void buflah(int cn, combufrec *obuf)
   {
	 static wchar_t tmsg[] = L"JONO   T�YSI EI L�HETETTY";
    static wchar_t tmsg1[] = L"Tiedonsiirtojono t�ynn�, ei l�hetet�";
    static wchar_t tmsg2[] =
        L"L�htevien tietojen jono kasvaa, tarkista tiedonsiirto";
	static int esto = 0, nj0 = 0;
    char *cb;
	int  v, nj;
	__int64 pos, pos0;
	wchar_t *pmsg = NULL;
	 combufrec *ob;

//    EnterCriticalSection(&outb_CriticalSection);
//    Sleep(1);
    if (levypusk) {
        obuf->lahetetty = 0;
		pos = _lseeki64(obfile, 0 ,SEEK_END);
//		pos = _filelengthi64(obfile);
		pos0 = ((cjseur[cn]+1)*maxyhteys+cn)*(__int64)combufsize;
		if (pos0+combufsize > pos) {
//			pos = _lseeki64(obfile, 0 ,SEEK_END);
			cb = (char *) calloc(maxyhteys*combufsize,20);
			if (pos > 0)
			   pos = _write(obfile, cb, maxyhteys*combufsize);
            free(cb);
            if ((kirjheti & 2) && pos > 0) {
			   fflush(fobfile);
			   }
			}
		if (pos > 0) {
			pos = _lseeki64(obfile, pos0 ,SEEK_SET);
			if (pos > 0) {
				if (_write(obfile, obuf, combufsize) != combufsize)
					writeerror_w(L"Virhe rutiinissa 'buflah', koodi 1", 0);
				}
			}
		else {
			writeerror_w(L"Virhe rutiinissa 'buflah', koodi 2", 0);
			}
        cjseur[cn]++;
//        nj = (int) (cjseur[cn] - cjens[cn]);
        }
    else {
        v = ((int) cjseur[cn] + 1) % OUTBUFL;
        if( (v == (INT) cjens[cn]) ) {
			pmsg = tmsg1;
			if (loki) {
					 tmsg[5] = cn + L'1';
				wkirjloki(tmsg);
					 }
            }
        else {
			ob = (combufrec *) ((char *)outbuf[cn] + (int)cjseur[cn]*combufsize);
			memcpy(&ob->pkgclass, &obuf->pkgclass, obuf->len+5);
            cjseur[cn] = v;
            nj = ((int) cjseur[cn] + OUTBUFL - (int) cjens[cn]) % OUTBUFL;
            if(nj == OUTBUFL - OUTBUFL/2 || nj == OUTBUFL - OUTBUFL/4) {
                if (nj > nj0) {
                    pmsg = tmsg2;
                    }
                }
            }
        }
//	LeaveCriticalSection(&outb_CriticalSection);
    if (pmsg && !esto) {
        esto = 1;
		writeerror_w(pmsg, 0);
        esto = 0;
        }
    DSPQ(ySize-6, cn);
	}

#ifdef PING
#if defined(_BORLANDC_)
#pragma warn -par
#endif
//DWORD WINAPI ping_thread(LPVOID lpCn)
void ping_thread(LPVOID lpCn)
    {
	int  nj,cn, dh, nPing, kPing;
    INT32 pos;
    int ed_ping, ping_delay, t_ping;
    char msg[40];
    union {
        unsigned int u_ip;
        char c_ip[4];
        } u;
    char ipstr[6][16];

    ed_ping = mstimer();
    for (nPing = 0; nPing < 6; nPing++) {
	   if (!TCP_on[nPing])
          break;
       u.u_ip = ntohl(TCP_on[nPing]);
       sprintf(ipstr[nPing], "%3.3d.%3.3d.%3.3d.%3.3d",
          (int) u.c_ip[3], (int) u.c_ip[2], (int) u.c_ip[1], (int) u.c_ip[0]);
       }
	 if (!nPing)
       return(0);
    kPing = 0;
    while (comfl) {
       if (!ping_gap) {
           Sleep(5000);
           continue;
           }
       if (!TCP_on[kPing])
          kPing = (kPing+1) % nPing;
		 else {
		  if ((t_ping = mstimer()) - ed_ping < ping_gap &&
             t_ping - latestping[kPing] < ping_gap)
             Sleep(ping_gap - t_ping + latestping[kPing]);
          ping_delay = ping_sub(TCP_on[kPing]);
          if (ping_delay > 0) {
             sprintf(msg, "IP %d: %s ping-viive %4d ms ", kPing+1,
                ipstr[kPing], ping_delay);
             }
          else if (ping_delay == 0) {
             sprintf(msg, "IP %d: %s ping-viive  <10 ms ", kPing+1,
                ipstr[kPing]);
             }
          else
             sprintf(msg, "IP %d: %s ping timed out     ", kPing+1,
					 ipstr[kPing]);
		  vidspmsg(16-nPing+kPing, 39, 7, 0, msg);
          ed_ping = t_ping;
          if (ping_delay >= 0)
             latestping[kPing] = ed_ping;
          kPing = (kPing+1) % nPing;
          }
       }
    }
#if defined(_BORLANDC_)
#pragma warn +par
#endif
#endif

unsigned short chksum(char *bufptr, int len)
	{
    unsigned short *bptr;
    char *cp;
	 unsigned short sum = 0;
    int  i;

    bptr = (unsigned short *) bufptr;
    for ( i = 0 ; i < len/2 ; i++) sum += *(bptr++);
    if (len % 2) {
		  cp = (char *) bptr;
        sum += *cp;
        }
    return(sum);
    }

int addseur(int cn, const combufrec * const cbuf)
    {
    int nj;
    int inbnext;
	combufrec *ibuf;

    if (keyclose[cn])
       return(1);
	if (cbuf->pkgclass == alkut) {
		if (tark_alku(cbuf, cn) == true) {
			com_keskeytt[cn] = 1;
			return(-1);
			}
		else {
			com_keskeytt[cn] = 0;
			}
		}
    EnterCriticalSection(&inb_CriticalSection);
    inbnext = (inbseur + 1) % INBUFL;
	ibuf = inbuf + inbseur;
    if (((comtype[cn] == comtpRS) && inbnext != inbens) ||
       ((comtype[cn] != comtpRS) && inbseur == inbens)) {
       if (cbuf->pkgclass == alkut && cbuf->d.k.pakota)
		  ansitowcs(naapuri[cn], (char *)&cbuf->d.k.pakota, 2);
		 memset(ibuf, 0, combufsize);
	   memcpy((char *)ibuf, (char *)cbuf, cbuf->len+14);
#ifdef DBGXX
	   swprintf(dflb+wcslen(dflb),L"%ld:%d:%d<A>", inbseur, (int)cbuf->id, (int)cbuf->d.v.entno);
#endif
       ibuf->portti = cn+1;
	   inbseur = inbnext;
	   }
    else
       inbnext = -1;
    LeaveCriticalSection(&inb_CriticalSection);
    if (status_on) {
       nj = (inbseur + INBUFL - inbens) % INBUFL;
       DSPIQ(nj);
       }
	 return(inbnext < 0);
	 }

void opencomRS(int cn)
    {
	int  er;

   closec(cn);
   if (openport_x(cn,portti[cn],comparam[cn].baud,comparam[cn].parity,comparam[cn].bits,comparam[cn].sbits,0))  {
#ifdef _CONSOLE
	  writeerror_w(L"",0);
#endif
	  return;
	  }
#ifndef __linux__
   setcomtimeouts_x(cn, 0, 0, MAXDWORD, 0, 0);
#endif
	i_flush_x(cn);
   o_flush_x(cn);
	quesize_x(cn,&er);
 	if (comparam[cn].baud > 6) intv[1] = 1;
   else if (comparam[cn].baud == 6 && intv[1] > 2) intv[1] = 2;
   comopen[cn] = TRUE;
   if (cn < maxyhteys) {
      if (lahcom[cn]) {
         lah_alku[cn] = 1;
         if (!varaserver) 
            lahcomserver[cn] = 1;
         }
      WRITEYHT(L"  ", cn);
      }
   }

static void luepaketti(INT cn)
   {
   char s[4];
   static char id1[MAX_LAHPORTTIY][LUEBUF+6];
   combufrec *cbuf[MAX_LAHPORTTIY];
   char *id[MAX_LAHPORTTIY];
   static INT16 pkgclass[MAX_LAHPORTTIY];
   INT    er,nch,koodi=0,inq, i, pkglen;
   UINT16  *chks;
   INT    hyvp ;
   static char inpakid[MAX_LAHPORTTIY];  /* odotettava paketin id */
   static INT  npak[MAX_LAHPORTTIY], sumerror[MAX_LAHPORTTIY];               /* Virhelaskuri */
   static INT  inq0[MAX_LAHPORTTIY] = {-10, -10};   /* saapuvan sanoman seuranta */
   wchar_t tenerror[] = L"10 tiedonsiirtovirhett� paketin siirrossa";
	wchar_t vmsg[] = L"Virhe   tiedonsiirrossa";
   static int od_alku[MAX_LAHPORTTIY];
  
   wchar_t emsg[80];
  
   if (!luepaketteja)
      return;
   for (i = 0; i < MAX_LAHPORTTIY; i++) {
	   cbuf[i] = (combufrec *) id1[i];
	   id[i] = (char *) &cbuf[i]->so;
		}
   hyvp = FALSE;
   if (quesize_x(cn, &inq)) {
      koodi = 1;
      goto err_esc;
      }
   if (!inq)
	   return;
   id[cn][0] = SOH;
   if (!vkesken[cn] || od_alku[cn]) {
	  vkesken[cn] = TRUE;
		if( inq < 17) {
         if (od_alku[cn] > 20) {
            koodi = 2;
			od_alku[cn] = 0;
            goto err_esc;
            }
			else {
            od_alku[cn]++;
            return;
			}
         }
	  if (comtype[cn] & comtpTCP) {
		  peek_TCP(hComm[cn], 5, id[cn]+1, &nch);
		  pkglen = *(INT16 *)&id[cn][4];
		  if (id[cn][1] + id[cn][2] != 255 || (nch == 5 && class_len[id[cn][3]] != pkglen && (id[cn][3] != alkut || pkglen != 3))) {
			  vkesken[cn] = FALSE;
			  return;
			  }
		  }
		er = read_st_x(cn,5,id[cn]+1,&nch,&inq);
		od_alku[cn] = FALSE;
		pkgclass[cn] = id[cn][3];
		pkglen = *(INT16 *)&id[cn][4];
		if ((er == OK) && (nch == 5) && (id[cn][1] + id[cn][2] == 255)
			&& (class_len[pkgclass[cn]] == pkglen || (pkglen == 3 && pkgclass[cn] == alkut))) {
			if (pkgclass[cn] == alkut && id[cn][1] == 1)
				inpakid[cn] = 0;
			if (initid[cn]) 
				inpakid[cn] = id[cn][1] - 1;
			initid[cn] = FALSE;
			if( id[cn][1] != (char) (inpakid[cn] + 1) ) {
				if( id[cn][1] == inpakid[cn] ) {
					if (vastcom[cn]) {
						if (loki) {
							wkirjloki(L"Tiedonsiirtopaketti uusittu");
							}
						hyvp = TRUE;            /* Kuittaa saman paketin uusinta */
						}
					}
				else {
					writeerror_w(L"Tiedonsiirtovirhe, v��r� paketin numero",0);
					initid[cn] = TRUE;
					if (loki) {
				  wkirjloki(L"V��r� tiedonsiirtopaketin numero");
                  }
               }
		    for (i = 0; i < 10; i++) {
			   quesize_x(cn, &inq);
			   if (inq >= pkglen+2)
				   break;
			   Sleep(30);
			   }
		    read_st_x(cn,pkglen+2,id[cn]+6,&nch,&inq);
			goto escape;
			}
         }
	  else {
         koodi = 3;
			swprintf(emsg, L"Virhe : %d, Pituus = %d, Merkit: %d %d %d %d %d",
				er, (int) nch, (int) id[cn][1], (int) id[cn][2],
				(int) id[cn][3], (int) id[cn][4], (int) id[cn][5]);
			goto err_esc;
         }
      }
    for (i = 0; i < 3; i++) {
       if (quesize_x(cn, &inq)) {
          koodi = 1;
          goto err_esc;
          }
	   if (inq >= pkglen+2)
			break;
		if (comtype[cn] & comtpTCP)
		   Sleep(30);
	   else
		   Sleep(chcomkesto[cn]*(pkglen+2 - inq));
		}
      if( inq < pkglen+2 && inq > inq0[cn]+3) {
         inq0[cn] = inq;
         return;
         }
//   seurbuf = (char *) &(inbuf[cn][inbseur[cn]].checksum);
//   inbuf[cn][inbseur[cn]].pkgclass = pkgclass[cn];
   er = read_st_x(cn,pkglen+2,id[cn]+6,&nch,&inq);
   chks = (UINT16 *) (id[cn]+6);
#if !DEBUG1
   if ((er == OK) && (nch == pkglen+2) && 
      (chksum(seurbuf+2, pkglen) == *chks)) {
#else
   if (er) koodi = 4;
   else {
      if (er == -1) koodi = 5;
      else {
         if (nch - pkglen-2) {
			koodi = 6;
			swprintf(emsg, L"Merkkej� luettu: %d, Vaadittu: %d, Luokka: %d",
			   (int) nch, (int) pkglen+2, pkgclass[cn]);
            }
         else if (chksum(id[cn]+8, pkglen) - *chks)
            koodi = 7;
         }
      }
#endif
   if (!koodi) {  
		if (vastcom[cn] && !addseur(cn, cbuf[cn])) {
			inpakid[cn] = id[cn][1];
			hyvp = TRUE;
			}
		}
	else {
		err_esc:
		if (loki) {
			vmsg[6] = L'0'+koodi;
			wkirjloki(vmsg);
			if (koodi == 3 || koodi == 6)
				wkirjloki(emsg);
			}
		if( ++sumerror[cn] == 10 ) {
			writeerror_w(tenerror,0);
			sumerror[cn] = 0;
			npak[cn] = 0;
         }
      }
   escape:
   o_flush_x(cn);
   i_flush_x(cn);
   if( hyvp ) {
      if (++npak[cn] > 100) {
         npak[cn] = 0;
         sumerror[cn] = 0;
		 }
		s[0] = ACK;
      s[1] = id[cn][1];
      s[2] = id[cn][2];
	  s[3] = id[cn][1];
      wrt_st_x(cn,4,s,&nch);
      }
	else
		wrt_ch_x(cn,NAK);
	if (comtype[cn] & comtpRS)
		Sleep(4*chcomkesto[cn]);
   lahfl[cn] = FALSE;
   vkesken[cn] = FALSE;
   inq0[cn] = -10;
   }
  
#ifdef UDPYHTEYS
static void opencomUDP(int cn)
    {
   wchar_t msg[80];

#ifdef PROFILOI
   put_profile(L"openportUDP 1", cn);
#endif
	if (inLopetus)
		return;
   if (openportUDP(hComm+cn, ipparam+cn)) {
	  swprintf(msg, L"UDP-yhteyden %d avaaminen ei onnistunut", cn+1);
		writeerror_w(msg, 0);
	  return;
	  }
#ifdef PROFILOI
   put_profile(L"openportUDP 2", cn);
#endif
   comopen[cn] = TRUE;
   if (lahcom[cn]) {
	  lah_alku[cn] = 1;
	  if (!varaserver)
		 lahcomserver[cn] = 1;
	  }
   WRITEYHT(L"  ", cn);
   }

static void stopUDP(void)
   {
   int cn;

   for (cn = 0; cn < MAX_PORTTI; cn++)
      if (comopen[cn] && (comtype[cn] & comtpUDP)) {
#ifdef PROFILOI
		 put_profile(L"closeportUDP 1", cn);
#endif
		 closeportUDP(hComm+cn);
#ifdef PROFILOI
		 put_profile(L"closeportUDP 1", cn);
#endif
		 }
	}

#ifndef __linux__
void luepakettiUDP(LPVOID lpCn)
#else
void *luepakettiUDP(LPVOID lpCn)
#endif
	{
	char s[4], buf1[LUEBUF+6];
	combufrec * const cbuf = (combufrec * const) buf1;
	char *buf = (char *) &cbuf->so;
	int    yhtcount = 0, cn, nch,koodi;
	int    hyvp;
	long  edyht, tm;
	char inpakid = 0;  /* odotettava paketin id */
	int  npak = 0, sumerror = 0;          /* Virhelaskuri */
	wchar_t tenerror[] = L"10 tiedonsiirtovirhett� paketin siirrossa";
	wchar_t vmsg[] = L"Virhe   tiedonsiirrossa";
	wchar_t emsg[100], emsg1[100] = L"";
#ifdef PROFILOI
	wchar_t profmsg[30];
#endif

	cn = *(int *)lpCn;
	opencomUDP(cn);
	if (!comopen[cn])
#ifndef __linux__
	   return;
#else
	   return(0);
#endif
	hTsThread[cn] = _beginthread(tiedonsiirtoUDP, 40960, lpCn);
	Sleep(1000);
	edyht = mstimer();
	while (comfl && luepaketteja) {
		if (!comopen[cn] || keyclose[cn]) {
			Sleep(300);
			continue;
			}
        hyvp = 0;
        koodi = 0;
#ifdef PROFILOI
        put_profile(L"luepakettiUDP 1", cn);
#endif

        read_UDP(hComm[cn], LUEBUF, buf, &nch);

#ifdef PROFILOI
        swprintf(profmsg, L"luepakettiUDP 2:%3.3d", nch);
        put_profile(profmsg, cn);
#endif

		// nch == 0 vain timeout-tilanteessa, joka osoittaa, ett� sanomaa ei ole

        if (nch) {
			yhtfl[cn] = 5;
            yhtcount = 0;
            edyht = mstimer();
			}
        else {
            if (((tm = mstimer()) > edyht + 1000 || tm < edyht) && yhtfl[cn] > 0)
               yhtfl[cn]--;
				if (yhtcount < 5)
               yhtcount++;
            Sleep(yhtcount*UDPviive_lue);
            continue;
            }
        if (cbuf->so == NAK) {
			Sleep(100);
            continue;
			}
        if (cbuf->so != ACK && cbuf->so != SOH)
            koodi = 1;
        if (!koodi && cbuf->so == ACK) {
            koodi = 2;
            }
		if (!koodi && (
			nch <= 6 ||
			cbuf->id + cbuf->iid != 255 ||
			(class_len[cbuf->pkgclass] > 0 && cbuf->len != class_len[cbuf->pkgclass] && ((cbuf->len != 3 && cbuf->len != 10) || cbuf->pkgclass != alkut)) ||
			nch < cbuf->len + 6)) {
				koodi = 3;
				swprintf(emsg1, L"class=%d,id=%d, iid=%d, cl_len=%d, len=%d", (int)cbuf->pkgclass, (int)cbuf->id, (int)cbuf->iid,
					(int)class_len[cbuf->pkgclass], (int)cbuf->len);
			}

		if (!koodi && (UDPvarmistamaton[cn] & 2) == 0) {
            if (cbuf->pkgclass == alkut && cbuf->id == 1) 
				inpakid = 0;
            if (initid[cn]) 
				inpakid = cbuf->id - 1;
            initid[cn] = FALSE;
            if (cbuf->id != (char) (inpakid + 1)) {
                if (cbuf->id == inpakid || (char)(cbuf->id+1) == inpakid || 
						 (char)(cbuf->id+2) == inpakid) {
						if (vastcom[cn]) {
							  if (loki) {
									swprintf(emsg, L"Tiedonsiirtopaketti uusittu, yht: %d, id: %d, vuorossa: %d", cn+1, (int)cbuf->id, (int)inpakid+1);
									wkirjloki(emsg);
									}
							  if (cbuf->id == inpakid)
								  hyvp = TRUE;            /* Kuittaa saman paketin uusinta */
							  koodi = -1;
							  }
							}
					 else {
						  initid[cn] = TRUE;
						  koodi = 4;
						  if (loki) {
							  swprintf(emsg, L"V��r� paketin numero, yht: %d, id: %d, vuorossa: %d", cn+1, (int)cbuf->id, (int)inpakid+1);
								wkirjloki(emsg);
//								writeerror_w(emsg,0);
								}
						  }
					 }
				}
          if (!koodi && (UDPvarmistamaton[cn] & 2) == 2)
			  hyvp = TRUE;

		  if (!koodi && chksum((char *)&cbuf->d, cbuf->len) - cbuf->checksum) {
				koodi = 7;
				}
#ifdef PROFILOI
		  swprintf(profmsg, L"luepakettiUDP 3:id %d", (int)cbuf->id);
		  put_profile(profmsg, cn);
#endif
		  if (!koodi) {
			  if (vastcom[cn] && !addseur(cn, cbuf)) {
#ifdef PROFILOI
               swprintf(profmsg, L"luepakettiUDP 4:id %d", (int)cbuf->id);
               put_profile(profmsg, cn);
#endif
               inpakid = cbuf->id;
               hyvp = TRUE;
              }
				}
        if (koodi > 0) {
            if (loki) {
                vmsg[6] = L'0'+koodi;
				wkirjloki(vmsg);

                if (koodi == 1 || koodi == 3) {
					swprintf(emsg, L"Virhe sanomassa, Jono %d, Pituus %d, Merkit: "
						L"%d %d %d %d %d %d %d %d",
						cn, nch, (int) buf[0], (int) buf[1],
						(int) buf[2], (int) buf[3], (int) buf[4], (int) buf[5], (int) buf[6], (int) buf[7]);
					wkirjloki(emsg);
					if (emsg1[0])
						wkirjloki(emsg1);
					}
                }
            if( ++sumerror == 10 ) {
				writeerror_w(tenerror,0);
                sumerror = 0;
					 npak = 0;
                }
			}
		if( hyvp ) {
            if (++npak > 100) {
				npak = 0;
                sumerror = 0;
                }
            s[0] = ACK;
            s[1] = cbuf->id;
            s[2] = 255 - s[1];
            s[3] = s[1];
#ifdef PROFILOI
			swprintf(profmsg, L"luepakettiUDP 5:id %d", (int)cbuf->id);
			put_profile(profmsg, cn);
#endif
            wrt_st_x_srv(cn,4,(char *)s,&nch);
#ifdef PROFILOI
            swprintf(profmsg, L"luepakettiUDP 6:nch %d", nch);
            put_profile(profmsg, cn);
#endif
            }
        else {
           s[0] = NAK;
           wrt_st_x_srv(cn, 1, (char *)s,&nch);
           }
        }
    Sleep(500);
    ts_close[cn] = 1;
#ifdef __linux
    return(0);
#endif
    }
#endif

void merk_uusinta(int cna, int cny, long ta, long ty, int autouusinta)
   {
   int n, cn, n1 = 0, n2;
   long t;
   __int64 pos;
   combufrec cbuf;

   if (autouusinta) {
      ta = -24*TUNTI;
      ty = 24*TUNTI;
		}
   EnterCriticalSection(&outb_CriticalSection);
   Sleep(1);
   for (cn = cna; cn <= cny; cn++) {
      if (!comopen[cn] || !lahcom[cn] || lahkellovain[cn] || !lahcomserver[cn])
		 continue;
	  n2 = cjens[cn]-1;
      for (n = cjens[cn]-1; n >= 0; n--) {
         pos = ((n+1)*maxyhteys+cn)*(__int64)combufsize + ((char *)&cbuf.lahetetty - (char *)&cbuf);
		 pos = _lseeki64(obfile, pos ,SEEK_SET);
		 if (pos > 0) {
			if (_read(obfile, &t, 4) != 4)
				writeerror_w(L"Virhe rutiinissa 'merk_uusinta', koodi 1", 0);
			else {
				t *= KSEK;
			   if (autouusinta) {
				  if (n1-n > 30)
					 break;
				  if (ta < -13*TUNTI && t != 0 && n < n2-1) {
					 n1 = n;
					 ty = t;
					 ta = t-240*SEK;
					 if (ta < -12*TUNTI)
						ta += 24*TUNTI;
					 }
				  }
			   if (t && ((ta <= ty && t < ta) ||
				  (ta > ty && t < ta && t > ty+3*TUNTI)))
				  break;
			   pos = _lseeki64(obfile, pos ,SEEK_SET);
			   if (pos > 0) {
				  t = 0;
				  if (_write(obfile, &t, 4) != 4) {
					 writeerror_w(L"Virhe rutiinissa 'merk_uusinta', koodi 2", 0);
					 break;
                     }
                  if (t == 0) {
                     cjens[cn]--;
                     lah_alku[cn] = 2;
                     }
                  }
               }
            }
         }
		}
   LeaveCriticalSection(&outb_CriticalSection);
   }

#if defined(TCPSIIRTO) || defined(TCPLUKIJA)

void lahetaXML_TCP(combufrec *obuf, int cn);

static void opencomTCP(int cn)
   {
   static int init[MAX_PORTTIY];
   int err = 0;
   wchar_t msg[80];

   while ((err = openportTCP(hComm+cn, ipparam+cn) && (comtype[cn] == comtpTCP)) != 0) {
	   bool poistu = false;
	   switch (err) {
		   case 98:   // yhteys jo avoinna
				return;
		   case WSAEACCES:
		   case WSAEISCONN:
		   case WSAEADDRINUSE:
				swprintf(msg, L"TCP-yhteyden %d portti %d varattu toiseen k�ytt��n\n", cn+1, ipparam[cn].srvport);
				poistu = true;
				break;
		   case WSAEADDRNOTAVAIL:
		   case WSAENETUNREACH:
				swprintf(msg, L"TCP-yhteyden %d osoite tuntematon\n", cn+1);
				poistu = true;
				break;
		   case 99:   // WSA:n avvaminen ei ole onnistunut
		   case WSAESHUTDOWN:
		   case WSAEALREADY:
				poistu = true;
				break;
		   case WSAEHOSTDOWN:
		   case WSAECONNREFUSED:
				swprintf(msg, L"TCP-yhteyden %d avaaminen ei onnistunut, yrityksia jatketaan\n", cn+1);
				break;
		   case WSAEINPROGRESS:
		   case WSAEWOULDBLOCK:
				break;
		   default :
				swprintf(msg, L"TCP-yhteyden %d avaaminen ei onnistunut\n", cn+1);
				poistu = true;
				break;
		   }
	   if (poistu)
		   break;
#ifdef _CONSOLE
	  writeerror_w(msg, 2000);
#endif
	  Sleep(4000);
	  }
	if (err)
	  return;
	if (fileyht < 0)
	  fileyht = cn;
   if (!init[cn]) {
	  InitializeCriticalSection(&tcp_CriticalSection[cn]);
      init[cn] = 1;
      }
   comopen[cn] = TRUE;

   if (lahcom[cn]) {
	  lah_alku[cn] = 1;
      if (!varaserver) 
		 lahcomserver[cn] = 1;
      }
   WRITEYHT(L"  ", cn);
   }

static void stopTCP(void)
   {
   int cn;

   for (cn = 0; cn < MAX_PORTTI; cn++)
      if (comopen[cn] && (comtype[cn] & comtpTCP)) {

#ifdef PROFILOI
		 put_profile(L"closeportTCP 1", cn);
#endif

		 closeportTCP(hComm+cn);

#ifdef PROFILOI
		 put_profile(L"closeportTCP 2", cn);
#endif

		 }
   }

void tiedonsiirtoTCP(LPVOID lpCn)
	{
	long  odaika = 0;
   char ch;
   INT er,cn;
	int maxodotus = MAXODOTUS, vkerroin = 1, edpaketti = -1;
	int inq;
	long t = 0, ednak = 0, edlahetys = 0;

	cn = *(int *)lpCn;
	if (TCPviive_lah[cn] == 0)
		TCPviive_lah[cn] = 30;
	if (TCPviive_nak[cn] == 0)
		TCPviive_nak[cn] = 1000;
	if (TCPviive_pak[cn] == 0)
		TCPviive_pak[cn] = 100;
	opencomTCP(cn);
	if (!comopen[cn])
	   return;

	while (comfl && !inLopetus) {
		if(!comopen[cn] || keyclose[cn])
			{
			yhteys_on[cn] = 0;
			WRITEYHT(L"Ei", cn);
			Sleep(1000);
			continue;
			}
		if (!TCPyht_on(hComm[cn])) {
			yhteys_on[cn] = 0;
			WRITEYHT(L"Ei", cn);
			if (comtype[cn] != comtpTCPSRV && comtype[cn] != comtpTCPSRVXML) {
				reconnectTCP(hComm[cn]);
				}
			lah_alku[cn] = 1;
			outpakid[cn] = 1;
			initid[cn] = 1;
			Sleep(1000);
			continue;
			}
		if (tcpkaynnistys[(int)hComm[cn]]) {
			lah_alku[cn] = 1;
			outpakid[cn] = 1;
			initid[cn] = 1;
			tcpkaynnistys[(int)hComm[cn]] = 0;
			}
		if (comtype[cn] == comtpTCPXML || comtype[cn] == comtpTCPSRVXML) {
			yhteys_on[cn] = 1;
			WRITEYHT(L"On", cn);
			if ((lah_alku[cn] || lahettamatta(cn)) && !com_keskeytt[cn]) {
				lahetapaketti(cn);
				}
			else {
				Sleep(100);
				}
			continue;
			}

		else {
			Sleep(50);
			if (hyvkesken[cn]) {
			  kirjaahyvStream(cn);
			  hyvkesken[cn] = 0;
			  continue;
			  }
			if (vkesken[cn]) {
			  if (luepaketteja)
				luepaketti(cn);
			  continue;
			  }
			quesize_x(cn, &inq);
			if (inq > 20000) {
				i_flush_x(cn);
				continue;
				}
		  do {
			 er = read_ch_x(cn, &ch, &inq);
			 if (er != OK && er != IN_Q_EMPTY) {                /* virhe */
				odaika = mstimer();
				yhteys_on[cn] = 0;
				WRITEYHT(L"H�", cn);
				wrt_ch_x(cn,NAK);
				er = 999;
				continue;
				}                                               /* virhe */
			 } while (!er && inq > 0 && ch != SOH && ch != ACK && ch != NAK);
		  if (er == 999)
			 continue;
		  if( er == IN_Q_EMPTY ) {                              /* tyhj� */
				ch = 0;
				if (mstimer()-odaika  >= maxodotus*1000) {
					odaika = mstimer();
					yhteys_on[cn] = 0;
					WRITEYHT(L"Ei", cn);
					}
				}                                                  /* tyhj� */
		  else {                                                /* ok */
			 odaika = mstimer();
			 switch(ch) {
				case ACK :
					yhteys_on[cn] = 1;
					WRITEYHT(naapuri[cn], cn);
					kirjaahyvStream(cn);
					break;
				case SOH :
					if (!luepaketteja)
					  break;
					yhteys_on[cn] = 1;
					WRITEYHT(naapuri[cn], cn);
					luepaketti(cn);
					if (!vkesken[cn]) {
						edlahetys = mstimer();
						ednak = edlahetys;
						}
					break;
				case NAK :
					yhteys_on[cn] = 1;
					WRITEYHT(naapuri[cn], cn);
					break;
				}
				if ((vkesken[cn] && luepaketteja) || hyvkesken[cn])
					continue;
				}                                                  /* ok */
			if ((ch == NAK || ch == ACK || (ch == 0  && (t = mstimer()) - edlahetys < 0 || t - edlahetys > vkerroin*TCPviive_pak[cn])) && !com_keskeytt[cn]) {
				if (lah_alku[cn] || (cjens[cn] != cjseur[cn])) {
					if (cjens[cn] == edpaketti || lah_alku[cn]) {
						if (vkerroin < 1000 || (lah_alku[cn] && vkerroin < 100))
							vkerroin = 2 * vkerroin;
						}
					else
						vkerroin = 1;
					lahetapaketti(cn);
					edpaketti = cjens[cn];
					edlahetys = mstimer();
					ednak = edlahetys;
					}
				else {
					lahfl[cn] = FALSE;
					}
				}
			if (!lahfl[cn] && (t = mstimer()) - ednak < 0 || t - ednak > TCPviive_nak[cn]) {
				wrt_ch_x(cn,NAK);
				ednak = t;
				}
			}
		}
	}

#endif  // TCPSIIRTO

static int lahetapaketti(int cn)
	{
   int n, nch, ret = 1;
   long t;
   __int64 pos;
   static INT32 aktpak[MAX_LAHPORTTIY];
   static INT32 edpak[MAX_LAHPORTTIY];
   static INT32 toisto[MAX_LAHPORTTIY];
   static combufrec obuf[MAX_LAHPORTTIY];
   static int init = 1;
   char buf[LUECLIBUF+1];
	combufrec *ob;
#ifdef PROFILOI
	wchar_t profmsg[30];
#endif

	if (lahfl[cn] && ((comtype[cn] & comtpUDP) || comtype[cn] == comtpTCPXML || comtype[cn] == comtpTCPSRVXML)) {
	  Sleep(10);
	  return(1);
	  }
   if (init) {
	  for (n = 0; n < maxyhteys; n++) aktpak[n] = -1;
	  init = 0;
	  }
   lahfl[cn] = TRUE;
   tiedostosanoma[cn] = false;
   if (lah_alku[cn]) {
		outpakid[cn] = 1;
		obuf[cn].pkgclass = alkut;
		obuf[cn].len = class_len[alkut];
		alkusanoma(obuf+cn, cn, 0);
		}
   else {
		if (!lahcomserver[cn]) {
			buf[0] = outpakid[cn];
			buf[1] = 255-buf[0];
			buf[2] = buf[0];
			kirjaahyv(cn, (char *) buf);
#ifdef PROFILOI
			swprintf(profmsg, L"lahetapaketti V:id %d", (int)outpakid[cn]);
			put_profile(profmsg, cn);
#endif
			lahfl[cn] = FALSE;
			return(1);
			}
#ifdef PROFILOI
	  swprintf(profmsg, L"lahetapaketti 1:id %d", (int)outpakid[cn]);
	  put_profile(profmsg, cn);
#endif
	  if (lah_tied_seur[cn]) {
		  if (get_tied_sanoma(cn, &obuf[cn])) {
			  lahfl[cn] = FALSE;
			  return(1);
			  }
		  tiedostosanoma[cn] = true;
		  }
	  else {
		  EnterCriticalSection(&outb_CriticalSection);
		  if (levypusk) {
			 if (cjens[cn] != aktpak[cn]) {
				pos = _lseeki64(obfile, ((cjens[cn]+1)*(__int64)maxyhteys+cn)*combufsize,
				   SEEK_SET);
				if (pos > 0) {
				   if (_read(obfile, obuf+cn, combufsize) != combufsize)
					   writeerror_w(L"Virhe rutiinissa 'lahetapaketti', koodi 1", 0);
				   }
				else {
				   writeerror_w(L"Virhe rutiinissa 'lahetapaketti', koodi 2", 0);
				   }
				aktpak[cn] = cjens[cn];
				}
			if (obuf[cn].pkgclass == 0 && cjens[cn] < cjseur[cn]) {
				pos = ((cjens[cn]+1)*maxyhteys+cn+1)*(__int64)combufsize - 6;
				pos = _lseeki64(obfile, pos ,SEEK_SET);
				if (pos > 0) {
					t = t_time_l(biostime(0, 0), t0);
					if (t == 0) t = 1;
					if (_write(obfile, &t, 4) != 4)
						writeerror_w(L"Virhe rutiinissa 'lahetapaketti', koodi 3", 0);
					}
				++cjens[cn];
				LeaveCriticalSection(&outb_CriticalSection);
				lahfl[cn] = 0;
				return(1);
				}
			 }
		  else {
			  ob = (combufrec *) ((char *)outbuf[cn] + (int)cjens[cn]*combufsize);
			  memcpy(&obuf[cn].pkgclass, &ob->pkgclass, ob->len+5);
			 }
		  LeaveCriticalSection(&outb_CriticalSection);
		  }
	  }
   if (comtype[cn] == comtpTCPXML || comtype[cn] == comtpTCPSRVXML) {
	  lahetaXML_TCP(obuf+cn, cn);
		return(1);
      }
   obuf[cn].so = SOH;
   obuf[cn].id = outpakid[cn];
   obuf[cn].iid = 255 - outpakid[cn];
   obuf[cn].checksum =
      chksum((char *)&obuf[cn].d, class_len[obuf[cn].pkgclass]);
#ifdef PROFILOI
   swprintf(profmsg, L"lahetapaketti 2:id %d", (int)outpakid[cn]);
   put_profile(profmsg, cn);
#endif
   wrt_st_x(cn,obuf[cn].len+8, (char *)&obuf[cn].so,&n);
#ifdef PROFILOI
   swprintf(profmsg, L"lahetapaketti 3:id %d", (int)outpakid[cn]);
   put_profile(profmsg, cn);
#endif
   if (comtype[cn] == comtpRS)
      Sleep(n*chcomkesto[cn]);
#ifdef UDPYHTEYS
   if (comtype[cn] & comtpTCP) {
         Sleep(TCPviive_lah[cn]);
		}
   if (comtype[cn] & comtpUDP) {
		if (toisto[cn] < 3)
			toisto[cn]++;
		if (aktpak[cn] != edpak[cn]) {
			toisto[cn] = 1;			
			edpak[cn] = aktpak[cn];
			}
	  if (UDPvarmistamaton[cn] & 1) {
           buf[0] = outpakid[cn];
           buf[1] = 255-buf[0];
           buf[2] = buf[0];
           kirjaahyv(cn, buf);
		   }
	  else {	
		  for (n = 0; n < 3*toisto[cn]; n++) {
			 Sleep(UDPviive_lah+n*50);
#ifdef PROFILOI
				put_profile(L"lahetapaketti 4", cn);
#endif
				read_UDPcli(hComm[cn], 4, buf, &nch);

			 if (!luepaketteja) {
				nch = 0;
				break;
				}
#ifdef PROFILOI
			 swprintf(profmsg, L"lahetapaketti 5:n %d, nch: %d", n, nch);
			 put_profile(profmsg, cn);
#endif
			  if (nch) {
					if (buf[0] == ACK && nch >= 4) {
						ret = kirjaahyv(cn, (char *)(buf+1));
#ifdef PROFILOI
					 swprintf(profmsg, L"lahetapaketti 6:n %d, hyv: %d", n, ret);
					   put_profile(profmsg, cn);
#endif
						if (ret == 1) {
							n = 0;
							continue;
							}
						}
					break;
					}
				}
          }
	  lahfl[cn] = 0;
      }
#endif
	return(ret);
	}

static int kirjaahyv(int cn, char *id)
    {
    int  er = 0;
    long t;
    __int64 pos;

	if (id[0] + id[1] == 255 && id[0] == id[2]) {
		EnterCriticalSection(&outb_CriticalSection);
		if (lahfl[cn] && (lah_alku[cn] || lahettamatta(cn))) {
			if (id[0] == outpakid[cn]) {
				if (lah_alku[cn]) {
					lah_alku[cn] = 0;
					lah_vaihto[cn] = 0;
					}
				else if (tiedostosanoma[cn]) {
					kirjaa_tiedhyv(cn);
					}
				else {
					if (levypusk) {
						if (cjens[cn] < cjseur[cn]) {
							pos = ((cjens[cn]+1)*maxyhteys+cn)*(__int64)combufsize;
							pos = _lseeki64(obfile, pos ,SEEK_SET);
							if (pos > 0) { 
								t = t_time_l(biostime(0, 0), t0);
								if (t == 0) 
									t = 1;
								if (_write(obfile, &t, 4) != 4)
									writeerror_w(L"Virhe rutiinissa 'kirjaahyv', koodi 1", 0);
								}
							cjens[cn]++;
							}
						else
							er = -1;
						}
					else
						cjens[cn] = (cjens[cn] + 1) % OUTBUFL;
					}
				outpakid[cn]++;
				}
			else
				er = (id[0]+'\x01' == outpakid[cn]) ? 1 : -1;
			}
		else
			er = -1;
		LeaveCriticalSection(&outb_CriticalSection);
		}
	else
		er = -1;
	DSPQ(ySize-6, cn);
	lahfl[cn] = FALSE;
	return(er);
}

static int kirjaahyvStream(int cn)
   {
   char id[3];
   INT  i,er, nch, inq;

	for (i = 0; i < 20; i++) {
		if ((er = quesize_x(cn,&inq)) != OK)
			return(1);
		if (inq >= 3)
			break;
		Sleep(10);
      }
   if (inq < 3 && !hyvkesken[cn]) {
      hyvkesken[cn] = 1;
		return(0);
		}
   hyvkesken[cn] = 0;
   if (comtype[cn] & comtpTCP) {
	  peek_TCP(hComm[cn], 3, id, &nch);
      if (id[0] + id[1] != 255 || id[0] != id[2]) {
		  return(0);
		  }
	  }
	er = read_st_x(cn,3,id,&nch,&inq);
	if (er == OK && nch == 3)
	  kirjaahyv(cn, (char *) id);
   return(er);
   }

//DWORD WINAPI tiedonsiirto(LPVOID lpCn)
void tiedonsiirto(LPVOID lpCn)
{
	INT  odaika = 0;      /* odotusajan laskuri */
	INT juurilahetetty = 0;
	char ch;
	INT  outq;
	INT er,cn;
	int inq, count;
	long edaika = 0;

	cn = *(int *)lpCn;
	if (comtype[cn] != comtpRS)
		return;
	opencomRS(cn);
	if (!comopen[cn])
		return;
	if (comkesto[cn] > 17)
		chcomkesto[cn] = (80*(comkesto[cn]-17))/200;
	else
		chcomkesto[cn] = 1;
	while (comfl) {
		if(!comopen[cn] || keyclose[cn])
			{
			Sleep(luepaketteja ? 1000 : 10);
			continue;
			}
		outq = 1;
		for (count = 0; count < 5; count++) {
			outquesize_x(cn, &outq);
			if (!outq)
				break;
			if (count && !luepaketteja)
				break;
			Sleep(outq*chcomkesto[cn]);
			}
		if (outq) {
			yhteys_on[cn] = 0;
			WRITEYHT(L"H�", cn);
			continue;
			}
		Sleep(50);
		if (hyvkesken[cn]) {
			kirjaahyvStream(cn);
			hyvkesken[cn] = 0;
			continue;
			}
		if (vkesken[cn]) {
			if (luepaketteja)
				luepaketti(cn);
			continue;
			}

			//       if ((tt = mstimer()) - edaika < lahvali && tt >= edaika)
			//          continue;

		do {
			er = read_ch_x(cn, &ch, &inq);
			if (er != OK && er != IN_Q_EMPTY) {                /* virhe */
				odaika = 0;
				i_flush_x(cn);
				o_flush_x(cn);
				yhteys_on[cn] = 0;
				WRITEYHT(L"H�", cn);
				wrt_ch_x(cn,NAK);
				er = 999;
				continue;
				}                                               /* virhe */
			} while (inq > 0 && ch != SOH && ch != ACK);
		if (er == 999)
			continue;
		if( er == IN_Q_EMPTY ) {                              /* tyhj� */
			if( ++odaika >= MAXODOTUS ) {
				odaika = 0;
				o_flush_x(cn);
				wrt_ch_x(cn,NAK);
				yhteys_on[cn] = 0;
				WRITEYHT(L"Ei", cn);
				}
			}                                                  /* tyhj� */
		else {                                                /* ok */
			odaika = 0;
			switch(ch) {
				case ACK :
					yhteys_on[cn] = 1;
#ifdef KONETUNN
					WRITEYHT(naapuri[cn], cn);
#else
					WRITEYHT(L"On", cn);
#endif
					juurilahetetty = 0;
					kirjaahyvStream(cn);
					break;
				case SOH :
					if (!luepaketteja)
						break;
					yhteys_on[cn] = 1;
#ifdef KONETUNN
					WRITEYHT(naapuri[cn], cn);
#else
					WRITEYHT(L"On", cn);
#endif
					juurilahetetty = 0;
					luepaketti(cn);
					break;
				case NAK :
					if (!luepaketteja)
						break;
					yhteys_on[cn] = 1;
#ifdef KONETUNN
					WRITEYHT(naapuri[cn], cn);
#else
					WRITEYHT(L"On", cn);
#endif
					if (juurilahetetty) {
						juurilahetetty--;
						break;
						}
					if ((lah_alku[cn] || lahettamatta(cn)) && !com_keskeytt[cn]) {
						lahetapaketti(cn);
						juurilahetetty = 2;
						}
					else {
						lahfl[cn] = FALSE;
						}
					break;
				}
			if( !lahfl[cn] && ch != SOH ) {
				i_flush_x(cn);
				o_flush_x(cn);
				wrt_ch_x(cn,NAK);
				}
			}                                                  /* ok */
		}
}

#ifdef UDPYHTEYS

int udpjarru, udpnyt;

//DWORD WINAPI tiedonsiirtoUDP(LPVOID lpCn)
#ifndef __linux__
void tiedonsiirtoUDP(LPVOID lpCn)
#else
void *tiedonsiirtoUDP(LPVOID lpCn)
#endif
    {
    int cn;
    int yhtcount = 0;
    long t, edaika = 0;

	cn = *(int *)lpCn;
    while (comfl && !ts_close[cn]) {
       if(!comopen[cn] || keyclose[cn])
			{
 	    	 Sleep(300);
		    continue;
    	    }
	   if (yhtfl[cn] > 0) {
           yhtcount = 0;
		   yhteys_on[cn] = 1;
#ifdef KONETUNN
			  if (*UDPnaapuri(hComm[cn]))
				  ansitowcs(naapuri[cn], UDPnaapuri(hComm[cn]), 3);
		   WRITEYHT(naapuri[cn], cn);
#else
		   WRITEYHT(L"On", cn);
#endif
           }
       else {
           if (yhtcount > 10) {
			  yhteys_on[cn] = 0;
              WRITEYHT(L"Ei", cn);
			  }
           if (yhtcount > 50) {
              Sleep(1000);
				  yhtcount = 0;
				  }
           else
			  yhtcount++;
           }
//		 if (!luepaketteja) {
//			 Sleep(50);
//          continue;
//			 }
		 if (udpjarru && !udpnyt && (t = mstimer()) > edaika && t < edaika + udpjarru) {
			 Sleep(50);
			 continue;
			 }
		 if ((lah_alku[cn] || lahettamatta(cn)) && !com_keskeytt[cn]) {
			 if (!lahetapaketti(cn) && yhtcount > 0)
				 yhtcount = 0;
			 udpnyt = 0;
          edaika = mstimer();
          }
       else {
          if ((t = mstimer()) < edaika || t > edaika + nakviive) {
			 if (comtype[cn] == comtpUDP) {

#ifdef PROFILOI
                put_profile(L"tiedonsiirtoUDP 1", cn);
#endif

                wrt_ch_x(cn,NAK);
					 if (yhtfl[cn] > 0)
						 yhtfl[cn]--;

#ifdef PROFILOI
                put_profile(L"tiedonsiirtoUDP 2", cn);
#endif

					 }
             edaika = t;
			 }
          else
             Sleep(UDPviive_ts);
          }
	   }

#ifdef __linux__
	return(0);
#endif
   }
#endif

void closec(int cn)
	{
    int i, cfl = 0;

    if (comopen[cn]) {
        comopen[cn] = FALSE;
        for (i = 0; i < max_portti; i++)
            if (comopen[i])
               cfl = TRUE;
        if (!cfl)
            comfl = 0;
        if (cn < 4)
           WRITEYHT(L"Su", cn);
		if (!(comtype[cn] & comtpUDP))
		   closeport_x(cn);
		if (comtype[cn] & comtpTCP)
			  DeleteCriticalSection(&tcp_CriticalSection[cn]);
		 }
	 }

void closecom(void)
	{
	int cn;

	for (cn = 0; cn < max_portti; cn++)
		if (comopen[cn])
			break;
	if (cn >= max_portti)
		return;
	luepaketteja = 0;
	Sleep(1500);
	stopUDP();
	stopTCP();
	Sleep(1000);
	comfl = 0;
	Sleep(1000);
	for (cn = 0; cn < max_portti; cn++) {
		closec(cn);
		}
	EnterCriticalSection(&outb_CriticalSection);
//	if (levypusk && fobfile) {
//		fclose(fobfile);
//		_close(obfile);
//		}
	LeaveCriticalSection(&outb_CriticalSection);
#ifdef DBGXX
	 fclose(dfl);
#endif
	}

#if defined(SIIMPORT)
static void opencomTCPin(int cn)
	{
	static init[MAX_PORTTIY];
	wchar_t msg[80];

	if (openportTCP(hComm+cn, ipparam+cn)) {
		swprintf(msg, L"TCP-yhteyden %d avaaminen ei onnistunut, yrityksia jatketaan\n", cn+1);
#ifdef _CONSOLE
		writeerror_w(msg, 2000);
#else
		Sleep(4000);
#endif
		}
	if (!init[cn]) {
		InitializeCriticalSection(&tcp_CriticalSection[cn]);
		init[cn] = 1;
		}
	comopen[cn] = TRUE;

	WRITEYHT(L"  ", cn);
	}

INT tall_regnly(char *vastaus, int r_no);

#ifndef __linux__
void luepakettiTCPSI(LPVOID lpCn)
#else
void *luepakettiTCPSI(LPVOID lpCn)
#endif
	{
	char buf[LUEBUF+2], *p, fbuf[100];
	int cn, nch, nbuf = 0, len;
	FILE *sifile;
	wchar_t sifilenm[60] = L"sifile.txt";
#ifdef PROFILOI
	wchar_t profmsg[30];
#endif

	cn = *(int *)lpCn;
	opencomTCPin(cn);
	sifile = fopen(sifilenm, L"ab");
	if (!comopen[cn])
#ifndef __linux__
		 return;
#else
		 return(0);
#endif
	while (comfl && luepaketteja && !keyclose[cn]) {
		if (!comopen[cn] || keyclose[cn]) {
			Sleep(300);
			continue;
			}
#ifdef PROFILOI
		put_profile(L"luepakettiTCP 1", cn);
#endif
		read_TCP(hComm[cn], LUEBUF-nbuf, buf+nbuf, &nch);
		nbuf += nch;

		for (;;) {
			for (p = buf; p < buf+nbuf; p++)
				if (*p == '\r' || *p == '\n')
					break;
			if (*p != '\r' && *p != '\n')
				break;
			*p = 0;
			if (p-buf > 22 && !memcmp(buf, "dt\t", 3)) {
				if (sifile) {
					len = p-buf;
					if (len > sizeof(fbuf)-3)
						len = sizeof(fbuf)-3;
					memcpy(fbuf, buf, len);
					strcpy(fbuf+len, "\r\n");
					fputs(fbuf, sifile);
					}
				tall_regnly(buf+3, -1);
				}
			memmove(buf, p+1, buf+nbuf-p);
			nbuf -= p+1-buf;
			if (nbuf < 23)
				break;
			}
		}
	wrt_st_TCP(hComm[cn], 5, "exit\n", &nch);
	Sleep(500);
	fclose(sifile);
   ts_close[cn] = 1;
#ifdef __linux
   return(0);
#endif
   }

void si_tcpimport(void)
	{
	static init = 0;
	int nch;
	wchar_t ch;

	if (!init) {
		clrln(ySize-3);
      viwrrect(ySize-3,0,ySize-3,31,
         L"Anna maalikellon k�ynnistysaika:",7,0,0);
      INPUTAIKAW(&t0_regnly,t0,8,50,ySize-3,L"\xD",&ch);
      maaliajat[9] = t0_regnly;
		init = 1;
		}
	ch = L' ';
	wselectopt(L"A)vaa yhteys, K)aikki tiedot, J)atkuva, S)ulje, T)iedosto", L"AKJLST\x1b", &ch);
	switch (ch) {
		case L'A':
			keyclose[siimport] = 0;
			opencom(siimport, 0,0,0,0);
			break;
		case L'K':
			if (comopen[siimport] && !keyclose[siimport])
				wrt_st_TCP(hComm[siimport], 11, "gaterepeat\n", &nch);
			else
				writeerror_w(L"Yhteystt� ei ole avattu", 0);
			break;
		case L'J':
			if (comopen[siimport] && !keyclose[siimport])
				wrt_st_TCP(hComm[siimport], 16, "gatelisten\ttrue\n", &nch);
			else
				writeerror_w(L"Yhteystt� ei ole avattu", 0);
			break;
		case L'L':
			if (comopen[siimport] && !keyclose[siimport])
				wrt_st_TCP(hComm[siimport], 17, "gatelisten\tfalse\n", &nch);
			else
				writeerror_w(L"Yhteystt� ei ole avattu", 0);
			break;
		case L'S':
			if (comopen[siimport] && !keyclose[siimport]) {
				wrt_st_TCP(hComm[siimport], 5, "exit\n", &nch);
				keyclose[siimport] = 1;
				Sleep(5000);
				closec(siimport);
				}
			break;
		case L'T':
			importsifile();
			break;
		}
	}
#endif

#if defined(TCPLUKIJA) || defined(LAJUNEN)
void openlukijaUDP(int cn)
	{
	wchar_t msg[80];
	int err =  0;

	if (!inLopetus && (comtype[cn] & comtpUDP) && (err = openportUDP(hComm+cn, ipparam+cn)) != 0) {
		bool poistu = false;
		switch (err) {
		    case WSAEACCES:
			case WSAEISCONN:
			case WSAEADDRINUSE:
				swprintf(msg, L"Laitteen %d UDP-portti %d varattu toiseen k�ytt��n\n",
					cn+1-max_lahportti, ipparam[cn].srvport);
				poistu = true;
				break;
			case WSAEADDRNOTAVAIL:
			case WSAENETUNREACH:
				swprintf(msg, L"Laitteen %d UDP-osoite tuntematon\n", cn+1-max_lahportti);
				poistu = true;
				break;
			case WSAESHUTDOWN:
			case WSAEALREADY:
				poistu = true;
				break;
			case WSAEINPROGRESS:
			case WSAEWOULDBLOCK:
				break;
			default :
				swprintf(msg, L"Laitteen %d UDP-yhteyden avaaminen ei onnistunut\n", cn+1-max_lahportti);
				poistu = true;
				break;
			}
		if (poistu) {
			regnly_open[cn+1-max_lahportti] = 0;
			}
		writeerror_w(msg, 4000);
		return;
		}
	comtype[cn] = comtpUDPSTREAM;
	comopen[cn] = 1;
	}

int openlukijaTCP(int cn)
   {
   wchar_t msg[80];
   static int count = 0;
   int err = 0;
   int poistu = 0;

   if ((err = openportTCP(hComm+cn, ipparam+cn)) != 0) {
	   switch (err) {
		   case 98:   // yhteys jo avoinna
				return(0);
		   case WSAEACCES:
		   case WSAEISCONN:
		   case WSAEADDRINUSE:
				swprintf(msg, L"Laitteen %d TCP-portti %d varattu toiseen k�ytt��n\n",
					cn+1-max_lahportti, ipparam[cn].srvport);
				poistu = 1;
				break;
		   case WSAEADDRNOTAVAIL:
		   case WSAENETUNREACH:
				swprintf(msg, L"Laitteen %d TCP-osoite tuntematon\n", cn+1-max_lahportti);
				poistu = 1;
				break;
		   case 99:   // WSA:n avvaminen ei ole onnistunut
		   case WSAESHUTDOWN:
		   case WSAEALREADY:
				poistu = 1;
				break;
		   case WSAEHOSTDOWN:
		   case WSAECONNREFUSED:
				swprintf(msg, L"Laitteen %d TCP-yhteyden avaaminen ei onnistunut, yrityksia jatketaan\n", cn+1-max_lahportti);
				break;
		   case WSAEINPROGRESS:
		   case WSAEWOULDBLOCK:
				break;
		   default :
				swprintf(msg, L"Laitteen %d TCP-yhteyden avaaminen ei onnistunut\n", cn+1-max_lahportti);
				poistu = 1;
				break;
		   }
	   writeerror_w(msg, 2000);
	   if (poistu) {
		   regnly_open[cn+1-max_lahportti] = 0;
		   return(poistu);
		   }
	   else {
			Sleep(5000);
#ifdef EI_OLE
			virhesanoma.on = false;
			virhesanoma.kesto = 2000;
			swprintf(virhesanoma.msg, L"Lukijan %d TCP-yhteyden avaaminen ei onnistunut\n"
				 , cn+1-max_lahportti);
			virhesanoma.on = true;
			Sleep(7000);
#endif
			count++;
			if (count > 5)
				Sleep(55000);
			if (count > 10)
				Sleep(180000);
			}
		}
	else {
		count = 0;
		comopen[cn] = 1;
		}
	return(poistu);
	}
#endif

void opencom(int cn, int baud, char parity, int bits, int sbits)
	 {
	static int init;
	int  i;

	 if (!init) {
		  for (i = 1; i < max_portti; i++)
				acn[i] = i;
		  init = 1;
		  }
#ifdef PROFILOI
	 open_profile();
#endif
	 comfl = TRUE;
	if (comtype[cn] == comtpRS) {
		comparam[cn].baud = baud;
		comparam[cn].parity = parity;
		comparam[cn].bits = bits;
		comparam[cn].sbits = sbits;
		}
	if (cn < maxyhteys) {
		if (comtype[cn] == comtpRS) {
			 hTsThread[cn] = _beginthread(tiedonsiirto, 40960, acn+cn);
			 }
#ifdef UDPYHTEYS
		 if (comtype[cn] & comtpUDP) {
			 hLuepakThread[cn] = _beginthread(luepakettiUDP, 40960, acn+cn);
			 }
#endif
#ifdef SIIMPORT
		 if (comtype[cn] ==  comtpTCPIMPORT && !lahcom[cn]) {
			 hLuepakThread[cn] = _beginthread(luepakettiTCP, 40960, acn+cn);
			 }
#endif
#ifdef TCPSIIRTO
		 if (comtype[cn] & comtpTCP) {
			hTsThread[cn] = _beginthread(tiedonsiirtoTCP, 40960, acn+cn);
			}
#endif
		 }
#if NREGNLY > 0
	 else {
		 hLuepakThread[cn] = _beginthread(lue_regnlyThread, 40960, acn+cn);
		 }
#endif
	}

static void openTCPloki(int cn)
{
	int err, count = 0;

	while ((err = openportTCP(&hLoki, ipparam+cn)) != 0 && ++count < 5) {
		Sleep(300);
		}
	if (err != 0) {
		wchar_t msg[100];

		swprintf(msg, L"TCP-lokin k�ynnistys ei onnistunut (%d)", err);
		writeerror_w(msg, 2000, true);
		lokiTCP = 0;
		}
	else {
		vidspwmsg(ySize-8, 0, 7, 0, L"TCP-loki k�ynnistetty");
		lokiTCP = 1;
		}  
}

int opencomfile(int vaihto)
{
	combufrec *ob;
	wchar_t ch, msg[90];
	char obfbuf[MAX_LAHPORTTI*sizeof(combufrec)];
	__int64 l;
	int f_ok = 0;
	int lah = 0;
	int av = 0;

	if (obfile > -1)
		_close(obfile);
	obfile = _wopen(obfilename, O_CREAT | O_RDWR | O_BINARY,
		S_IREAD | S_IWRITE);
	if (obfile < 0)
	   writeerror_w(L"Virhe rutiinissa 'opencomfile'. Tiedoston avaaminen ei onnistu ", 0);
	else {
		if (_lseeki64(obfile, 0, SEEK_END) > 0) {
			_lseeki64(obfile, 0, SEEK_SET);
			for (int i = 0;; i++) {
				if ((l = _read(obfile, obfbuf, maxyhteys*combufsize)) < combufsize)
					break;
				if (i == 0) {
					ob = (combufrec *)obfbuf;
					if (ob->so != maxyhteys || ob->len != combufsize ||
						l < maxyhteys*combufsize) {
						if (ob->so <= MAX_LAHPORTTI && ob->so > maxyhteys && ob->len == combufsize) {
							maxyhteys = ob->so;
						}
						else {
							if (_lseeki64(obfile, 0, SEEK_END) == (int)ob->so * (int)ob->len) {
								_lseeki64(obfile, 0, SEEK_SET);
								ch = L'T';
							}
							else {
								swprintf(msg, L"Aiempi tiedosto COMFILE.DAT ei yhteensopiva: MAXYHTEYS=%d, tietuekoko %d",
									(int)ob->so, (int)ob->len);
#ifdef _CONSOLE
								vidspwmsg(24, 0, 0, 7, msg);
								ch = L' ';
								wselectopt(L"T)uhoa aiempi COMFILE.DAT tai K)eskeyt� ohjelman k�ynnistys", L"TK", &ch);
								if (ch == L'K')
									return(1);
#else
								if (select3(2, msg, L"Yhteensopimattomat asetukset", L"Tuhoa aiempi COMFILE.DAT",
									L"Keskeyt� ohjelman k�ynnistys", L"", 0) == 2)
									return(1);
#endif
							}
							av = -1;
							break;
						}
					}
					l = _lseeki64(obfile, maxyhteys*combufsize, SEEK_SET);
					continue;
				}
				if (l < maxyhteys*(__int64)combufsize)
					break;
				for (int j = 0; j < maxyhteys; j++) {
					ob = (combufrec *)(obfbuf + j*combufsize);
					if (ob->len > 0 &&
						ob->len == class_len[ob->pkgclass]) {
						if (cjseur[j] == i - 1) cjseur[j] = i;
						if (ob->lahetetty != 0 && cjens[j] == i - 1)
							cjens[j] = i;
					}
				}
			}
		}
		for (int j = 0; j < maxyhteys; j++) {
			lah += cjens[j];
			av += cjseur[j];
		}
		if (av < 0 || (av && levypusk == 1 && !vaihto)) {    // COMFILE=S => levypusk == 2
			if (av > 0) {
				av -= lah;
#ifdef _CONSOLE
				swprintf(msg, L"Tiedostossa COMFILE %ld l�hetetty� ja"
					L" %ld l�hetett�v�� tietoa S)�ilyt� T)uhoa", lah, av);
				ch = L' ';
				wselectopt(msg, L"ST", &ch);
#else
				swprintf(msg, L"Tiedostossa COMFILE %ld l�hetetty� ja"
					L" %ld l�hetett�v�� tietoa. S�ilytet���nk�?", lah, av);
				if (Application->MessageBoxW(msg, L"Tietojen s�ilytt�minen", MB_YESNO) == IDYES)
					ch = L'S';
				else
					ch = L'T';
#endif
				}
			if (av < 0 || ch == L'T') {
				_close(obfile);
				obfile = _wopen(obfilename,
					O_CREAT | O_RDWR | O_BINARY | O_TRUNC,
					S_IREAD | S_IWRITE);
				if (obfile < 0)
					writeerror_w(L"Virhe 2 rutiinissa 'opencomfile'. Tiedoston avaaminen ei onnistu ", 0);
				for (int j = 0; j < maxyhteys; j++) {
					cjens[j] = 0;
					cjseur[j] = 0;
				}
			}
		}
		//               fobfile = _fdopen(obfile, L"r+b");
		//               setvbuf(fobfile, NULL, _IONBF, 0);
		if (_lseeki64(obfile, 0, SEEK_END) == 0) {
			memset(obfbuf, 0, maxyhteys*combufsize);
			ob = (combufrec *)obfbuf;
			ob->so = maxyhteys;
			ob->len = combufsize;
			if (_write(obfile, obfbuf, maxyhteys*combufsize) < maxyhteys*combufsize)
			   writeerror_w(L"Virhe rutiinissa 'opencomfile', koodi 3 ", 0);
		}
		f_ok = 1;
	}
	return(f_ok);
}

int initcom(int cn)
	{
	int ok = 1;
	static int init = 1, f_ok = 0;
	static int tarkcomstarted;
	//   DWORD IDThread;

	if (comtype[cn] == comtpTCPLOKI) {
		openTCPloki(cn);
		return(0);
		}
    EnterCriticalSection(&outb_CriticalSection);
	initid[cn] = 1;
	outpakid[cn] = 1;
	if (init) {
#ifdef DBGXX
		dfl = fopen(L"comdump", L"wb");
		memset(dflb, 0, sizeof(dflb));
#endif
//		initmstimer();
		init_class_len();
		if (*konetunn)
			UDPsetkonetunn(konetunn);
			ok = (inbuf = (combufrec *) calloc((INBUFL+1), sizeof(combufrec))) != 0;
		}
	if (ok && (combuf[cn] = (char *) malloc(INQSIZE+OUTQSIZE+10)) != NULL) {
		if (levypusk) {
			if (init) {
				f_ok = opencomfile(0);
				}
			}
		else {              // Ei levypuskurointia
			f_ok = 1;
			if ((outbuf[cn] = (combufrec *)calloc(OUTBUFL,combufsize)) == NULL)
				ok = 0;
			}
		if (ok && f_ok) {
			opencom(cn, baud[cn], pty[cn], combits[cn], stopbits[cn]);
			if (comfl && !tarkcomstarted) {
				hTarkcomThread = _beginthread(tarkcom, 40960, NULL);
				tarkcomstarted = 1;
				}
			}
		else
			ok = 0;
		}
	if (!ok) writeerror_w(L"Muisti ei riit� tiedonsiirtopuskurille",0);
		vastcom0[cn] = vastcom[cn];
	init = 0;
	LeaveCriticalSection(&outb_CriticalSection);
	return(0);
	}

#ifdef SERVER

void yhteysasetukset(void)
   {
   wchar_t ch = L' ', ch2, lj, msg[81];
   INT yno = 0, ala, yla;
   int x;

   wselectopt(L"Muuta Y)ksi yhteys, K)aikki yhteydet, V)iiveet, Esc: peruuta", L"YKV\x1b", &ch);
   if (ch == ESC)
	  return;
	if (ch == L'V') {
		ch2 = L' ';
		wselectopt(L"U)DP, T)CP", L"UT", &ch2);
		if (ch2 == L'T') {
			vidspwmsg(ySize-3, 0, 7, 0, L"Yhteyden numero         .  Esc: peruuta");
			INPUTINTW(&yno, 3, 17, ySize-3, L"\r\x1b", &ch);
			if (ch == ESC || yno < 1 || yno > maxyhteys) 
				return;
			wselectopt(L"K)�ttely, KuittausO)dotus, S)anomav�li, Esc: peruuta", L"KOS\x1b", &ch);
			clrln(ySize-3);
			switch (ch) {

				case L'S':
					x = TCPviive_pak[yno-1];
					vidspwmsg(ySize-3, 0, 7, 0, L"Per�kk�isten sanomien v�li           ms.  Esc: peruuta");
					INPUTINTW(&x, 7, 28, ySize-3, L"\r\x1b", &ch);
					if (ch != ESC) {
						TCPviive_pak[yno-1] = x;
						}
					break;
				case L'K':
					x = TCPviive_nak[yno-1]/1000;
					vidspwmsg(ySize-3, 0, 7, 0, L"K�ttelysanomien v�li         s.  Esc: peruuta");
					INPUTINTW(&x, 4, 23, ySize-3, L"\r\x1b", &ch);
					if (ch != ESC) {
						TCPviive_nak[yno-1] = 1000*x;
						}
					break;
				case L'O':
					x = TCPviive_lah[yno-1];
					vidspwmsg(ySize-3, 0, 7, 0, L"Kuittauksen odotus       ms.  Esc: peruuta");
					INPUTINTW(&x, 3, 19, ySize-3, L"\r\x1b", &ch);
					if (ch != ESC) {
						TCPviive_lah[yno-1] = x;
						}
					break;
				}
			return;
			}
		else {
		ch = L' ';
	   wselectopt(L"K)�ttely, KuittausO)dotus, UDP T)ime-out, V)ast.otto, L)�hetys, Esc: peruuta", L"JYKTOVL\x1b", &ch);
		clrln(ySize-3);
      switch (ch) {
			case L'Y':
				udpnyt = 1;
				break;
			case L'J':
		      vidspwmsg(ySize-3, 0, 7, 0, L"Per�kk�isten sanomien v�li           ms.  Esc: peruuta");
				INPUTINTW(&udpjarru, 7, 28, ySize-3, L"\r\x1b", &ch);
				break;
			case L'K':
		      vidspwmsg(ySize-3, 0, 7, 0, L"K�ttelysanomien viive        ms.  Esc: peruuta");
				INPUTINTW(&nakviive, 4, 23, ySize-3, L"\r\x1b", &ch);
				break;
			case L'T':
		      vidspwmsg(ySize-3, 0, 7, 0, L"UDP-kuittauksen time-out       ms.   Esc: peruuta");
				INPUTINTW(&UDPCliWait, 4, 26, ySize-3, L"\r\x1b", &ch);
				if (ch == L'\r')
					UDPsetCliWait(UDPCliWait);
				break;
			case L'V':
		      vidspwmsg(ySize-3, 0, 7, 0, L"Vastaanoton viive         ms.  Esc: peruuta");
				INPUTINTW(&UDPviive_lue, 3, 19, ySize-3, L"\r\x1b", &ch);
				break;
			case L'L':
		      vidspwmsg(ySize-3, 0, 7, 0, L"L�hetysv�li        ms.  Esc: peruuta");
				INPUTINTW(&UDPviive_ts, 3, 12, ySize-3, L"\r\x1b", &ch);
				break;
			case L'O':
		        vidspwmsg(ySize-3, 0, 7, 0, L"Kuittauksen odotus       ms.  Esc: peruuta");
				INPUTINTW(&UDPviive_lah, 3, 19, ySize-3, L"\r\x1b", &ch);
				break;
			}
		return;
		}
	}
   if (ch == L'Y') {
      clrln(ySize-3);
      vidspwmsg(ySize-3, 0, 7, 0, L"Anna yhteyden numero         Esc: peruuta");
      INPUTINTW(&yno, 2, 22, ySize-3, L"\r\x1b", &ch);
      ala = yno-1;
      yla = yno-1;
	  }
   else {
      yno = -1;
      ala = 0;
      yla = maxyhteys-1;
      }
   if (ch == ESC)
      return;
   lj = L' ';
   wselectopt(L"AVAA: 1:l�htev� 2:saapuva 3:molemmat  SULJE: 4:l�htev� 5:saapuva 6:molemmat",
      L"123456\x1b", &lj);
   switch (lj) {
      case ESC:
         return;
	  case L'1':
         wcscpy(msg, L"Avaa l�htev� yhteys: ");
         break;
      case L'2':
         wcscpy(msg, L"Avaa saapuva yhteys: ");
         break;
      case L'3':
         wcscpy(msg, L"Avaa l�htev� ja saapuva yhteys: ");
         break;
      case L'4':
         wcscpy(msg, L"Sulje l�htev� yhteys: ");
         break;
      case L'5':
         wcscpy(msg, L"Sulje saapuva yhteys: ");
         break;
		case L'6':
         wcscpy(msg, L"Sulje l�htev� ja saapuva yhteys: ");
         break;
      }
   if (yno < 0)
      wcscat(msg, L"kaikki  - Vahvista (K/E)");
   else
      swprintf(msg+wcslen(msg), L"%d  - Vahvista (K/E)", yno);
	ch = L' ';
	wselectopt(msg, L"KE", &ch);
   if (ch == L'E')
      return;
   for (yno = ala; yno <= yla; yno++) {
#ifdef SIIMPORT
		if (yno == siimport) {
			if (lj == L'2' || lj == L'3') {
				int nch;

				keyclose[siimport] = 0;
				comfl = 1;
				opencom(siimport, 0,0,0,0);
				Sleep(5000);
				wrt_st_TCP(hComm[siimport], 11, "gaterepeat\n", &nch);
				}
			if (lj == L'5' || lj == L'6') {
				keyclose[siimport] = 1;
				}
			}
#endif
      if ((comtype[yno] == comtpRS && portti[yno] > 0) ||
         ((comtype[yno]  & comtpUDP) && ipparam[yno].srvport) ||
		 ((comtype[yno]  & comtpTCP) && *TCPdestaddr(hComm[yno]))) {
         switch (lj) {
            case L'1':
               if (keyclose[yno]) {
                  vastcom[yno] = 0;
                  keyclose[yno] = 0;
                  }
               lahcom[yno] = 1;
               lahcomserver[yno] = 1;
               lah_alku[yno] = 2;
               break;
            case L'2':
               if (keyclose[yno]) {
                  lahcomserver[yno] = 0;
                  keyclose[yno] = 0;
                  }
               vastcom[yno] = 1;
					break;
            case L'3':
               lahcom[yno] = 1;
               lahcomserver[yno] = 1;
               vastcom[yno] = 1;
               if (keyclose[yno]) {
                  keyclose[yno] = 0;
                  }
               lah_alku[yno] = 2;
               break;
            case L'4':
               lahcomserver[yno] = 0;
               break;
            case L'5':
               vastcom[yno] = 0;
               break;
            case L'6':
               if (comopen[yno]) {
				  keyclose[yno] = 1;
				  }
				WRITEYHT(L"  ", yno);
			   break;
			}
		 }
	  }
   }

void uusintaTCP(int cn);
void emitva_uusinta(int cn, INT32 tietue);

void uusintalahetys(void)
   {
   wchar_t ch, ch2, msg[82], as[20];
   INT jakso = 0, yht = 0, cna, cny;
   INT32 ta, ty;

   if (!levypusk)
      return;
	for (yht = 0; yht < maxyhteys; yht++) {
		if (comtype[yht] & comtpTCP)
			break;
//#if defined(EMIT) && !defined(ESITARK)
#if defined(EMIT)
		if (com_emitva[yht] > 0)
			break;
#endif
		}
	if (yht < maxyhteys) {
	   ch = L' ';
		wselectopt(L"Uusinta L)�hetysajan perusteella, M)uu valinta yhteen yhteyteen, Esc: peruuta",
			L"LM\x1b", &ch);
	   if (ch == ESC)
		   return;
		}
	else
		ch = L'L';
   if (ch == L'L') {
      clrln(ySize-3);
      vidspwmsg(ySize-3, 0, 7, 0, L"L�het� uudelleen viimeisten        minuutin sanomat");
      INPUTINTW(&jakso, 5, 28, ySize-3, L"\r", &ch);
      ty = t_time_l(biostime(0, 0), t0);
      ta = ty - jakso * 600L;
      if (ta < -12*36000L)
			ta += 24*36000L;
      swprintf(msg, L"L�het� %8.8s j�lkeiset sanomat K)aikkiin, Y)hteen yhteyteen, Esc: peruuta",
		 aikatowstr_ls(as, ta, t0));
      ch = L' ';
      wselectopt(msg, L"KY\x1b", &ch);
      if (ch == ESC)
		 return;
      }
   cna = 0;
   cny = maxyhteys-1;
   if (ch == L'Y' || ch == L'M') {
      clrln(ySize-3);
      vidspwmsg(ySize-3, 0, 7, 0, L"Anna yhteyden numero         Esc: peruuta");
      INPUTINTW(&yht, 3, 21, ySize-3, L"\r\x1b", &ch2);
      if (ch2 == ESC || yht < 1 || yht > maxyhteys)
         return;
      yht--;
//#if defined(EMIT) && !defined(ESITARK)
#if defined(EMIT)
		if (ch == L'M') {
			if (com_emitva[yht] > 0) {
				if (comtype[yht] & comtpTCP) {
					ch = L' ';
					wselectopt(L"E)mit-v�liajat, M)uut tiedot, Esc: peruuta", L"EM\x1b", &ch);
					}
				else
					ch = L'E';
				if (ch == ESC)
					return;
				}
			}
#endif
		if (ch == L'M') {
         ch = L'U';
         wselectopt(L"L�het� Y)hten� tiedostona kaikki/U)seana sanomana valitut tiedot, Esc: peruuta", L"YU\x1b", &ch);
         switch (ch) {
            case 'Y':
#ifndef MAXOSUUSLUKU
               wselectopt(L"T)uloksen saaneet, I)lmoittautuneet", L"IT", &ch);
               autofile(ch == L'I');
#else
               autofile();
#endif
               break;
            case L'U':
               uusintaTCP(yht);
               break;
			}
         return;
         }
//#if defined(EMIT) && !defined(ESITARK)
#if defined(EMIT) && !defined(LUENTA)
		else if (ch == L'E' && com_emitva[yht] > 0) {
			vidspwmsg(ySize-5,0,7,0,L"Emit-v�liaikojen uusintal�hetys");
			emitva_uusinta(yht, -1);
			clrtxt(ySize-5,0,64);
			return;
			}
#endif
      cna = yht;
      cny = yht;
      }
   merk_uusinta(cna, cny, KSEK*ta, KSEK*ty, 0);
   }

void yhteydet(void)
   {
   INT nrivi, r, cn;
   UINT32 ipno;
   wchar_t ch, ch2, line[65], ipaddr[20];
   char che;
   char tunnus[3];

   nrivi = min(ySize - 7, maxyhteys);
   clrscr();
   header = L"YHTEYDET";
   kehys(1);
   server_on = 1;
   vidspwmsg(2,0,7,0, L"Nro Portti  Vastaosoite            Suunta Tila L�htevi� Kirjattu");
   vidspwmsg(ySize-3, 0, 7, 0, L"PgDn, PgUp : selaa, A)setukset, U)usintal�hetys, Esc: poistu");
   for (;;) {
      ch = readkbd(&che, 0, 0);
      if (ch == ESC)
         break;
      if (!che)
         Sleep(200);
      switch (ch) {
         case L'a' :
		 case L'A' :
            yhteysasetukset();
            clrln(ySize-3);
            vidspwmsg(ySize-3, 0, 7, 0, L"PgDn, PgUp : selaa, A)setukset, U)usintal�hetys, Esc: poistu");
            break;
         case L'u' :
         case L'U' :
            uusintalahetys();
            clrln(ySize-3);
            vidspwmsg(ySize-3, 0, 7, 0, L"PgDn, PgUp : selaa, A)setukset, U)usintal�hetys, Esc: poistu");
			break;
			case L's' :
			case L'S' :
				if (sulkusalasana[0]) {
	            clrln(ySize-3);
					vidspwmsg(ySize-3, 0, 7, 0, L"Anna salasana                  Esc : Peruuta");
					for (;;) {
						line[0] = 0;
						inputwstr(line, 10, 15, ySize-3, L"\r\x1b", &ch2, 0);
						if (ch2 == ESC)
							break;
						upcasewstr(line);
						if (!wcscmpU(sulkusalasana, line)) {
							ch2 = L' ';
							wselectopt(L"Sulkuk�sky K)aikille koneelle, Y)hdelle koneelle, Esc: Peruuta", L"KY\x1b", &ch2);
							if (ch2 == ESC)
								break;
							memset(line, 0, 4);
							if (ch2 == L'Y') {
								vidspwmsg(ySize-3, 0, 7, 0, L"Anna suljettavan koneen tunnus       Esc : Peruuta");
								inputwstr(line, 2, 32, ySize-3, L"\r\x1b", &ch2, 0);
								if (ch2 == ESC)
									break;
								upcasewstr(line);
								}
							lahetasulku((char *)wcstooem((char *)tunnus,line, 2), 0);
							break;
							}
						writeerror_w(L"V��r� salasana", 0, true);
						}
	            clrln(ySize-3);
		         vidspwmsg(ySize-3, 0, 7, 0, L"PgDn, PgUp : selaa, A)setukset, U)usintal�hetys, Esc: poistu");
					}
				break;
         case 0 :
            switch (che) {
               case 73:                 // PgUp
                  yhteysalku -= nrivi-1;
                  break;
			   case 81:                 //
                  yhteysalku += nrivi-1;
                  break;
               }
            if (yhteysalku > maxyhteys - nrivi)
               yhteysalku = maxyhteys - nrivi;
            if (yhteysalku < 0)
			   yhteysalku = 0;
            break;
         }
      for (r = 0; r < nrivi; r++) {
         cn = r + yhteysalku;
		 memset(line, 0, sizeof(line));
         if (!comopen[cn]) {
            swprintf(line, L"%3d Ei avattu", cn+1);
            wmemset(line+wcslen(line), L' ', 64-wcslen(line));
				}
         else if (comtype[cn] >= 0) {
            switch (comtype[cn]) {
               case comtpRS :
                  swprintf(line, L"%3d COM%d", cn+1, portti[cn]);
                  break;
               case comtpUDP :
                  wcsncpy(ipaddr, ipparam[cn].destaddr, 15);
                  if (!memcmp(ipaddr, L"AUTO", 5) && (ipno = UDPaddr(hComm[cn])) != 0)
                     swprintf(ipaddr, L"%u.%u.%u.%u", ipno/16777216L, (ipno/65536L) & 255,
                        (ipno/256) & 255, ipno & 255);
                  ipaddr[15] = 0;
                  swprintf(line, L"%3d %5d %s:%d", cn+1, (int)ipparam[cn].srvport,
                     ipaddr, (int)UDPcliport(hComm[cn]));
                  break;
               case comtpTCP :
               case comtpTCPXML :
				   ansitowcs(ipaddr, TCPdestaddr(hComm[cn]), 16);
                  swprintf(line, L"%3d TCP   %15.15s:%d", cn+1,
                     ipaddr, TCPdestport(hComm[cn]));
                  break;
               case comtpTCPSRV :
			   case comtpTCPSRVXML :
				   ansitowcs(ipaddr, TCPdestaddr(hComm[cn]), 16);
                  swprintf(line, L"%3d TCP   %15.15s", cn+1, ipaddr);
                  break;
               }
			line[35] = 0;
            wmemset(line+wcslen(line), L' ', 35-wcslen(line));
            if (vastcom[cn])
               wcscat(line, L"In ");
            else
               wcscat(line, L"   ");
            if (lahcom[cn] && lahcomserver[cn])
			   wcscat(line, L"Out");
            else
               wcscat(line, L"   ");
			}
         vidspwmsg(r+3, 0, 7, 0, line);
         DSPQ(ySize-6, cn);
         }
      }
   server_on = 0;
   }
#endif


static FILE *lahtiedbuf;
static int lahtiedpos[MAX_LAHPORTTI];
static int lahtiedlen;
struct {
	int koodi;
	int alku;
	int len;
	int time;
} lahtiedostot[10];

static void kirjaa_tiedhyv(int cn)
{
	if (lahtiedpos[cn] < lahtiedlen)
		lahtiedpos[cn]++;
	lah_tied_seur[cn] = lahtiedlen - lahtiedpos[cn];
}

static int get_tied_sanoma(int cn, combufrec *obuf)
{
	int err = 0;
	if (lahtiedpos[cn] < lahtiedlen) {
		if (fseek(lahtiedbuf, combufsize * lahtiedpos[cn], SEEK_SET)) {
			err = 1;
			}
		else if ((int) fread(obuf, 1, combufsize, lahtiedbuf) < combufsize)
			err = 1;
		}
	if (err) {
		lahtiedpos[cn] = lahtiedlen;
		lah_tied_seur[cn] = 0;
		memset(obuf, 0, combufsize);
		}
	return(err);
}

int lah_tiedosto(wchar_t *tiednimi, int kielto, int flags)
	{
	INT  cn, i, n = 0;
	fpos_t nalku = 0;
	UINT32 koodi, pos;
	UINT32 len;
	combufrec obuf;
	FILE *lahfile;

	if ((len = wcslen(WorkingDir)) > 0 && !wmemcmpU(tiednimi, WorkingDir, len)) {
		if (WorkingDir[len] != L'\\' && tiednimi[len] == L'\\')
			len++;
		wmemmove(tiednimi, tiednimi+len, wcslen(tiednimi+len)+1);
		}
	else {
		for (len = wcslen(tiednimi)-1; len > 0; len--)
			if (tiednimi[len] == L'\\')
				break;
		if (len > 0 && wmemcmpU(tiednimi, WorkingDir, len) == 0) {
			wchar_t st[200] = L"";

			for (wchar_t *p = WorkingDir+len; *p; p++)
				if (p[1] == L'\\' || (p[1] == 0 && p[0] != L'\\'))
					wcscat(st, L"..\\");
			if (st[0] && wcslen(st)+wcslen(tiednimi+len+1) < 200) {
				wcscpy(st+wcslen(st), tiednimi+len+1);
				wcscpy(tiednimi, st);
				}
			}
		}
	if (!lahkaikkitied && (tiednimi[1] == L':' || tiednimi[0] == L'\\')) {
		return(1);
		}
	if ((lahfile = _wfopen(tiednimi, L"rb")) == NULL)
		return(1);
	if (!lahtiedbuf)
		lahtiedbuf = _wfopen(L"__FCOMFILE__.TMP", L"w+b");
	if (!lahtiedbuf)
		return(1);
	EnterCriticalSection(&outb_CriticalSection);
	fseek(lahtiedbuf, 0, SEEK_END);
	fgetpos(lahtiedbuf, &nalku);
	nalku /= combufsize;
	for (i = 0; i >= 0; i++) {
		obuf.pkgclass = FILESEND;
		obuf.len = class_len[FILESEND];
		memset(&obuf.d.fl, 0, class_len[FILESEND]);
		if (i == 0) {
			koodi = mstimer();
			obuf.d.fl.pos = -1;
			wcsncpy((wchar_t *)obuf.d.fl.buf, tiednimi, (class_len[FILESEND] - 18)/2);
			pos = 0;
			}
		else {
			len = fread(obuf.d.fl.buf, 1, class_len[FILESEND]-16, lahfile);
			if (len == 0) {
				obuf.d.fl.pos = -2;
				i = -2;
				}
			else {
				obuf.d.fl.pos = pos;
				obuf.d.fl.len = len;
				pos += len;
				}
			}
		obuf.d.fl.koodi = koodi;
		obuf.d.fl.flags = flags;
		fwrite(&obuf, combufsize, 1, lahtiedbuf);
		n++;
		}
	for (i = 0; i < sizeof(lahtiedostot)/sizeof(lahtiedostot[0]); i++) {
		if (lahtiedostot[i].koodi == 0)
			break;
		}
	lahtiedostot[i].koodi = koodi;
	lahtiedostot[i].alku = (int) nalku;
	lahtiedostot[i].len = n;
	lahtiedostot[i].time = mstimer();
	lahtiedlen = (int) nalku + n;
	i = (i+1) % (sizeof(lahtiedostot)/sizeof(lahtiedostot[0]));
	for (cn = 0; cn < maxyhteys; cn++) {
		if ( !comopen[cn] || !lahcom[cn] || (lah_tiedostot[cn] & flags) == 0 ||
			lahkellovain[cn] || com_ajat[cn] == 2 ||
			cn == kielto - 1 || (kielto && estavalitys[cn]) || comtype[cn] != comtpUDP)
			continue;
		if (lahtiedpos[cn] < lahtiedostot[i].alku +	lahtiedostot[i].len)
			lahtiedpos[cn] = lahtiedostot[i].alku +	lahtiedostot[i].len;
		lah_tied_seur[cn] = lahtiedlen - lahtiedpos[cn];
		}
	memset(lahtiedostot+i, 0, sizeof(lahtiedostot[0]));
	LeaveCriticalSection(&outb_CriticalSection);
	fclose(lahfile);
	return(0);
	}

static struct {
	UINT32 koodi;
	UINT32 pos;
	FILE *tiedvast;
	wchar_t tiednimi[200];
	} FileRcv[MAX_LAHPORTTI];

void tark_tiedosto(INT cn)
	{
	combufrec *ibuf;

	ibuf = inbuf + inbens;
	if (ibuf->d.fl.pos < 0) {
		if (FileRcv[cn].tiedvast != NULL) {
			fclose(FileRcv[cn].tiedvast);
			FileRcv[cn].tiedvast = NULL;
			}
		if (ibuf->d.fl.pos == -1) {
			FileRcv[cn].koodi = ibuf->d.fl.koodi;
			wcsncpy(FileRcv[cn].tiednimi, (wchar_t *)ibuf->d.fl.buf, sizeof(FileRcv[cn].tiednimi)/2-1);
			FileRcv[cn].tiedvast = _wfopen(FileRcv[cn].tiednimi, L"wb");
			FileRcv[cn].pos = 0;
			}
		else if (ibuf->d.fl.pos == -2 && FileRcv[cn].pos > 0) {
			 lah_tiedosto(FileRcv[cn].tiednimi, cn+1, ibuf->d.fl.flags);
			 }
		}
	else {
		if (FileRcv[cn].tiedvast) {
			if (FileRcv[cn].pos == ibuf->d.fl.pos) {
				if (fwrite(ibuf->d.fl.buf, ibuf->d.fl.len, 1, FileRcv[cn].tiedvast) == 1)
					FileRcv[cn].pos += ibuf->d.fl.len;
				else {
					fclose(FileRcv[cn].tiedvast);
					FileRcv[cn].tiedvast = NULL;
					}
				}
			else {
				fclose(FileRcv[cn].tiedvast);
				FileRcv[cn].tiedvast = NULL;
				}
			}
		}
	}

void ReadComTestParams(void)
{
	TextFl *paramFl;
	wchar_t paramFlNm[] = L"ComTestParam.txt";
	int ta = TMAALI0;

	sendtestparam.NJono = 1;
	sendtestparam.Jono = 0;
	sendtestparam.cn = -1;
	sendtestparam.ValiKerroin = 1;
	sendtestparam.valilaji = 1;
	sendtestparam.aloituslaji = 0;
	sendtestparam.t_alku = TMAALI0;
	sendtestparam.t_aloitus = TMAALI0;
	sendtestparam.ens = 1;
	sendtestparam.viim = 999999999;
	sendtestparam.RecLen = combufsize;

	paramFl = new TextFl(paramFlNm, L"rt");
	if (paramFl->IsOpen()) {
		while (!paramFl->Feof()) {
			wchar_t ln[200]=L"", *p;
			paramFl->ReadLine(ln, 198);
			upcasewstr(ln);
			elimwbl(ln);
			if (wmemcmp(ln, L"TIED", 4) == 0) {
				p = wcsstr(ln, L"=");
				if (p)
					wcscpy(sendtestparam.ComFileName, p+1);
				continue;
				}
			if (wmemcmp(ln, L"JONOLKM", 5) == 0) {
				p = wcsstr(ln, L"=");
				if (p)
					sendtestparam.NJono = _wtoi(p+1);
				continue;
				}
			if (wmemcmp(ln, L"JONO=", 5) == 0) {
				p = wcsstr(ln, L"=");
				if (p)
					sendtestparam.Jono = _wtoi(p + 1) - 1;
				continue;
				}
			if (wmemcmp(ln, L"L�HJONO", 5) == 0) {
				p = wcsstr(ln, L"=");
				if (p)
					sendtestparam.cn = _wtoi(p + 1) - 1;
				continue;
				}
			if (wmemcmp(ln, L"TASAV", 5) == 0) {
				sendtestparam.valilaji = 0;
				continue;
				}
			if (wmemcmp(ln, L"V�LI=", 5) == 0) {
				p = wcsstr(ln, L"=");
				if (p)
					sendtestparam.ValiKerroin = StrToDouble(p + 1);
				continue;
				}
			if (wmemcmp(ln, L"ALOITUS", 7) == 0) {
				sendtestparam.aloituslaji = 1;
				p = wcsstr(ln, L"=");
				if (p)
					sendtestparam.t_aloitus = wstrtoaika_vap(p + 1, t0);
				continue;
				}
			if (wmemcmp(ln, L"VERTAIKA", 7) == 0) {
				p = wcsstr(ln, L"=");
				if (p)
					sendtestparam.t_vert = wstrtoaika_vap(p + 1, t0);
				continue;
				}
			}
		if (sendtestparam.ComFileName[0])
			sendtestparam.ComFile = _wfopen(sendtestparam.ComFileName, L"rb");
		}
	delete paramFl;
}

#ifdef _CONSOLE
void naytaTestRivi(void) {
	vidint(ySize - 6, 68, 6, sendtestparam.LahRivi);
	VIDSPAIKA(ySize - 5, 68, sendtestparam.t_vert, t0);
}
#endif

void SendCopyThread(LPVOID lpCn)
	{
	combufrec combuf;
	int r;
	static int tn;
	bool alkuohitettu = false;

	r = sendtestparam.ens;
	if (!sendtestparam.ComFile || r <= 0 || r > sendtestparam.viim) {
		return;
		}
	sendtestparam.LahRivi = r;
	if (sendtestparam.valilaji == 0 && sendtestparam.aloituslaji == 1) {
		sendtestparam.t_alku = sendtestparam.t_aloitus;
		for (;;) {
			if (NORMKELLO(Nyt() - sendtestparam.t_alku) > 0)
				break;
			Sleep(100);
			}
		}
	sendtestparam.t_alku = Nyt();

	sendtestparam.inSendCopy = true;
	for (; r <= sendtestparam.viim && sendtestparam.inSendCopy && taustaon;) {
		fseek(sendtestparam.ComFile, (r * sendtestparam.NJono + sendtestparam.Jono) * sendtestparam.RecLen, SEEK_SET);
		if (fread(&combuf, sendtestparam.RecLen, 1, sendtestparam.ComFile) != 1) {
			break;
			}
		if (sendtestparam.valilaji == 1) {
			if (sendtestparam.t_vert == TMAALI0)
				sendtestparam.t_vert = KSEK*combuf.lahetetty;
			else {
				int tt, t = KSEK*combuf.lahetetty;
				while (sendtestparam.inSendCopy && taustaon) {
					if (NORMKELLO((tn = Nyt()) - (tt = sendtestparam.t_alku +
						(int)(sendtestparam.ValiKerroin * NORMKELLO(t - sendtestparam.t_vert)))) >= 0)
						break;
					Sleep(100);
					alkuohitettu = true;
					}
				if (sendtestparam.valilaji == 0 || alkuohitettu) {
					sendtestparam.t_alku = tt;
					sendtestparam.t_vert = t;
					}
				}
			}
		if (sendtestparam.inSendCopy) {
			lahetaKopiot(sendtestparam.cn, combuf);
			Sleep(30);
			if (sendtestparam.valilaji == 0)
				Sleep((int)(1000*sendtestparam.ValiKerroin));
			r++;
			sendtestparam.LahRivi = r;
			naytaTestRivi();
			}
		}
	sendtestparam.LahRivi = r;
	sendTestLopetus();
	sendtestparam.inSendCopy = false;
	}
//---------------------------------------------------------------------------

