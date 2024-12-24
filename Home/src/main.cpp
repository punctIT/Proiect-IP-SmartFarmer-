#include <iostream>
#include <graphics.h>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <functional>
#include <time.h>
#include <mmsystem.h>
#include <windows.h>


using namespace std;

#define ThemeNumber 2

const unsigned length = 9, width = 7;

char GameBoard[width + 1][length + 1];

bool globalDragging = false; // evita palpairea si luarea simultata a mai multor piese
bool GameIsFinised;
bool musicOnOff=true;
bool CancelBtn = 0; // folosit la editor,

int theme = 0;
int page = 0;
int languageStatus=0;

int seconds, minutes;
time_t start, now;
bool stopTimer = 0;

void *BackgroundBuffer[ThemeNumber], *HorseBuffer[ThemeNumber], *CowBuffer[ThemeNumber], *SheepBuffer[ThemeNumber], *PigBuffer[ThemeNumber], *FenceBuffer[ThemeNumber], *GrassBuffer[ThemeNumber], *EmptyAnimalBuffer[ThemeNumber], *MiniFenceBuffer[ThemeNumber], *MenuBackGroundBuffer[ThemeNumber];
void *LevelMenuBackgroundBuffer[ThemeNumber], *WaterBuffer[ThemeNumber], *GameRulesBuffer[ThemeNumber], *LevelCompleteBuffer[ThemeNumber];

int ButtonColor, ButtonHoverColor, ButtonTextColor, ButtonHoverTextColor;

struct GamePieces
{
    POINT UpLeft, DownRight, InitialPosition; // coordonatele piesei

    bool dragging; // daca e in  starea de drag
    bool isPlaced;
    bool isRotated;

    int Side;
    int GB[width][length]; // matricea caracteristica a piesei 7x9

} Fence[4], AnimalsAndOther[8];

struct Buttons
{
    int x, y;
    int length, height;
    string text;
    function<void()> Function;

} btnGame[10], BtnMenu[10], BtnLevel[65], BtnLevel1[33], BtnLevelType[5], BtnEditor[3], BtnSave[3],CustomLevels[24];

struct LevelsStatus
{
    int seconds, minutes;
    bool isSolved;

} level[100];
struct language{
    string back;
    string selectLevel;
    string levelEditor;
    string gameRules;
    string settings;
    string exit;
    string mainLevels;
    string customLevels;
    string randomLevels;
    string level;
    string nextPage;
    string previousPage;
    string theme;
    string musicOnOff;
    string restartLevel;
    string saveLevel;
    string saveText;
    string save;
    string confirm;
    string areYouSure;
    string cancel;
    string deleteText; // Renamed from delete
    string typeHere;
    string complited;
    string insuficentSpace;
    string LevelName;
    string language;
    string defaultTheme;
    string chritmasTheme;
}languageText[10];
void readLanguage()
{
    string languageType[10]={"Engleza.txt","Romana.txt","Germana.txt","Franceza.txt"};
    for(int i=0;i<4;i++)
    {
        string path="Languages/"+languageType[i];
        ifstream fin(path);
        getline(fin, languageText[i].back);
        getline(fin, languageText[i].selectLevel);
        getline(fin, languageText[i].levelEditor);
        getline(fin, languageText[i].gameRules);
        getline(fin, languageText[i].settings);
        getline(fin, languageText[i].exit);
        getline(fin, languageText[i].mainLevels);
        getline(fin, languageText[i].customLevels);
        getline(fin, languageText[i].randomLevels);
        getline(fin, languageText[i].level);
        getline(fin, languageText[i].nextPage);
        getline(fin, languageText[i].previousPage);
        getline(fin, languageText[i].theme);
        getline(fin, languageText[i].musicOnOff);
        getline(fin, languageText[i].restartLevel);
        getline(fin, languageText[i].saveLevel);
        getline(fin, languageText[i].saveText);
        getline(fin, languageText[i].save);
        getline(fin, languageText[i].confirm);
        getline(fin, languageText[i].areYouSure);
        getline(fin, languageText[i].cancel);
        getline(fin, languageText[i].deleteText);
        getline(fin, languageText[i].typeHere);
        getline(fin, languageText[i].complited);
        getline(fin, languageText[i].insuficentSpace);
        getline(fin,languageText[i].LevelName);
        getline(fin,languageText[i].language);
        getline(fin,languageText[i].defaultTheme);
        getline(fin,languageText[i].chritmasTheme);
        fin.close();
    }
}
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
    fin.close();
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
    fin.close();
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
                            if (strchr("*0W", CopyGameBoard[iNext][jNext]) == NULL)
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
void WriteRandomGbFIle()
{
    ofstream fout("CustomLevels/Random.txt");
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < length; j++)
            fout << GameBoard[i][j] << ' ';
        fout << '\n';
    }
    fout.close();
}
void GenerateRandomGameboard()
{
    srand(time(0));
    ReadGameBoard("GameBoards/GameBoard.txt");
    char Type[4] = {'C', 'P', 'S', 'H'};
    char A1 = '0', A2 = '0';    // trebuie macar 2 tipuri de animale
    bool HasWater = rand() % 2; // daca e nivel cu apa
    for (int i = 0; i < 4; i++)
        if (rand() % 2) // sanse 50% sa fie animalul respectiv
        {
            int HowMany = 1 + rand() % 2;
            for (int j = 1; j <= HowMany; j++)
            {
                int x, y;
                do
                {
                    x = 1 + 2 * (rand() % 3);
                    y = 1 + 2 * (rand() % 4);
                } while (GameBoard[x][y] != '*');
                GameBoard[x][y] = Type[i]; // pune animalele
                if (A1 == '0')
                    A1 = Type[i];
                else if (A2 == '0')
                    A2 = Type[i];
            }
            if (HasWater)
            {
                int x, y;
                do
                {
                    x = 1 + 2 * (rand() % 3);
                    y = 1 + 2 * (rand() % 4);
                } while (GameBoard[x][y] != '*');
                GameBoard[x][y] = 'W';
            }
        }
    if (A1 == '0')
    {
        int HowMany = 1 + rand() % 2;
        A1 = Type[rand() % 4];
        for (int j = 1; j <= HowMany; j++)
        {
            int x, y;
            do
            {
                x = 1 + 2 * (rand() % 3);
                y = 1 + 2 * (rand() % 4);
            } while (GameBoard[x][y] != '*');
            GameBoard[x][y] = A1;
        }
        if (HasWater)
        {
            int x, y;
            do
            {
                x = 1 + 2 * (rand() % 3);
                y = 1 + 2 * (rand() % 4);
            } while (GameBoard[x][y] != '*');
            GameBoard[x][y] = 'W';
        }
    }
    if (A2 == '0')
    {
        int HowMany = 1 + rand() % 2;
        A2 = Type[rand() % 4];
        while (A1 == A2)
            A2 = Type[rand() % 4];
        for (int j = 1; j <= HowMany; j++)
        {
            int x, y;
            do
            {
                x = 1 + 2 * (rand() % 3);
                y = 1 + 2 * (rand() % 4);
            } while (GameBoard[x][y] != '*');
            GameBoard[x][y] = A2;
        }
        if (HasWater)
        {
            int x, y;
            do
            {
                x = 1 + 2 * (rand() % 3);
                y = 1 + 2 * (rand() % 4);
            } while (GameBoard[x][y] != '*');
            GameBoard[x][y] = 'W';
        }
    }
    bool cow = 0, pig = 0, sheep = 0, horse = 0;
    for (int i = 0; i < width; i++)
        for (int q = 0; q < length; q++)
        {
            if (GameBoard[i][q] == 'C')
                cow = 1;
            if (GameBoard[i][q] == 'H')
                horse = 1;
            if (GameBoard[i][q] == 'P')
                pig = 1;
            if (GameBoard[i][q] == 'S')
                sheep = 1;
        }
    if (cow + sheep + pig + horse < 2)
        GenerateRandomGameboard();
    WriteRandomGbFIle();
}

