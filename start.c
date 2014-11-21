#include <cpu.h>
#include <inttypes.h>
#include "ecran.h"
#include <stdio.h>
#include <horloge.h>
#include <processus.h>

uint32_t time = 0;
uint32_t clock_freq = 50;/*Fréquence de l'horloge en Hertz*/
extern void traitant_IT_32();
char *heure = "00:00:00";


void kernel_start(void)
{
    //initialisations
    //     ...
    /*On désactive les interruptions pour la séance 3 */ /*
    init_traitant_IT(32, traitant_IT_32);
    set_clock_freq();
    masque_IRQ(0, false);

    
    // démasquage des interruptions externes
    sti();
*/
    //initialisation des structures de processus
    printf("\f");//efface l'écran
    Process s_idle;
    //Process s_proc1;
    proc_table[0] = s_idle;
    //proc_table[1] = s_proc1;
    //init_proc(proc_table + 1, 1, "proc1", ACTIVABLE);
    init_proc(proc_table, 0, "idle", ELU);
    cree_processus( proc1, "proc1");
    cree_processus( proc1, "proc2");
    cree_processus( proc1, "proc3");

    //Début de la Pile à STACK_SIZE-1
    //proc_table[1].reg[I_ESP] = (int) &proc_table[1].stack[STACK_SIZE - 1];
    //proc_table[1].stack[STACK_SIZE - 1] = (int *)proc1;
    
    //demarrage du processus par défaut
    actif = &proc_table[0];
    idle();

    // boucle d'attente
	while (1)
    {
        // cette fonction arrete le processeur
        hlt();
    }
}
