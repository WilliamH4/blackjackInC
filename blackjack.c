#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "gameLogic.h"

#define MAX_IMAGES 100

// Function to load an image from a file
HBITMAP LoadImageFromFile(LPCSTR filename) {
    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBitmap) {
        printf("Failed to load image: %s\n", filename);
    }
    return hBitmap;
}

// Function to load all bitmap images from a directory
int LoadImagesFromDirectory(LPCSTR directory, HBITMAP images[], int maxImages) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*.bmp", directory);

    hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("No images found in the directory.\n");
        return 0;
    }

    int count = 0;
    do {
        if (count >= maxImages) {
            printf("Reached maximum number of images.\n");
            break;
        }

        char filePath[MAX_PATH];
        snprintf(filePath, sizeof(filePath), "%s\\%s", directory, findFileData.cFileName);

        HBITMAP hBitmap = LoadImageFromFile(filePath);
        if (hBitmap) {
            images[count++] = hBitmap;
        } else {
            printf("Failed to load image: %s\n", filePath);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
    return count;
}

// Function to display the bitmap
void DisplayBitmap(HDC hdc, HBITMAP hBitmap,int x,int y) {
    HDC hMemDC = CreateCompatibleDC(hdc);
    SelectObject(hMemDC, hBitmap);

    BITMAP bitmap;
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);
    BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hMemDC, 0, 0, SRCCOPY);

    DeleteDC(hMemDC);
}

void displayDelerCards(HDC hdc ,HBITMAP hbitmap[100],int screenwidth,int screenhight,int cards[],bool revealFirst){
    int ypos=screenhight/6;
    int xpos=screenwidth/2;
    //xpos=0;

    int NumCards=0;

    for(int i=0;i<200;i++){
        if (cards[i]!=-998){
            if (i==0&&!revealFirst){
                DisplayBitmap(hdc,hbitmap[0],xpos+i*20,ypos);

            }else{
                DisplayBitmap(hdc,hbitmap[cards[i]+1],xpos+i*20,ypos);
            }
        }else{
            break;
        }
    }
    

    //DisplayBitmap(hdc,hbitmap[0],xpos,ypos);
    //DisplayBitmap(hdc,hbitmap[cards[1]],xpos+20,ypos);
}

void displayPlayerCards(HDC hdc ,HBITMAP hbitmap[100],int screenwidth,int screenhight,int cards[]){
    int ypos=screenhight/1.9;
    int xpos=screenwidth/2;
    // for(int i=0;i<20;i++){
    //     printf("cards=%d\n",cards[i]);
    // }
    for (int i=0;i<20;i++){
        if (cards[i]!=-998){
            DisplayBitmap(hdc,hbitmap[cards[i]+1],xpos+i*20,ypos);
        }
    }
    char text[10];
    sprintf(text,"%s%d","total= ",getTotal(cards));
    TextOut(hdc,xpos,ypos+100,text,sizeof(text));
}
void DisplayBetOptions(HDC hdc,int screenwidth,int screenHight,int Bet){

    //sprintf(output, "%s%d%s", str1, num, str2);
    int ypos=screenHight/3;
    int xpos=screenwidth/2;
    char text[20];
    char text2[20];
    sprintf(text,"%s%d","current bet: ",Bet);

    TextOut(hdc,xpos,ypos,"place Bet",10);
    TextOut(hdc,xpos,ypos+18*1,text,strlen(text));
    TextOut(hdc,xpos,ypos+18*2,"1: 5",5);
    TextOut(hdc,xpos,ypos+18*3,"2: 10",6);
    TextOut(hdc,xpos,ypos+18*4,"3: 25",6);
    TextOut(hdc,xpos,ypos+18*5,"4: 50",6);
    TextOut(hdc,xpos,ypos+18*6,"5: 100",7);
    TextOut(hdc,xpos,ypos+18*7,"6: 500",7);
    TextOut(hdc,xpos,ypos+18*8,"7: 1000",8);
    TextOut(hdc,xpos,ypos+18*9,"8: 5000",8);
    TextOut(hdc,xpos,ypos+18*10,"9: all in!!",12);


    TextOut(hdc,xpos,ypos+18*11,"0: Reset bet",13);


}
void DisplayHitOptions(HDC hdc, int screenwidth,int screenHight){
    int ypos=screenHight/20;
    int xpos=screenwidth/2;
    TextOut(hdc,xpos,ypos,"Hit or stay?",13);
    TextOut(hdc,xpos,ypos+18,"1: hit",7);
    TextOut(hdc,xpos,ypos+18*2,"2: stay",8);
}

void DeclareWiner(HDC hdc,bool *Playerwin,int playerTotal,int DealerTotal){
    char Text[25];
    if (Playerwin==NULL){
        sprintf(Text,"%s%d","Tie at ",playerTotal);
    }else if(*Playerwin){
        sprintf(Text,"%s%d%s%d","player won ",playerTotal," to ",DealerTotal);
    }else{
        sprintf(Text,"%s%d%s%d","player lost ",playerTotal," to ",DealerTotal);
    }
    TextOut(hdc,100,100,Text,strlen(Text));
    Sleep(1500);
}
void Push(HDC hdc){}

