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
    int GB[7][9];

} Fence;

void readFence(GamePieces &fence, string FileName)
{
    ifstream fin(FileName);
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            fin>>fence.GB[i][q];
}

bool VerifyFencePosition(GamePieces fence, int x, int y)
{
     for(int i=0;i<width;i++)
        for(int q=0;q<length;q++)
            if(fence.GB[i][q]&&strchr("01234",GameBoard[i][q])==NULL)
                return false;
    return true;
}

void addFance(GamePieces fence)
{
    if(!VerifyFencePosition(fence,0,0))
        {
            cout<<"erroare";
            return;
        }
    for(int i=0;i<width;i++)
        for(int q=0;q<length;q++)
            GameBoard[i][q]+=fence.GB[i][q];
}
void moveFence(GamePieces &fence, int x, int y) // x coloana , y linia(indicele , cu cat sa se mute)
{
    if (!VerifyFencePosition(fence, x, y))
    {
        cout << "erroare1" << endl;
        return;
    }
    GamePieces auxMatrix={0};
    for (int i = 0; i < width; i++)
    {
        for (int q = 0; q < length; q++)
           if(fence.GB[i][q])
           {
                GameBoard[i][q]--;
                //GameBoard[i+y][q+x]++;
                fence.GB[i][q]=0;
                auxMatrix.GB[i+y][q+x]=1;
           }           
    }
    fence=auxMatrix;
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

int LeftBorder, UpBorder, gbWidth, gbHeight, gbSideLength,DownBorder,RightBorder;
void DrawInBoardGame(int x, int y, int color)
{
    // circle(LeftBorder+ gbSideLength * (x - 1) + gbSideLength/ 2, UpBorder + gbSideLength * (y - 1) +gbSideLength / 2, 10);
    setfillstyle(SOLID_FILL, color);
    floodfill(LeftBorder + gbSideLength * (x - 1) + gbSideLength / 2, UpBorder + gbSideLength * (y - 1) + gbSideLength / 2, 15);
}
void DrawBoardGame()
{
    gbWidth = 400;
    gbHeight = 400;
    gbSideLength = gbWidth / 5;
    UpBorder = 50;
    LeftBorder = 50;
    RightBorder=50;
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

POINT MouseCoordonates()//RETURNEAZA coodonatele in matrice, unde se afla mouseul
{
    POINT  GB;
    if(ismouseclick(WM_LBUTTONDOWN))
    {
        Beep(250,600);
        clearmouseclick(WM_LBUTTONDOWN);
        int x=mousex();
        int y=mousey();
        GB.y=(x-LeftBorder)/gbSideLength;
        GB.x=(y-UpBorder)/gbSideLength;
        ofstream fout("GameBoards/coordonate.txt", ios_base::app);
        fout<<GB.x<<" "<<GB.y<<endl;
    }
    return GB;
}
int main()
{

    readGameBoard("GameBoards/GameBoard.txt");
    readFence(Fence, "GameBoards/Piesa1.txt");
    addFance(Fence);
    moveFence(Fence,2,2);

    initwindow(1400, 700);
    DrawBoardGame();
    while(true){
        MouseCoordonates();
    }
        
    getch();
    closegraph();

    return 0;
    
}