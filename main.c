/******************************************************************************
* Projet : TimeToShutdown
* version : 0.1
* Auteur : AXeL
* Date de création : 26/10/2013
* Date de la dernière modification : 27/10/2013
* Fichier : main.c
* Lacunes : Rien de visible, sauf quelque lacunes dans la gestion d'erreurs
* Idees d'amélioration : 
        - Précision en heure, minute, seconde dans la fenetre de configuration
        - Afficher l'heure restante aussi dans la fonction AfficherEtat()
        - Ajouter la possibilité d'arreter un processus dans la fenetre de
          configuration
        - Ajouter une fenetre qui contiendra une barre de chargement et affichera
          le temp restant au lieu d'un MessageBox
        - WaitForSingleObject(thread, INFINITE); pourrait etre utilisé en cas
          de double thread
        - Ajouter un checkbox qui permet de désactiver la mise en veille automatique
*
******************************************************************************/

#include "ressources.h"


/******************************************************************************
* Fonction principale
*
******************************************************************************/

int WINAPI WinMain(HINSTANCE hinstanceActuelle, HINSTANCE hinstancePrecedente, LPSTR lpszArgument, int nFunsterStil)
{
   MSG message;
   WNDCLASS winClass;
   
   /* Icone de l'application */
   Hicon = (HICON)LoadIcon(hinstanceActuelle, MAKEINTRESOURCE(ID_ICON));
   
   ZeroMemory(&winClass, sizeof(WNDCLASS));
   winClass.lpszClassName       = NOM_APP;
   winClass.hInstance           = Hinst = hinstanceActuelle;
   winClass.lpfnWndProc         = WindowProcedure;
   winClass.style               = CS_HREDRAW | CS_VREDRAW;
   winClass.hIcon               = Hicon;
   winClass.hCursor             = LoadCursor (NULL, IDC_ARROW);
   winClass.hbrBackground       = (HBRUSH) COLOR_BACKGROUND;  
   
   if (!RegisterClass(&winClass))
      return 0;
   CreateWindow(NOM_APP, NOM_APP, 0, 0, 0, 0, 0, 0, 0, hinstanceActuelle, 0);
   
   while (GetMessage (&message, NULL, 0, 0))
   {
      TranslateMessage(&message);
      DispatchMessage(&message);
   }
   
   DestroyIcon(Hicon);
   Shell_NotifyIcon(NIM_DELETE, &TrayIcon);
  
return message.wParam;
}

/******************************************************************************
* Procedure de gestion des appels et messages (WindowProcedure)
*
******************************************************************************/

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
       case WM_CREATE:
            /* Gestion pour suppression de l'icone dans la barre de tache en cas de plantage explorer */
            WM_TASKBARCREATED = RegisterWindowMessage("TaskbarCreated");
            TrayCreate(hwnd);
            /* Création de la fenetre de configuration et lancement(affichage) */
            Hconfig = CreateDialog(0, MAKEINTRESOURCE(DIAG_CONFIG), hwnd, DlgConfig);
            break;
       case MY_WM_NOTIFYICON:
            switch (lParam)
            {
                case WM_RBUTTONUP:
                     AfficherPopup(hwnd);
                     break;
                case WM_LBUTTONUP:
                     if (!ACTIVE_MSG_ANNULER)
                        montrerCacherFenetre();
                     else if (!ACTIVE_AFFICHER_ETAT)
                     {
                        ACTIVE_AFFICHER_ETAT = 1;
                        AfficherEtat();
                     }
                     break;
            }
            break;
       case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case MSG_QUITTER:
                     if (ACTIVE_MSG_ANNULER)
                     {
                        if (MessageBox(hwnd, "Etes vous sûr de vouloir quitter " NOM_APP " ?", NOM_APP, MB_YESNO|MB_TOPMOST|MB_ICONQUESTION) == IDYES)
                           TuerThread();
                        else
                           break;
                     }
                     PostQuitMessage(0);
                     break;
                case MSG_APROPOS:
                     MessageBox(hwnd, NOM_APP
                     "\nPar : AXeL"
                     "\nCompatibilité : Windows XP/7"
                     "\nLibre de droit et d'utilisations.\n"
                     ,"A propos",MB_OK|MB_TOPMOST|MB_ICONINFORMATION); 
                     break;  
                case MSG_CONFIGURER:
                     montrerCacherFenetre();
                     break;  
                case MSG_ARRETER:
                     if (MessageBox(hwnd, "Etes vous sûr de vouloir arrêter le système ?", NOM_APP, MB_YESNO|MB_TOPMOST|MB_ICONQUESTION) == IDYES)
                        AppliquerAction("ARRETER"); 
                     break;
                case MSG_REDEMARRER:
                     if (MessageBox(hwnd, "Etes vous sûr de vouloir redémarrer le système ?", NOM_APP, MB_YESNO|MB_TOPMOST|MB_ICONQUESTION) == IDYES)
                        AppliquerAction("REDEMARRER");
                     break;
                case MSG_METTRE_EN_VEILLE:
                     if (MessageBox(hwnd, "Etes vous sûr de vouloir mettre en veille le système ?", NOM_APP, MB_YESNO|MB_TOPMOST|MB_ICONQUESTION) == IDYES)
                     {
                        if (ACTIVE_MSG_ANNULER)
                        {
                           TuerThread();
                           Reset();
                        }
                        SetSuspendState(FALSE, FALSE, FALSE);
                     }
                     break;
                case MSG_ANNULER:
                     TuerThread();
                     Reset();
                     break;
                case MSG_ETAT:
                     if (!ACTIVE_AFFICHER_ETAT)
                     {
                        ACTIVE_AFFICHER_ETAT = 1;
                        AfficherEtat();
                     }
                     break;
            }
            break; 
       default:
            if (message == WM_TASKBARCREATED)
               TrayCreate(hwnd);
            else
               return DefWindowProc (hwnd, message, wParam, lParam);      
   }
   
