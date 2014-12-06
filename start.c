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
    //initialisations pour les interruptions d'horloge
    init_traitant_IT(32, traitant_IT_32);
    set_clock_freq();
    masque_IRQ(0, false);

    
    // démasquage des interruptions externes
    //sti();

    //initialisation des structures de processus
    printf("\f");//efface l'écran
    init_proc(proc_table, 0, "idle", ELU);

    //Création dynamique de processus : les trois fonctions suivantes sont appelées par les processus eux-mêmes:
    //cree_processus( proc1, "proc1");
    //cree_processus( proc2, "proc2");
    //cree_processus( proc3, "proc3");

        
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