void MarkFinisedLevel(int n, int minutes, int seconds)
{
    LevelsStatus newLevelST[100];
    ifstream fin("GameBoards/LevelsStatus.txt");
    for (int i = 1; i <= 60; i++)
    {
        fin >> newLevelST[i].isSolved >> newLevelST[i].minutes >> newLevelST[i].seconds;
        if (i == n)
        {
            newLevelST[i].isSolved = true;
            newLevelST[i].minutes = minutes;
            newLevelST[i].seconds = seconds;
        }
    }
    fin.close();
    ofstream fout("GameBoards/LevelsStatus.txt");
    for (int i = 1; i <= 60; i++)
    {
        fout << newLevelST[i].isSolved << " " << newLevelST[i].minutes << " " << newLevelST[i].seconds << endl;
    }
    fout.close();
}
void refreshLevelStatus()
{
    LevelsStatus newLevelST[100];
    ifstream fin("GameBoards/LevelsStatus.txt");
    for (int i = 1; i <= 60; i++)
    {
        fin >> newLevelST[i].isSolved >> newLevelST[i].minutes >> newLevelST[i].seconds;
        level[i] = newLevelST[i];
    }
    fin.close();
}

int NumberMainLevel(string path)
{
    int nr = 0;
    for (int i = 0; i < path.length(); i++)
        if (path[i] < '9' && path[i] > '0')
            nr = nr * 10 + (path[i] - '0');
    return nr;
}



// Partea de Grafica

int LeftBorder, UpBorder, gbWidth, gbHeight, gbSideLength = 80, DownBorder, RightBorder;

void ExitButton()
{
    cleardevice();
    closegraph();
    terminate();
}


void DrawMenu();
void SelectLevelMenu();
void SelectLevel2Menu();
void LevelTypeMenu();
void CustomLevelsMenu();
void ThemeMenu();
void GameRulesMenu();
void LevelEditorMenu();
void SettingsMenu();
void SelectLanguageMenu();

void StartLevel(string FileName,void back());
void LevelSave();
void DeleteConfirmation(string levelName);
void SaveButton();
void CancelButton();
void randombutton();

void ClearMainLevelsProgres()
{
    ofstream fout("GameBoards/LevelsStatus.txt");
    for(int i=1;i<=60;i++)
        fout<<0<<" "<<-1<<" "<<-1<<endl;
    fout.close();
}

void DrawButton(Buttons btn, int BackColor, int TextColor)
{

    setcolor(BackColor);
    setcolor(BLACK);
    int aux = 0;
    while (aux != btn.height)// un fel de flood fill , care are un efect special
    {
        if (aux < 5)
            rectangle(btn.x + aux, btn.y + aux, btn.x + btn.length - aux, btn.y + btn.height - aux);
        else
        {
            setcolor(BackColor);
            rectangle(btn.x + aux, btn.y + aux, btn.x + btn.length - aux, btn.y + btn.height - aux);
        }
        aux++;
    }

    char c_text[btn.text.length()];
    strcpy(c_text, btn.text.c_str());
    setbkcolor(BackColor);
    setcolor(TextColor);
    outtextxy((btn.x + btn.length / 2) - textwidth(c_text) / 2, (btn.y + btn.height / 2) - textheight(c_text) / 2, c_text);
    setcolor(WHITE);
    setbkcolor(BLACK);
}

void ActiveButton(Buttons btnGame[], int st, int en)
{
    POINT mouse;
    GetCursorPos(&mouse);

    for (int i = st; i < en; i++)
        if (mouse.x > btnGame[i].x && mouse.y > btnGame[i].y && mouse.x < btnGame[i].x + btnGame[i].length && mouse.y < btnGame[i].y + btnGame[i].height)
            DrawButton(btnGame[i], ButtonHoverColor, ButtonHoverTextColor);

    if (ismouseclick(WM_LBUTTONDOWN))
    {
        clearmouseclick(WM_LBUTTONDOWN);
        for (int i = st; i < en; i++)
            if (mouse.x > btnGame[i].x && mouse.y > btnGame[i].y && mouse.x < btnGame[i].x + btnGame[i].length && mouse.y < btnGame[i].y + btnGame[i].height)
            {
               // Beep(500, 100);
                DrawButton(btnGame[i], ButtonHoverColor, ButtonHoverColor);
                btnGame[i].Function();
            }
    }
}

