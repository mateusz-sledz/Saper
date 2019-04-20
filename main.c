//
//  main.c
//  kolejka
//
//  Created by Mateusz Sledz on 08.01.2018.
//  Copyright © 2018 Mateusz Sledz. All rights reserved.
//

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define CMIN 5
#define CMAX 50
#define RMIN 5
#define RMAX 20
#define MMIN 8
#define STCK 5000


void create(int r, int c);
void clean_count(void);
void field(int r, int c);
int check(int x, int y);
void push(int a, int b);
void uncover(int x, int y);
void to_screen(int xr ,int xc);
void first_mine(void);
void you_won(void);
void admin(void);
int pop(void);
int r, c ;
int n, sp  ,xr ,xc ;
int x, y;
int mine_count=0, mines=0, opened=0;
int stack[STCK];

WINDOW *create_newwin(int r, int c);
WINDOW *my_win;

struct cell{
    bool open;                  // pole odkryte
    bool flag ;                 //flaga na polu
    bool mine ;                 //mina na polu
    int count ;                 //liczba min w sasiedztwie
    
}*tab[21][51];

int main() {
    
    int i, p;
    int ch;
    puts("Podaj ilosc wierszy: ");
    scanf("%d",&r);
    puts("Podaj ilosc kolumn: ");
    scanf("%d",&c);
    
    if(r < RMIN) r = RMIN;          // Jeśli n nie mieści się w zakresie,
    if(r > RMAX) r = RMAX;          // to je odpowiednio dopasowujemy
    // Ustalamy liczbę kolumn
    if(c < CMIN) c = CMIN;            // Jeśli c nie mieści się w zakresie
    if(c > CMAX) c = CMAX;
    
    
    initscr();
    cbreak();
    keypad(stdscr,TRUE);
    create(r,c);
    field(r,c);
    noecho();
    printw("Press F1 to exit\n");
    printw("Press F2 to go admin mode");
    refresh();
    my_win = create_newwin(r, c);
    refresh();
    wmove(my_win,1,1);
    getyx(my_win,y,x);
    
    for(i=1; i <=r; i++)
    {
        for(p=1; p<=c; p++)
            
        {
            waddch(my_win, ACS_BLOCK );
        }
        y=y+1;
        wmove(my_win,y,x);
        
    }
    
    wmove(my_win,1,1);
    wrefresh(my_win);
    getyx(my_win,y,x);
    while((ch = getch()) != KEY_F(1) )
    {
        switch(ch)
        {    case KEY_LEFT:
                if(x-1 <1) break;
                wmove(my_win,y,x-1);
                wrefresh(my_win);
                x=x-1;
                break;
                
            case KEY_RIGHT:
                if(x+1>c) break;
                wmove(my_win,y,x+1);
                wrefresh(my_win);
                x=x+1;
                break;
                
            case KEY_UP:
                if(y-1<1) break;
                wmove(my_win,y-1,x);
                wrefresh(my_win);
                y=y-1;
                break;
                
            case KEY_DOWN:
                if(y+1>r) break;
                wmove(my_win,y+1,x);
                wrefresh(my_win);
                y=y+1;
                break;
                
            case ' ':
                if(opened==0 && tab[y][x]->mine)
                {
                    tab[y][x]->mine = 0;
                    first_mine();
                }
                if(check(x,y)==9)
                {
                    wmove(my_win,1,1);
                    getyx(my_win,y,x);
                    
                    for(i=1; i <=r; i++)
                    {
                        for(p=1; p<=c; p++)
                            
                        {
                            if(tab[i][p]->mine)
                            {
                                if(tab[i][p]->flag)
                                    continue;
                                else
                                {
                                    wmove(my_win, i, p);
                                    waddch(my_win, 'm' );
                                    wrefresh(my_win);
                                }
                            }
                            else if(tab[i][p]->flag)
                            {
                                wmove(my_win, i, p);
                                waddch(my_win, 'x' );
                                wrefresh(my_win);
                            }
                            else continue;
                        }
                        y=y+1;
                        wmove(my_win,y,x);
                        
                    }
                    wmove(my_win,1,1);
                    wrefresh(my_win);
                    
                    move(r/2 +1,c+6);
                    printw("KONIEC GRY");
                    refresh();
                    move(r+6,2);
                    refresh();
                    return 0;
                }
                if(tab[y][x]->open)
                    break;
                if((tab[y][x]->count)==0)
                {
                    uncover(x,y);
                    if(mine_count==0 && (opened==(r*c - mines)))
                    {
                        you_won();
                        return 0;
                    }
                    break;
                }
                else
                {   tab[y][x]->open = 1;
                    opened++;
                    check(x,y);
                    if(mine_count==0 && (opened==(r*c - mines)))
                    {
                        you_won();
                        return 0;
                    }
                } break;
                
            case 'f':
                
                if( (tab[y][x]->flag ) && !(tab[y][x]->open))
                {
                    waddch(my_win, '#');
                    wmove(my_win, y,x);
                    wrefresh(my_win);
                    tab[y][x]->flag=0;
                    if(tab[y][x]->mine)
                    {mine_count++;
                    }
                    break;
                }
                if(tab[y][x]->open)
                    break;
                
                else
                {
                    waddch(my_win, '~');
                    wmove(my_win, y,x);
                    wrefresh(my_win);
                    tab[y][x]->flag=1;
                    if(tab[y][x]->mine==1)
                    {
                        mine_count--;
                        if(mine_count==0 && (opened==(r*c - mines)))
                        {
                            you_won();
                            return 0;
                        }
                        
                        
                    }
                    break;
                }
                
            case KEY_F(2):
                admin();
                break;
                
            default:
                wmove(my_win, y,x);
                wrefresh(my_win);
                break;
                
        }
    }
    
    endwin();
    return 0;
}



