#include <iostream>
#include <graphics.h>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <functional>

using namespace std;

const int length = 9, width = 7;

char GameBoard[width + 1][length + 1];

bool globalDragging = false; // evita palpairea si luarea simultata a mai multor piese
bool gameIsFinised;
bool LevelFinished[60];

void *BackgroundBuffer, *HorseBuffer, *CowBuffer, *SheepBuffer, *PigBuffer, *FenceBuffer, *GrassBuffer, *EmptyAnimalBuffer, *MiniFenceBuffer, *FarmBuffer;
void *LevelBackgroundBuffer;
struct GamePieces
{
    POINT UpLeft, DownRight, InitialPositionOfFence; // coordonatele piesei
    int x, y;
    bool dragging; // daca e in  starea de drag
    bool isPlaced;
    int Side;
    int isRotated;
    int GB[width][length]; // matricea caracteristica a piesei 7x9

} Fence[4],AnimalsAndOther[5];;

struct Buttons
{
    int x, y;
    int length, height;
    string text;
    function<void()> Function;

} btnGame[10], btnMenu[10], BtnLevel[61], BtnLevelType[3], BtnEditor[3];

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

bool VerifyFenceExtremities(int i, int j, GamePieces fence)
{
    int notExtremities = 0;
    for (int k = 1; k <= 3; k++)
        if (Fence[k].isPlaced && Fence[k].GB[i][j])
        {
            if (Fence[k].GB[i - 1][j] && Fence[k].GB[i + 1][j]) // gard vertical
                notExtremities++;
            else if (Fence[k].GB[i][j - 1] && Fence[k].GB[i][j + 1]) // gard orizontal
                notExtremities++;
            else if (Fence[k].GB[i][j - 1] && Fence[k].GB[i + 1][j]) // colt gard L
                notExtremities++;
            else if (Fence[k].GB[i + 1][j] && Fence[k].GB[i][j + 1]) // colt gard L rotit o data
                notExtremities++;
            else if (Fence[k].GB[i - 1][j] && Fence[k].GB[i][j + 1]) // colt gard L rotit de 2 ori
                notExtremities++;
            else if (Fence[k].GB[i][j - 1] && Fence[k].GB[i - 1][j]) // colt gard L rotit de 3 ori
                notExtremities++;
        }
    if (fence.GB[i][j])
    {
        if (fence.GB[i - 1][j] && fence.GB[i + 1][j])
            notExtremities++;
        else if (fence.GB[i][j - 1] && fence.GB[i][j + 1])
            notExtremities++;
        else if (fence.GB[i][j - 1] && fence.GB[i + 1][j]) // colt gard L
            notExtremities++;
        else if (fence.GB[i + 1][j] && fence.GB[i][j + 1]) // colt gard L rotit o data
            notExtremities++;
        else if (fence.GB[i - 1][j] && fence.GB[i][j + 1]) // colt gard L rotit de 2 ori
            notExtremities++;
        else if (fence.GB[i][j - 1] && fence.GB[i - 1][j]) // colt gard L rotit de 3 ori
            notExtremities++;
    }
    if (notExtremities > 1)
        return false;
    return true;
}