void initialization()
{
    GameIsFinised = false;
    globalDragging = 0;
    gbWidth = gbSideLength * length;
    gbHeight = gbSideLength * width;
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
    Fence[1].InitialPosition.x = LeftBorder + gbWidth + LeftBorder;
    Fence[1].InitialPosition.y = UpBorder;
    Fence[2].InitialPosition.x = LeftBorder + gbWidth + LeftBorder;
    Fence[2].InitialPosition.y = UpBorder + gbSideLength * 2;
    Fence[3].InitialPosition.x = LeftBorder + gbWidth + LeftBorder;
    Fence[3].InitialPosition.y = UpBorder + gbSideLength * 4;

    settextstyle(3, 0, 2);
    btnGame[0] = {getmaxx()-300, getmaxy() - 100, 150, 40, languageText[languageStatus].exit, ExitButton};
   

    BtnMenu[0] = {getmaxx() / 2 - 75, getmaxy() / 2 + 60, 150, 50, languageText[languageStatus].selectLevel, LevelTypeMenu};
    BtnMenu[2] = {getmaxx() / 2 - 75, getmaxy() / 2 + 120, 150, 50, languageText[languageStatus].levelEditor, LevelEditorMenu};
    BtnMenu[3] = {getmaxx() / 2 - 75, getmaxy() / 2 + 180, 150, 50, languageText[languageStatus].gameRules, GameRulesMenu};
    BtnMenu[1] = {getmaxx() / 2 - 75, getmaxy() / 2 + 300, 150, 50, languageText[languageStatus].exit, ExitButton};
    BtnMenu[4] = {getmaxx() / 2 - 75, getmaxy() / 2 + 240, 150, 50,languageText[languageStatus].settings, SettingsMenu};

    BtnLevelType[3] = {getmaxx() / 2 - 75, getmaxy() / 2 + 190, 150, 50,languageText[languageStatus].randomLevels, randombutton};
    BtnLevelType[2] = {getmaxx() / 2 - 75, getmaxy() / 2 + 50, 150, 50, languageText[languageStatus].mainLevels, SelectLevelMenu};
    BtnLevelType[1] = {getmaxx() / 2 - 75, getmaxy() / 2 + 120, 150, 50, languageText[languageStatus].customLevels, CustomLevelsMenu};
    BtnLevelType[0] = {getmaxx() / 2 - 75, getmaxy() / 2 + 260, 150, 50, languageText[languageStatus].back, DrawMenu};

    BtnLevel[0] = { gbSideLength, getmaxy() - 100, 150, 40, languageText[languageStatus].back, LevelTypeMenu};
    BtnLevel1[0] = { gbSideLength, getmaxy() - 100, 150, 40, languageText[languageStatus].back, LevelTypeMenu};

    BtnEditor[0] = { gbSideLength, getmaxy() - 100, 150, 40, languageText[languageStatus].back, DrawMenu};
    BtnEditor[1] = { gbSideLength + 200, getmaxy() - 100, 150, 40, languageText[languageStatus].saveLevel, LevelSave};

    BtnSave[0] = {getmaxx() / 2 - 200, getmaxy() / 2 + 200, 150, 40, languageText[languageStatus].cancel, CancelButton};
    BtnSave[1] = {getmaxx() / 2, getmaxy() / 2 + 200, 150, 40, languageText[languageStatus].save, SaveButton};

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
        string text = languageText[languageStatus].level;
        text+=" ";
        if (i < 10)
            text += (char)(i + '0');
        else
        {
            text += char(i / 10 + '0');
            text += char(i % 10 + '0');
        }
        BtnLevel[i] = {LeftBorder + (width) * 200, UpBorder + height * 80, 150, 50, text, [path]()
                       { StartLevel(path,SelectLevelMenu); }};
        width = i % 6;
        if (i % 6 == 0)
            height++;
    }
    height = 1;

    for (int q = 1; q <= 30; q++)
    {
        int i = q + 30;
        string path = "GameBoards/GameBoard";
        if (i < 10)
            path += char(i + '0');
        else
        {
            path += char(i / 10 + '0');
            path += char(i % 10 + '0');
        }
        path += ".txt";
        string text = languageText[languageStatus].level;
        if (i < 10)
            text += (char)(i + '0');
        else
        {
            text += char(i / 10 + '0');
            text += char(i % 10 + '0');
        }
        BtnLevel1[q] = {LeftBorder + (width) * 200, UpBorder + height * 80, 150, 50, text, [path]()
                        { StartLevel(path,SelectLevel2Menu); }};
        width = q % 6;
        if (q % 6 == 0)
            height++;
    }

    // editor
    ReadFence(AnimalsAndOther[0], "EditorGameBoards/Sheep.txt");
    ReadFence(AnimalsAndOther[1], "EditorGameBoards/Horse.txt");
    ReadFence(AnimalsAndOther[2], "EditorGameBoards/Pig.txt");
    ReadFence(AnimalsAndOther[3], "EditorGameBoards/Cow.txt");
    ReadFence(AnimalsAndOther[4], "EditorGameBoards/Water.txt");
    ReadFence(AnimalsAndOther[5], "EditorGameBoards/Grass.txt");
    ReadFence(AnimalsAndOther[6], "EditorGameBoards/StoneGrass.txt");
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
                putimage(x, y, FenceBuffer[theme], COPY_PUT);
            }
            else if (GameBoard[i][q] == 'C') // CPHS
            {
                putimage(x, y, CowBuffer[theme], COPY_PUT);
            }
            else if (GameBoard[i][q] == 'P')
            {
                putimage(x, y, PigBuffer[theme], COPY_PUT);
            }
            else if (GameBoard[i][q] == 'H')
            {
                putimage(x, y, HorseBuffer[theme], COPY_PUT);
            }
            else if (GameBoard[i][q] == 'S')
            {
                putimage(x, y, SheepBuffer[theme], COPY_PUT);
            }
            else if (GameBoard[i][q] == '*')
            {
                putimage(x, y, EmptyAnimalBuffer[theme], COPY_PUT);
            }
            else if (GameBoard[i][q] == 'W')
            {
                putimage(x, y, WaterBuffer[theme], COPY_PUT);
            }
            else
                putimage(x, y, GrassBuffer[theme], COPY_PUT);
        }
}

