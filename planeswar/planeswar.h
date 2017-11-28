#pragma once
#include <assert.h>
#include <mmsystem.h>
#include <time.h>
#include "resource.h"

#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"winmm.lib")

#define MAX_LOADSTRING	100

//定义窗口尺寸
#define WNDWIDTH		380
#define WNDHEIGHT		550

#define BMPCOUNT		39		//bmp数量
#define TIMER			1		//timer ID
#define MAXPLANENUM		100		//最大可容纳飞机数量

#define WM_START (WM_USER + 1)	//自定义开始事件
#define WM_PLAYBACKSOUND (WM_USER + 2)	//自定义播放背景音乐事件

//声明结构体
typedef enum {
	WELCOME,
	RUNING,
	OVER,
}GAMESTATUS;//游戏所处状态

typedef enum {
	BIG = 0,
	MIDDLE = 1,
	SMALL = 2,
}PLANETYPE;

typedef struct {
	GAMESTATUS		g_status;
	UINT			g_score;
	UINT			g_plNum;
	BOOL			g_bigAdd;
	UINT			g_pause_status;
	UINT			g_sound_status;
	UINT			g_backGround_status;
	UINT			g_backGround_couter;//用于调节背景动画播放速度
	UINT			g_bullet_counter;	//辅助调节子弹生成速度
}GAME;

typedef struct {
	PLANETYPE		p_type;
	SIZE			p_size;
	POINT			p_point;
	UINT			p_speed;
	UINT			p_maxHP;
	UINT			p_hitCount;
}PLANE;

typedef struct {
	UINT			t_speed;
	UINT			t_maxHP;
	SIZE			t_size;
}TYPEINFO;

typedef struct {
	HBITMAP hbmpPlane;
	UINT	moveSpeed;
	UINT	fireSpeed;
	POINT	postion;
	SIZE	size;
}MYPLANE;

typedef struct bullet{
	HBITMAP hbmp;
	UINT	move_speed;
	POINT	bullet_pos;
	SIZE	bullet_size;
	struct bullet *next;
	struct bullet *pre;
}BULLET;

typedef struct {
	BULLET *head;
	BULLET *tail;
}BULLET_LINK;

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HBITMAP hbBmp[BMPCOUNT];						// 储存位图句柄的数组
int uiBmpName[BMPCOUNT] = {						// 储存位图ID的数组
	IDB_BG1,		//0
	IDB_BIG,		//1
	IDB_EXIT,		//2
	IDB_GAMEOVER,	//3
	IDB_LOGO,		//4
	IDB_MIDDLE,		//5
	IDB_RESTART,	//6
	IDB_SMALL,		//7
	IDB_START,		//8
	IDB_PLAYORPAUSE,//9
	IDB_MYPLANE,	//10
	IDB_MUSICON,	//11
	IDB_MUSICOFF,	//12
	IDB_BG1,		//13
	IDB_BG1,		//14
	IDB_BG2,		//15
	IDB_BG3,		//16
	IDB_BG4,		//17
	IDB_BG5,		//18
	IDB_BG6,		//19
	IDB_BG7,		//20
	IDB_BG8,		//21
	IDB_BG9,		//22
	IDB_BG10,		//23
	IDB_BG11,		//24
	IDB_BG12,		//25
	IDB_BG13,		//26
	IDB_BG14,		//27
	IDB_BG15,		//28
	IDB_BG16,		//29
	IDB_BG17,		//30
	IDB_BG18,		//31
	IDB_BG19,		//32
	IDB_BG20,		//33
	IDB_BG21,		//34
	IDB_BG22,		//35
	IDB_BG23,		//36
	IDB_BG24,		//37
	IDB_BULLET,		//38
};
GAME s_game;
TYPEINFO typeinfo[3];//big_info, middle_info, small_info;
PLANE planes[MAXPLANENUM];
MYPLANE	myplane;

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

//init/free:
VOID				initgame(LPARAM, HWND, HBITMAP**, BULLET_LINK*);//初始化全局变量
VOID				freeItem(HBITMAP*);							//游戏结束时释放资源

//scence:
VOID				drawSenceUI(HDC, HBITMAP*, BULLET_LINK*);	//分场景画出界面
VOID				drawbackground(HDC, HDC, HBITMAP*, BITMAP);			//画背景
VOID				drawSenceI(HDC, HDC, HBITMAP*, BITMAP);		//画场景一：WELCOME
VOID				drawSenceII(HDC, HDC, HBITMAP*, BITMAP, BULLET_LINK*);//画场景二：RUNING
VOID				drawSenceIII(HDC, HDC, HBITMAP*, BITMAP);	//画场景三：OVER

//message event:
VOID				lButtondownProc(HWND, LPARAM);				//鼠标左键按下事件
BOOL				checkbutton_start(POINT);					//检测开始按钮是否被按下
BOOL				checkbutton_restart(POINT);					//检测重新开始按钮是否被按下
BOOL				checkbutton_exit(POINT);					//检测退出按钮是否被按下
BOOL				checkbutton_pause_play(POINT);				//检测暂停/开始按钮是否被按下
BOOL				checkbutton_sound_on_off(POINT);			//检测音乐开关按钮是否被按下
VOID				timerProc(HWND, BULLET_LINK*);				//计时器事件

//enime:
VOID				updatePlaneInfo(BOOL, UINT);				//跟新敌机信息
VOID				checkPlaneInfo();							//检查敌机信息
BOOL				checkPlanePos();							//检查飞机位置，判断是否有飞机到达屏幕底部

//sound:
VOID				playBackSound();							//播放背景音乐

//control myplane:
VOID				moveMyplane(WPARAM, HWND);					//移动己方飞机，用上下左右，移动

//bullet:
VOID				createBullet(BULLET_LINK*);					//生成子弹
VOID				drawBullet(BULLET_LINK*, HDC, HDC);			//画出子弹
VOID				moveBullet(BULLET_LINK*);					//移动子弹
BULLET*				checkBulletPos(BULLET_LINK*);				//检查子弹是否出界
BULLET*				checkBulletInfo(BULLET_LINK*);				//检查子弹是否击中敌机
VOID				freeBullet(BULLET_LINK*, BULLET*);			//释放子弹资源