bool VerifyFencePosition(GamePieces fence)
{
    int overlap = 0;
    for (int i = 0; i <= width; i++)
        for (int q = 0; q <= length; q++)
        {
            if (fence.GB[i][q] && strchr("01234", GameBoard[i][q]) == NULL)
                return false;
            if (i != 0 && q != 0 && i != width && q != length)
                if (fence.GB[i][q] && VerifyFenceExtremities(i, q, fence) == false)
                    return false;
            if (fence.GB[i][q] && i != 0 && q != 0 && i != width && q != length)
                if (fence.GB[i][q] + (GameBoard[i][q] - '0') > 1)
                    overlap++;
        }
    if (overlap > 1)
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
void CopyMatrix(int a[width][length], int b[width][length])
{
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            a[i][q] = b[i][q];
}
void RotateFence(GamePieces &fence)
{
    NormalizeFence(fence);

    int newFence[width][length];
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
    bool LevelHasWater = 0;
    for (int i = 0; i <= width; i++)
        for (int j = 0; j <= length; j++)
        {
            CopyGameBoard[i][j] = GameBoard[i][j];
            if (GameBoard[i][j] == 'W')
                LevelHasWater = 1;
        }
    for (int i = 1; i < width; i++)
    {
        for (int j = 1; j < length; j++)
        {
            if (strchr("*0CPHSW", CopyGameBoard[i][j])) // C pentru cow, P pentru pig, etc
            {
                char Animal = CopyGameBoard[i][j];
                int CountAnimal = 1;
                bool Water = 0;
                if (CopyGameBoard[i][j] == 'W')
                    Water = 1;

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
                        if (strchr("*0CPHSW", CopyGameBoard[iNext][jNext]))
                        {
                            if (CopyGameBoard[iNext][jNext] != '0' && CopyGameBoard[iNext][jNext] != '*')
                            {
                                if (Animal == '0' || Animal == '*' || Animal == 'W')
                                    Animal = CopyGameBoard[iNext][jNext];
                                else if (CopyGameBoard[iNext][jNext] == Animal)
                                    CountAnimal++;
                                else
                                    return false; // daca sunt mai multe animale de tipuri diferite atunci nu e pus bine gardul
                            }
                            if (CopyGameBoard[iNext][jNext] == 'W')
                                Water = 1;
                            CopyGameBoard[iNext][jNext] = '1';
                            right++;
                            Q[right].row = iNext;
                            Q[right].column = jNext;
                        }
                    }
                    left++;
                }
                if (CountAnimal != CountAnimalInBoard(Animal))
                    return false; // nu sunt toate animalele de tipul asta in acelasi tarc
                if (strchr("CPHS", Animal) == NULL)
                    return false; // tarcul este gol
                if (Water != LevelHasWater)
                    return false;
            }
        }
    }
    return true;
}
// Partea de Grafica

int LeftBorder, UpBorder, gbWidth, gbHeight, gbSideLength = 80, DownBorder, RightBorder;

void ExitButton()
{
    cleardevice();
    closegraph();
    terminate();
}

void StartLevel(string FileName);
void DrawMenu();
void SelectLevel();
void LevelType();
void LevelEditor();

void DrawButton(Buttons btn, int BackColor, int TextColor)
{
    setcolor(BLACK);
    rectangle(btn.x, btn.y, btn.x + btn.length, btn.y + btn.height);
    setcolor(BackColor);
    int aux = 1;
    while (aux != btn.height)
    {
        setcolor(BackColor);
        rectangle(btn.x + aux, btn.y + aux, btn.x + btn.length - aux, btn.y + btn.height - aux);
        aux++;
    }

    char c_text[btn.text.length()];
    strcpy(c_text, btn.text.c_str());
    setbkcolor(BackColor);
    setcolor(TextColor);

    outtextxy((btn.x + btn.length / 2) - textwidth(c_text) / 2, (btn.y + btn.height / 2) - textheight(c_text) / 2, c_text);
    // floodfill(btn.x+2,btn.y+2,15);
}

void ActiveButton(Buttons btnGame[], int n)
{
    POINT mouse;

    GetCursorPos(&mouse);
    for (int i = 0; i < n; i++)
        if (mouse.x > btnGame[i].x && mouse.y > btnGame[i].y && mouse.x < btnGame[i].x + btnGame[i].length && mouse.y < btnGame[i].y + btnGame[i].height)
            DrawButton(btnGame[i], GREEN, WHITE);
    if (ismouseclick(WM_LBUTTONDOWN))
    {
        clearmouseclick(WM_LBUTTONDOWN);
        for (int i = 0; i < n; i++)
            if (mouse.x > btnGame[i].x && mouse.y > btnGame[i].y && mouse.x < btnGame[i].x + btnGame[i].length && mouse.y < btnGame[i].y + btnGame[i].height)
            {
                Beep(1000, 100);
                DrawButton(btnGame[i], GREEN, WHITE);
                btnGame[i].Function();
            }
    }
}