void DrawFence(GamePieces &fenceToDraw, int x, int y, int side)
{
    int ZeroLineNumber = 0, ZeroColumnNumber = 0;
    GamePieces fence = fenceToDraw;
    NormalizeFence(fence);
    for (int i = 0; i < width; i++) // partea de desenare
        for (int q = 0; q < length; q++)
            if (fence.GB[i][q] != 0)
            {
                x += side * (q);
                y += side * (i);

                setcolor(WHITE);
                rectangle(x, y, x + side, y + side);
                if (side != gbSideLength)
                    putimage(x, y, MiniFenceBuffer[theme], COPY_PUT);
                else
                    putimage(x, y, FenceBuffer[theme], COPY_PUT);

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
}

void DrawTime(int seconds, int minutes, int x, int y)
{
    char Timer[6]; 
    snprintf(Timer, sizeof(Timer), "%02d:%02d", minutes, seconds); 
    outtextxy(x, y, Timer);
}

void MouseDraggingPiece(GamePieces &Fence)
{
    POINT GB;
    setactivepage(page);
    setvisualpage(1 - page);
    setfillstyle(SOLID_FILL, BLACK);
    
    if (!stopTimer)
    {
        now = time(NULL);
        if (difftime(now, start) >= 1)
        {                // Dacă a trecut 1 secundă
            seconds++;   // Incrementează secundele
            start = now; // Actualizează timpul de start
        }
        if (seconds >= 60)
            seconds = 0, minutes++;
    }

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
        if (IsKeyPressed('r') || IsKeyPressed('r'))
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
                    Fence.UpLeft = Fence.InitialPosition;
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
                Fence.UpLeft = Fence.InitialPosition;
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
    putimage(0, 0, BackgroundBuffer[theme], COPY_PUT);
    DrawTime(seconds, minutes, getmaxx() / 2 - 30, 60);
    DrawBoardGame();
    DrawButton(btnGame[0], ButtonColor, ButtonTextColor);
    DrawButton(btnGame[2], ButtonColor, ButtonTextColor);
    for (int i = 1; i <= 3; i++)
        if (!::Fence[i].dragging)
            DrawFence(::Fence[i], ::Fence[i].UpLeft.x, ::Fence[i].UpLeft.y, ::Fence[i].Side); // cele care nu sunt in dragging trebuie afisate
    DrawFence(Fence, Fence.UpLeft.x, Fence.UpLeft.y, Fence.Side);                             // se evita efectul de palpaiala , libraria GRAPHICS.H e naspa.
    page = 1 - page;

   
}
void DrawLevel(string GameBoardFileName,void back())
{
    btnGame[2] = { gbSideLength, getmaxy() - 100, 150, 40,languageText[languageStatus].back, back};
    ReadGameBoard(GameBoardFileName);
    initialization();
    
    for (int i = 1; i <= 3; i++)
    {
        Fence[i].Side = gbSideLength * (0.5);
        DrawFence(Fence[i], Fence[i].InitialPosition.x, Fence[i].InitialPosition.y, Fence[i].Side);
    }

    start = time(NULL);
    seconds = minutes = 0;
    stopTimer = 0;
    
    while (true)
    {

        if (AnimalsAreFenced())
        {
            cleardevice();
            setcolor(WHITE); // Setează culoarea textului
            putimage(0, 0, LevelCompleteBuffer[theme], COPY_PUT);

            int x = 290, y = 270;
            bar(x, y, 1250, 630);
            char text[] = "Solve time:";
            settextstyle(3, 0, 4);
            outtextxy(x + 20, y + 40, text);
            settextstyle(3, 0, 2);
            DrawTime(seconds, minutes, x + 250, y + 40);
            GameIsFinised = true;

            if (GameBoardFileName.find("GameBoards/Gameboard"))
                MarkFinisedLevel(NumberMainLevel(GameBoardFileName), minutes, seconds);
            DrawButton(btnGame[0], ButtonColor, ButtonTextColor);
            DrawButton(btnGame[2], ButtonColor, ButtonTextColor);
            ActiveButton(btnGame, 0, 3);
            stopTimer = 1;
        }
        bool SomethingHappend = false;
        POINT mouse;
        GetCursorPos(&mouse);
        if (!GameIsFinised)
            for (int i = 1; i <= 3; i++)
                if (mouse.x >= Fence[i].UpLeft.x && mouse.x <= Fence[i].DownRight.x && mouse.y >= Fence[i].UpLeft.y && mouse.y <= Fence[i].DownRight.y || Fence[i].dragging)
                {
                    if (Fence[i].dragging || !globalDragging)
                    { // doar daca nu e nici o pisa in starea de dragging , sau doar una
                        MouseDraggingPiece(Fence[i]);
                    }
                    SomethingHappend = true;
                }
        if (!SomethingHappend)
        {
            MouseDraggingPiece(Fence[0]);
            ActiveButton(btnGame, 0, 3);
            while(kbhit())//pe scurt rezolva CACATUL ala de bug de disparea gardul principal
                getch();//ramanea pe buffer cacartere , un cacat
        }
    }
}
void StartLevel(string FileName, void back())
{
    GameIsFinised = false;
    globalDragging = false;
    setvisualpage(1);
    setactivepage(0);
    cleardevice();

    DrawLevel(FileName,back);
}
void LoadingScreen(int progress)
{
    setvisualpage(0);
    cleardevice();
    char loading[] = "Loading...";
    outtextxy(getmaxx() / 2 - 50, getmaxy() / 2 - 40, loading);
    char percent[5];                     // Buffer suficient pentru numere între 0 și 100 + '%'
    sprintf(percent, "%d %%", progress); // Formatăm ca procentaj
    outtextxy(getmaxx() / 2 - 25, getmaxy() / 2, percent);
    // delay(100);
    setvisualpage(1);
}

void UploadImages()
{
    int progress = 0, percent = 100 / 28;
    readimagefile("Images/background.jpg", 0, 0, getmaxx(), getmaxy());
    BackgroundBuffer[0] = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), BackgroundBuffer[0]);

    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/MenuBackground.jpg", 0, 0, getmaxx(), getmaxy());
    MenuBackGroundBuffer[0] = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), MenuBackGroundBuffer[0]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/LevelComplite.jpg", 0, 0, getmaxx(), getmaxy());
    LevelCompleteBuffer[0] = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), LevelCompleteBuffer[0]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/GameRules.jpg", 0, 0, getmaxx(), getmaxy());
    GameRulesBuffer[0] = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), GameRulesBuffer[0]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/levelBackground.jpg", 0, 0, getmaxx(), getmaxy());
    LevelMenuBackgroundBuffer[0] = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), LevelMenuBackgroundBuffer[0]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/horse.jpg", 0, 0, gbSideLength, gbSideLength);
    HorseBuffer[0] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, HorseBuffer[0]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/cow.jpg", 0, 0, gbSideLength, gbSideLength);
    CowBuffer[0] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, CowBuffer[0]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/sheep.jpg", 0, 0, gbSideLength, gbSideLength);
    SheepBuffer[0] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, SheepBuffer[0]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/pig.jpg", 0, 0, gbSideLength, gbSideLength);
    PigBuffer[0] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, PigBuffer[0]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/grass.jpg", 0, 0, gbSideLength, gbSideLength);
    GrassBuffer[0] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, GrassBuffer[0]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/fence.jpg", 0, 0, gbSideLength, gbSideLength);
    FenceBuffer[0] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, FenceBuffer[0]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/Rock.jpg", 0, 0, gbSideLength, gbSideLength);
    EmptyAnimalBuffer[0] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, EmptyAnimalBuffer[0]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/Water.jpg", 0, 0, gbSideLength, gbSideLength);
    WaterBuffer[0] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, WaterBuffer[0]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/fence.jpg", 0, 0, (gbSideLength) / 2, (gbSideLength) / 2);
    MiniFenceBuffer[0] = malloc(imagesize(0, 0, (gbSideLength) / 2, (gbSideLength) / 2));
    getimage(0, 0, (gbSideLength) / 2, (gbSideLength) / 2, MiniFenceBuffer[0]);
    progress += percent;
    LoadingScreen(progress);
    // Chritmas Theme
    readimagefile("Images/Chritmasbackground.jpg", 0, 0, getmaxx(), getmaxy());
    BackgroundBuffer[1] = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), BackgroundBuffer[1]);

    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/ChritmasMenuBackground.jpg", 0, 0, getmaxx(), getmaxy());
    MenuBackGroundBuffer[1] = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), MenuBackGroundBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/ChritmasLevelComplite.jpg", 0, 0, getmaxx(), getmaxy());
    LevelCompleteBuffer[1] = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), LevelCompleteBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/ChritmasGameRules.jpg", 0, 0, getmaxx(), getmaxy());
    GameRulesBuffer[1] = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), GameRulesBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/ChritmaslevelBackground.jpg", 0, 0, getmaxx(), getmaxy());
    LevelMenuBackgroundBuffer[1] = malloc(imagesize(0, 0, getmaxx(), getmaxy()));
    getimage(0, 0, getmaxx(), getmaxy(), LevelMenuBackgroundBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/Chritmashorse.jpg", 0, 0, gbSideLength, gbSideLength);
    HorseBuffer[1] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, HorseBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/Chritmascow.jpg", 0, 0, gbSideLength, gbSideLength);
    CowBuffer[1] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, CowBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/Chritmassheep.jpg", 0, 0, gbSideLength, gbSideLength);
    SheepBuffer[1] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, SheepBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/Chritmaspig.jpg", 0, 0, gbSideLength, gbSideLength);
    PigBuffer[1] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, PigBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/Chritmasgrass.jpg", 0, 0, gbSideLength, gbSideLength);
    GrassBuffer[1] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, GrassBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/Chritmasfence.jpg", 0, 0, gbSideLength, gbSideLength);
    FenceBuffer[1] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, FenceBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/ChritmasRock.jpg", 0, 0, gbSideLength, gbSideLength);
    EmptyAnimalBuffer[1] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, EmptyAnimalBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/ChritmasWater.jpg", 0, 0, gbSideLength, gbSideLength);
    WaterBuffer[1] = malloc(imagesize(0, 0, gbSideLength, gbSideLength));
    getimage(0, 0, gbSideLength, gbSideLength, WaterBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    readimagefile("Images/Chritmasfence.jpg", 0, 0, (gbSideLength) / 2, (gbSideLength) / 2);
    MiniFenceBuffer[1] = malloc(imagesize(0, 0, (gbSideLength) / 2, (gbSideLength) / 2));
    getimage(0, 0, (gbSideLength) / 2, (gbSideLength) / 2, MiniFenceBuffer[1]);
    progress += percent;
    LoadingScreen(progress);

    cleardevice();
    setvisualpage(0);
}

void DrawMenu()
{
    initialization();
    setvisualpage(1);
    setactivepage(0);
    cleardevice();

    int page1 = 0;
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        putimage(0, 0, MenuBackGroundBuffer[theme], COPY_PUT);
        for(int i=0;i<5;i++)
             DrawButton(BtnMenu[i], ButtonColor, ButtonTextColor);
        ActiveButton(BtnMenu, 0, 5);
        page1 = 1 - page1;
    }
}
void DrawButtonColorByTime(int i,Buttons btn)
{
    int time=level[i].minutes*60+level[i].seconds;
    if(time>60)
        DrawButton(btn,RED,ButtonTextColor);
    if(time >30&&time<=60)
        DrawButton(btn,RGB(255,99,71),ButtonTextColor);
    if(time<=30)
        DrawButton(btn,GREEN,ButtonTextColor);
    
}
void SelectLevelMenu()
{
    int page1 = 0;
    setvisualpage(1);
    setactivepage(0);
    cleardevice();
    BtnLevel[31] = {getmaxx() - 300, getmaxy() - 100, 150, 40, languageText[languageStatus].nextPage, SelectLevel2Menu};

    refreshLevelStatus();
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        putimage(0, 0, LevelMenuBackgroundBuffer[theme], COPY_PUT);
        DrawButton(BtnLevel[61], ButtonColor, ButtonTextColor);
        for (int i = 0; i <= 31; i++)
        {
            if (level[i].isSolved == false)
                DrawButton(BtnLevel[i], ButtonColor, ButtonTextColor);
            else{
                string saveText=BtnLevel[i].text;
                BtnLevel[i].text=languageText[languageStatus].complited;
                DrawButtonColorByTime(i,BtnLevel[i]);
                BtnLevel[i].text=saveText;
            }
        }
        ActiveButton(BtnLevel, 0, 32);
        page1 = 1 - page1;
    }
}

