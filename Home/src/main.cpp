#include <iostream>
#include <graphics.h>
#include <winbgim.h>

int main()
{
// int gd = DETECT, gm;
// initgraph(&gd, &gm, "");
  initwindow(800,600);
  bar(1,10,200,200);
  getch(); return 0;
}