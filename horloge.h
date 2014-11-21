#ifndef __HORLOGE_H__
#define  __HORLOGE_H__
#include <segment.h>/*La ou est définie la constante : KERNEL_CS*/
#include <stdio.h>
#include <stdlib.h>
#include <cpu.h>
#include <segment.h>
#include <cpu.h>
#include <inttypes.h>
#include "ecran.h"
#include <stdbool.h>

void afficher_l0_c72(char *str);
void tic_PIT(void);
void init_traitant_IT(int32_t num_IT, void (*traitant)(void));
void set_clock_freq();
void masque_IRQ(uint32_t num_IRQ, bool masque);
#endif
