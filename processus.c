#include <processus.h>
int32_t latest_pid = 0;



void idle (void)
{
    for (;;)
    {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100000000; i++);
        {
            ordonnance();
        }
    }
}

void proc1(void)
{
    for (;;)
    {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100000000; i++);
        {
            ordonnance(); 
        }
    }

}

void procr2(void)
{
    for (;;)
    {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100000000; i++);
        {
            ordonnance(); 
        }
    }

}

void proc3(void)
{
    for (;;)
    {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100000000; i++);
        {
            ordonnance(); 
        }
    }

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
        proc_table[latest_pid].reg[I_ESP] = (int) &proc_table[latest_pid].stack[STACK_SIZE - 1];
        proc_table[latest_pid].stack[STACK_SIZE - 1] = (int *) code;

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
    int previous = actif->pid;
    actif->state = ACTIVABLE;
    actif = &proc_table[(actif->pid + 1) % NB_PROCESSES];
    actif->state = ELU;
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