return 0;        
}

/******************************************************************************
* Fonction qui crée une icone de l'application dans la barre de tache
*
******************************************************************************/

void TrayCreate(HWND hwnd)
{
    ZeroMemory(&TrayIcon, sizeof(NOTIFYICONDATA));
    TrayIcon.cbSize = sizeof(NOTIFYICONDATA);
    TrayIcon.hWnd = hwnd;
    TrayIcon.uID = 0;
    TrayIcon.hIcon = Hicon;
    TrayIcon.uCallbackMessage = MY_WM_NOTIFYICON;
    TrayIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    strcpy(TrayIcon.szTip, NOM_APP);
    Shell_NotifyIcon(NIM_ADD, &TrayIcon);
}

/******************************************************************************
* Fonction qui affiche le popup (les options) si clic droit sur l'application 
  dans la barre de tache
*
******************************************************************************/

void AfficherPopup(HWND hwnd)
{
   POINT pos;
   HMENU htmp, htmp2;
   
   /* Affichage du popup */
   if (GetCursorPos(&pos) != 0)
   {
      if ((htmp = LoadMenu(Hinst, "MY_POPUP")) != 0)
      {
         if ((htmp2 = GetSubMenu(htmp, 0)) != 0)
         {
            /* Désactivation des options */
            if (ACTIVE_DIAG_CONFIG || ACTIVE_MSG_ANNULER)
               EnableMenuItem(htmp2, MSG_CONFIGURER, MF_BYCOMMAND|MF_GRAYED);
            if (!ACTIVE_MSG_ANNULER)
               EnableMenuItem(htmp2, MSG_ANNULER, MF_BYCOMMAND|MF_GRAYED);
            if (!ACTIVE_MSG_ETAT)
               EnableMenuItem(htmp2, MSG_ETAT, MF_BYCOMMAND|MF_GRAYED);
          
            SetForegroundWindow(hwnd);
            TrackPopupMenuEx(htmp2, 0, pos.x, pos.y, hwnd, NULL);
            DestroyMenu(htmp2);    
         }
         DestroyMenu(htmp);
      }
   }
}

/******************************************************************************
* Fenetre de configuration de l'application
*
******************************************************************************/