void SelectLevel2Menu()
{
    int page1 = 0;
    //elimina efectul de IMAGINE neagra, PAC meniu
    setvisualpage(1);
    setactivepage(0);
    cleardevice();
    refreshLevelStatus();
    BtnLevel1[31] = {getmaxx() - 300, getmaxy() - 100, 150, 40, languageText[languageStatus].previousPage, SelectLevelMenu};
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        putimage(0, 0, LevelMenuBackgroundBuffer[theme], COPY_PUT);
        for (int i = 0; i <= 31; i++)
        {
            if (level[i + 30].isSolved == false)
                DrawButton(BtnLevel1[i], ButtonColor, ButtonTextColor);
            else{
                 string saveText=BtnLevel1[i].text;
                 BtnLevel1[i].text=languageText[languageStatus].complited;
                 DrawButtonColorByTime(i+30,BtnLevel1[i]);
                 BtnLevel1[i].text=saveText;
            }
                
        }
        ActiveButton(BtnLevel1, 0, 32);
        page1 = 1 - page1;
    }
}
void LevelTypeMenu()
{
    int page1 = 0;
    setvisualpage(1);
    setactivepage(0);
    cleardevice();
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        putimage(0, 0, MenuBackGroundBuffer[theme], COPY_PUT);
        DrawButton(BtnLevelType[0], ButtonColor, ButtonTextColor);
        DrawButton(BtnLevelType[1], ButtonColor, ButtonTextColor);
        DrawButton(BtnLevelType[2], ButtonColor, ButtonTextColor);
        DrawButton(BtnLevelType[3], ButtonColor, ButtonTextColor);
        ActiveButton(BtnLevelType, 0, 4);
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
        putimage(x, y, SheepBuffer[theme], COPY_PUT);
    if (a == 2)
        putimage(x, y, HorseBuffer[theme], COPY_PUT);
    if (a == 3)
        putimage(x, y, PigBuffer[theme], COPY_PUT);
    if (a == 4)
        putimage(x, y, CowBuffer[theme], COPY_PUT);
    if (a == 5)
        putimage(x, y, WaterBuffer[theme], COPY_PUT);
    if (a == 6)
        putimage(x, y, GrassBuffer[theme], COPY_PUT);
    if (a == 7)
        putimage(x, y, EmptyAnimalBuffer[theme], COPY_PUT);
}
void addAnimal(POINT poz, int i)
{
    if (GameBoard[poz.x][poz.y] != '#')
    {
        if (i == 0)
            GameBoard[poz.x][poz.y] = 'S';
        if (i == 1)
            GameBoard[poz.x][poz.y] = 'H';
        if (i == 2)
            GameBoard[poz.x][poz.y] = 'P';
        if (i == 3)
            GameBoard[poz.x][poz.y] = 'C';
        if (i == 4)
            GameBoard[poz.x][poz.y] = 'W';
        if (i == 5)
            GameBoard[poz.x][poz.y] = '0';
        if (i == 6)
            GameBoard[poz.x][poz.y] = '*';
    }
}

int NumberOfLevel()
{
    ifstream fin("CustomLevels/LevelNames.txt");
    int nr = 0;
    char levelTitle[100];
    while (fin >> levelTitle)
        nr++;
    return nr;
    fin.close();
}

