#include "ecran.h"
#include <string.h>
#include <inttypes.h>
#include <cpu.h>

void place_curseur(uint32_t lig, uint32_t col);
void defilement(void);

/*renvoie un pointeur sur la case mémoire correspondant aux coordonnées fournies*/
uint16_t *ptr_mem(uint32_t lig, uint32_t col)
{
	uint16_t *ptr_mem = NULL;
	ptr_mem = (uint16_t*)(0xB8000 + 2 * (lig * 80 + col));
	return (ptr_mem);
}

/*écrit le caractère c aux coordonnées spécifiées*/
void ecrit_car(uint32_t lig, uint32_t col, char c, uint32_t cl, uint32_t cf, uint32_t ct)
{
	uint16_t *ptr = ptr_mem(lig, col);
	*ptr = c |(cl << 15) | (cf << 12) | (ct<<8) ;
    if(col == 79 && lig < 24)
    {
        place_curseur(lig + 1, 0);
    }
    else if(col < 79)
    {
        place_curseur(lig, col + 1);
    }
    else
    {
        defilement();
        place_curseur(24, 0);
    }
}

/*place le curseur à la position donnée */
void place_curseur(uint32_t lig, uint32_t col)
{
	uint16_t pos = col + lig * 80;

	/*envoye la commande 0x0F sur le port de commande (0x3D4) pour indiquer à la carte que l'on va envoyer la partie basse de la position du curseur*/
	outb(0x0F, 0x3D4);
	/*envoye cette partie basse sur le port de données (0x3D5)*/
	uint8_t value = pos;
	outb(value, 0x3D5);
	/*envoye la commande 0x0E sur le port de commande pour signaler qu'on envoie maintenant la partie haute*/
	outb(0x0E, 0x3D4);
	/*envoye la partie haute de la position sur le port de données. */
	value = pos >> 8;
	outb(value, 0x3D5);
}

/*efface l'écran*/
void efface_ecran(void)
{
	uint32_t lig = 0;
	uint32_t col = 0;
	
	while(lig < 24)
	{
		if(col == 80)
		{
			col = 0;
			lig++;
		}
		ecrit_car(lig, col,' ',0,0,15);
		col++;
	}
    lig = 1;
    while (lig < 80)
    {
		ecrit_car(24, lig++,' ',0,0,15);
	}
    place_curseur(0, 0);
}

/*traite un caractère donné (c'est à dire l'affiche si c'est un caractère normal ou implante l'effet voulu si c'est un caractère de contrôle) */
void traite_car(char c)
{

    uint16_t place = 0;
	uint8_t pos = 0;
        /*envoye la commande 0x0F sur le port de commande (0x3D4) pour indiquer à la carte que l'on va recevoir la partie basse de la position du curseur*/
	outb(0x0F, 0x3D4);
	/*revoit cette partie basse sur le port de données (0x3D5)*/
	pos = inb(0x3D5);
	/*envoye la commande 0x0E sur le port de commande pour signaler qu'on recoit maintenant la partie haute*/
	outb(0x0E, 0x3D4);
	/*recoit la partie haute de la position sur le port de données. */
	place = inb(0x3D5);
    place = place << 8;/*met les 8bits de la partie haute de la position du curseur dans la partie haute de la variable place*/
    place |= pos;/*met la partie basse de la position dans place grace à un ou bit a bit*/
    uint32_t col = place % 80;
    uint32_t lig = (place - col) / 80 ;
	if(c > 31 && c < 127)
	{
		ecrit_car(lig, col,c,0,0,15);
	}
	else
    {
        switch(c)
        {
            /*Recule le curseur d'une colonne s'il n'est pas sur la première colonne */
            case 8:
                if (col > 0)
                {
                    place_curseur(lig, col - 1);
                }
                break;
            /*Avance à la prochaine tabulation (colonnes 1, 9, 17, ..., 65, 73, 80)*/
            case 9:
                place = col + (8 - (col % 8)) % 8;// col + (8 - (col - 1) % 8) % 8);
                if (place >= 80)
                    place_curseur(lig, 79);
                else
                    place_curseur(lig, place);
                break;
            /*Déplace le curseur sur la ligne suivante, colonne 1 */
            case 10:
                if (lig < 25)
                {
                    if(lig == 24)
                    {
                        defilement();
                        lig--;
                    }
                    place_curseur(lig + 1, 0);
                }
                break;
            /*Efface l'écran et place le curseur sur la colonne 1 de la ligne 1 */
            case 12:
                efface_ecran();
                place_curseur(0, 0);
                break;
            /* Déplace le curseur sur la ligne actuelle, colonne 1 */
            case 13:
                place_curseur(lig, 0);
                break;
            default:
                {}
                break;
        }
        
        
    }
//		ecrit_car(lig, uint32_t col, char c, char cl, char cf, char ct)
}

/*fait remonter d'une ligne l'affichage à l'écran*/
void defilement(void)
{
    int i = 0;
    memmove(ptr_mem(0, 0), ptr_mem(1, 0), 3840);
    for (i = 0 ; i < 79 ; i++)
    {
		ecrit_car(24, i,' ',0,0,15);
    }
	uint16_t *ptr = ptr_mem(24, 79);
	*ptr = ' ';
}

void console_putbytes(char *chaine, int32_t taille)
{
    int i = 0;
    while (i < taille)
    {
        traite_car(chaine[i]);
        i++;
    }
}
