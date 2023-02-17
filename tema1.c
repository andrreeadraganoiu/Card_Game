#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#define LENGTH 52
typedef struct node
{
    char cardNo;
    char color;
    int value;
    struct node* next;
} TNode, *TList;

typedef struct SList
{
	TNode* first; // link to the first node
    TNode* intermediate;
    TNode* razboi_card;
	TNode* last; // link to the last node
    int size;

}*SList, StList;

//adaug un nod la sfarsitul listei
TList add_next(TList head, char cardNo, char color, int value)
{
    TList p;

    TNode* node = (TNode*)malloc(sizeof(TNode));
    node->cardNo = cardNo;
    node->color = color;
    node->value = value;
    node->next = NULL;

    if(head == NULL)
        head = node;
    else
    {
        p = head;
        while(p->next != NULL)
            p = p->next;
        p->next = node;
    }
    return head;
}

//creez pachetul
TList init_packet(TList head)
{
    char cardNo[] = "AKQJT98765432";
    char color[] = "shcd";
    int value[] = {15, 14, 13, 12, 10, 9, 8, 7, 6, 5, 4, 3, 2};
    int a = strlen(cardNo);
    int b = strlen(color);
    for(int i = 0; i < a; i++)
    {
        for(int j = 0; j < b; j++)
        {
            head  = add_next(head, cardNo[i], color[j], value[i]);
        }
    }
    return head;
}

//////////////////// AMESTECARE /////////////////////

TList inversare(TList head)
{
    TList current = head;
    TList inverse = NULL;
    while(current != NULL)
    {
        head = head->next;
        current->next = inverse;
        inverse = current;
        current = head;
    }
    return inverse;

}

void freeList(TList head) 
{
    TList aux;
    while (head != NULL)
    {
        aux = head;
        head = head->next;
        free(aux);
    }
}

void free_sList(SList* L)
{
    if (*L)
    {    
        freeList((*L)->first);
        free(*L);
    }
}

void free_resources(SList Player[], int n, SList Table)
{
    free_sList(&Table);
    for (int i = 0; i < n; i++)
        free_sList(&Player[i]);
}

TList npiles(TList head, int nr_packs, int index[])
{
    int one_pack_size = LENGTH/nr_packs;
    int cnt = 0;

    TList subpack[nr_packs];
    for(int i = 0; i < nr_packs; i++)
        subpack[i] = NULL;
    TList p = head;
    while(p != NULL)
    {
        for(int i = 0; i < one_pack_size; i++)
        {
            subpack[cnt] = add_next(subpack[cnt], p->cardNo, p->color, p->value);
            p = p->next;
        }
        cnt++;
    }
    freeList(head);
    TList list = NULL, aux;
    for(int i = 0; i < nr_packs; i++)
    {
        TList p = subpack[index[i] - 1];
        while  (p != NULL)
        {
            list = add_next(list, p->cardNo, p->color, p->value);
            aux  = p;
            p = p->next; 
            free(aux);
        }
    }
    return list;
}

TList intercalare(TList head)
{
    TList p1, p2, q1, q2;
    p1 = q1 = head;
    p2 = head->next;
    for(int i = 1; i <= 26; i++)
        q1 = q1->next;
    q2 = q1->next;

    while(q2->next != NULL)
    {
        p1->next = q1;
        q1->next = p2;
        p1 = p2;
        q1 = q2;
        p2 = p2->next;
        q2 = q2->next;
    }
    p1->next = q1;
    q1->next = p2;
    p2->next = q2;
    return head;
}

TList mongean(TList head)
{
    TList prev = head;
    TList current  = head->next;
    while(current != NULL)
    {
        prev->next = current->next;
        current->next = head;
        head = current;
        prev = prev->next;
        if(prev == NULL)
            break;
        current  = prev->next;
 
    }
    return head;
}

void print_list(TList head, FILE* outputFile)
{
    TList p = head;
    while(p != NULL)
    {
       fprintf(outputFile,"%c%c ", p->cardNo, p->color);
       p = p->next;
    }
    printf("\n");
    
}
TList read_amestecare(TList head, FILE* inputFile)
{
    int n, nr_packs, index[52]={0};
    char op_amestecare[20];
    fscanf(inputFile, "%d", &n);
    for(int i = 1; i <= n; i++)
    {
        fscanf(inputFile, "%s", op_amestecare);
        if(!strcmp(op_amestecare, "inversare"))
        {
            head = inversare(head);
        }
            
        if(!strcmp(op_amestecare, "mongean"))
        {
            head = mongean(head);
        }
                
        if(!strcmp(op_amestecare, "npiles"))
        {
            fscanf(inputFile, "%d", &nr_packs);
            for(int j = 0; j < nr_packs; j++)
            fscanf(inputFile, "%d", &index[j]);
            head = npiles(head, nr_packs, index);
        }        
        if(!strcmp(op_amestecare, "intercalare"))
        {
            head = intercalare(head);
        }
    }
    return head;
}