void push(int a, int b)
{
    stack[sp]=a;
    sp++;
    stack[sp]=b;
    sp++;
}

int pop()
{
    int pom;
    sp--;
    pom= stack[sp];
    stack[sp]=0;
    return pom;
}

WINDOW *create_newwin(int r, int c)
{
    WINDOW *local_win;
    local_win = newwin(r+2, c+2, 4,1 );
    box(local_win, 0 , 0);
    wrefresh(local_win);
    return local_win;
}

void create(int r, int c)
{
    int i,p;
    for(p=1; p<=r ;p++)
    {
        
        for(i=1; i<=c;  i++)
        {
            tab[p][i] = (struct cell *)malloc(sizeof(struct cell));
            tab[p][i]->open = 0 ;
            tab[p][i]->flag = 0 ;
            tab[p][i]->mine = 0 ;
            tab[p][i]->count = 0 ;
            
        }
    }
    
}

void first_mine()
{
    int i, j, p, ii, jj;
    
    for(p=1; p<=r ;p++)
    {
        
        for(i=1; i<=c;  i++)
        {
            tab[p][i]->count = 0 ;
            
        }
    }
    
    
    for( i = 1; i <= r; i++)
        for( j = 1; j <= c; j++)
            for( ii = i - 1; ii <= i + 1; ii++)
                if((ii >= 1) && (ii <= r))
                    for( jj = j - 1; jj <= j + 1; jj++)
                        if((jj >= 1) && (jj <= c) && (tab[ii][jj]->mine))
                            tab[i][j]->count++;
    
}

