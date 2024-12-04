#include <iostream>
#include <graphics.h>
#include <cstring>
#include <fstream>
#include <cstdio> //pentru Remove
using namespace std;

char GameBoard[72][9];
int length = 9, width = 7;

struct GamePieces
{
    POINT UpLeft, DownRight,InitialPositionOfFence; // coordonatele piesei
    int x, y;
    bool dragging; // daca e in  starea de drag
    bool isPlaced;
    int Side;
    int GB[7][9]; // matricea caracteristica a piesei 7x9

} Fence[4];

bool IsKeyPressed(char key) {
    if (kbhit()) { 
        char pressed = getch(); 
        if (pressed == key) {
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
void RotateFence(GamePieces &fence)
{
    NormalizeFence(fence);
    GamePieces newFence=fence;
    for(int i=0;i<width;i++)
    {
        for(int q=0;q<width;q++)
        {
            newFence.GB[i][q]=fence.GB[q][i];
        }
    }
     for(int i=0;i<width;i++)
    {
        for(int q=width;q<length;q++)
        {
            newFence.GB[i][q]=0;
        }
    }
    fence=newFence;
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

// Partea de Grafica

int LeftBorder, UpBorder, gbWidth, gbHeight, gbSideLength, DownBorder, RightBorder;
void DrawInBoardGame(int x, int y, int color)
{
    // circle(LeftBorder+ gbSideLength * (x - 1) + gbSideLength/ 2, UpBorder + gbSideLength * (y - 1) +gbSideLength / 2, 10);
    setfillstyle(SOLID_FILL, color);
    floodfill(LeftBorder + gbSideLength * (x - 1) + gbSideLength / 2, UpBorder + gbSideLength * (y - 1) + gbSideLength / 2, 15);
}
void DrawBoardGame()
{

    gbSideLength = 80;
    gbWidth = gbSideLength * length;
    gbHeight = 400;
    UpBorder = 50;
    LeftBorder = 50;
    RightBorder = 50;
    for (int i = 1; i <= length; i++)
        for (int j = 1; j <= width; j++)
            rectangle(LeftBorder + gbSideLength * (i - 1), UpBorder + gbSideLength * (j - 1), LeftBorder + gbSideLength * i, UpBorder + gbSideLength * j);
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            if (GameBoard[i][q] == '#')
                DrawInBoardGame(q + 1, i + 1, 1);
            else
                DrawInBoardGame(q + 1, i + 1, GameBoard[i][q] - '0');
}

void drawFence(GamePieces &fenceToDraw, int x, int y, int side)
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

                setcolor(GREEN);
                rectangle(x, y, x + side, y + side);
                setcolor(WHITE);

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
    bar(0, 0, getmaxx(), getmaxy());

    if (ismouseclick(WM_LBUTTONDOWN))
    {
        POINT mouse;
        clearmouseclick(WM_LBUTTONDOWN);
        GetCursorPos(&mouse);
        if (mouse.x >= Fence.UpLeft.x && mouse.x <= Fence.DownRight.x && mouse.y >= Fence.UpLeft.y && mouse.y <= Fence.DownRight.y)
        {
            Fence.dragging = 1;
         //   if(Fence.Side*2<=gbSideLength)
                // Fence.Side*=2;
        }

        // ofstream fout("GameBoards/coordonate.txt", ios_base::app);
        // fout << GB.x << " " << GB.y << endl;
    }
    
    if (Fence.dragging)
    {
        POINT mouse;
        if(IsKeyPressed('r'))
           {
                RotateFence(Fence);
           }
        if (Fence.isPlaced)
            RemoveFence(Fence), Fence.isPlaced = false;
        GetCursorPos(&mouse);
        Fence.UpLeft.x = mouse.x - 40;
        Fence.UpLeft.y = mouse.y - 40;
        if (ismouseclick(WM_RBUTTONDOWN))
        {
            Fence.dragging = 0;
            //Fence.Side/=2;
            clearmouseclick(WM_RBUTTONDOWN);
            GB.y = (mouse.x - LeftBorder) / gbSideLength;
            GB.x = (mouse.y - UpBorder) / gbSideLength;
            if (GB.x <= length && GB.y <= width)
            {
                NormalizeFence(Fence);
                GamePieces newFence=Fence;
                MoveFence(newFence, GB.x, GB.y);
                if (VerifyFencePosition(newFence))
                {
                    MoveFence(Fence, GB.x, GB.y);
                    AddFence(Fence);

                    Fence.UpLeft.x = GB.y * gbSideLength + LeftBorder;
                    Fence.UpLeft.y = GB.x * gbSideLength + UpBorder;
                    Fence.isPlaced = true;
                }
                else
                {
                    Fence.UpLeft = Fence.InitialPositionOfFence;
                    MouseDraggingPiece(Fence);
                }
            }
            else
            {
            }
        }
    }
    DrawBoardGame();

    for (int i = 1; i <= 3; i++)
        if (!::Fence[i].dragging)
            drawFence(::Fence[i], ::Fence[i].UpLeft.x, ::Fence[i].UpLeft.y, ::Fence[i].Side);
    drawFence(Fence, Fence.UpLeft.x, Fence.UpLeft.y, Fence.Side);

    page = 1 - page;
    delay(10);
    return GB;
}

int main()
{

    ReadGameBoard("GameBoards/GameBoard.txt");
    ReadFence(Fence[3], "GameBoards/Piesa3.txt");
    ReadFence(Fence[1], "GameBoards/Piesa1.txt");
    ReadFence(Fence[2], "GameBoards/Piesa2.txt");
    
    initwindow(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),"",-3,-3);
    DrawBoardGame();

    Fence[0].UpLeft.x=Fence[0].UpLeft.y=0;
    Fence[0].DownRight.x=getmaxx();Fence[0].DownRight.y=getmaxy();
    Fence[1].InitialPositionOfFence.x=LeftBorder + gbWidth + LeftBorder;
    Fence[1].InitialPositionOfFence.y=UpBorder;
    Fence[2].InitialPositionOfFence.x=LeftBorder + gbWidth + LeftBorder;
    Fence[2].InitialPositionOfFence.y=UpBorder + gbSideLength * 2;
    Fence[3].InitialPositionOfFence.x=LeftBorder + gbWidth + LeftBorder;
    Fence[3].InitialPositionOfFence.y=UpBorder + gbSideLength * 5;

    for(int i=1;i<=3;i++)
    {
         Fence[i].Side=gbSideLength;
        drawFence(Fence[i], Fence[i].InitialPositionOfFence.x, Fence[i].InitialPositionOfFence.y, Fence[i].Side);
       
    }


    while (true)
    {
        if(IsKeyPressed('q'))
        {
            return 0;
        }
        bool SomethingHappend=false;
        POINT mouse;
        GetCursorPos(&mouse);
        for(int i=1;i<=3;i++)
            if (mouse.x >= Fence[i].UpLeft.x && mouse.x <= Fence[i].DownRight.x && mouse.y >= Fence[i].UpLeft.y && mouse.y <= Fence[i].DownRight.y || Fence[i].dragging)
            {
                MouseDraggingPiece(Fence[i]);
                SomethingHappend=true;
            }
        if(!SomethingHappend)
            MouseDraggingPiece(Fence[0]);
        
    }

    getch();
    closegraph();

    return 0;
}