void LevelEditorMenu()
{
    if (NumberOfLevel() < 6)
    {
        setvisualpage(1);
        setactivepage(0);
        cleardevice();
        int page1 = 0;
        if (!CancelBtn)
        {
            cleardevice();
            ReadGameBoard("EditorGameBoards/GameBoardEditor.txt");
        }
        CancelBtn = 0;
        int contor = 0, contor2 = 0;
        for (int i = 0; i <= 6; i++)
        {

            AnimalsAndOther[i].InitialPosition.x = LeftBorder + gbWidth + LeftBorder + 150 * contor;
            AnimalsAndOther[i].InitialPosition.y = UpBorder + 100 * contor2;
            AnimalsAndOther[i].DownRight.x = AnimalsAndOther[i].InitialPosition.x + gbSideLength;
            AnimalsAndOther[i].DownRight.y = AnimalsAndOther[i].InitialPosition.y + gbSideLength;
            AnimalsAndOther[i].UpLeft = AnimalsAndOther[i].InitialPosition;
            AnimalsAndOther[i].dragging = 0;
            contor2 = (i + 1) % 6;
            if ((i + 1) % 6 == 0)
                contor++;
        }
        bool Draging = 0;
        while (true)
        {
            setactivepage(page1);
            setvisualpage(1 - page1);
            putimage(0, 0, BackgroundBuffer[theme], COPY_PUT);
            POINT GB;

            bool Something = 0;
            {
                POINT mouse;
                GetCursorPos(&mouse);
                for (int i = 0; i <= 6; i++)
                    if (AnimalsAndOther[i].dragging || mouse.x >= AnimalsAndOther[i].UpLeft.x && mouse.x <= AnimalsAndOther[i].DownRight.x && mouse.y >= AnimalsAndOther[i].UpLeft.y && mouse.y <= AnimalsAndOther[i].DownRight.y)
                        Something = 1;
                GB.y = (mouse.x - LeftBorder) / gbSideLength;
                GB.x = (mouse.y - UpBorder) / gbSideLength;
            }

            if (ismouseclick(WM_LBUTTONDOWN))
            {
                POINT mouse;
                GetCursorPos(&mouse);
                clearmouseclick(WM_LBUTTONDOWN);
                GB.y = (mouse.x - LeftBorder) / gbSideLength;
                GB.x = (mouse.y - UpBorder) / gbSideLength;
                for (int i = 0; i <= 6; i++)
                {
                    if (!Draging && mouse.x >= AnimalsAndOther[i].UpLeft.x && mouse.x <= AnimalsAndOther[i].DownRight.x && mouse.y >= AnimalsAndOther[i].UpLeft.y && mouse.y <= AnimalsAndOther[i].DownRight.y)
                    {
                        AnimalsAndOther[i].dragging = 1;
                        Draging = 1;
                    }
                }
            }
            for (int i = 0; i <= 6; i++)
                if (AnimalsAndOther[i].dragging)
                {
                    POINT mouse;
                    GetCursorPos(&mouse);
                    AnimalsAndOther[i].UpLeft.x = mouse.x - 40; // mutarea efectiva dupa mouse
                    AnimalsAndOther[i].UpLeft.y = mouse.y - 40;
                    AnimalsAndOther[i].DownRight.x = AnimalsAndOther[i].UpLeft.x + gbSideLength;
                    AnimalsAndOther[i].DownRight.y = AnimalsAndOther[i].UpLeft.y + gbSideLength;
                    if (ismouseclick(WM_RBUTTONDOWN)) // plasarea piesiei jos , cu click dreapta
                    {
                        AnimalsAndOther[i].dragging = 0;
                        Draging = 0;
                        clearmouseclick(WM_RBUTTONDOWN);
                        GB.y = (mouse.x - LeftBorder) / gbSideLength;
                        GB.x = (mouse.y - UpBorder) / gbSideLength;                                                                            // afla coordonatele pe grid
                        if (mouse.x <= LeftBorder + gbWidth && mouse.y <= UpBorder + gbHeight && mouse.x >= LeftBorder && mouse.y >= UpBorder) // verifica daca se afla in matricea GameBoard
                        {
                            Beep(100, 100);
                            addAnimal(GB, i);
                            AnimalsAndOther[i].UpLeft = AnimalsAndOther[i].InitialPosition;
                            AnimalsAndOther[i].DownRight.x = AnimalsAndOther[i].UpLeft.x + gbSideLength;
                            AnimalsAndOther[i].DownRight.y = AnimalsAndOther[i].UpLeft.y + gbSideLength;
                        }
                        else
                        {
                            AnimalsAndOther[i].UpLeft = AnimalsAndOther[i].InitialPosition;
                            AnimalsAndOther[i].DownRight.x = AnimalsAndOther[i].UpLeft.x + gbSideLength;
                            AnimalsAndOther[i].DownRight.y = AnimalsAndOther[i].UpLeft.y + gbSideLength;
                        }
                    }
                }
            DrawBoardGame();
            DrawButton(BtnEditor[0], ButtonColor, ButtonTextColor);
            DrawButton(BtnEditor[1], ButtonColor, ButtonTextColor);
            for (int i = 0; i <= 6; i++)
                if (!AnimalsAndOther[i].dragging)
                    drawAnimal(AnimalsAndOther[i], AnimalsAndOther[i].UpLeft.x, AnimalsAndOther[i].UpLeft.y);
            for (int i = 0; i <= 6; i++)
                if (AnimalsAndOther[i].dragging)
                    drawAnimal(AnimalsAndOther[i], AnimalsAndOther[i].UpLeft.x, AnimalsAndOther[i].UpLeft.y);
            if (!Something)
                ActiveButton(BtnEditor, 0, 2);
            page1 = 1 - page1;
        }
    }
    else
    {
        int page1 = 0;
        setvisualpage(1);
        setactivepage(0);
        cleardevice();
        while (true)
        {
            setactivepage(page1);
            setvisualpage(1 - page1);
            cleardevice();
            DrawButton(BtnEditor[0], ButtonColor, ButtonTextColor);
            ActiveButton(BtnEditor, 0, 1);
            char txt[100] ;
            strcpy(txt,languageText[languageStatus].insuficentSpace.c_str());
            outtextxy(200, 200, txt);
            page1 = 1 - page1;
        }
    }
}