void field(int r, int c)
{
    int mn=(r*c)/50;
    int i,j,ii,jj;
    // Wstawiamy miny na pole do komórek         W mines będzie ostateczna liczba min
    mine_count = 0;
    srand(time(NULL));
    while(mine_count < mn)
    {
        int rr = rand() % (r+1);         // Generujemy losowe pole planszy
        int cc = rand() % (c+1);
        if( !(rr && cc)) continue;
        if(tab[rr][cc]->mine) continue; // Jeśli zawiera minę, to powtarzamy
        tab[rr][cc]->mine = 1;         // Wstawiamy minę w pole
        mine_count++;                        // Zwiększamy licznik min
    }
    mines = mine_count;
    
    for( i = 1; i <= r; i++)
        for( j = 1; j <= c; j++)
            for( ii = i - 1; ii <= i + 1; ii++)
                if((ii >= 1) && (ii <= r))
                    for( jj = j - 1; jj <= j + 1; jj++)
                        if((jj >= 1) && (jj <= c) && tab[ii][jj]->mine)
                            tab[i][j]->count++;
}


int check(int x, int y)
{
    if(tab[y][x]->mine)
    {
        return 9;
    }
    
    switch(tab[y][x]->count)
    {
        case 0:
            waddch(my_win, ' ');
            wmove(my_win, y,x);
            wrefresh(my_win);
            break;
            
        case 1:
            waddch(my_win, '1');
            wmove(my_win, y,x);
            wrefresh(my_win);
            break;
        case 2:
            
            waddch(my_win, '2');
            wmove(my_win, y,x);
            wrefresh(my_win);
            break;
        case 3:
            
            waddch(my_win, '3');
            wmove(my_win, y,x);
            wrefresh(my_win);
            break;
        case 4:
            
            waddch(my_win, '4');
            wmove(my_win, y,x);
            wrefresh(my_win);
            break;
        case 5:
            
            waddch(my_win, '5');
            wmove(my_win, y,x);
            wrefresh(my_win);
            break;
        case 6:
            
            waddch(my_win, '6');
            wmove(my_win, y,x);
            wrefresh(my_win);
            break;
        case 7:
            
            waddch(my_win, '7');
            wmove(my_win, y,x);
            wrefresh(my_win);
            break;
        case 8:
            
            waddch(my_win, '8');
            wmove(my_win, y,x);
            wrefresh(my_win);
            break;
            
        default: break;
    }
    return 0;
}

void uncover(int x, int y)
{
    int i,j;
    sp=0;
    push(y,x);
    
    while(stack[0] && sp>=0)
    {
        xc = pop();
        xr= pop();
        if(tab[xr][xc]->open) continue;
        tab[xr][xc]->open = 1 ;
        opened++;
        to_screen(xr, xc);
        if((tab[xr][xc]->count)==0)
        {
            for(i= xr-1; i<= xr+1 ; i++)
            {
                if((i>= 1) && (i <=r))
                {
                    for(j=xc-1 ; j<= xc+1 ; j++)
                        if((j>=1) && (j <= c) && (tab[i][j]->open == 0))
                        {
                            push(i,j);
                        }
                }
            }
            
        }
    }
    to_screen(y,x);
}

void to_screen(int xr ,int xc)
{
    
    switch(tab[xr][xc]->count)
    {
        case 0:
            wmove(my_win, xr,xc);
            waddch(my_win, ' ');
            wmove(my_win, xr,xc);
            wrefresh(my_win);
            break;
            
        case 1:
            wmove(my_win, xr,xc);
            waddch(my_win, '1');
            wmove(my_win, xr,xc);
            wrefresh(my_win);
            break;
            
        case 2:
            wmove(my_win, xr,xc);
            waddch(my_win, '2');
            wmove(my_win,xr,xc);
            wrefresh(my_win);
            break;
            
        case 3:
            wmove(my_win, xr,xc);
            waddch(my_win, '3');
            wmove(my_win, xr,xc);
            wrefresh(my_win);
            break;
            
        case 4:
            wmove(my_win, xr,xc);
            waddch(my_win, '4');
            wmove(my_win, xr,xc);
            wrefresh(my_win);
            break;
            
        case 5:
            wmove(my_win, xr,xc);
            waddch(my_win, '5');
            wmove(my_win, xr,xc);
            wrefresh(my_win);
            break;
        case 6:
            wmove(my_win, xr,xc);
            waddch(my_win, '6');
            wmove(my_win, xr,xc);
            wrefresh(my_win);
            break;
        case 7:
            wmove(my_win, xr,xc);
            waddch(my_win, '7');
            wmove(my_win, xr,xc);
            wrefresh(my_win);
            break;
        case 8:
            wmove(my_win, xr,xc);
            waddch(my_win, '8');
            wmove(my_win, xr,xc);
            wrefresh(my_win);
            break;
            
        default: break;
    }
}