BOOL CALLBACK DlgConfig(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
       case WM_INITDIALOG:
            {
            HANDLE h_tmp;
            /* Application de l'icone */
            SetClassLong(hwnd, GCL_HICON, (long)Hicon);
            /* Chargement des Combo Box */
            h_tmp = GetDlgItem(hwnd, CB_TIME);
               SendMessage(h_tmp, CB_ADDSTRING,0, (LPARAM)"10");
               SendMessage(h_tmp, CB_ADDSTRING,0, (LPARAM)"15");
               SendMessage(h_tmp, CB_ADDSTRING,0, (LPARAM)"30");
               SendMessage(h_tmp, CB_ADDSTRING,0, (LPARAM)"60");
            /* Désactivation du button appliquer */
            EnableWindow(GetDlgItem(hwnd, BT_APPLIQUER), FALSE);
            /* Affichage de la fenetre  */
            ShowWindow(hwnd, SW_SHOW);
            SetFocus(hwnd);
            SetFocus(GetDlgItem(hwnd, BT_ANNULER));
            /* On indique que la fenetre a été affichée */
            ACTIVE_DIAG_CONFIG = 1;
            }
            break;
       case WM_PAINT:
            /* On dessine les images */
            CreateWindow("STATIC", "ID_ARRETER", WS_CHILD | WS_VISIBLE | SS_BITMAP,  10, 40, 0, 0, hwnd, NULL, Hinst, NULL);
            CreateWindow("STATIC", "ID_REDEMARRER", WS_CHILD | WS_VISIBLE | SS_BITMAP,  10, 80, 0, 0, hwnd, NULL, Hinst, NULL);
            CreateWindow("STATIC", "ID_METTRE_EN_VEILLE", WS_CHILD | WS_VISIBLE | SS_BITMAP,  10, 122, 0, 0, hwnd, NULL, Hinst, NULL);
            break;
       case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case BT_ARRETER:
                     ACTIVE_BT_ARRETER = 1;
                     ACTIVE_BT_REDEMARRER = 0;
                     ACTIVE_BT_METTRE_EN_VEILLE = 0;
                     EnableWindow(GetDlgItem(hwnd, BT_APPLIQUER), TRUE);
                     break;
                case BT_REDEMARRER:
                     ACTIVE_BT_ARRETER = 0;
                     ACTIVE_BT_REDEMARRER = 1;
                     ACTIVE_BT_METTRE_EN_VEILLE = 0;
                     EnableWindow(GetDlgItem(hwnd, BT_APPLIQUER), TRUE);
                     break;
                case BT_METTRE_EN_VEILLE:
                     ACTIVE_BT_ARRETER = 0;
                     ACTIVE_BT_REDEMARRER = 0;
                     ACTIVE_BT_METTRE_EN_VEILLE = 1;
                     EnableWindow(GetDlgItem(hwnd, BT_APPLIQUER), TRUE);
                     break;
                case BT_APPLIQUER:
                     {
                     char tmp[32];
                     GetWindowText(GetDlgItem(hwnd,CB_TIME), tmp, 32);
                     temp = atoi(tmp);
                     if (temp > 0) 
                     {
                        Hthread = CreateThread(NULL, 0, TimeToShutdown, 0, 0, 0); 
                        ACTIVE_MSG_ETAT = 1;
                        ACTIVE_MSG_ANNULER = 1;
                        montrerCacherFenetre();
                     }
                     else
                        MessageBox(hwnd, "Erreur , le temp saisie n'est pas valide !"
                        ,NOM_APP,MB_OK|MB_TOPMOST|MB_ICONEXCLAMATION); 
                     }    
                     break;
                case BT_ANNULER:
                     montrerCacherFenetre();
                     break;
            }
            break;
       //case WM_SYSCOMMAND:
            //switch(LOWORD(wParam))
            //{ 
                //case SC_MINIMIZE:
                    // break;
            //}
            //break;
       case WM_CLOSE:
            if (MessageBox(hwnd, "Etes vous sûr de vouloir quitter " NOM_APP " ?", NOM_APP, MB_YESNO|MB_TOPMOST|MB_ICONQUESTION) == IDYES)
               PostQuitMessage(0);
            break;
   }
   
return 0;
}

/******************************************************************************
* Thread qui attend le temp spécifié avant d'appliquer l'action demandée
*
******************************************************************************/

