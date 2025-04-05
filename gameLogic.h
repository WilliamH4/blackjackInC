#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int GetChips(){
    long chips[2];
    FILE *ChipFile;
    ChipFile=fopen("chips.bin","rb");
    fread(chips,sizeof(int),2,ChipFile);
    fclose(ChipFile);
    return chips[0];
}



void MakeChipsFile(){
    long chip;
    printf("enter number of chips: \n");
    scanf("%f",&chip);


    long Chips[2]={chip,time(NULL)};
    FILE *ChipFile;
    ChipFile=fopen("chips.bin","wb");
    fwrite(Chips,sizeof(long),2,ChipFile);
    fclose(ChipFile);
}

void UpdateChipsFile(){
    FILE *ChipFile;
    int chips=GetChips();
    long chipl=chips;
    ChipFile=fopen("chips.bin","wb");
    long ctime[2]={chipl,time(NULL)};
    fwrite(ctime,sizeof(long),2,ChipFile);
    fclose(ChipFile);

}

long GetTime(){
    FILE *ChipFile;
    ChipFile=fopen("chips.bin","rb");
    long chips[2];
    fread(chips,sizeof(long),2,ChipFile);
    fclose(ChipFile);
    return chips[1];

}

void returnChips(int Chip){
    if (Chip<=0){
        Chip=0;
    }
    long Chips[2]={Chip,GetTime()};
    FILE *ChipFile;
    ChipFile=fopen("chips.bin","wb");
    fwrite(Chips,sizeof(long),2,ChipFile);
    fclose(ChipFile);
}

void editChips(){
    int NewChips;
    printf("enter new chip count: ");
    scanf("%d",&NewChips);
    printf("newchips:%d\n",NewChips);
    returnChips(NewChips);


}

bool ValidRun(){
    long ptime=GetTime();
    long ctime=time(NULL);
    if (ctime-ptime>60*60*24){
        return 0;
    }
    return 1;
}

int getRandCard(int deck[]){
    for(int i=0;i<52;i++){
        if (deck[i]!=-998){
            break;
        }
    }
    int num=rand()%52;
    while(deck[num]==-998){
        num=(num+1)%52;
    }
    int randNum=deck[num];
    deck[num]=-998;
    return randNum;
}

int *makeDeck(int numDecks){
    int *Deck;
    Deck = (int*)malloc(numDecks*52*sizeof(int));
    //int Deck[numDecks*52];
    for(int i=0;i<numDecks;i++){
        for(int j=0;j<52;j++){
            Deck[j*(i+1)]=j;
        }
    }
    return Deck;
}
int *resetDeck(int Deck[],int numDecks){
    for(int i=0;i<numDecks;i++){
        for(int j=0;j<52;j++){
            Deck[j*(i+1)]=j+1;
        }
    }
    return Deck;
}

int *MakeHand(int Deck[]){
    int *Hand;
    Hand=(int*)malloc(20*sizeof(int));
    for(int i=0;i<20;i++){
        Hand[i]=-998;
    }
    Hand[0]=getRandCard(Deck);
    Hand[1]=getRandCard(Deck);
    return Hand;
}

int *GiveHandCard(int Deck[],int Hand[20]){
    int numCards=0;
    for(int i=0;i<20;i++){
        numCards=i;
        if (Hand[i]==-998){
            break;
        }
    }
    Hand[numCards]=getRandCard(Deck);
}

int *ClearHand(int Hand[20]){
    Hand[0]=-998;
    for (int i=0;i<20;i++){
        if (Hand[i]!=-998){
            Hand[i]=-998;
        }
    }
    return Hand;

}
int *DealHand(int Hand[20],int Deck[20]){
    Hand=ClearHand(Hand);
    GiveHandCard(Deck,Hand);
    GiveHandCard(Deck,Hand);
    // for(int i=0;i<20;i++){
    //     printf("hand=%d\n",Hand[i]);
    // }
    return Hand;
}

int getTotal(int Hand[20]){
    int total=0;
    int numCards=0;
    int numAces=0;
    int card;
    //printf("card1 =%d\n",Hand[0]%13);
    //printf("card2 =%d\n",Hand[1]%13);


    //int Hand[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,52};
    for(int i=0;i<20;i++){
        numCards=i;
        if (Hand[i]==-998){
            break;
        }
    }
    total=0;
    for(int i=0;i<numCards;i++){
        //card=Hand[i];
        //printf("I=%d\n",i);
        card=Hand[i]%13;
        //printf("card =%d\n",card);

        if (card>=10){
            total+=10;
        }else if(card==0){
            //printf("ran %s\n","ace found");
            numAces+=1;
            total=total+11;
        }else{
            //printf("card =%d\n",card);

            total+=card+1;
        }
        //printf("card =%d\n",card);

    }
    //printf("total     =%d\n",total);
    //printf("num aces=%d\n",numAces);
    while (total>21&&numAces>0){
        total-=10;
        numAces-=1;
    }
    //printf("total     =%d\n",total);

    //printf("num aces=%d\n",numAces);
    //printf("total=%d\n",total);

    // if (total>21&&numAces<0){
        
    //}
    return total;
}

int getNumCards(int deck[],int numDecks){
    int numCards=0;
    for (int i=0;i<52*numDecks;i++){
        //printf("card:%d\n",i%13);
        if (deck[i]!=-998){
            numCards+=1;
        }
    }
    return numCards;
}

bool *findWinner(int PlayerHand[],int DealerHand[],int Bet){
    int Chips=GetChips();
    bool playerWin=NULL;
    bool *returnval=&playerWin;
    if(getTotal(PlayerHand)>21){
        Chips-=Bet;
        playerWin=0;
    }else if(getTotal(DealerHand)>21){
        Chips+=Bet;
        playerWin=1;
    }else if (getTotal(PlayerHand)>getTotal(DealerHand)){
        Chips+=Bet;
        playerWin=1;
    }else if (getTotal(PlayerHand)<getTotal(DealerHand)){
        Chips-=Bet;
        playerWin=0;
    }else if (getTotal(PlayerHand)==getTotal(DealerHand)){
        Chips=Chips;
        returnval=NULL;
    }
    printf("playerwin:%d\n",playerWin);
    returnChips(Chips);
    return returnval;   
}

void printDeck(int Deck[52]){
    for (int i;i<52;i++){
        printf("card:%d\n",Deck[i]);
    }
}

// int main(){
//     printf("chips: %d\n",GetChips());
//     editChips();
//     printf("chips: %d\n",GetChips());

//     //returnChips(GetChips()+1000);
//     // UpdateChipsFile();
//     // printf("chips: %d\n",GetChips());
//     // printf("ptime: %ld\n",GetTime());
//     // printf("ctime: %ld\n",time(NULL));
//     // printf("valid run: %d\n",ValidRun());

//     return 0;

// }
