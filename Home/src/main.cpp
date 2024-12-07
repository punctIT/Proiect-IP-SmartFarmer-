#include <iostream>
#include <graphics.h>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <algorithm>

using namespace std;

char GameBoard[10][10];
int length = 9, width = 7;

bool globalDragging=false;

void *BackgroundBuffer;
void *HorseBuffer;
void *CowBuffer;
void *SheepBuffer;
void *PigBuffer;
void *FenceBuffer;
void *GrassBuffer;
void *EmptyAnimalBuffer;
void *MiniFenceBuffer;
struct GamePieces
{
    POINT UpLeft, DownRight, InitialPositionOfFence; // coordonatele piesei
    int x, y;
    bool dragging; // daca e in  starea de drag
    bool isPlaced;
    int Side;
    int isRotated;
    int GB[7][9]; // matricea caracteristica a piesei 7x9


} Fence[4];

bool IsKeyPressed(char key)
{
    if (kbhit())
    {
        char pressed = getch();
        if (pressed == key)
        {
            return true;
        }
    }
    return false;
}

void ReadFence(GamePieces &fence, string FileName)
{
    ifstream fin(FileName);
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            fin >> fence.GB[i][q];
}

bool VerifyFencePosition(GamePieces fence)
{
    for (int i = 0; i <= width; i++)
        for (int q = 0; q <= length; q++)
            if (fence.GB[i][q] && strchr("01234", GameBoard[i][q]) == NULL)
                return false;
    return true;
}

void AddFence(GamePieces fence)
{
    if (!VerifyFencePosition(fence))
    {
        cout << "erroare";
        return;
    }
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            GameBoard[i][q] += fence.GB[i][q];
}
void RemoveFence(GamePieces fence)
{
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            if (fence.GB[i][q])
                if (GameBoard[i][q] != '0')
                    GameBoard[i][q]--;
                else
                    GameBoard[i][q] = '0';
}
void NormalizeFence(GamePieces &fence) // pe scurt , aduce gardul in colt sus, elementele de 1 cat mai aproape de (0,0) in matrice , util la afisare
{
    int ZeroLineNumber = 0, ZeroColumnNumber = 0;
    for (int i = 0; i < width; i++) // numara nr de lini vide
    {
        bool IsFullOfZero = true;
        for (int q = 0; q < length; q++)
            if (fence.GB[i][q])
            {
                IsFullOfZero = false;
                break;
            }
        if (IsFullOfZero)
            ZeroLineNumber++;
        else
            break;
    }
    for (int i = 0; i < length; i++) // coloane vide
    {
        bool IsFullOfZero = true;
        for (int q = 0; q < width; q++)
            if (fence.GB[q][i])
            {
                IsFullOfZero = false;
                break;
            }
        if (IsFullOfZero)
            ZeroColumnNumber++;
        else
            break;
    }
    for (int i = 0; i < width; i++) // elimina linitile vide/coloanele
        for (int q = 0; q < length; q++)
            fence.GB[i][q] = fence.GB[i + ZeroLineNumber][q + ZeroColumnNumber];
    for (int i = width - ZeroLineNumber; i < width; i++) // le adauga la final
        for (int q = 0; q < length; q++)
            fence.GB[i][q] = 0;
    for (int i = length - ZeroColumnNumber; i < length; i++) // le adauga la final
        for (int q = 0; q < width; q++)
            fence.GB[q][i] = 0;
}
void MoveFence(GamePieces &fence, int x, int y) // x coloana , y linia(indicele , cu cat sa se mute)
{
    NormalizeFence(fence);
    for (int e = 1; e <= y; e++)
        for (int i = 0; i < width; i++)
        {
            for (int q = length - 1; q > 0; q--)
                fence.GB[i][q] = fence.GB[i][q - 1];
            fence.GB[i][0] = 0;
        }
    for (int e = 1; e <= x; e++)
        for (int i = 0; i < length; i++)
        {
            for (int q = width - 1; q > 0; q--)
                fence.GB[q][i] = fence.GB[q - 1][i];
            fence.GB[0][i] = 0;
        }
}
void CopyMatrix(int a[7][9], int b[7][9])
{
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            a[i][q] = b[i][q];
}
void RotateFence(GamePieces &fence)
{
    NormalizeFence(fence);

    int newFence[7][9];
    CopyMatrix(newFence, fence.GB);
    for (int i = 0; i < width; i++)
        for (int q = 0; q < width + 1; q++)
            newFence[width - q - 1][i] = fence.GB[i][q];
    for (int i = 0; i < width; i++)
        for (int q = width; q < length; q++)
            newFence[i][q] = 0;
    CopyMatrix(fence.GB, newFence);
}
void DrawGameBoardMatrix()
{
    for (int i = 0; i < width; i++)
    {
        for (int q = 0; q < length; q++)
            cout << GameBoard[i][q] << " ";
        cout << endl;
    }
}
void ReadGameBoard(string FileName)
{
    ifstream fin(FileName);
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            fin >> GameBoard[i][q];
}