void admin()
{
    int i,p,ch;
    move(1,0);
    printw("Press F2 to exit admin mode");
    wmove(my_win,1,1);
    refresh();
    getyx(my_win,y,x);
    for(i=1; i <=r; i++)
    {
        for(p=1; p<=c; p++)
            
        {
            if(tab[i][p]->mine)
                waddch(my_win, '1' );
            else waddch(my_win, ' ' );
        }
        y=y+1;
        wmove(my_win,y,x);
        
    }
    wmove(my_win, 1,1);
    wrefresh(my_win);
    getyx(my_win,y,x);
    
    while((ch = getch()) != KEY_F(2) )
    {
        switch(ch)
        {    case KEY_LEFT:
                if(x-1 <1) break;
                wmove(my_win,y,x-1);
                wrefresh(my_win);
                x=x-1;
                break;
                
            case KEY_RIGHT:
                if(x+1>c) break;
                wmove(my_win,y,x+1);
                wrefresh(my_win);
                x=x+1;
                break;
                
            case KEY_UP:
                if(y-1<1) break;
                wmove(my_win,y-1,x);
                wrefresh(my_win);
                y=y-1;
                break;
                
            case KEY_DOWN:
                if(y+1>r) break;
                wmove(my_win,y+1,x);
                wrefresh(my_win);
                y=y+1;
                break;
                
            case ' ':
                if(tab[y][x]->mine)
                {
                    tab[y][x]->mine=0;
                    waddch(my_win, ' ');
                    wmove(my_win, y,x);
                    wrefresh(my_win);
                    mines--;
                    mine_count = mines;
                }
                else
                {
                    tab[y][x]->mine=1;
                    waddch(my_win, '1');
                    wmove(my_win, y,x);
                    wrefresh(my_win);
                    mines++;
                    mine_count = mines;
                }
                break;
            default:
                break;
        }
    }
    clean_count();
    
    wmove(my_win,1,1);
    getyx(my_win,y,x);
    
    for(i=1; i <=r; i++)
    {
        for(p=1; p<=c; p++)
            
        {
            waddch(my_win, ACS_BLOCK );
        }
        y=y+1;
        wmove(my_win,y,x);
        
    }
    move(1,0);
    printw("Press F2 to go admin mode  ");
    refresh();
    wmove(my_win,1,1);
    wrefresh(my_win);
    getyx(my_win,y,x);
}

void clean_count()
{
    int i, j, p, ii, jj;
    opened = 0;
    for(p=1; p<=r ;p++)
    {
        
        for(i=1; i<=c;  i++)
        {
            tab[p][i]->open = 0 ;
            tab[p][i]->flag = 0 ;
            tab[p][i]->count = 0 ;
            
        }
    }
    
    for( i = 1; i <= r; i++)
        for( j = 1; j <= c; j++)
            for( ii = i - 1; ii <= i + 1; ii++)
                if((ii >= 1) && (ii <= r))
                    for( jj = j - 1; jj <= j + 1; jj++)
                        if((jj >= 1) && (jj <= c) && tab[ii][jj]->mine)
                            tab[i][j]->count++;
}

void you_won()
{
    move(r/2 +1,c+6);
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_WHITE);
    attron(COLOR_PAIR(1));
    printw("***YOU WON***");
    attroff(COLOR_PAIR(1));
    refresh();
    move(r+6,2);
    refresh();
}
