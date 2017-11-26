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

#define BMPCOUNT		13		//bmp数量
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
	POINT	postion;
	SIZE	size;
}MYPLANE;

typedef struct {
	HBITMAP hbmp;

}BULLET;

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HBITMAP hbBmp[BMPCOUNT];						// 储存位图句柄的数组
int uiBmpName[BMPCOUNT] = {						// 储存位图ID的数组
	IDB_BackColor,
	IDB_BIG,
	IDB_EXIT,
	IDB_GAMEOVER,
	IDB_LOGO,
	IDB_MIDDLE,
	IDB_RESTART,
	IDB_SMALL,
	IDB_START,
	IDB_PLAYORPAUSE,
	IDB_MYPLANE,
	IDB_MUSICON,
	IDB_MUSICOFF,
};
GAME s_game;
TYPEINFO typeinfo[3];//big_info, middle_info, small_info;
PLANE planes[MAXPLANENUM];
MYPLANE	myplane;

// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

//init:
VOID				initgame(LPARAM, HWND);						//初始化全局变量

//scence:
VOID				drawSenceUI(HDC);							//分场景画出界面
VOID				drawbackground(HDC, HDC, HBITMAP*);			//画背景
VOID				drawSenceI(HDC, HDC, HBITMAP*, BITMAP);		//画场景一：WELCOME
VOID				drawSenceII(HDC, HDC, HBITMAP*, BITMAP);	//画场景二：RUNING
VOID				drawSenceIII(HDC, HDC, HBITMAP*, BITMAP);	//画场景三：OVER

//message event:
VOID				lButtondownProc(HWND, LPARAM, UINT*);		//鼠标左键按下事件
VOID				buttondown_hit(POINT);						//鼠标点击攻击
BOOL				checkbutton_start(POINT);					//检测开始按钮是否被按下
BOOL				checkbutton_restart(POINT);					//检测重新开始按钮是否被按下
BOOL				checkbutton_exit(POINT);					//检测退出按钮是否被按下
BOOL				checkbutton_pause_play(POINT);				//检测暂停按钮是否被按下
VOID				timerProc(HWND);							//计时器事件

//enime:
VOID				updatePlaneInfo(BOOL, UINT);				//跟新敌机信息
VOID				checkPlaneInfo();							//检查敌机信息
BOOL				checkPlanePos();							//检查飞机位置，判断是否有飞机到达屏幕底部

//sound
VOID				playBackSound();							//播放背景音乐

//control myplane
VOID				moveMyplane(WPARAM, HWND);			//移动己方飞机，用上下左右，移动