//////////////////// RAZBOI /////////////////////

// mut cartea de la inceput la sfarsitul pachetului aceluiasi jucator
SList fromSamePlayer(SList Player)
{
    Player->last->next = Player->first;
    Player->last = Player->first;
    Player->first = Player->last->next;
    Player->last->next = NULL;
    return Player;
}
//prima carte a lui Player2 se adauga la sfarsitul pachetului lui Player1
void fromPlayertoPlayer(SList* Player1, SList* Player2)   
{                                                           
    (*Player1)->last->next = (*Player2)->first;
    (*Player1)->last = (*Player2)->first;
    (*Player2)->first = (*Player1)->last->next;
    (*Player1)->last->next = NULL;
}

int min2(int a, int b)
{
    return a < b ? a : b;
}
int minim(int a, int b, int c, int d)
{
    return min2(min2(a, b), min2(c, d));
}

void printWinner(SList Player1, SList Player2, FILE* outputFile)
{
    char cardNo[] = "0123456789TxJQKA"; //valori : 2 3 4 5 6 7 8 9 10 12 13 14 15 
    int cards_1[16] = {0};
    int cards_2[16] = {0};
    TList p = Player1->first;
    while(p != NULL)
    {
        cards_1[p->value]++;
        p = p->next;
    }
    p = Player2->first;
    while(p != NULL)
    {
        cards_2[p->value]++;
        p = p->next;
    }
    int i;
    
    for(i = 15; i >= 2 && cards_1[i] == cards_2[i]; i--);

    if(cards_1[i] > cards_2[i])
        fprintf(outputFile, "1%c", cardNo[i]);
    else
        fprintf(outputFile, "2%c", cardNo[i]);
}

void razboiMove(SList* Player, int min)
{
    (*Player)->razboi_card = (*Player)->first;
    for(int i = 1; i <= min; i++)
        //Player->razboi_card = valoarea cartii care decide cine a castigat acest razboi
        (*Player)->razboi_card = (*Player)->razboi_card->next; 
    //Player->intermediate = valoarea dinaintea valorii care decide cine a castigat razboiul    
    (*Player)->intermediate = (*Player)->razboi_card->next;            

    (*Player)->razboi_card->next = NULL; // rup legatura primei parti pana la intermediate pentru ca vreau sa inversez 
                                         //aceasta lista si sa o lipesc la loc inversata
                                         // ca sa pot folosi functiile care imi iau cartea de la inceputul pachetului     
    TList beginning = (*Player)->first; 
    (*Player)->first = inversare(beginning);    
    if((*Player)->size == min + 1)
    {
        (*Player)->last = beginning;
        (*Player)->last->next = NULL;
    }
    while(beginning->next != NULL)
        beginning = beginning->next;
    beginning->next = (*Player)->intermediate;     //refac legaturile
}

void razboi(TList head, FILE* outputFile)
{
    int hands = 0; 
    SList Player1 = (SList)malloc(sizeof(StList));
    SList Player2 = (SList)malloc(sizeof(StList));

    TList p = head;
    for(int i = 1; i <= 25; i++)
        p = p->next;
    Player1->first = head;   //prima carte din pachetul primului jucator
    Player1->last = p;         //ultima carte din pachetul primului jucator
    Player2->first = p->next; //prima carte din pachetul celui de-al doilea jucator
    while(p->next != NULL)
        p = p->next;
    Player2->last = p;   //ultima carte a jucatorului 2 

    Player1->last->next = NULL;  //rup legatura ultimei carti a primului jucator 
    Player1->size = Player2->size = LENGTH/2;

 while(Player1->size != 0 && Player2->size != 0 && hands < 100)
 {   
    
    if(Player1->first->value > Player2->first->value)
    {       
        Player1 = fromSamePlayer(Player1);  //iau prima carte a primului jucator si o pun la sfarsit
        fromPlayertoPlayer(&Player1, &Player2); //iar prima carte a jucatorului 2 si o pun la sfarsitul pachetului jucatorului 1
        Player1->size++;
        Player2->size--;
        hands++;
    }
    else
        if(Player1->first->value < Player2->first->value)
        {
            Player2 = fromSamePlayer(Player2);    //iau prima carte a jucatorului 2 si o pun la sfarsit
            fromPlayertoPlayer(&Player2, &Player1);   /*iar prima carte a jucatorului 1 si o pun la sfarsitul 
                                                       pachetului jucatorului 2*/
            Player2->size++; 
            Player1->size--;
            hands++;
        }
        else
            if(Player1->first->value == Player2->first->value)
            {
                hands++;
                Player1->razboi_card = Player1->first;     
                Player2->razboi_card = Player2->first;
                int min;
                min = minim(Player1->size , Player1->first->value, Player2->size , Player2->first->value);
            
                if(Player1->size == min || Player2->size == min)
                    min--;

                razboiMove(&Player1, min);
                razboiMove(&Player2, min);
                
                if(Player1->razboi_card->value > Player2->razboi_card->value) //razboi_card decide catre ce jucator de duc cartile
                {
                    for(int i = 1; i <= min + 1; i++)
                    {
                        Player1 = fromSamePlayer(Player1);
                        
                    }
                    for(int i = 1; i <= min + 1; i++)
                    {
                        fromPlayertoPlayer(&Player1, &Player2);
                        Player1->size++;
                        Player2->size--;   
                    }
                }
                else
                    if(Player2->razboi_card->value > Player1->razboi_card->value)
                    {
                        for(int i = 1; i <= min + 1; i++)
                        {
                        Player2 = fromSamePlayer(Player2);
                        
                        }  
                        for(int i = 1; i <= min + 1; i++)
                        {  
                            fromPlayertoPlayer(&Player2, &Player1);
                            Player2->size++;
                            Player1->size--;          
                        }
                    }         
            }      
 }
    if(!Player1->size)   //daca Player1 a ramas fara carti
        fprintf(outputFile,"2");
    else
    {
        if(!Player2->size)  //daca Player2 a ramas fara carti
            fprintf(outputFile,"1");
        else
        {
            printWinner(Player1, Player2, outputFile);
        }
    }
        free_sList(&Player1);
        free_sList(&Player2);
}

