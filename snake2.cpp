#include<windows.h>
#include<string.h>
#include<time.h>
#define TOP 0
#define LEFT 0
#define WIDTH 20
#define HEIGHT 20
#define DIR_UP 0
#define DIR_DOWN 1
#define DIR_LEFT 2
#define DIR_RIGHT 3
#define STEP 20
#define TIMER_ID 100
#define NOTURN 0
#define VTURN 1
#define HTURN 2

typedef struct BodySegTag{
        int top;
        int left;
        int width;
        int height;
        int direction;
        BodySegTag *prev;
        BodySegTag *next;
}BodySeg;

BodySeg *snake=NULL;
BodySeg food;
int foodFlag = 0;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM InitApp(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
void InitSnake(int);
void drawSnake(HDC, HWND);
void drawFood(HDC);
void move(int);
void addBody(int);
void setFood(int, int , int, int);
int sensorContact(BodySeg*, BodySeg*);
void BMPToMemoryDC(LPCTSTR,HDC*);
void myBitblt(HDC, int, int,int, int, HDC, int, int, DWORD, int);
TCHAR szClassName[] = TEXT("snake");
HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow)
{
    MSG msg;
    BOOL bRet;
    hInst = hCurInst;
    if (!InitApp(hCurInst))
       return FALSE;
    if (!InitInstance(hCurInst, nCmdShow))
       return FALSE;
    
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0){
          if (bRet == -1) 
             break;
          else {
               TranslateMessage(&msg);
               DispatchMessage(&msg);
          }                    
          
    }
  
    return (int)msg.wParam;
}

ATOM InitApp(HINSTANCE hInst)
{
     WNDCLASSEX wc;
     wc.style = CS_VREDRAW | CS_HREDRAW;
     
     wc.cbSize = sizeof(WNDCLASSEX);
     wc.cbClsExtra = 0;
     wc.cbWndExtra = 0;
     
     wc.lpszClassName = szClassName;
     wc.lpszMenuName = NULL;
     wc.lpfnWndProc = WndProc;
     
     wc.hInstance = hInst;
     wc.hIcon = (HICON)LoadImage(NULL,
                MAKEINTRESOURCE(IDI_APPLICATION),
                IMAGE_ICON,
                0,
                0,
                LR_DEFAULTSIZE | LR_SHARED);
     wc.hIconSm = NULL;
     wc.hCursor = (HCURSOR)LoadImage(NULL,
                MAKEINTRESOURCE(IDC_ARROW),
                IMAGE_CURSOR,
                0,
                0,
                LR_DEFAULTSIZE | LR_SHARED);
     wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
     
     return (RegisterClassEx(&wc));
}

BOOL InitInstance(HINSTANCE hInst, int nCmdShow)
{
     HWND hWnd;
     
     hWnd = CreateWindow(szClassName,
            "Â²©ö³g­¹³D",
            WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            500,
            500,
            NULL,
            NULL,
            hInst,
            NULL);
     if (!hWnd)
        return FALSE;
     ShowWindow(hWnd, nCmdShow);
     UpdateWindow(hWnd);
     return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
        HDC hdc;
        PAINTSTRUCT ps;
        static int direction = DIR_DOWN;
        switch (msg) {
             case WM_CREATE:
                  InitSnake(direction);
                  srand((UINT)time(NULL));
                  SetTimer(hWnd, TIMER_ID, 100, NULL);
                  setFood(HEIGHT, WIDTH, (rand() % 20) * 20, (rand() % 20) * 20);
                  break;
             case WM_PAINT:
                  hdc = BeginPaint(hWnd, &ps);
                  drawSnake(hdc, hWnd);
                  drawFood (hdc);
                  break;
          
             case WM_TIMER: 
                  
                  EndPaint(hWnd, &ps);
                  if (wp != TIMER_ID)
                     return (DefWindowProc(hWnd, msg, wp, lp));
                  if (sensorContact(snake,&food))
                  {
                     setFood(HEIGHT, WIDTH, (rand() % 20) * 20, (rand() % 20) * 20);
                     addBody(direction);
                  }
                  move(direction);
                  InvalidateRect( hWnd ,NULL, TRUE);
                  InvalidateRect( hWnd ,NULL, FALSE);
                  break;
             case WM_KEYDOWN:
                  switch (wp) {
                         case VK_UP:
                              if (direction != DIR_DOWN)
                                 direction = DIR_UP;
                              break;
                         case VK_DOWN:
                              if (direction != DIR_UP)
                                 direction = DIR_DOWN;
                              break;
                         case VK_LEFT:
                              if (direction != DIR_RIGHT)
                                 direction = DIR_LEFT;
                              break;
                         case VK_RIGHT:
                              if (direction != DIR_LEFT)
                                 direction = DIR_RIGHT;
                  }
                  break;
             case WM_DESTROY:
                  PostQuitMessage(0);
                  KillTimer(hWnd, TIMER_ID);
                  free(snake);
                  break;
             default:
                  return (DefWindowProc(hWnd, msg, wp, lp));
        }
        return 0;
}