// Window procedure function
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HBITMAP images[MAX_IMAGES];
    static int imageCount;
    static int currentImageIndex = 0;
    static BOOL GetingBet=1;
    static BOOL Delt=0;
    static int currentBet=0;
    static int DealerHand [20];
    static int playerHand [20];
    static bool playerTurn=0;
    static bool DealerTurn=0;
    static bool BlackJack=0;
    static int numDecks=6;
    static int *Deck=NULL;
    int  screenWidth=GetSystemMetrics(SM_CXSCREEN);
    int  screenHeight=GetSystemMetrics(SM_CYSCREEN);
    //printf("Received message: uMsg = %u, wParam = %lu, lParam = %ld\n", uMsg, wParam, lParam);

    if (Deck==NULL){
        printf("makeing deck\n");
        Deck=(int*)malloc(52*numDecks*sizeof(int));
        if (Deck==NULL){
            printf("faild to allocate memory for the Deck");
        }
        Deck=makeDeck(numDecks);
        printf("deck:%d\n",Deck);
        printf("numcards:%d\n",getNumCards(Deck,numDecks));
    }

    switch (uMsg) {

    case WM_CREATE:
        printf("created\n");

        srand(time(NULL));
        printf("Loading images...\n");
    
        imageCount = LoadImagesFromDirectory("card-BMPs", images, MAX_IMAGES);
        if (imageCount == 0) {
            MessageBox(hwnd, "No images loaded!", "Error", MB_OK | MB_ICONERROR);
            return -1; // Exit if no images are loaded
        }
    
        printf("Images loaded successfully. Image count: %d\n", imageCount);
        break;

    case WM_PAINT: {
        //printf("painting\n");


        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        HBRUSH hBrush = CreateSolidBrush(RGB(30, 92, 58));
        FillRect(hdc, &ps.rcPaint, hBrush);
        DeleteObject(hBrush);
        SetBkColor(hdc, RGB(30, 92, 58));
        SetTextColor(hdc, RGB(255, 255, 255));

        //printf("the bool=%d\n",turn);
        //int cards[60];
        // for (int i=0;i<53;i++){
        //     cards[i]=i;
        // }
        // cards[0]=4;
        // cards[1]=52;
        // cards[2]=5;
        // cards[3]=-998;
        // displayDelerCards(hdc,images,screenWidth,screenHeight,cards,1);


        RECT rect;
        GetClientRect(hwnd, &rect);
        int screenWidth = rect.right - rect.left;
        int screenHeight = rect.bottom - rect.top;


        char text[24];//buffer
        sprintf(text,"%s%d","Total chips: ",GetChips());
        TextOut(hdc,0,0,text,strlen(text));


        if (GetingBet==true){
            DisplayBetOptions(hdc,screenWidth,screenHeight,currentBet);
        }
        //EndPaint(hwnd, &ps);
        //printf("delt        %d\n",Delt);
        //printf("player turn %d\n",playerTurn);
        //printf("DealerTurn  %d\n",DealerTurn);
        if (BlackJack==1){
            displayDelerCards(hdc,images,screenWidth,screenHeight,DealerHand,DealerTurn);
            displayPlayerCards(hdc,images,screenWidth,screenHeight,playerHand);
            Sleep(500);
            TextOut(hdc,100,100,"Black jack player win!",23);
            int chips=GetChips()+currentBet*(1.5);
            returnChips(chips);
            Sleep(1500);
            BlackJack=0;
            GetingBet=1;
            playerTurn=0;
            DealerTurn=0;
            Delt=0;
            InvalidateRect(hwnd,NULL,0);
        }

        if (getNumCards(Deck,numDecks)<10){
            TextOut(hdc,screenWidth/2,0,"last turn before reshuffle",27);
            //Sleep(1000);
        }


        if (Delt==true){
            //int cards[20]={currentImageIndex,currentImageIndex+1,currentImageIndex+2,currentImageIndex+3,currentImageIndex+4};
            // for(int i=0;i<20;i++){
            //     if (playerHand[i]==-998){
            //         break;
            //     }
            //     printf("player hand=%d%s%d\n",playerHand[i]," ",playerHand[i]%13);
            // }
            displayDelerCards(hdc,images,screenWidth,screenHeight,DealerHand,DealerTurn);
            displayPlayerCards(hdc,images,screenWidth,screenHeight,playerHand);
            //playerHand[0]=26;
            //printf("deler hand%d\n",getTotal(DealerHand));
            if (playerTurn){
                DisplayHitOptions(hdc,screenWidth,screenHeight);
            }else if (DealerTurn&&(getTotal(DealerHand)<17)){
                GiveHandCard(Deck,DealerHand,numDecks);
                Sleep(500);
                InvalidateRect(hwnd,NULL,TRUE);
            }else if(getTotal(DealerHand)>=17){
                printf("player Total=%d\n",getTotal(playerHand));
                for(int i=0;i<20;i++){
                    if (playerHand[i]!=-998){
                    //printf("player hand=%d\n",playerHand[i]%13);
                    }
                }
                printf("Dealer Total=%d\n",getTotal(DealerHand));
                for(int i=0;i<20;i++){
                    if (DealerHand[i]!=-998){
                    //printf("deler hand=%d\n",DealerHand[i]%13);
                    }
                }
                Sleep(200);
                printf("numcards:%d\n",getNumCards(Deck,numDecks));
                //printDeck(Deck);
                Delt=0;
                DealerTurn=0;
                bool *playerWin=findWinner(playerHand,DealerHand,currentBet);
                DeclareWiner(hdc,playerWin,getTotal(playerHand),getTotal(DealerHand));
                printf("chips=%d\n",GetChips());
                currentBet=0;
                GetingBet=1;
                InvalidateRect(hwnd,NULL,TRUE);

            }
        }
        // if (currentImageIndex < imageCount) {
        //     DisplayBitmap(hdc, images[currentImageIndex],500,500);
        // }
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_SIZE:
        printf("resized\n");

        InvalidateRect(hwnd, NULL, TRUE);
    case WM_KEYDOWN:
        //printf("key down ran\n");

        if (wParam == 0) {
            //printf("Received key event with wParam == 0, which is unexpected.\n");
            //Sleep(100);
            break;
        } else {
            //printf("Key pressed with wParam = %lu\n", wParam);
        }

        //printf("wParam = %d\n", wParam);

        if(((((wParam<58&&wParam>48)||(wParam<105&&wParam>97))||wParam==VK_RETURN)||wParam==48)&&GetingBet){//cheking if one of the button keys is pressing
            if (wParam==49||wParam==97){//one key actions
                currentBet+=5;
            }else if (wParam==50||wParam==98){//two key actions
                currentBet+=10;
            }else if (wParam==51||wParam==99){//three key actions
                currentBet+=25;
            }else if (wParam==52||wParam==100){//four key actions
                currentBet+=50;
            }else if (wParam==53||wParam==101){//five key actions
                currentBet+=100;
            }else if (wParam==54||wParam==102){//six key actions
                currentBet+=500;
            }else if (wParam==55||wParam==103){//7
                currentBet+=1000;
            }else if (wParam==56||wParam==104){//8
                currentBet+=5000;
            }else if (wParam==57||wParam==105){//9
                currentBet=GetChips();
            }else if (wParam==48){//if the zero key is pressing reset the bet
                currentBet=0;
            }
            if (currentBet>GetChips()){
                currentBet=GetChips();
            }
            if (wParam==VK_RETURN){
                if (currentBet<=0){
                    currentBet=1;
                }
                GetingBet=0;
                //reshuffles if we are runing out of cards
                printf("num cards at deal:%d%s%d\n",getNumCards(Deck,numDecks),"\nroot devision by num decks:",getNumCards(Deck,numDecks)-(52*(numDecks-1)));
                if (getNumCards(Deck,numDecks)<10){
                    Deck=makeDeck(numDecks);
                }
                DealHand(playerHand,Deck,numDecks);
                DealHand(DealerHand,Deck,numDecks);
                // playerHand[0]=0;
                // playerHand[1]=12;
                if (getTotal(playerHand)==21){
                    BlackJack=1;
                }else{
                    playerTurn=1;
                    Delt=1;
                }
                Sleep(150);
            }
            InvalidateRect(hwnd, NULL, TRUE);
            // playerHand[0]=33;
            // playerHand[1]=36;
            // playerHand[2]=1;
            // DealerHand[0]=0;
            // DealerHand[1]=12;
        }

        if(Delt){
            if ((wParam==49||wParam==97)&&playerTurn){
                GiveHandCard(Deck,playerHand,numDecks);
                InvalidateRect(hwnd, NULL, TRUE);
                if(getTotal(playerHand)>21){
                    playerTurn=0;
                    DealerTurn=1;
                }

            }else if(wParam==50||wParam==98){
                playerTurn=0;
                DealerTurn=1;       
                InvalidateRect(hwnd, NULL, TRUE);
         
            }
        }

       
        break;

    case WM_DESTROY:
        for (int i = 0; i < imageCount; i++) {
            DeleteObject(images[i]);
        }
        printf("Received message: uMsg = %u, wParam = %lu, lParam = %ld\n", uMsg, wParam, lParam);

        printf("closing %s\n","window");
        PostQuitMessage(0);
        break;

    default:
        //printf("default ran\n");

        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "SampleWindowClass";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);


    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "BlackJack Project",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        MessageBox(NULL, "Window creation failed!", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }    

    printf("validrun:%d\n",ValidRun());
    if (!ValidRun()){
        MessageBox(NULL, "this file is older then one month \nplease get new exe from will at \nwilliam.horning4@gmail.com", "Invalid file run", MB_OK | MB_ICONERROR);
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}