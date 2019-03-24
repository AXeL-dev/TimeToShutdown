/******************************************************************************
* Projet : TimeToShutdown
* Fichier : ressources.h
*
******************************************************************************/

#include <windows.h>
#include <PowrProf.h> /* Pour fct de mise en veille */
#include <time.h>


/******************************************************************************
* Constantes
*
******************************************************************************/

#define NOM_APP               "TimeToShutdown"

#define ID_ICON               100

#define MY_WM_NOTIFYICON      WM_USER+1

#define DIAG_CONFIG           101 

/* Composants */

#define MSG_APROPOS           102
#define MSG_QUITTER           103
#define MSG_ARRETER           104
#define MSG_REDEMARRER        105
#define MSG_METTRE_EN_VEILLE  106
#define MSG_ANNULER           107
#define MSG_CONFIGURER        108
#define MSG_ETAT              109     

#define BT_ARRETER            110
#define BT_REDEMARRER         111
#define BT_METTRE_EN_VEILLE   112

#define GRP_SYSTEME           113
#define ST_TEXT_1             114
#define CB_TIME               115
#define ST_TEXT_2             116

#define BT_ANNULER            117
#define BT_APPLIQUER          118

/******************************************************************************
@ Variables globales
*
******************************************************************************/

UINT WM_TASKBARCREATED;
NOTIFYICONDATA TrayIcon;
HICON Hicon;
HINSTANCE Hinst;
HANDLE Hthread;
HANDLE Hconfig; /* Handle de la fenetre de configuration */

BOOL ACTIVE_BT_ARRETER, ACTIVE_BT_REDEMARRER, ACTIVE_BT_METTRE_EN_VEILLE;
BOOL ACTIVE_DIAG_CONFIG, ACTIVE_MSG_ANNULER, ACTIVE_MSG_ETAT, ACTIVE_AFFICHER_ETAT;

int temp;

time_t tempDeDebut;
//SYSTEMTIME tempDeDebut;

/******************************************************************************
* Prototypes
*
******************************************************************************/

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void TrayCreate(HWND hwnd);
void AfficherPopup(HWND hwnd);
BOOL CALLBACK DlgConfig(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void AppliquerAction(char *action);
void TuerThread();
void montrerCacherFenetre();
void Reset();
void AfficherEtat();

/******************************************************************************
* Threads
*
******************************************************************************/

DWORD  WINAPI TimeToShutdown(LPVOID lParam);
