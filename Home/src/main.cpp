#include <iostream>
#include <winbgim.h>
#include <graphics.h>
#include <cstring>
#include <fstream>
#include <cstdio> //pentru Remove
using namespace std;

char GameBoard[7][9];
int length = 9, width = 7;

struct GamePieces
{
    int x, y;
    bool dragging;           // daca e in  starea de drag
    POINT UpLeft, DownRight; // coordonatele piesei
    int GB[7][9];            // matricea caracteristica a piesei 7x9

} Fence[4];

void readFence(GamePieces &fence, string FileName)
{
    ifstream fin(FileName);
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            fin >> fence.GB[i][q];
}

bool VerifyFencePosition(GamePieces fence, int x, int y)
{
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            if (fence.GB[i][q] && strchr("01234", GameBoard[i][q]) == NULL)
                return false;
    return true;
}

void addFance(GamePieces fence)
{
    if (!VerifyFencePosition(fence, 0, 0))
    {
        cout << "erroare";
        return;
    }
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            GameBoard[i][q] += fence.GB[i][q];
}
void removeFance(GamePieces fence)
{
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            GameBoard[i][q] -= fence.GB[i][q];
}
void Normalizare(GamePieces& fence)
{
    int ZeroLineNumber = 0, ZeroColumnNumber = 0;
    for (int i = 0; i < width; i++) // numara nr de lini vide
    {
        bool ok = true;
        for (int q = 0; q < length; q++)
            if (fence.GB[i][q])
            {
                ok = false;
                break;
            }
        if (ok)
            ZeroLineNumber++;
        else
            break;
    }
    for (int i = 0; i < length; i++) // coloane vide
    {
        bool ok = true;
        for (int q = 0; q < width; q++)
            if (fence.GB[q][i])
            {
                ok = false;
                break;
            }
        if (ok)
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
void moveFence(GamePieces &fence, int x, int y) // x coloana , y linia(indicele , cu cat sa se mute)
{
    Normalizare(fence);
    for(int e=1;e<=y;e++)
        for(int i=0;i<width;i++){
            for(int q=length-1;q>0;q--)
                fence.GB[i][q]=fence.GB[i][q-1];
            fence.GB[i][0]=0;
        }
    for(int e=1;e<=x;e++)
        for(int i=0;i<length;i++){
            for(int q=width-1;q>0;q--)
                fence.GB[q][i]=fence.GB[q-1][i];
            fence.GB[0][i]=0;
        }
    
    
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
void readGameBoard(string FileName)
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

void drawFence(GamePieces &f, int x, int y, int side)
{
    int ZeroLineNumber = 0, ZeroColumnNumber = 0;
    GamePieces fence = f;
    Normalizare(fence);
    /*
    ofstream fout("GameBoards/coordonate.txt", ios_base::app);
    for(int i=0;i<width;i++)
    {
        for(int q=0;q<length;q++)
            fout<<fence.GB[i][q]<< " ";
        fout<<endl;
    }
    */
    for (int i = 0; i < width; i++) // partea de desenare
    {
        //  ofstream fout("GameBoards/coordonate.txt", ios_base::app);
        for (int q = 0; q < length; q++)
        {
            //  fout<<fence.GB[i][q]<<" ";
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
        }
    }
    // salvarea noii pozitii a piesei;
    f.UpLeft.x = x;
    f.UpLeft.y = y;
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            if (fence.GB[i][q])
            {
                f.DownRight.x = f.UpLeft.x + side * q + side;
                f.DownRight.y = f.UpLeft.y + side * i + side;
            }
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

        clearmouseclick(WM_LBUTTONDOWN);
        POINT mouse;
        GetCursorPos(&mouse);
        if (mouse.x >= Fence.UpLeft.x && mouse.x <= Fence.DownRight.x && mouse.y >= Fence.UpLeft.y && mouse.y <= Fence.DownRight.y)
            Fence.dragging = 1;
        GB.y = (mouse.x - LeftBorder) / gbSideLength;
        GB.x = (mouse.y - UpBorder) / gbSideLength;
        // ofstream fout("GameBoards/coordonate.txt", ios_base::app);
        // fout << GB.x << " " << GB.y << endl;
    }
    if (Fence.dragging)
    {
        POINT mouse;
        GetCursorPos(&mouse);
        Fence.UpLeft.x = mouse.x - 40;
        Fence.UpLeft.y = mouse.y - 40;
        if (ismouseclick(WM_RBUTTONDOWN))
        {
            Fence.dragging = 0;
            clearmouseclick(WM_RBUTTONDOWN);
            GB.y = (mouse.x - LeftBorder) / gbSideLength;
            GB.x = (mouse.y - UpBorder) / gbSideLength;
            if(GB.x<=length&&GB.y<=width)
            {
                Normalizare(Fence);
                moveFence(Fence,GB.x,GB.y);
                if(VerifyFencePosition(Fence,GB.x,GB.y))
                {
                    addFance(Fence);
                }
            }
           
            
            ofstream fout("GameBoards/coordonate.txt", ios_base::app);
            fout << GB.x << " " << GB.y << endl;
        }
    }
    DrawBoardGame();
    for(int i=1;i<=3;i++)
       if(!::Fence[i].dragging)
        drawFence(::Fence[i],::Fence[i].UpLeft.x, ::Fence[i].UpLeft.y, gbSideLength);
       

    drawFence(Fence, Fence.UpLeft.x, Fence.UpLeft.y, 80);

    page = 1 - page;
    delay(10);
    return GB;
}

int main()
{

    readGameBoard("GameBoards/GameBoard.txt");
    readFence(Fence[3], "GameBoards/Piesa3.txt");
    readFence (Fence[1], "GameBoards/Piesa1.txt");
   // addFance(LFence);
   // addFance(FiveFence);
    moveFence(Fence[1], 1, 1);

    initwindow(1400, 700);
    DrawBoardGame();

    drawFence( Fence[1], LeftBorder + gbWidth + LeftBorder, UpBorder, gbSideLength);
    drawFence( Fence[3], LeftBorder + gbWidth + LeftBorder, UpBorder + gbSideLength * 3, gbSideLength);

    while (true)
    {
        POINT mouse;
        GetCursorPos(&mouse);
        if (mouse.x >= Fence[1].UpLeft.x && mouse.x <= Fence[1].DownRight.x && mouse.y >= Fence[1].UpLeft.y && mouse.y <= Fence[1].DownRight.y || Fence[1].dragging)
        {
            MouseDraggingPiece(Fence[1]);
        }
        if (mouse.x >= Fence[3].UpLeft.x && mouse.x <= Fence[3].DownRight.x && mouse.y >=Fence[3].UpLeft.y && mouse.y <=Fence[3].DownRight.y || Fence[3].dragging)
        {
            MouseDraggingPiece(Fence[3]);
        }
    }

    getch();
    closegraph();

    return 0;
}