//////////////////// ROLLING STONE /////////////////////

//daca Player are o carte de culoare color
int contains(SList Player, char color)
{
    TList p = Player->first;
    while(p != NULL)
    {
        if(p->color == color)
            return 1;
        p = p->next;
    }
    return 0;
}

//functie care imi returneaza adresa elementului de culoare color si actualizeaza lista jucatorului
TList ReturnCard(SList* Player, char color)
{
    //daca nodul de mutat este chiar primul
    if ((*Player)->first->color == color)
    {
        TList searched_color = (*Player)->first;
        (*Player)->first = (*Player)->first->next;
        (*Player)->size--;
        return searched_color;
    }

    //daca elementul de mutat nu este primul
    TList prev  = (*Player)->first;
    TList element = (*Player)->first->next;

    while(element != NULL)
    {
        if(element->color == color)
        {
            TList searched_color = element;
            prev->next = element->next;
            if (element->next == NULL)
            {
                (*Player)->last = prev;
            }
            (*Player)->size--;
            return searched_color;
        }
        prev = element;
        element = element->next;
    }
    return NULL;
}

void MoveToTable(SList *Table, TList TableCard)
{
    TableCard->next = NULL;
    if((*Table)->first == NULL)
    {
        (*Table)->first = (*Table)->last = TableCard;
    }
    else
    {        
        (*Table)->last->next = TableCard;            
        (*Table)->last = TableCard;    
    }
    (*Table)->size++;
}

//Adaug lista L2 la finalul lui L1
void addTableToPlayer(SList *L1, SList *L2)
{
    (*L1)->last->next = (*L2)->first;
    (*L1)->last = (*L2)->last;
    (*L1)->size += (*L2)->size;    
    (*L2)->first = (*L2)->last = NULL;
    (*L2)->size = 0;    
}

void init_Player(TList head, SList* Player)
{
    (*Player)->first = head;
    TList p = head;
    for(int i = 1; i <= 12; i++)
        p = p->next;    
    (*Player)->last = p;             
}

void init_PlayerStruct(TList head, SList Player[4])
{
    TList p = head;

    for (int i = 0; i < 4; i++ )
    {
        init_Player(p, &Player[i]);
        p = Player[i]->last->next;
        Player[i]->last->next = NULL;
        Player[i]->size = LENGTH/4;
    }
}

void init_list(SList* L)
{
    (*L) = (SList)malloc(sizeof(StList));
    (*L)->first = (*L)->last = NULL;
    (*L)->size = 0;
}

void rollingstone(TList head, FILE* outputFile)
{
    SList Player[4], Table;
    int i, j;
    char color[] = "shcd";
    TList TableCard;

    init_list(&Table);
    for(int i = 0; i < 4; i++)
        init_list(&Player[i]);  //aloc memorie
    init_PlayerStruct(head, Player); //initializez pointerii fiecarui jucator (first si last) catre partea lor de pachet
                                     // iar pe Player il trimit ca vector
    i = j = 0;
    while (!(Player[0]->size == 0 || Player[1]->size == 0 || Player[2]->size == 0 || Player[3]->size == 0))
    {
        if (contains(Player[i % 4], color[j % 4]))
        {
            // ReturnCard returneaza adresa elementului de culoare color si actualizeaza lista jucatorului
            TableCard = ReturnCard(&Player[i % 4], color[j % 4]); //TableCard = cartea cu adresa cautata       
            MoveToTable(&Table, TableCard); //adaug cartea la sfarsitul tablei
            i++;
        }
        else
        {         
            if (Table->last) //daca tabla nu este vida
                addTableToPlayer(&Player[i % 4], &Table); //adaug cartile de pe tabla jucatorului si golesc tabla     
            j++;            
        }       
    }
    for (i = 0; i < 4; i++)
       if (!Player[i]->size)
       {
           fprintf(outputFile, "%d", i + 1);
           break;
       }
    free_resources(Player, 4, Table);
}