DWORD  WINAPI TimeToShutdown(LPVOID lParam)
{
   /* Récupération du temp actuel */
      tempDeDebut = time(NULL);
      //GetSystemTime(&tempDeDebut);
   /* Attente du temp spécifié */
      sleep(temp*60*1000); /* conversion en milisecondes */
   /* Application de l'action demandée */
      if (ACTIVE_BT_ARRETER)
         AppliquerAction("ARRETER");
      else if (ACTIVE_BT_REDEMARRER)
         AppliquerAction("REDEMARRER");
      else if (ACTIVE_BT_METTRE_EN_VEILLE)
      {
         Reset();
         SetSuspendState(FALSE, FALSE, FALSE);  /* Mise en veille */
      } 
      
return 0;
}

/******************************************************************************
* Fonction chargée d'arreter ou de redémarrer le système
*
******************************************************************************/

void AppliquerAction(char *action)
{
   /* Récupération des privilèges */
   HANDLE hToken;
   TOKEN_PRIVILEGES tkp; /* Pointeur sur la structure de token */
   OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken); /* Récupération du LUID pour les privileges d'arret */
   LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
   tkp.PrivilegeCount = 1; /* Un privilège à mettre */
   tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; /* Récupération du privilège d'arret pour ce processus */ 
   AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0);/* Pour tester la valeur retournée de AdjustTokenPrivileges.if (GetLastError() != ERROR_SUCCESS) */
   CloseHandle(hToken);
   /* Choix d'actions */
   if (!strcmp(action , "ARRETER"))
      ExitWindowsEx(EWX_POWEROFF, 0); /* Fermeture de windows */
   else if (!strcmp(action , "REDEMARRER"))
      ExitWindowsEx(EWX_REBOOT, 0); /* Redémarrage de windows */
}

/******************************************************************************
* Fontion qui tue le thread Hthread (plus précisament) => TimeToShutdown()
*
******************************************************************************/

void TuerThread()
{
   /* On tue le thread Hthread */
   DWORD IDThread;
   GetExitCodeThread(Hthread, &IDThread);
   TerminateThread(Hthread, IDThread);
}

/******************************************************************************
* Fonction qui montre/cache la fenetre de configuration
*
******************************************************************************/

void montrerCacherFenetre()
{
   /* On cache la fenetre de configuration ou bien on la montre+ maximisation */
   ShowWindow(Hconfig, ACTIVE_DIAG_CONFIG?SW_HIDE:SW_SHOW&SW_MAXIMIZE);
   ACTIVE_DIAG_CONFIG = !ACTIVE_DIAG_CONFIG;
}

/******************************************************************************
* Fonction qui réactive des buttons dans la notification de la barre de tache
*
******************************************************************************/

void Reset()
{
   ACTIVE_MSG_ETAT = 0;
   ACTIVE_MSG_ANNULER = 0;     
}

/******************************************************************************
* Fonction qui affiche l'etat actuel du thread lancé
*
******************************************************************************/

void AfficherEtat()
{
   /* Récupération du temp actuel */
      time_t tempActuel;
      tempActuel = time(NULL);
      //SYSTEMTIME tempActuel; 
      //GetSystemTime(&tempActuel);
   /* Calcul de la durée du temp passée depuis le début du thread */
      int tempEcoulee = difftime(tempDeDebut, time(&tempActuel));
      //tempEcoulee = (tempActuel.wHour - tempDeDebut.wHour)*60; /* *60 => conversion en minutes */
      //tempEcoulee += (tempActuel.wMinute - tempDeDebut.wMinute);
      char etatActuel[128], action[32];
      if (ACTIVE_BT_ARRETER)
         sprintf(action, "Arrêt");
      else if (ACTIVE_BT_REDEMARRER)
         sprintf(action, "Redémarrage");
      else if (ACTIVE_BT_METTRE_EN_VEILLE)
         sprintf(action, "Mise en veille");
      sprintf(etatActuel, "%s dans %d minute(s) et %d seconde(s) !", action, ((temp*60)+tempEcoulee)/60, ((temp*60)+tempEcoulee)%60);
   /* Affichage de l'état */
      switch (MessageBox(0, etatActuel, NOM_APP, MB_OK|MB_TOPMOST|MB_ICONINFORMATION))
      {
         case IDOK:
         case IDCANCEL:
              ACTIVE_AFFICHER_ETAT = 0;
              break;
      }
}