void initialization()
{
   
    gbWidth = gbSideLength * length;
    gbHeight = 400;
    UpBorder = 0.17 * GetSystemMetrics(SM_CYSCREEN);
    LeftBorder = 0.1 * GetSystemMetrics(SM_CXSCREEN);
    RightBorder = 50;

    for (int i = 1; i <= 3; i++)
    {
        string FilePathAux = "GameBoards/";
        FilePathAux += "Piesa";
        FilePathAux += char(i + '0');
        FilePathAux += ".txt";
        ReadFence(Fence[i], FilePathAux);
        Fence[i].isPlaced = Fence[i].isRotated = 0;
    }

    Fence[0].UpLeft.x = Fence[0].UpLeft.y = 0;
    Fence[0].DownRight.x = getmaxx();
    Fence[0].DownRight.y = getmaxy();
    Fence[1].InitialPositionOfFence.x = LeftBorder + gbWidth + LeftBorder;
    Fence[1].InitialPositionOfFence.y = UpBorder;
    Fence[2].InitialPositionOfFence.x = LeftBorder + gbWidth + LeftBorder;
    Fence[2].InitialPositionOfFence.y = UpBorder + gbSideLength * 2;
    Fence[3].InitialPositionOfFence.x = LeftBorder + gbWidth + LeftBorder;
    Fence[3].InitialPositionOfFence.y = UpBorder + gbSideLength * 5;

    btnGame[0] = {0, getmaxy() - 100, 150, 40, "Exit Game ", ExitButton};
    btnGame[2] = {200, getmaxy() - 100, 150, 40, "Back", SelectLevel};

    btnMenu[0] = {LeftBorder, UpBorder, 150, 50, "SELECT LEVEL", LevelType};
    btnMenu[2] = {LeftBorder, UpBorder + 100, 150, 50, "LEVEL EDITOR", LevelEditor};
    btnMenu[1] = {LeftBorder, UpBorder + 200, 150, 50, "EXIT", ExitButton};

    BtnLevelType[0] = {LeftBorder, UpBorder, 150, 50, "Main Levels", SelectLevel};
    BtnLevelType[1] = {LeftBorder, UpBorder + 100, 150, 50, "Custom Levels", ExitButton};
    BtnLevelType[2] = {LeftBorder, UpBorder + 200, 150, 50, "Back", DrawMenu};

    BtnLevel[0] = {200, getmaxy() - 100, 150, 40, "Back", LevelType};
    BtnEditor[0] = {200, getmaxy() - 100, 150, 40, "Back", DrawMenu};

    int height = 1, width = 0;
    for (int i = 1; i <= 30; i++)
    {
        string path = "GameBoards/GameBoard";
        if (i < 10)
            path += char(i + '0');
        else
        {
            path += char(i / 10 + '0');
            path += char(i % 10 + '0');
        }
        path += ".txt";
        string text = "Level ";
        if (i < 10)
            text += (char)(i + '0');
        else
        {
            text += char(i / 10 + '0');
            text += char(i % 10 + '0');
        }
        BtnLevel[i] = {LeftBorder + (width) * 200, UpBorder + height * 80, 150, 50, text, [path]()
                       { StartLevel(path); }};
        width = i % 6;
        if (i % 6 == 0)
            height++;
    }


    //editor
     ReadFence(AnimalsAndOther[0], "EditorGameBoards/Sheep.txt");
    ReadFence(AnimalsAndOther[1], "EditorGameBoards/Horse.txt");
    ReadFence(AnimalsAndOther[2], "EditorGameBoards/Pig.txt");
    ReadFence(AnimalsAndOther[3], "EditorGameBoards/Cow.txt");

}