int CountAnimalInBoard(char Animal)
{
    int cnt = 0;
    for (int i = 1; i <= width; i++)
        for (int j = 1; j <= length; j++)
            if (GameBoard[i][j] == Animal)
                cnt++;
    return cnt;
}

bool AnimalsAreFenced()
{
    char CopyGameBoard[7][9];
    for (int i = 0; i <= width; i++)
        for (int j = 0; j <= length; j++)
            CopyGameBoard[i][j] = GameBoard[i][j];
    for (int i = 1; i < width; i++)
    {
        for (int j = 1; j < length; j++)
        {
            if (strchr("*0CPHS", CopyGameBoard[i][j])) // C pentru cow, P pentru pig, etc
            {
                char Animal = CopyGameBoard[i][j];
                int CountAnimal = 1;
                // bool Water = 0;

                // algoritm fill
                int iDir[4] = {0, 0, -1, 1}, jDir[4] = {-1, 1, 0, 0};
                int left = 0, right = 0;
                struct Queue
                {
                    int row, column;
                } Q[length * width];
                Q[0].row = i;
                Q[0].column = j;
                CopyGameBoard[i][j] = '1';
                while (left <= right)
                {
                    int row = Q[left].row, column = Q[left].column;
                    for (int k = 0; k < 4; k++)
                    {
                        int iNext = row + iDir[k];
                        int jNext = column + jDir[k];
                        if (strchr("*0CPHS", CopyGameBoard[iNext][jNext]))
                        {
                            if (CopyGameBoard[iNext][jNext] != '0' && CopyGameBoard[iNext][jNext] != '*')
                            {
                                if (Animal == '0' || Animal == '*')
                                    Animal = CopyGameBoard[iNext][jNext];
                                else if (CopyGameBoard[iNext][jNext] == Animal)
                                    CountAnimal++;
                                else
                                    return 0; // daca sunt mai multe animale de tipuri diferite atunci nu e pus bine gardul
                            }
                            CopyGameBoard[iNext][jNext] = '1';
                            right++;
                            Q[right].row = iNext;
                            Q[right].column = jNext;
                        }
                    }
                    left++;
                }
                if (CountAnimal != CountAnimalInBoard(Animal))
                    return 0; // nu sunt toate animalele de tipul asta in acelasi tarc
            }
        }
    }
    return 1;
}

// Partea de Grafica

int LeftBorder, UpBorder, gbWidth, gbHeight, gbSideLength = 80, DownBorder, RightBorder;
void Button1()
{
    cout << 1;
}
void DrawButton(int x, int y, void FunctieButon())
{

    FunctieButon();
}

void DrawInBoardGame(int x, int y, int color)
{
    // circle(LeftBorder+ gbSideLength * (x - 1) + gbSideLength/ 2, UpBorder + gbSideLength * (y - 1) +gbSideLength / 2, 10);
    setfillstyle(SOLID_FILL, color);
    floodfill(LeftBorder + gbSideLength * (x - 1) + gbSideLength / 2, UpBorder + gbSideLength * (y - 1) + gbSideLength / 2, 15);
}