char Savetext[100];
void LevelSave()
{
    
    int NumberOfCustomLevels = 0;
    int x = getmaxx() / 4, y = getmaxy() / 2;
    strcpy(Savetext, languageText[languageStatus].typeHere.c_str());
    int page1 = 0;
   
    int cursorPos = 0;        // Poziția cursorului în text
    int maxChars = (700) / 8; // Estimează câte caractere încap pe lățimea chenarului
    setvisualpage(1);
    setactivepage(0);
    cleardevice();
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        putimage(0, 0, BackgroundBuffer[theme], COPY_PUT);
        DrawButton(BtnEditor[0], ButtonColor, ButtonTextColor);
        DrawButton(BtnEditor[1], ButtonColor, ButtonTextColor);
        DrawBoardGame();

        for (int i = 0; i <= 6; i++)
            if (!AnimalsAndOther[i].dragging)
                drawAnimal(AnimalsAndOther[i], AnimalsAndOther[i].UpLeft.x, AnimalsAndOther[i].UpLeft.y);
        int bgColor = RGB(220, 220, 220);
        setbkcolor(bgColor);
        setcolor(bgColor);
        
        int aux = 1;
        while (aux != x)
        {
            setcolor(bgColor);
            rectangle(x + aux, y, x + 700 - aux, y + 300);
            aux++;
        }
        setcolor(BLACK);
        rectangle(x, y, x + 700, y + 300);
        setcolor(ButtonColor);
        rectangle(x + 1, y + 1, x + 700 - 1, y + 300 - 1);
        if (kbhit())
        {
            char ch = getch(); // Așteaptă input de la utilizator
            if (ch == 13)      // adica enter
                Beep(600, 100), SaveButton();
            else if (ch == 8)
            { // Backspace
                if (cursorPos > 0)
                {
                    cursorPos--;
                    Savetext[cursorPos] = '\0';
                }
            }
            else if (cursorPos < maxChars - 1 && cursorPos < 20)
            {
                if (ch == ' ') // fisierle nu pot avea nume cu spatiu
                    ch = '-';
                Savetext[cursorPos++] = ch;
                Savetext[cursorPos] = '\0';
            }
        }

        setcolor(BLACK);
        char confirmationTXT[100] ;
        strcpy(confirmationTXT,languageText[languageStatus].LevelName.c_str());
        outtextxy(x + 30, y + 50, confirmationTXT);
        outtextxy(x + 140, y + 50, Savetext);
        DrawButton(BtnSave[0], ButtonColor, ButtonTextColor);
        DrawButton(BtnSave[1], ButtonColor, ButtonTextColor);
        ActiveButton(BtnSave, 0, 2);

        page1 = 1 - page1;
    }
}

void SaveButton()
{

    if (strcmp(Savetext, languageText[languageStatus].typeHere.c_str()) == 0)
        strcpy(Savetext, "default");
    string FileName = "CustomLevels/";
    FileName += Savetext;
    FileName += ".txt";

    ifstream fin("CustomLevels/LevelNames.txt", ios_base::app);
    string names;
    bool isThere = false;
    while (fin >> names)
        if (names == Savetext)
            isThere = true;
    fin.close();
    ofstream fout("CustomLevels/LevelNames.txt", ios_base::app);
    if (!isThere)
        fout << Savetext << endl;
    fout.close();
    ofstream foutLV(FileName);
    for (int i = 0; i < width; i++)
    {
        for (int q = 0; q < length; q++)
            foutLV << GameBoard[i][q] << " ";
        foutLV << endl;
    }

    strcpy(Savetext, "");
    foutLV.close();
    DrawMenu();
}
void CancelButton()
{
    CancelBtn = 1;
    LevelEditorMenu();
}
void deleteCustomLevel(string LevelName)
{

    string customLV[11];
    int count = 0;
    int p = -1;

    ifstream fin("CustomLevels/LevelNames.txt");
    while (fin >> customLV[count])
    {
        if (customLV[count] == LevelName)
        {
            p = count;
        }
        count++;
    }
    fin.close();

    if (p == -1)
    {
        return;
    }
    string levelPath = "CustomLevels/" + customLV[p] + ".txt";
    const char *filePath = levelPath.c_str();

    remove(filePath);
    ofstream fout("CustomLevels/LevelNames.txt");
    for (int i = 0; i < count; ++i)
    {
        if (i != p)
        {
            fout << customLV[i] << endl;
        }
    }

    fout.close();
    CustomLevelsMenu();
}

void CustomLevelsMenu()
{
    int NumberOfL = NumberOfLevel();
    ifstream fin("CustomLevels/LevelNames.txt");
    for (int i = 0; i < NumberOfL; i++)
    {
        string LevelTitle;
        fin >> LevelTitle;
        string LevelPath = "CustomLevels/" + LevelTitle + ".txt";
        CustomLevels[i] = {LeftBorder, UpBorder + (i + 1) * 80, 150, 50, LevelTitle, [LevelPath]()
                           { StartLevel(LevelPath,CustomLevelsMenu); }};
    }
    fin.close();
    fin.open("CustomLevels/LevelNames.txt");
    for (int i = NumberOfL; i < 2 * NumberOfL; i++)
    {
        string LevelTitle;
        fin >> LevelTitle;
        string LevelPath = "CustomLevels/" + LevelTitle + ".txt";
        CustomLevels[i] = {LeftBorder + 200, UpBorder + (i - NumberOfL + 1) * 80, 150, 50, languageText[languageStatus].deleteText +" "+ LevelTitle, [LevelTitle]()
                           { DeleteConfirmation(LevelTitle); }};
    }

    CustomLevels[2 * NumberOfL] = { gbSideLength, getmaxy()-100, 150, 50, languageText[languageStatus].back, LevelTypeMenu};
    int page1 = 0;
    setvisualpage(1);
    setactivepage(0);
    cleardevice();
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        putimage(0, 0, LevelMenuBackgroundBuffer[theme], COPY_PUT);
        for (int i = 0; i <= 2 * NumberOfLevel(); i++)
            DrawButton(CustomLevels[i], ButtonColor, ButtonTextColor);
        ActiveButton(CustomLevels, 0, 2 * NumberOfLevel() + 1);
        page1 = 1 - page1;
    }
}