void InitSnake(int direction)
{
     addBody(direction);
     addBody(direction);
     addBody(direction);
     return ;
}
void drawSnake(HDC hdc, HWND hWnd)
{
     RECT rc ;
     GetClipBox(hdc,&rc);
     BodySeg *bodyPtr = snake;
     HDC mdc = CreateCompatibleDC(hdc);
     HDC bufDC = GetDC(hWnd);
     HBITMAP hBmp = CreateCompatibleBitmap(bufDC, rc.right, rc.bottom);
     SelectObject(bufDC, hBmp);
     char pathFirst[6];
     while (bodyPtr != NULL){
           if (bodyPtr->prev == NULL)
              strcpy(pathFirst,"HEAD_");
           else if(bodyPtr->next == NULL)
              strcpy(pathFirst,"TAIL_"); 
           else
              strcpy(pathFirst,"BODY_");   
           switch (bodyPtr->direction){
                  case DIR_UP:
                       BMPToMemoryDC(TEXT(strcat(pathFirst,"V")), &mdc);
                       myBitblt(bufDC, bodyPtr->left, bodyPtr->top, WIDTH, HEIGHT, mdc, 0, 0, SRCCOPY,NOTURN);
                       break;
                  case DIR_DOWN:
                       BMPToMemoryDC(TEXT(strcat(pathFirst,"V")), &mdc);
                       myBitblt(bufDC, bodyPtr->left, bodyPtr->top, WIDTH, HEIGHT, mdc, 0, 0, SRCCOPY,VTURN);
                       break;
                  case DIR_LEFT:
                       BMPToMemoryDC(TEXT(strcat(pathFirst,"H")), &mdc);
                       myBitblt(bufDC, bodyPtr->left, bodyPtr->top, WIDTH, HEIGHT, mdc, 0, 0, SRCCOPY,HTURN);
                       break;
                  case DIR_RIGHT:
                       BMPToMemoryDC(TEXT(strcat(pathFirst,"H")), &mdc); 
                       myBitblt(bufDC, bodyPtr->left, bodyPtr->top, WIDTH, HEIGHT, mdc, 0, 0, SRCCOPY,NOTURN);                                                
           }   
           bodyPtr = bodyPtr->next;
     }
     SetBkColor(bufDC,RGB(255,255,255));
     BitBlt(hdc, 0, 0, rc.right, rc.bottom, bufDC, 0, 0, SRCCOPY);
     DeleteDC(bufDC); 
     DeleteDC(mdc);                            
}

void move(int direction)
{
     BodySeg *bodyPtr = snake;
      
      while (bodyPtr->next != NULL){
            bodyPtr = bodyPtr->next;       
      }      
       while (bodyPtr != NULL){
             if (bodyPtr->prev != NULL)
                bodyPtr->direction = bodyPtr->prev->direction;
             else
                bodyPtr->direction = direction ;
            bodyPtr = bodyPtr->prev;       
      }
      
      bodyPtr = snake;
             
     while (bodyPtr != NULL){
           switch (bodyPtr->direction) {
                  case  DIR_UP:
                        bodyPtr->top -= STEP;
                        break;
                  case  DIR_DOWN:
                        bodyPtr->top += STEP;
                        break;
                  case  DIR_LEFT:
                        bodyPtr->left -= STEP;
                        break;
                  case  DIR_RIGHT:
                        bodyPtr->left += STEP;
                        break;   
           }  
           bodyPtr = bodyPtr->next;                  
     }
     
}