//////////////////// SCARABEII EGIPTENI /////////////////////

//Adaug un nod la inceputul listei
void addCardToTableTop(SList *Table, TList TableCard)
{
    TableCard->next = NULL;
    if((*Table)->first == NULL)
    {
        (*Table)->first = (*Table)->last = TableCard;
    }
    else
    {
        TList p = (*Table)->first;
        (*Table)->first = TableCard;
        (*Table)->first->next = p;
    }
    (*Table)->size++;
}

int isScarabeu(char cardNo)
{
        return cardNo == 'A' || cardNo == 'K' || cardNo == 'Q' || cardNo == 'J';
}

int isNumber(char cardNo)
{
        return cardNo != 'A' && cardNo != 'K' && cardNo != 'Q' && cardNo != 'J' && cardNo != '8';
}

TList getFirstCard(SList* Player)
{
    TList card = (*Player)->first;
    (*Player)->first = (*Player)->first->next;
    if((*Player)->first == NULL)   //am luat ultima carte a player-ului
        (*Player)->last = NULL;
    (*Player)->size--;

    return card;
}

int getNextPlayerIndex(int i, int dir)
{
    i = i + dir;
    if (dir == 1 && i == 4)
        i = 0;
    else
        if (dir == -1 && i == -1)
            i = 3;
    return i;
}

void scarabei(TList head, FILE* outputFile)
{           
    TList CurrentCard, PreviousCard;
    SList Player[4], Table, PreviousPlayer;
    int i = 0, j = -1, dir = 1;

    init_list(&Table);
    for(int i = 0; i < 4; i++)
        init_list(&Player[i]);   // aloc memorie
    init_PlayerStruct(head, Player);  //initializez pointerii fiecarui jucator (first si last) catre partea lor de pachet
    
    PreviousCard = NULL;
    PreviousPlayer = NULL;
    while (!(Player[0]->size == 52 || Player[1]->size == 52 || Player[2]->size == 52 || Player[3]->size == 52)) //cat timp nu s-a terminat jocul
    {       
        CurrentCard = getFirstCard(&Player[i]);                     //extrag prima carte a jucatorului curent
        addCardToTableTop(&Table, CurrentCard);                     //si o pun pe masa

        if (CurrentCard->cardNo == '8')
            dir = -dir;
        else 
        {   if (isNumber(CurrentCard->cardNo))
            {
                if (PreviousCard && isScarabeu(PreviousCard->cardNo))
                {
                    if (Table->last)        //Daca tabla nu este vida
                    {
                        addTableToPlayer(&PreviousPlayer, &Table);      //castigatorul ramane la masa
                        PreviousCard = NULL;                            
                        PreviousPlayer = NULL;                    
                        i = j;
                        continue;
                    }
                }                
            }
        }
        PreviousCard = CurrentCard;
        PreviousPlayer = Player[i];
        j = i;
        do
        {
            i = getNextPlayerIndex(i, dir);
        } while (Player[i]->size == 0);      
    }

    for (i = 0; i < 4; i++)
        if (Player[i]->size == 52)
        {
            fprintf(outputFile, "%d", i + 1);
            break;
        }   
    free_resources(Player, 4, Table); 
}

int main()
{
    char operation[20];
    TList head = NULL; 
    head = init_packet(head);

    FILE *inputFile = fopen("./in", "r");
    FILE *outputFile = fopen("./out", "w");
    
    fscanf(inputFile, "%s", operation);
    if(!strcmp(operation, "amestecare"))
    {
        head = read_amestecare(head, inputFile);
        print_list(head, outputFile);
        freeList(head);
    }
    if(!strcmp(operation, "razboi"))
    {
        head = read_amestecare( head, inputFile);
        razboi(head , outputFile);
    }
    if(!strcmp(operation, "rollingstone"))
    {
        head = read_amestecare(head, inputFile);
        rollingstone(head , outputFile);
    }
    if(!strcmp(operation, "scarabei"))
    {
        head = read_amestecare(head, inputFile);
        scarabei(head , outputFile);
    }
    fclose(inputFile);
    fclose(outputFile);
    
    return 0;
}