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
    POINT UpLeft, DownRight;
    int GB[7][9];

} Fence;

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
         cout<<"erroare";
        return;
    }
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            GameBoard[i][q] += fence.GB[i][q];
}
void moveFence(GamePieces &fence, int x, int y) // x coloana , y linia(indicele , cu cat sa se mute)
{
    if (!VerifyFencePosition(fence, x, y))
    {
        cout << "erroare ceva "<<x<<" "<<y << endl;
        return;
    }
    GamePieces auxMatrix = {0};
    for (int i = 0; i < width; i++)
    {
        for (int q = 0; q < length; q++)
            if (fence.GB[i][q])
            {
                GameBoard[i][q]--;
                // GameBoard[i+y][q+x]++;
                fence.GB[i][q] = 0;
                auxMatrix.GB[i + y][q + x] = 1;
            }
    }
    fence = auxMatrix;
    addFance(fence);
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
    for (int i = 0; i < width; i++)
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
    for (int i = 0; i < length; i++)
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
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            fence.GB[i][q] = fence.GB[i + ZeroLineNumber][q + ZeroColumnNumber];
    for (int i = width - ZeroLineNumber; i < width; i++)
        for (int q = 0; q < length; q++)
            fence.GB[i][q] = 0;
    for (int i = length - ZeroColumnNumber; i < length; i++)
        for (int q = 0; q < width; q++)
            fence.GB[q][i] = 0;
    /*
    ofstream fout("GameBoards/coordonate.txt", ios_base::app);
    for(int i=0;i<width;i++)
    {
        for(int q=0;q<length;q++)
            fout<<fence.GB[i][q]<< " ";
        fout<<endl;
    }
    */
    for (int i = 0; i < width; i++)
    {
        //  ofstream fout("GameBoards/coordonate.txt", ios_base::app);
        for (int q = 0; q < length; q++)
        {
            //  fout<<fence.GB[i][q]<<" ";
            if (fence.GB[i][q])
            {
                x += side * (q);
                y += side * (i);
               
              
                rectangle(x, y, x + side, y + side);
                 setfillstyle(SOLID_FILL, BLUE);
                  floodfill(x+1,y+1,15);
                 
                y -= side * (i);
                x -= side * (q);
            }
        }
    }
    f.UpLeft.x = x;
    f.UpLeft.y = y;
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
        if(fence.GB[i][q])
        {
            f.DownRight.x = f.UpLeft.x + side * q+side;
            f.DownRight.y = f.UpLeft.y + side * i+side;
        }
    /*
    ofstream fout("GameBoards/coordonate.txt", ios_base::app);
    fout<<f.UpLeft.x<<" "<<f.UpLeft.y<<endl;
    fout<<f.DownRight.x<<" "<<f.DownRight.y<<endl;
    fout<<endl;
    */
}
int page = 0;
POINT MouseDraggingPiece(GamePieces& Fence) 
{
    POINT GB;
    bool dragging;
    setactivepage(page);
    setvisualpage(1 - page);
    setfillstyle(SOLID_FILL, BLACK);
    bar(0, 0, getmaxx(), getmaxy());
    if (ismouseclick(WM_LBUTTONDOWN))
    {
       
        clearmouseclick(WM_LBUTTONDOWN);
        POINT mouse;
        GetCursorPos(&mouse);
        
        if (mouse.x >= Fence.UpLeft.x && mouse.x <= Fence.DownRight.x && mouse.y >= Fence.UpLeft.y && mouse.y <=Fence.DownRight.y) {
                dragging = 1; // Începe drag-and-drop
        }
        GB.y = (mouse.x - LeftBorder) / gbSideLength;
        GB.x = (mouse.y - UpBorder) / gbSideLength;
        ofstream fout("GameBoards/coordonate.txt", ios_base::app);
        fout << GB.x << " " << GB.y << endl;
    }
    if(dragging)
    {
          POINT mouse;
          GetCursorPos(&mouse);
          Fence.UpLeft.x=mouse.x-40;
          Fence.UpLeft.y=mouse.y-40;
          if (ismouseclick(WM_RBUTTONDOWN)){
                dragging = 0; // Termină drag-and-drop
                clearmouseclick(WM_RBUTTONDOWN);
                 GB.y = (mouse.x - LeftBorder) / gbSideLength;
                GB.x = (mouse.y - UpBorder) / gbSideLength;
                ofstream fout("GameBoards/coordonate.txt", ios_base::app);
                fout << GB.x << " " << GB.y << endl;
                
          }
    }
    DrawBoardGame();
    drawFence(Fence,Fence.UpLeft.x,Fence.UpLeft.y,80);

    page = 1 - page;
    delay(10); 
    return GB;
}
int main()
{

    readGameBoard("GameBoards/GameBoard.txt");
    readFence(Fence, "GameBoards/Piesa3.txt");
    addFance(Fence);
    moveFence(Fence,2,0);

    initwindow(1400, 700);
    DrawBoardGame();
    
    

    drawFence(Fence, LeftBorder + gbWidth + LeftBorder, UpBorder, gbSideLength);
    rectangle(Fence.UpLeft.x,Fence.UpLeft.y,Fence.DownRight.x, Fence.DownRight.y);
    while (true)
    {
        MouseDraggingPiece(Fence);
    }

    getch();
    closegraph();

    return 0;
}