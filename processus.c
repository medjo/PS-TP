#include <processus.h>
#include <horloge.h>
#include <ecran.h>

int32_t latest_pid = 0;
extern uint32_t clock_freq;

void idle()
{
    for (;;)
    {
        sti();
        hlt();
        cli();
    }
}

void proc1(void)
{
    for (uint32_t i = 0 ; i < 8 ; i++)
    {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(), mon_nom(), mon_pid());
        dors(2);
    }
    //fin_processus();
}

void proc2(void)
{
    for (uint32_t i = 0 ; i < 5 ; i++)
    {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(), mon_nom(), mon_pid());
        dors(3);
    }
    //fin_processus();
}

void proc3(void)
{
    for (uint32_t i = 0 ; i < 3 ; i++)
    {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(), mon_nom(), mon_pid());
        dors(5);
    }
    //fin_processus();
}

uint32_t nbr_secondes()
{
    return time;
}

int32_t cree_processus(void (*code)(void), char *nom)
{
    if( latest_pid < NB_PROCESSES - 1 )
    {
        Process s_proc;
        s_proc.pid = ++latest_pid;
        s_proc.name = malloc (NAME_SIZE * sizeof(*(s_proc.name)));
        strncpy(s_proc.name, nom, NAME_SIZE);
        s_proc.state = ACTIVABLE; 
        proc_table[latest_pid] = s_proc;
        proc_table[latest_pid].reg[I_ESP] = (int) &proc_table[latest_pid].stack[STACK_SIZE - 2];
        proc_table[latest_pid].stack[STACK_SIZE - 2] = (int *) code;
        proc_table[latest_pid].stack[STACK_SIZE - 1] = (int *) fin_processus;

        return latest_pid;
    }
    else
        return -1;
}

void init_proc(Process *proc, int p_pid, char *p_name, State p_state)
{
    proc->pid = p_pid;
    proc->name = malloc (NAME_SIZE * sizeof(*(proc->name)));
    strncpy(proc->name, p_name, NAME_SIZE);
    proc->state = p_state; 
}


void ordonnance(void)
{
    cli();
    int previous = actif->pid;
    if ( actif->state == ELU )
         actif->state = ACTIVABLE;
    actif = &proc_table[(actif->pid + 1) % NB_PROCESSES];
    while ( ((actif->state == ENDORMI) && (actif->ttw > time)) || actif->state == MORT )
    {
        actif = &proc_table[(actif->pid + 1) % NB_PROCESSES];
    }
    actif->state = ELU;
    affiche_etats();
    sti();
    ctx_sw(proc_table[previous].reg, proc_table[actif->pid].reg);

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

void fin_processus()
{
    cli();//Pour ne pas qu'une interruption empêche le changement d'état
    actif->state = MORT;
    sti();
    ordonnance();
}

void affiche_etats(void)
{
    int i = 0;
    int l = 2;
    uint32_t col = 0;
    uint32_t lig = 0 ;
    get_cursor(&lig, &col);

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