void DrawBoardGame()
{

    gbWidth = gbSideLength * length;
    gbHeight = 400;
    UpBorder = 0.17 * GetSystemMetrics(SM_CYSCREEN);
    LeftBorder = 0.1 * GetSystemMetrics(SM_CXSCREEN);
    RightBorder = 50;
    for (int i = 1; i <= length; i++)
        for (int j = 1; j <= width; j++)
            rectangle(LeftBorder + gbSideLength * (i - 1), UpBorder + gbSideLength * (j - 1), LeftBorder + gbSideLength * i, UpBorder + gbSideLength * j);
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
        {
            int x=LeftBorder+q*gbSideLength;
            int y=UpBorder+i*gbSideLength;
           
            if (GameBoard[i][q] == '#' || strchr("12345", GameBoard[i][q]))
            {
                //DrawInBoardGame(q + 1, i + 1, 1);
                putimage(x,y,FenceBuffer,COPY_PUT);
            }
            else if (GameBoard[i][q] == 'C') // CPHS
            {
                putimage(x,y,CowBuffer,COPY_PUT);
            }
            else if (GameBoard[i][q] == 'P')
            {
                putimage(x,y,PigBuffer,COPY_PUT);
            }
            else if (GameBoard[i][q] == 'H')
            {   
                putimage(x,y,HorseBuffer,COPY_PUT);
            }
            else if (GameBoard[i][q] == 'S')
            {
               putimage(x,y,SheepBuffer,COPY_PUT);
            }
            else if(GameBoard[i][q]=='*')
            {
                putimage(x,y,EmptyAnimalBuffer,COPY_PUT);
            }
            else
                putimage(x,y,GrassBuffer,COPY_PUT);
             
        }
}

void DrawFence(GamePieces &fenceToDraw, int x, int y, int side)
{
    int ZeroLineNumber = 0, ZeroColumnNumber = 0;
    GamePieces fence = fenceToDraw;
    NormalizeFence(fence);
    //  ofstream fout("GameBoards/coordonate.txt", ios_base::app);
    for (int i = 0; i < width; i++) // partea de desenare
        for (int q = 0; q < length; q++)
            if (fence.GB[i][q])
            {
                x += side * (q);
                y += side * (i);

                setcolor(WHITE);
                rectangle(x, y, x + side, y + side);
                if(side!=gbSideLength)
                    putimage(x,y,MiniFenceBuffer,COPY_PUT);
                else putimage(x,y,FenceBuffer,COPY_PUT);
        

                y -= side * (i);
                x -= side * (q);
            }
    // salvarea noii pozitii a piesei;
    fenceToDraw.UpLeft.x = x;
    fenceToDraw.UpLeft.y = y;
    int LastX, LastY;
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            if (fence.GB[i][q])
                LastX = i, LastY = q;
    fenceToDraw.DownRight.x = fenceToDraw.UpLeft.x + side * LastY + side;
    fenceToDraw.DownRight.y = fenceToDraw.UpLeft.y + side * LastX + side;
    /*
    ofstream fout("GameBoards/coordonate.txt", ios_base::app);
    fout<<f.UpLeft.x<<" "<<f.UpLeft.y<<endl;
    fout<<f.DownRight.x<<" "<<f.DownRight.y<<endl;
    fout<<endl;
    */
}

int page = 0;

