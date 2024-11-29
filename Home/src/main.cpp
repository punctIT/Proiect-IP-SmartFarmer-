#include <iostream>
#include <cstring>
#include <winbgim.h>
#include <stdlib.h>
#include <fstream>
using namespace std;



char GameBoard[7][9];
int length=9,width=7;

struct GamePieces{
    int x,y;
    struct{
       int n;
       pair <int,int> coordonate[7]; //coordonatele in maticea de marcare, first- x , second y
       char symbol='5';
    }GB;
    
}Fence;
void readFence(GamePieces &fence,string FileName)
{
    ifstream fin(FileName);
    fin>>fence.GB.n;
    for(int i=1;i<=fence.GB.n;i++)
        fin>>fence.GB.coordonate[i].first>>fence.GB.coordonate[i].second;
    
}
bool VerifyFencePosition(GamePieces fence,int x,int y)
{
    for (int i = 1; i <= fence.GB.n; i++)
        if (strchr("01234", GameBoard[fence.GB.coordonate[i].first+y][fence.GB.coordonate[i].second+x])==NULL)
            return false;
    return true;
}
void addFance(GamePieces fence)
{
    if(!VerifyFencePosition(fence,0,0)){
        cout<<"erroare"<<endl;
        return;
    }
    for (int i = 1; i <= fence.GB.n; i++)
        GameBoard[fence.GB.coordonate[i].first][fence.GB.coordonate[i].second]++;
}
void moveFence(GamePieces& fence,int x,int y)//x coloana , y linia
{
    if(!VerifyFencePosition(fence,x,y)){
        cout<<"erroare1"<<endl;
        return;
    }
    for (int i = 1; i <= fence.GB.n; i++){
        GameBoard[fence.GB.coordonate[i].first][fence.GB.coordonate[i].second]--;
        fence.GB.coordonate[i].first+=y;
        fence.GB.coordonate[i].second+=x;
    }
    addFance(fence);
   
}
void DrawGameBoard()
{
    for(int i=0;i<width;i++)
    {
        for(int q=0;q<length;q++)
            cout<<GameBoard[i][q]<<" ";
        cout<<endl;
    }
}
void readGameBoard()
{
    ifstream fin("GameBoards/GameBoard.txt");
    for(int i=0;i<width;i++)
        for(int q=0;q<length;q++)
            fin>>GameBoard[i][q];
}
int main()
{
    readGameBoard();
    readFence(Fence,"GameBoards/Piesa1.txt");
    readFence(fence2,"GameBoards/Piesa1.txt");
    redraw:
    DrawGameBoard();
    cout<<endl;
    moveFence(Fence,1,0);
    moveFence(Fence,1,0);
    moveFence(Fence,-1,2);
    DrawGameBoard();
}