void DeleteConfirmation(string levelName)
{
    int NumberOfCustomLevels = 0;
    int x = getmaxx() / 4, y = getmaxy() / 2;
    int page1 = 0;
    setvisualpage(1);
    setactivepage(0);
    cleardevice();

    Buttons BtnConform[3];
    BtnConform[0] = {getmaxx() / 2 - 200, getmaxy() / 2 + 200, 150, 40,languageText[languageStatus].cancel, CustomLevelsMenu};
    BtnConform[1] = {getmaxx() / 2, getmaxy() / 2 + 200, 150, 40, languageText[languageStatus].confirm, [levelName](){deleteCustomLevel(levelName);}};

    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        //fundal
        putimage(0, 0, LevelMenuBackgroundBuffer[theme], COPY_PUT);
        for (int i = 0; i <= 2 * NumberOfLevel(); i++)
            DrawButton(CustomLevels[i], ButtonColor, ButtonTextColor);
        
        int bgColor = RGB(220, 220, 220);
  
        int aux = 1;
        while (aux != x)
        {
            setcolor(bgColor);
            rectangle(x + aux, y, x + 700 - aux, y + 300);
            aux++;
        }
        setcolor(BLACK);
        rectangle(x, y, x + 700, y + 300);
        setcolor(ButtonColor);
        rectangle(x + 1, y + 1, x + 700 - 1, y + 300 - 1);
        DrawButton(BtnConform[0],ButtonColor,ButtonTextColor);
        DrawButton(BtnConform[1],ButtonColor,ButtonTextColor);
        setbkcolor(bgColor);
        setcolor(BLACK);
        
        char confirmationTXT[100];
        strcpy(confirmationTXT,languageText[languageStatus].areYouSure.c_str());
        outtextxy(x + 30, y + 50, confirmationTXT);
        strcpy(confirmationTXT,languageText[languageStatus].LevelName.c_str());
        outtextxy(x + 30, y + 90, confirmationTXT);
        strcpy(confirmationTXT,levelName.c_str());
        outtextxy(x + 150, y + 90, confirmationTXT);
        ActiveButton(BtnConform,0,2);
        page1 = 1 - page1;
    }
}
void randombutton()
{
    initialization();
    GenerateRandomGameboard();
    DrawLevel("CustomLevels/nivel-custom.txt",LevelTypeMenu);
}
void GameRulesMenu()
{
    Buttons BackBTN[1];
    BackBTN[0] = {100, getmaxy() - 100, 150, 40, languageText[languageStatus].back, DrawMenu};
    int page1 = 0;
    setvisualpage(1);
    setactivepage(0);
    cleardevice();
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        putimage(0, 0, GameRulesBuffer[theme], COPY_PUT);
        DrawButton(BackBTN[0], ButtonColor, ButtonTextColor);
        ActiveButton(BackBTN, 0, 1);
        page1 = 1 - page1;
    }
}
void defaultTheme()
{
    theme = 0;
    if(musicOnOff)
    PlaySound(TEXT("Sounds/music.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    ButtonColor = RGB(5, 231, 178);
    ButtonHoverColor = RGB(152, 255, 152);
    ButtonHoverTextColor = ButtonTextColor = BLACK;
    DrawMenu();
}
void ChritmasTheme()
{
    theme = 1;
    if(musicOnOff)
    PlaySound(TEXT("Sounds/christmasmusic.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    ButtonColor = RGB(178, 34, 34);
    ButtonHoverColor = RGB(34, 139, 34);
    ButtonHoverTextColor = ButtonTextColor = WHITE;
    DrawMenu();
}

void musicStatus()
{
    musicOnOff= int(musicOnOff+1)%2;
    if(musicOnOff)
    {
        if(theme==1)
        {
             PlaySound(TEXT("Sounds/christmasmusic.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        }
        if(theme==0)
        {
             PlaySound(TEXT("Sounds/music.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        }
    }
    else {
         PlaySound(NULL, NULL, 0);
    }
}
Buttons SettingButtons[5];
void SettingsMenu()
{
    setvisualpage(1);
    setactivepage(0);
    cleardevice();  
    SettingButtons[0] = {gbSideLength, getmaxy() - 100, 150, 40, languageText[languageStatus].back, DrawMenu};
    SettingButtons[1] = {LeftBorder, UpBorder, 150, 40, languageText[languageStatus].theme, ThemeMenu};
    SettingButtons[2] = {LeftBorder, UpBorder+60, 150, 40, languageText[languageStatus].restartLevel, ClearMainLevelsProgres};
    SettingButtons[3] = {LeftBorder, UpBorder+120, 150, 40, languageText[languageStatus].musicOnOff, musicStatus};
    SettingButtons[4] = {LeftBorder, UpBorder+180, 150, 40, languageText[languageStatus].language, SelectLanguageMenu};
    int page1 = 0;
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        cleardevice();
        if(musicOnOff)
            SettingButtons[3].text=languageText[languageStatus].musicOnOff+" ON";
        else
            SettingButtons[3].text=languageText[languageStatus].musicOnOff+" OFF";
        //putimage(0, 0, MenuBackGroundBuffer[theme], COPY_PUT);
        for(int i=0;i<5;i++)
        {
            if(i!=3||musicOnOff)
               DrawButton(SettingButtons[i], ButtonColor, ButtonTextColor);
            else {
                DrawButton(SettingButtons[i], RED, ButtonTextColor);
            }
        }
        ActiveButton(SettingButtons, 0, 5);
        page1 = 1 - page1;
    }
}
void SelectLanguage(int i)
{
    languageStatus=i;
    SettingsMenu();
}
void SelectLanguageMenu()
{
      Buttons languageBtn[4];
    languageBtn[0] = {gbSideLength, getmaxy() - 100, 150, 40, languageText[languageStatus].back, SettingsMenu};
    languageBtn[1] = {LeftBorder+200, UpBorder, 150, 40, "English", [](){SelectLanguage(0);}};
    languageBtn[2] = {LeftBorder+200, UpBorder + 60, 150, 40, "Romana", [](){SelectLanguage(1);}};
    languageBtn[3] = {LeftBorder+200, UpBorder + 120, 150, 40, "Deutsch", [](){SelectLanguage(2);}};
    languageBtn[4] = {LeftBorder+200, UpBorder + 180, 150, 40, "Francais", [](){SelectLanguage(3);}};
    int page1 = 0;
    setvisualpage(1);
    setactivepage(0);
    cleardevice();
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        cleardevice();
        for(int i=0;i<5;i++)
        {
            if(i!=3||musicOnOff)
               DrawButton(SettingButtons[i], ButtonColor, ButtonTextColor);
            else {
                DrawButton(SettingButtons[i], RED, ButtonTextColor);
            }
        }
             
        for(int i=0;i<5;i++)
        DrawButton(languageBtn[i], ButtonColor, ButtonTextColor);
        ActiveButton(languageBtn, 0, 5);
        page1 = 1 - page1;
    }
}
void ThemeMenu()
{
    Buttons themeBtn[4];
    themeBtn[0] = {gbSideLength, getmaxy() - 100, 150, 40, languageText[languageStatus].back, SettingsMenu};
    themeBtn[1] = {LeftBorder+200, UpBorder, 150, 40, languageText[languageStatus].defaultTheme, defaultTheme};
    themeBtn[2] = {LeftBorder+200, UpBorder + 60, 150, 40,languageText[languageStatus].chritmasTheme, ChritmasTheme};
    int page1 = 0;
    setvisualpage(1);
    setactivepage(0);
    cleardevice();
    while (true)
    {
        setactivepage(page1);
        setvisualpage(1 - page1);
        cleardevice();
        for(int i=0;i<5;i++)
             {
            if(i!=3||musicOnOff)
               DrawButton(SettingButtons[i], ButtonColor, ButtonTextColor);
            else {
                DrawButton(SettingButtons[i], RED, ButtonTextColor);
            }
        }
        DrawButton(themeBtn[0], ButtonColor, ButtonTextColor);
        DrawButton(themeBtn[1], ButtonColor, ButtonTextColor);
        DrawButton(themeBtn[2], ButtonColor, ButtonTextColor);
        ActiveButton(themeBtn, 0, 3);
        page1 = 1 - page1;
    }
}
// de facut :: SETTONG   THEME < MUSIC OFF/ON // DELETE PROGRESS
int main()
{
    initwindow(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), "", -3, -3);
    readLanguage();
    initialization();
    
    UploadImages();
    defaultTheme();

    getch();
    closegraph();
    return 0;
}