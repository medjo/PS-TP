#ifndef __ECRAN_H__
#define __ECRAN_H__
#include <inttypes.h>

void console_putbytes(char *chaine, int32_t taille);
uint16_t *ptr_mem(uint32_t lig, uint32_t col);
void ecrit_car(uint32_t lig, uint32_t col, char c, uint32_t cl, uint32_t cf, uint32_t ct);
void place_curseur(uint32_t lig, uint32_t col);
void efface_ecran(void);
void traite_car(char c);
void defilement(void);

#endif
