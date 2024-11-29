#include <iostream>
#include <winbgim.h>
#include <graphics.h>
#include <cstring>
#include <fstream>
using namespace std;

char GameBoard[7][9];
int length = 9, width = 7;

struct GamePieces
{
    int x, y;
    struct
    {
        int n;
        pair<int, int> coordonate[7]; // coordonatele in maticea de marcare, first- x , second y
        char symbol = '5';
    } GB;

} Fence;
void readFence(GamePieces &fence, string FileName)
{
    ifstream fin(FileName);
    fin >> fence.GB.n;
    for (int i = 1; i <= fence.GB.n; i++)
        fin >> fence.GB.coordonate[i].first >> fence.GB.coordonate[i].second;
}
bool VerifyFencePosition(GamePieces fence, int x, int y)
{
    for (int i = 1; i <= fence.GB.n; i++)
        if (strchr("01234", GameBoard[fence.GB.coordonate[i].first + y][fence.GB.coordonate[i].second + x]) == NULL)
            return false;
    return true;
}
void addFance(GamePieces fence)
{
    if (!VerifyFencePosition(fence, 0, 0))
    {
        cout << "erroare" << endl;
        return;
    }
    for (int i = 1; i <= fence.GB.n; i++)
        GameBoard[fence.GB.coordonate[i].first][fence.GB.coordonate[i].second]++;
}
void moveFence(GamePieces &fence, int x, int y) // x coloana , y linia
{
    if (!VerifyFencePosition(fence, x, y))
    {
        cout << "erroare1" << endl;
        return;
    }
    for (int i = 1; i <= fence.GB.n; i++)
    {
        GameBoard[fence.GB.coordonate[i].first][fence.GB.coordonate[i].second]--;
        fence.GB.coordonate[i].first += y;
        fence.GB.coordonate[i].second += x;
    }
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
void readGameBoard()
{
    ifstream fin("GameBoards/GameBoard.txt");
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            fin >> GameBoard[i][q];
}

// Partea de Grafica
int LeftBorder, UpBorder, gbWidth, gbHeight, gbSideLength;
void DrawInBoardGame(int x, int y,int color)
{
   // circle(LeftBorder+ gbSideLength * (x - 1) + gbSideLength/ 2, UpBorder + gbSideLength * (y - 1) +gbSideLength / 2, 10);
    setfillstyle(SOLID_FILL,color);
    floodfill(LeftBorder+ gbSideLength * (x - 1) + gbSideLength/ 2, UpBorder + gbSideLength * (y - 1) +gbSideLength / 2,15);
    
}
void DrawBoardGame()
{
    gbWidth = 400;
    gbHeight = 400;
    gbSideLength = gbWidth / 5;
    UpBorder = 10;
    LeftBorder = 10;
    for (int i = 1; i <= length; i++)
        for (int j = 1; j <= width; j++)
            rectangle(LeftBorder + gbSideLength * (i - 1), UpBorder + gbSideLength * (j - 1), LeftBorder + gbSideLength * i, UpBorder + gbSideLength * j);
     for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            if(GameBoard[i][q]=='#')
            {
                DrawInBoardGame(q+1,i+1,2);
            }
            else if(GameBoard[i][q]=='*')
            {
                DrawInBoardGame(q+1,i+1,3);
            }
            else {
                DrawInBoardGame(q+1,i+1,10);
            }
            
    
}

int main()
{

     readGameBoard();
     readFence(Fence,"GameBoards/Piesa1.txt");
//   DrawGameBoard();

    initwindow(800, 600);
    DrawBoardGame();
    getch();
    closegraph();
    return 0;
    ;
}