#include "horloge.h"
#include <processus.h>
extern uint32_t time;
extern uint32_t clock_freq;
extern char *heure;
uint32_t hh = 0, mm = 0, ss = 0;
/* PIT : Programmable Interval Timer => horloge*/

/* IRQ : Interrupt Request => canal qui déclenche une interruption*/
/*Pour acquitter une des interruptions qu'on manipule dans ce TP, on doit envoyer la commande sur 8 bits 0x20 sur le port de commande 0x20
 *
 * Comme on veut le faire en C au tout début de la partie du traitant d'interruption gérant l'affichage de l'horloge, on utilisera le bout de code suivant : outb(0x20, 0x20);. */

/*L'horloge est connectée au contrôleur d'interruptions dont on a parlé plus haut. Lorsqu'elle émet un signal, celui-ci est transmis au contrôleur d'interruption via un canal appelé IRQ (Interrupt ReQuest). Dans le cas de l'horloge, il s'agit du canal IRQ0. Le contrôleur d'interruption transmet ce signal au processeur sous la forme d'une interruption : dans le cas de l'horloge, le contrôleur est programmé pour émettre l'interruption 32.*/

/*Il est possible de « masquer » ou « démasquer » chaque IRQ individuellement : si une IRQ est masquée, les signaux transmis seront ignorés par le contrôleur d'interruption. Dans ce TP, le masquage ou démasquage d'une IRQ se fera en deux temps :
 *
 *     il faut d'abord lire la valeur actuelle du masque sur le port de données 0x21 grâce à la fonction inb
 *         l'octet récupéré est en fait un tableau de booléens tel que la valeur du bit N décrit l'état de l'IRQ N : 1 si l'IRQ est masquée, 0 si elle est autorisée : il faut donc forcer la valeur du bit N à la valeur souhaitée (sans toucher les valeurs des autres bits) et envoyer ce masque sur le port de données 0x21 grâce à la fonction outb. */

/*prend en paramètre une chaine de caractères et l'affiche en haut à droite de l'écran : c'est cette fonction qui sera appelée par le traitant d'interruption quand on devra mettre à jour l'affichage de l'heure*/
void afficher_l0_c72(char *str)
{
    place_curseur(0, 72);
    printf("%s", str);
}

/*Traitement d'Interruptions : acquittement de l'interruption et partie gérant l'affichage*/
void tic_PIT(void)
{
    //cli(); 
    outb(0x20, 0x20);
    if (time % clock_freq == 0)
    {
        ss++;
        mm += ss / 60;
        hh += mm / 60;
        ss %= 60;
        mm %= 60;
        hh %= 100;
        /*ss++;
        if (ss == 60)
        {
            ss %= 60;
            mm++;
            if (mm == 60)
            {
                mm %= 60;
                hh++;
            }

        }*/
    }
    time++;
    sprintf(heure, "%02d:%02d:%02d", hh, mm, ss);
    afficher_l0_c72(heure);
    ordonnance();
    //sti();
}

/*initialiser l'entrée 32 dans la table des vecteurs d'interruptions*/
void init_traitant_IT(uint32_t num_IT, void (*traitant)(void))
{
    uint32_t *num_case = 0;
    uint32_t entree_idt = KERNEL_CS;
    uint32_t ad_traitant = (uint32_t) traitant;

    num_case = (uint32_t *) 0x1000;/*Division par 4 pour avoir num_case = 0x1000*/
    num_case = num_case + 2 * num_IT;/*Idem que ligne précédente*/

    /*ecriture du premier mot de l'entrée*/
    entree_idt <<= 16;
    ad_traitant &= 0x0000FFFF;
    entree_idt |= ad_traitant;
    //num_case[2 * num_IT] = entree_idt;
    *num_case = entree_idt;

    /*ecriture du deuxième mot de l'entrée*/
    ad_traitant = (uint32_t)traitant;
    ad_traitant &= 0xFFFF0000;
    entree_idt = ad_traitant | 0x00008E00;
    //num_case = 0;
    //num_case += 0x1000/4;/*Division par 4 pour avoir num_case = 0x1000*/
    //num_case +=  1;/*Idem que ligne précédente*/
    num_case[1] = entree_idt;


}

/*régler la fréquence de l'horloge programmable*/
void set_clock_freq()
{
    /* indique à l'horloge que l'on va lui envoyer la valeur de réglage de la fréquence sous la forme de deux valeurs de 8 bits chacunes qui seront émises sur le port de données*/
    outb(0x34, 0x43);

    /*envoie des 8 bits de poids faibles de la valeur de réglage de la fréquence sur le port de données 0x40*/
    uint16_t freq = (0x1234DD / clock_freq);
    uint8_t freq_8b = freq;
    /*freq_8b contient les 8 bits de poids faibles de freq*/
    outb(freq_8b, 0x40);

    /*freq_8b contient les 8 bits de poids forts de freq*/
    freq_8b = freq >> 8;
    /* envoie ensuite les 8 bits de poids forts de la valeur de réglage sur le même port 0x40*/
    outb(freq_8b, 0x40);
}

/*démasque l'IRQ0 pour autoriser les signaux en provenance de l'horloge*/
void masque_IRQ(uint32_t num_IRQ, bool masque)
{
    /*lecture de la valeur actuelle du masque*/
    uint8_t masque_actuel = inb(0x21);
    
    if(masque)
    {
        uint8_t set_bit_IRQ = 1;
        set_bit_IRQ <<= num_IRQ;
        /*Met à 1 le num_IRQ bit de masque actuel*/
        masque_actuel |= set_bit_IRQ;
        outb(masque_actuel, 0x21);
    }
    else
    {
        uint8_t reset_bit_IRQ = 1;
        reset_bit_IRQ <<= num_IRQ;
        reset_bit_IRQ ^= 0xFF;
        /*Met à 0 le num_IRQ bit de masque actuel*/
        masque_actuel &= reset_bit_IRQ;
        outb(masque_actuel, 0x21);
    }
}