void addBody(int direction)
{
     BodySeg *newSeg;
     newSeg = (BodySeg *)malloc(sizeof(BodySeg));
     newSeg->width = WIDTH;
     newSeg->height = HEIGHT;
     newSeg->prev = NULL;
     newSeg->direction = direction; 
     newSeg->next = snake;
     if (snake != NULL){
        snake->prev = newSeg;       
        switch (direction) {
               case DIR_UP:
                    newSeg->top = newSeg->next->top - HEIGHT;
                    newSeg->left =newSeg->next->left;
                    break;
               case DIR_DOWN:
                    newSeg->top = newSeg->next->top + HEIGHT;
                    newSeg->left = newSeg->next->left;
                    break;
               case DIR_LEFT:
                    newSeg->top = newSeg->next->top;
                    newSeg->left = newSeg->next->left - WIDTH;
                    break;
               case DIR_RIGHT:
                    newSeg->top = newSeg->next->top;
                    newSeg->left = newSeg->next->left + WIDTH;
        }    
     } else {
        newSeg->top = TOP;
        newSeg->left = LEFT;
     }
     snake = newSeg; 
     return; 
}
void setFood(int width, int height, int left, int top)
{
     food.width = width;
     food.height = height;
     food.left = left;
     food.top = top; 
     foodFlag = 1;
     return; 
}

void drawFood(HDC hdc)
{
     HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));
     SelectObject(hdc,hBrush);
             Rectangle(hdc,
             food.left,
             food.top, 
             food.left + food.width,
             food.top + food.height);  
     DeleteObject(hBrush);
}

int sensorContact(BodySeg* obj1, BodySeg* obj2)
{
    switch (obj1->direction) {
           case DIR_UP:
                if (obj1->left == obj2->left && obj1->top <= obj2->top + obj2->height)
                   return 1;             
                break;
           case DIR_DOWN:
                if (obj1->left == obj2->left && obj1->top + obj1->height >= obj2->top)
                   return 2;   
                break;
           case DIR_LEFT:
                if (obj1->top == obj2->top && obj1->left <= obj2->left + obj2->width)
                   return 3;   
                break;
           case DIR_RIGHT:
                if (obj1->top == obj2->top  && obj1->left + obj1->width >= obj2->left)
                   return 4;   
                break;     
    }
    return 0;
}

void BMPToMemoryDC(LPCTSTR path,HDC* mdc)
{
    HBITMAP hBmp = (HBITMAP)LoadImage(hInst, path, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
    SelectObject(*mdc, hBmp);
    DeleteObject(hBmp);
}

void myBitblt(HDC hdc, int dest_x, int dest_y, int w, int h, HDC src_mdc, int src_x, int src_y, DWORD mode, int turn)
{
     
     HDC dest_mdc = CreateCompatibleDC(hdc);
     HBITMAP hBmp;
     hBmp = CreateCompatibleBitmap(src_mdc, w, h);
     SelectObject(dest_mdc, hBmp);
     switch (turn){
        case NOTURN:
             for (int Y_POS =0; Y_POS < h;Y_POS++)
                 for (int X_POS =0; X_POS < w;X_POS++)
                     SetPixel(dest_mdc, X_POS, Y_POS, GetPixel(src_mdc, X_POS, Y_POS));
             break;
        case VTURN:
             for (int Y_POS =0; Y_POS < h; Y_POS++)
                 for (int X_POS =0; X_POS < w; X_POS++)
                     SetPixel(dest_mdc, X_POS, Y_POS, GetPixel(src_mdc, X_POS, h-Y_POS-1));
             break;
        case HTURN:
             for (int Y_POS = 0; Y_POS < h; Y_POS++)
                 for (int X_POS = 0; X_POS < w; X_POS++)
                     SetPixel(dest_mdc, X_POS, Y_POS, GetPixel(src_mdc, w-X_POS-1, Y_POS));            
     }
     BitBlt(hdc, dest_x, dest_y, w, h, dest_mdc, src_x, src_y, mode);
     DeleteDC(dest_mdc);
     DeleteObject(hBmp);
}