void DrawBoardGame()
{
    for (int i = 1; i <= length; i++)
        for (int j = 1; j <= width; j++)
            rectangle(LeftBorder + gbSideLength * (i - 1), UpBorder + gbSideLength * (j - 1), LeftBorder + gbSideLength * i, UpBorder + gbSideLength * j);
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
        {
            int x = LeftBorder + q * gbSideLength;
            int y = UpBorder + i * gbSideLength;

            if (GameBoard[i][q] == '#' || strchr("12345", GameBoard[i][q]))
            {
                // DrawInBoardGame(q + 1, i + 1, 1);
                putimage(x, y, FenceBuffer, COPY_PUT);
            }
            else if (GameBoard[i][q] == 'C') // CPHS
            {
                putimage(x, y, CowBuffer, COPY_PUT);
            }
            else if (GameBoard[i][q] == 'P')
            {
                putimage(x, y, PigBuffer, COPY_PUT);
            }
            else if (GameBoard[i][q] == 'H')
            {
                putimage(x, y, HorseBuffer, COPY_PUT);
            }
            else if (GameBoard[i][q] == 'S')
            {
                putimage(x, y, SheepBuffer, COPY_PUT);
            }
            else if (GameBoard[i][q] == '*')
            {
                putimage(x, y, EmptyAnimalBuffer, COPY_PUT);
            }
            else
                putimage(x, y, GrassBuffer, COPY_PUT);
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
            if (fence.GB[i][q] != 0)
            {
                x += side * (q);
                y += side * (i);

                setcolor(WHITE);
                rectangle(x, y, x + side, y + side);
                if (side != gbSideLength)
                    putimage(x, y, MiniFenceBuffer, COPY_PUT);
                else
                    putimage(x, y, FenceBuffer, COPY_PUT);

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
    bar(0, 0, getmaxx(), UpBorder);
    if (ismouseclick(WM_LBUTTONDOWN))
    {
        POINT mouse;
        clearmouseclick(WM_LBUTTONDOWN);
        GetCursorPos(&mouse);
        GB.y = (mouse.x - LeftBorder) / gbSideLength;
        GB.x = (mouse.y - UpBorder) / gbSideLength;
        if (mouse.x >= Fence.UpLeft.x && mouse.x <= Fence.DownRight.x && mouse.y >= Fence.UpLeft.y && mouse.y <= Fence.DownRight.y) // daca se afla in interiorul piesei
        {
            Fence.dragging = 1;
            globalDragging = 1;        // se foloseste pentru a elimina efectul de palpaiala a piesei
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
            globalDragging = 0;
            Fence.Side = gbSideLength / 2;

            clearmouseclick(WM_RBUTTONDOWN);
            GB.y = (mouse.x - LeftBorder) / gbSideLength;
            GB.x = (mouse.y - UpBorder) / gbSideLength;                                                                        // afla coordonatele pe grid
            if (mouse.x < LeftBorder + gbWidth && mouse.y < UpBorder + gbHeight && mouse.x > LeftBorder && mouse.y > UpBorder) // verifica daca se afla in matricea GameBoard
            {
                NormalizeFence(Fence); // normalizarea e pentru mutarea piesei, trebuie adusa in colt , iar dupa mutata  la x si y
                GamePieces newFence = Fence;
                MoveFence(newFence, GB.x, GB.y); // mutarea in grid, mutarea fictiva, matrice auxiara
                if (VerifyFencePosition(newFence))
                {
                    MoveFence(Fence, GB.x, GB.y);
                    AddFence(Fence);
                    Beep(100, 100);
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
    putimage(0, 0, BackgroundBuffer, COPY_PUT);
    DrawBoardGame();
    DrawButton(btnGame[0], WHITE, BLACK);
    DrawButton(btnGame[2], WHITE, BLACK);
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
    initialization();
    DrawBoardGame();
    for (int i = 1; i <= 3; i++)
    {
        Fence[i].Side = gbSideLength * (0.5);
        DrawFence(Fence[i], Fence[i].InitialPositionOfFence.x, Fence[i].InitialPositionOfFence.y, Fence[i].Side);
    }

    while (true)
    {
        if (AnimalsAreFenced())
        {
            cleardevice();
            setcolor(BLACK); // Setează culoarea textului
            char text[] = "FINAL";
            outtextxy(200, 200, text);
            gameIsFinised = true;
            DrawButton(btnGame[0], WHITE, BLACK);
            ActiveButton(btnGame, 3);
        }
        bool SomethingHappend = false;
        POINT mouse;
        GetCursorPos(&mouse);
        if (!gameIsFinised)
            for (int i = 1; i <= 3; i++)
                if (mouse.x >= Fence[i].UpLeft.x && mouse.x <= Fence[i].DownRight.x && mouse.y >= Fence[i].UpLeft.y && mouse.y <= Fence[i].DownRight.y || Fence[i].dragging)
                {
                    if (Fence[i].dragging || !globalDragging) // doar daca nu e nici o pisa in starea de dragging , sau doar una
                        MouseDraggingPiece(Fence[i]);
                    SomethingHappend = true;
                }
        if (!SomethingHappend)
        {
            MouseDraggingPiece(Fence[0]);
            ActiveButton(btnGame, 3);
        }
    }
}
void StartLevel(string FileName)
{
    gameIsFinised = false;
    globalDragging = false;

    setvisualpage(1);
    cleardevice();
    DrawLevel(FileName);
    AnimalsAreFenced();
}

void UploadImages()
{
    setvisualpage(0); // evita efectul de incarcare a imagililor, nu apare fiecare pe rand , apare negru si dupa toate odata
    readimagefile("Images/background.jpg", 0, 0, getmaxx(), getmaxy());
    BackgroundBuffer = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), BackgroundBuffer);

    readimagefile("Images/farm.jpg", 0, 0, getmaxx(), getmaxy());
    FarmBuffer = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), FarmBuffer);

    readimagefile("Images/levelBackground.jpg", 0, 0, getmaxx(), getmaxy());
    LevelBackgroundBuffer = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), LevelBackgroundBuffer);

    cleardevice();

    readimagefile("Images/horse.jpg", 1, 1, gbSideLength - 1, gbSideLength - 1);
    HorseBuffer = malloc(imagesize(1, 1, gbSideLength - 1, gbSideLength - 1));
    getimage(1, 1, gbSideLength - 1, gbSideLength - 1, HorseBuffer);

    readimagefile("Images/cow.jpg", 1, 1, gbSideLength - 1, gbSideLength - 1);
    CowBuffer = malloc(imagesize(1, 1, gbSideLength - 1, gbSideLength - 1));
    getimage(1, 1, gbSideLength - 1, gbSideLength - 1, CowBuffer);

    readimagefile("Images/sheep.jpg", 1, 1, gbSideLength - 1, gbSideLength - 1);
    SheepBuffer = malloc(imagesize(1, 1, gbSideLength - 1, gbSideLength - 1));
    getimage(1, 1, gbSideLength - 1, gbSideLength - 1, SheepBuffer);

    readimagefile("Images/pig.jpg", 1, 1, gbSideLength - 1, gbSideLength - 1);
    PigBuffer = malloc(imagesize(1, 1, gbSideLength - 1, gbSideLength - 1));
    getimage(1, 1, gbSideLength - 1, gbSideLength - 1, PigBuffer);

    readimagefile("Images/grass.jpg", 1, 1, gbSideLength - 1, gbSideLength - 1);
    GrassBuffer = malloc(imagesize(1, 1, gbSideLength - 1, gbSideLength - 1));
    getimage(1, 1, gbSideLength - 1, gbSideLength - 1, GrassBuffer);

    readimagefile("Images/fence.jpg", 1, 1, gbSideLength - 1, gbSideLength - 1);
    FenceBuffer = malloc(imagesize(1, 1, gbSideLength - 1, gbSideLength - 1));
    getimage(1, 1, gbSideLength - 1, gbSideLength - 1, FenceBuffer);

    readimagefile("Images/Rock.jpg", 1, 1, gbSideLength - 1, gbSideLength - 1);
    EmptyAnimalBuffer = malloc(imagesize(1, 1, gbSideLength - 1, gbSideLength - 1));
    getimage(1, 1, gbSideLength - 1, gbSideLength - 1, EmptyAnimalBuffer);

    readimagefile("Images/fence.jpg", 1, 1, (gbSideLength - 1) / 2, (gbSideLength - 1) / 2);
    MiniFenceBuffer = malloc(imagesize(1, 1, (gbSideLength - 1) / 2, (gbSideLength - 1) / 2));
    getimage(1, 1, (gbSideLength - 1) / 2, (gbSideLength - 1) / 2, MiniFenceBuffer);

    cleardevice();
    setvisualpage(0);
}
int newWindow;
void DrawMenu()
{
    initialization();
    int page1 = 0;
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        putimage(0, 0, FarmBuffer, COPY_PUT);
        DrawButton(btnMenu[0], WHITE, BLACK);
        DrawButton(btnMenu[1], WHITE, BLACK);
        DrawButton(btnMenu[2], WHITE, BLACK);
        ActiveButton(btnMenu, 3);
        page1 = 1 - page1;
    }
}
void SelectLevel()
{
    int page1 = 0;
    cleardevice();
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        putimage(0, 0, LevelBackgroundBuffer, COPY_PUT);
        for (int i = 0; i <= 30; i++)
        {
            DrawButton(BtnLevel[i], WHITE, BLACK);
        }
        ActiveButton(BtnLevel, 31);
        page1 = 1 - page1;
    }
}
void LevelType()
{
    int page1 = 0;
    cleardevice();
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        putimage(0, 0, FarmBuffer, COPY_PUT);
        DrawButton(BtnLevelType[0], WHITE, BLACK);
        DrawButton(BtnLevelType[1], WHITE, BLACK);
        DrawButton(BtnLevelType[2], WHITE, BLACK);
        ActiveButton(BtnLevelType, 3);
        page1 = 1 - page1;
    }
}
void drawAnimal(GamePieces animal, int x, int y)
{
    char a;
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
            if (animal.GB[i][q] != 0)
                a = animal.GB[i][q], q = length, i = width;
    if (a == 1)
    {
        putimage(x, y, SheepBuffer, COPY_PUT);
    }
    if (a == 2)
    {
        putimage(x, y, HorseBuffer, COPY_PUT);
    }
    if (a == 3)
    {
        putimage(x, y, PigBuffer, COPY_PUT);
    }
    if (a == 4)
    {
        putimage(x, y, CowBuffer, COPY_PUT);
    }
    if (a == 5)
    {
        putimage(x, y, MiniFenceBuffer, COPY_PUT);
    }
}
void LevelEditor()
{
    int page1 = 0;
    cleardevice(); 
    ReadGameBoard("EditorGameBoards/GameBoardEditor.txt");


    for (int i = 0; i < 4; i++)
    {
        AnimalsAndOther[i].InitialPositionOfFence.x = LeftBorder + gbWidth + LeftBorder;
        AnimalsAndOther[i].InitialPositionOfFence.y = UpBorder + 100 * i;
        AnimalsAndOther[i].DownRight.x = AnimalsAndOther[i].InitialPositionOfFence.x + gbSideLength;
        AnimalsAndOther[i].DownRight.y = AnimalsAndOther[i].InitialPositionOfFence.y + gbSideLength;
        AnimalsAndOther[i].UpLeft = AnimalsAndOther[i].InitialPositionOfFence;
        AnimalsAndOther[i].dragging = 0;
    }

    bool Draging = 0;
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        putimage(0, 0, FarmBuffer, COPY_PUT);
        POINT GB;
       
        bool Something = 0;
        {
            POINT mouse;
            GetCursorPos(&mouse);
            for (int i = 0; i < 5; i++)
                if (AnimalsAndOther[i].dragging|| mouse.x >= AnimalsAndOther[i].UpLeft.x && mouse.x <= AnimalsAndOther[i].DownRight.x && mouse.y >= AnimalsAndOther[i].UpLeft.y && mouse.y <= AnimalsAndOther[i].DownRight.y)
                    Something=1;
        }
        
        if (ismouseclick(WM_LBUTTONDOWN))
        {
            POINT mouse;
            GetCursorPos(&mouse);
            clearmouseclick(WM_LBUTTONDOWN);
            for (int i = 0; i < 5; i++)
                if (!Draging && mouse.x >= AnimalsAndOther[i].UpLeft.x && mouse.x <= AnimalsAndOther[i].DownRight.x && mouse.y >= AnimalsAndOther[i].UpLeft.y && mouse.y <= AnimalsAndOther[i].DownRight.y)
                {
                    AnimalsAndOther[i].dragging = 1;
                    Draging = 1;
                    GB.y = (mouse.x - LeftBorder) / gbSideLength;
                    GB.x = (mouse.y - UpBorder) / gbSideLength;
                }
        }
        for (int i = 0; i < 4; i++)
            if (AnimalsAndOther[i].dragging)
            {
                POINT mouse;
                GetCursorPos(&mouse);
                AnimalsAndOther[i].UpLeft.x = mouse.x - 40; // mutarea efectiva dupa mouse
                AnimalsAndOther[i].UpLeft.y = mouse.y - 40;
                AnimalsAndOther[i].DownRight.x =  AnimalsAndOther[i].UpLeft.x + gbSideLength;
                AnimalsAndOther[i].DownRight.y = AnimalsAndOther[i].UpLeft.y + gbSideLength;
                if (ismouseclick(WM_RBUTTONDOWN)) // plasarea piesiei jos , cu click dreapta
                {
                    AnimalsAndOther[i].dragging = 0;
                    Draging = 0;
                    clearmouseclick(WM_RBUTTONDOWN);
                    GB.y = (mouse.x - LeftBorder) / gbSideLength;
                    GB.x = (mouse.y - UpBorder) / gbSideLength; // afla coordonatele pe grid
                    if (mouse.x < LeftBorder + gbWidth && mouse.y < UpBorder + gbHeight && mouse.x > LeftBorder && mouse.y > UpBorder) // verifica daca se afla in matricea GameBoard
                    {
                        Beep(100, 100);
                        AnimalsAndOther[i].UpLeft.x = GB.y * gbSideLength + LeftBorder; // calculatre coordonate noi
                        AnimalsAndOther[i].UpLeft.y = GB.x * gbSideLength + UpBorder;
                        AnimalsAndOther[i].isPlaced = true;
                        AnimalsAndOther[i].Side = gbSideLength; // aducerea la marimea normala
                    }
                    else {
                        AnimalsAndOther[i].UpLeft=AnimalsAndOther[i].InitialPositionOfFence;
                        AnimalsAndOther[i].DownRight.x =  AnimalsAndOther[i].UpLeft.x + gbSideLength;
                        AnimalsAndOther[i].DownRight.y = AnimalsAndOther[i].UpLeft.y + gbSideLength;
                    }
                }
            }
        DrawBoardGame();
        DrawButton(BtnEditor[0], WHITE, BLACK);
        for (int i = 0; i < 4; i++)
            if (!AnimalsAndOther[i].dragging)
                drawAnimal(AnimalsAndOther[i], AnimalsAndOther[i].UpLeft.x, AnimalsAndOther[i].UpLeft.y);
        for (int i = 0; i < 4; i++)
            if (AnimalsAndOther[i].dragging)
                drawAnimal(AnimalsAndOther[i], AnimalsAndOther[i].UpLeft.x, AnimalsAndOther[i].UpLeft.y);
        if(!Something)
        ActiveButton(BtnEditor, 1);
        page1 = 1 - page1;
    }
}
int main()
{
    initwindow(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), "", -3, -3);
    UploadImages();
    initialization();
    DrawMenu();

    getch();
    closegraph();
    return 0;
}