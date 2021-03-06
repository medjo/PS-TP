#ifndef __PROCESSUS_H__
#define __PROCESSUS_H__

#include <stdio.h>
#include <cpu.h>
#include <string.h>
#include <inttypes.h>
#define STACK_SIZE 512
#define NB_PROCESSES 4
#define NAME_SIZE 10
#define I_EBX 0
#define I_ESP 1
#define I_EBP 2
#define I_ESI 3
#define I_EDI 4
extern void *malloc(size_t size);
extern void ctx_sw(int *reg1, int *reg2);
extern uint32_t hh, mm, ss;
extern uint32_t time;

//États des processus
enum state
{
    ELU,
    ACTIVABLE,
    ENDORMI,
    MORT
};

typedef enum state State;

struct process
{
    int pid;
    //char name[NAME_SIZE]; 
    char *name; 
    State state;//État du processus
    int reg[5];//tableau de sauvegarde des 5 registres principaux
    int *stack[STACK_SIZE];
    uint32_t ttw ;//Time To Wake up
};


typedef struct process Process;
Process proc_table[NB_PROCESSES];
Process *actif;
void idle();
void proc1();
void proc2();
void proc3();
int32_t cree_processus(void (*code)(void), char *nom);
void init_proc(Process *proc, int p_pid, char *p_name, State p_state);
void ordonnance(void);
int32_t mon_pid(void);
char *mon_nom(void);
void dors(uint32_t nbr_secs);
uint32_t nbr_secondes();
void fin_processus();
void affiche_etats(void);
#endif