POINT MouseDraggingPiece(GamePieces &Fence)
{
    POINT GB;
    setactivepage(page);
    setvisualpage(1 - page);
    setfillstyle(SOLID_FILL, BLACK);
    
     //bar(0, 0, getmaxx(), getmaxy());
    bar(0,0,getmaxx(),UpBorder);
    if (ismouseclick(WM_LBUTTONDOWN))
    {
        POINT mouse;
        clearmouseclick(WM_LBUTTONDOWN);
        GetCursorPos(&mouse);
        GB.y = (mouse.x - LeftBorder) / gbSideLength;
        GB.x = (mouse.y - UpBorder) / gbSideLength; 
        cout<<GB.x<<" "<<GB.y<<" "<<endl;
        if (mouse.x >= Fence.UpLeft.x && mouse.x <= Fence.DownRight.x && mouse.y >= Fence.UpLeft.y && mouse.y <= Fence.DownRight.y) // daca se afla in interiorul piesei
        {
            Fence.dragging = 1;
            globalDragging=1;//se foloseste pentru a elimina efectul de palpaiala a piesei
            Fence.Side = gbSideLength; // devine de dimenisune normala
        }
    }

    if (Fence.dragging)
    {
        POINT mouse;
        if (IsKeyPressed('r'))
        {
            RotateFence(Fence);
            Fence.isRotated = (Fence.isRotated + 1) % 2; // alterneaza intre 0 si 1 ( un bool pentru lenesi)
        }
        if (Fence.isPlaced) // aici verifica daca piesa e plasata cumva , si se muta
            RemoveFence(Fence), Fence.isPlaced = false;
        GetCursorPos(&mouse);
        Fence.UpLeft.x = mouse.x - 40; // mutarea efectiva dupa mouse
        Fence.UpLeft.y = mouse.y - 40;
        if (ismouseclick(WM_RBUTTONDOWN)) // plasarea piesiei jos , cu click dreapta
        {
            Fence.dragging = 0;
            globalDragging=0;
            Fence.Side = gbSideLength / 2;

            clearmouseclick(WM_RBUTTONDOWN);
            GB.y = (mouse.x - LeftBorder) / gbSideLength;
            GB.x = (mouse.y - UpBorder) / gbSideLength; // afla coordonatele pe grid
            if (mouse.x<LeftBorder+gbWidth&&mouse.y<UpBorder+gbHeight&&mouse.x>LeftBorder&&mouse.y>UpBorder)        // verifica daca se afla in matricea GameBoard
            {
                NormalizeFence(Fence); // normalizarea e pentru mutarea piesei, trebuie adusa in colt , iar dupa mutata  la x si y
                GamePieces newFence = Fence;
                MoveFence(newFence, GB.x, GB.y); // mutarea in grid, mutarea fictiva, matrice auxiara
                if (VerifyFencePosition(newFence))
                {
                    MoveFence(Fence, GB.x, GB.y);
                    AddFence(Fence);
                    Fence.UpLeft.x = GB.y * gbSideLength + LeftBorder; // calculatre coordonate noi
                    Fence.UpLeft.y = GB.x * gbSideLength + UpBorder;
                    Fence.isPlaced = true;
                    Fence.Side = gbSideLength; // aducerea la marimea normala
                }
                else
                {
                    Fence.UpLeft = Fence.InitialPositionOfFence;
                    Fence.Side = gbSideLength / 2;
                    if (Fence.isRotated)
                    {
                        Fence.isRotated = 0;
                        RotateFence(Fence);
                    }
                    MouseDraggingPiece(Fence);
                }
            }
            else
            {
                Fence.UpLeft = Fence.InitialPositionOfFence;
                Fence.Side = gbSideLength / 2;
                if (Fence.isRotated)
                {
                    Fence.isRotated = 0;
                    RotateFence(Fence);
                }
                MouseDraggingPiece(Fence);
            }
        }
    }
    putimage(0,0,BackgroundBuffer,COPY_PUT);
    DrawBoardGame();

    for (int i = 1; i <= 3; i++)
        if (!::Fence[i].dragging)
            DrawFence(::Fence[i], ::Fence[i].UpLeft.x, ::Fence[i].UpLeft.y, ::Fence[i].Side); // cele care nu sunt in dragging trebuie afisate
    DrawFence(Fence, Fence.UpLeft.x, Fence.UpLeft.y, Fence.Side);                             // se evita efectul de palpaiala , libraria GRAPHICS.H e naspa.

    page = 1 - page;
    delay(10);
    return GB;
}
void DrawLevel(string GameBoardFileName)
{
    ReadGameBoard(GameBoardFileName);
    ReadFence(Fence[3], "GameBoards/Piesa3.txt");
    ReadFence(Fence[1], "GameBoards/Piesa1.txt");
    ReadFence(Fence[2], "GameBoards/Piesa2.txt");

    DrawBoardGame();
    Fence[0].UpLeft.x = Fence[0].UpLeft.y = 0;
    Fence[0].DownRight.x = getmaxx();
    Fence[0].DownRight.y = getmaxy();
    Fence[1].InitialPositionOfFence.x = LeftBorder + gbWidth + LeftBorder;
    Fence[1].InitialPositionOfFence.y = UpBorder;
    Fence[2].InitialPositionOfFence.x = LeftBorder + gbWidth + LeftBorder;
    Fence[2].InitialPositionOfFence.y = UpBorder + gbSideLength * 2;
    Fence[3].InitialPositionOfFence.x = LeftBorder + gbWidth + LeftBorder;
    Fence[3].InitialPositionOfFence.y = UpBorder + gbSideLength * 5;

    for (int i = 1; i <= 3; i++)
    {
        Fence[i].Side = gbSideLength * (0.5);
        DrawFence(Fence[i], Fence[i].InitialPositionOfFence.x, Fence[i].InitialPositionOfFence.y, Fence[i].Side);
    }

    while (true)
    {

        bool SomethingHappend = false;
        POINT mouse;
        GetCursorPos(&mouse);
        for (int i = 1; i <= 3; i++)
            if (mouse.x >= Fence[i].UpLeft.x && mouse.x <= Fence[i].DownRight.x && mouse.y >= Fence[i].UpLeft.y && mouse.y <= Fence[i].DownRight.y || Fence[i].dragging)
            {
                if(Fence[i].dragging||!globalDragging)//doar daca nu e nici o pisa in starea de dragging , sau doar una
                    MouseDraggingPiece(Fence[i]);
                SomethingHappend = true;
            }
        if (!SomethingHappend)
            MouseDraggingPiece(Fence[0]);
        if (AnimalsAreFenced())
        {
            cleardevice();
            setcolor(WHITE); // Setează culoarea textului
            char text[] = "FINAL";
            outtextxy(200, 200, text);
        }
    }
}
void UploadImages()
{
   // setvisualpage(1);//evita efectul de incarcare a imagililor, nu apare fiecare pe rand , apare negru si dupa toate odata
    readimagefile("Images/background.jpg",0,0,getmaxx(),getmaxy());
    BackgroundBuffer=malloc(imagesize(0,0,getmaxx(),getmaxy()));
    getimage(0,0,getmaxx(),getmaxy(),BackgroundBuffer);
    readimagefile("Images/horse.jpg",1,1,gbSideLength-1,gbSideLength-1);
    HorseBuffer=malloc(imagesize(1,1,gbSideLength-1,gbSideLength-1));
    getimage(1,1,gbSideLength-1,gbSideLength-1, HorseBuffer);
    readimagefile("Images/cow.jpg",1,1,gbSideLength-1,gbSideLength-1);
    CowBuffer=malloc(imagesize(1,1,gbSideLength-1,gbSideLength-1));
    getimage(1,1,gbSideLength-1,gbSideLength-1, CowBuffer);
    readimagefile("Images/sheep.jpg",1,1,gbSideLength-1,gbSideLength-1);
    SheepBuffer=malloc(imagesize(1,1,gbSideLength-1,gbSideLength-1));
    getimage(1,1,gbSideLength-1,gbSideLength-1, SheepBuffer);
    readimagefile("Images/pig.jpg",1,1,gbSideLength-1,gbSideLength-1);
    PigBuffer=malloc(imagesize(1,1,gbSideLength-1,gbSideLength-1));
    getimage(1,1,gbSideLength-1,gbSideLength-1, PigBuffer);
    readimagefile("Images/pig.jpg",1,1,gbSideLength-1,gbSideLength-1);
    PigBuffer=malloc(imagesize(1,1,gbSideLength-1,gbSideLength-1));
    getimage(1,1,gbSideLength-1,gbSideLength-1, PigBuffer);
    readimagefile("Images/grass.jpg",1,1,gbSideLength-1,gbSideLength-1);
    GrassBuffer=malloc(imagesize(1,1,gbSideLength-1,gbSideLength-1));
    getimage(1,1,gbSideLength-1,gbSideLength-1,  GrassBuffer);
    readimagefile("Images/fence.jpg",1,1,gbSideLength-1,gbSideLength-1);
    FenceBuffer=malloc(imagesize(1,1,gbSideLength-1,gbSideLength-1));
    getimage(1,1,gbSideLength-1,gbSideLength-1,  FenceBuffer);
    readimagefile("Images/Rock.jpg",1,1,gbSideLength-1,gbSideLength-1);
    EmptyAnimalBuffer=malloc(imagesize(1,1,gbSideLength-1,gbSideLength-1));
    getimage(1,1,gbSideLength-1,gbSideLength-1,  EmptyAnimalBuffer);
    readimagefile("Images/fence.jpg",1,1,(gbSideLength-1)/2,(gbSideLength-1)/2);
    MiniFenceBuffer=malloc(imagesize(1,1,(gbSideLength-1)/2,(gbSideLength-1)/2));
    getimage(1,1,(gbSideLength-1)/2,(gbSideLength-1)/2,  MiniFenceBuffer);
    cleardevice();
   
}
int main()
{

    initwindow(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), "", -3, -3);
    UploadImages();
    DrawLevel("GameBoards/GameBoard1.txt");
   
    
    getch();
    closegraph();

    return 0;
}