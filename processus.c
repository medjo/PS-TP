#include <processus.h>
#include <horloge.h>
#include <ecran.h>

//PID du dernier processus créé
int32_t latest_pid = 0;
extern uint32_t clock_freq;
extern char *heure;

void idle()
{
    cree_processus( proc1, "proc1");
    for (;;)
    {
        sti();
        hlt();
        cli();
    }
}

void proc1(void)
{
    for (uint32_t i = 0 ; i < 3 ; i++)
    {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(), mon_nom(), mon_pid());
        dors(1);
    }
    cree_processus( proc2, "proc2");
    cree_processus( proc3, "proc3");
}

void proc2(void)
{
    for (uint32_t i = 0 ; i < 2 ; i++)
    {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(), mon_nom(), mon_pid());
        dors(1);
    }
    cree_processus( proc3, "proc3");
    cree_processus( proc1, "proc1");
}

void proc3(void)
{
    for (uint32_t i = 0 ; i < 1 ; i++)
    {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(), mon_nom(), mon_pid());
        dors(1);
    }
    cree_processus( proc1, "proc1");
    cree_processus( proc2, "proc2");
}

uint32_t nbr_secondes()
{
    return time;
}


int32_t cree_processus(void (*code)(void), char *nom)
{
    //nb_check est le nombre de fois ou l'on vérifie l'état d'un processus
    uint32_t nb_check = 0;

    //On vérifie au maximum une fois l'état de chaque processus
    while(nb_check < NB_PROCESSES)
    {
        //Soit le nombre de processus qui a été créé depuis le début est inférieur au nombre de processus maximum
        //Soit le processus vérifié est mort
        if( (latest_pid < NB_PROCESSES - 1) || proc_table[(latest_pid + nb_check + 1) % NB_PROCESSES].state == MORT   )
        {
            latest_pid++;

            //Changement du pid du processus mort
            proc_table[(latest_pid + nb_check) % NB_PROCESSES].pid = (latest_pid + nb_check) % NB_PROCESSES;

            //Changement du nom du processus mort
            proc_table[(latest_pid + nb_check) % NB_PROCESSES].name = malloc (NAME_SIZE * sizeof(char *));
            strncpy(proc_table[(latest_pid + nb_check) % NB_PROCESSES].name, nom, NAME_SIZE);

            //Chargement (dans la zone sauvegarde du pointeur sur pile) de l'adresse de la pile pointant vers le code du processus à exécuter
            proc_table[(latest_pid + nb_check) % NB_PROCESSES].reg[I_ESP] = (int) &proc_table[(latest_pid + nb_check) % NB_PROCESSES].stack[STACK_SIZE - 2];

            //Chargement de l'adresse de la fonction à exécuter juste en dessous du sommet de la pile du processus devant exécuter cette fonction
            proc_table[(latest_pid + nb_check) % NB_PROCESSES].stack[STACK_SIZE - 2] = (int *) code;
            //chargement au sommet de la pile l'adresse de la fonction terminant le processus auquelle la pile appartient.
            proc_table[(latest_pid + nb_check) % NB_PROCESSES].stack[STACK_SIZE - 1] = (int *) fin_processus;
            proc_table[(latest_pid + nb_check) % NB_PROCESSES].state = ACTIVABLE;
            return (latest_pid + nb_check) % NB_PROCESSES;

        }

        nb_check++; 
    }
    return -1;
}

//Initialise un processus
void init_proc(Process *proc, int p_pid, char *p_name, State p_state)
{
    proc->pid = p_pid;
    proc->name = malloc (NAME_SIZE * sizeof(*(proc->name)));
    strncpy(proc->name, p_name, NAME_SIZE);
    proc->state = p_state; 
}


void ordonnance(void)
{
    cli();//Pour ne pas qu'une interruption empêche le changement d'état
    int previous = actif->pid;

    //pour éviter une boucle infinie on ne testera qu'une fois chaque processus.
    int count = 0;
    if ( actif->state == ELU )
        actif->state = ACTIVABLE;
    actif = &proc_table[(actif->pid + 1) % NB_PROCESSES];
    
    //Regarde tous les processus jusq'à trouver le premier qui est soit ACTIVABLE soit ENDORMI mais dont l'heure de réveil est arrivée
    while ( (((actif->state == ENDORMI) && (actif->ttw > time)) || actif->state == MORT) && count < NB_PROCESSES)
    {
        actif = &proc_table[(actif->pid + 1) % NB_PROCESSES];
        count++;
    }
    actif->state = ELU;
    affiche_etats();
    sti();

    //Change le contexte d'execution : effectue le changement de processus actif
    ctx_sw(proc_table[previous % NB_PROCESSES].reg, proc_table[actif->pid % NB_PROCESSES].reg);

}

int32_t mon_pid(void)
{
    return actif->pid;
}

char *mon_nom(void)
{
    return actif->name;
}

void dors(uint32_t nbr_secs)
{
    cli();//Pour ne pas qu'une interruption empêche le changement d'état
    actif->state = ENDORMI;
    actif->ttw = time + (nbr_secs * clock_freq);
    sti();
    ordonnance();
}

//Met à mort l'état du processus actif, puis appelle ordonnance()
void fin_processus()
{
    cli();//Pour ne pas qu'une interruption empêche le changement d'état
    actif->state = MORT;
    sti();
    ordonnance();
}

//affiche les états des processus
void affiche_etats(void)
{
    int i = 0;
    int l = 2;
    uint32_t col = 0;
    uint32_t lig = 0 ;
    get_cursor(&lig, &col);

    while ( i < l )
    {
            place_curseur(i, 60);
            printf("                   ");
            i++;
    }
    afficher_l0_c72(heure);
    i = 0;
    while (i < NB_PROCESSES)
    {
        place_curseur(l, 60);
        if( proc_table[i].state == ENDORMI )
        {
            printf("                   ");
            place_curseur(l, 60);
            printf("%s : ENDORMI", proc_table[i].name);
        }
        else if ( proc_table[i].state == ELU )
        {
            printf("                   ");
            place_curseur(l, 60);
            printf("%s : ELU", proc_table[i].name);
        }
        else if ( proc_table[i].state == MORT )
        {
            printf("                   ");
            place_curseur(l, 60);
            printf("%s : MORT", proc_table[i].name);
        }
        else if ( proc_table[i].state == ACTIVABLE )
        {
            printf("                   ");
            place_curseur(l, 60);
            printf("%s : ACTIVABLE", proc_table[i].name);
        }
        i++;
        l++;
    }
    place_curseur(lig, col);
}
