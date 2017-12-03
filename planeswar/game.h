#pragma once
#include "windows.h"
#include "resource.h"
#include "assert.h"
#include <mmsystem.h>
#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"winmm.lib")
#ifndef __GAME_H__
#define __GAME_H__

#ifdef __cplusplus
extern "C" {
#endif

//定义窗口尺寸
#define WNDWIDTH		380
#define WNDHEIGHT		550

#define BMPCOUNT		39		//bmp数量
#define TIMER			1		//timer ID
#define MAXPLANENUM		100		//最大可容纳飞机数量


#define WM_START (WM_USER + 1)			//自定义开始事件
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
	UINT	hitCounter;
	UINT	maxHp;
}MYPLANE;

typedef struct bullet {
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



//init/free:
VOID		init_items(LPARAM, HWND, HBITMAP**, BULLET_LINK*, MYPLANE**, PLANE**, GAME*, HBITMAP*);//初始化全局变量
VOID		free_items(HBITMAP*, BULLET_LINK*, MYPLANE*, PLANE*);						//游戏结束时释放资源
VOID		restart_game(HWND, BULLET_LINK*, MYPLANE*, GAME*);							//重新开始游戏，重设变量

//scence:
VOID		draw_senceUI(HDC, HBITMAP*, BULLET_LINK*, MYPLANE*, PLANE*, GAME*, HBITMAP*);	//分场景画出界面
VOID		draw_background(HDC, HDC, HBITMAP*, BITMAP, GAME*, HBITMAP*);					//画背景
VOID		draw_senceI(HDC, HDC, HBITMAP*, BITMAP);										//画场景一：WELCOME
VOID		draw_senceII(HDC, HDC, HBITMAP*, BITMAP, BULLET_LINK*, MYPLANE*, PLANE*, GAME*);//画场景二：RUNING
VOID		draw_senceIII(HDC, HDC, HBITMAP*, BITMAP, GAME*);								//画场景三：OVER

//message event:
VOID		proc_lButtondown(HWND, LPARAM, BULLET_LINK*, MYPLANE*, GAME*, HBITMAP*);//鼠标左键按下事件
BOOL		check_button_start(POINT, HBITMAP*);					//检测开始按钮是否被按下
BOOL		check_button_restart(POINT, HBITMAP*);					//检测重新开始按钮是否被按下
BOOL		check_button_exit(POINT, HBITMAP*);						//检测退出按钮是否被按下
BOOL		check_button_pause_play(POINT, HBITMAP*);				//检测暂停/开始按钮是否被按下
BOOL		check_button_sound_on_off(POINT, HBITMAP*);				//检测音乐开关按钮是否被按下
VOID		proc_timer(HWND, BULLET_LINK*, MYPLANE*, PLANE*, GAME*, HBITMAP*);//计时器事件

//enime:
VOID		update_plane_info(BOOL, UINT, PLANE*, GAME*);	//跟新敌机信息
VOID		check_plane_info(PLANE*, GAME*);				//检查敌机信息
VOID		check_plane_pos(PLANE*, GAME*);					//检查飞机位置，判断是否有飞机到达屏幕底部，如是，update_plane_info
VOID		free_planes(PLANE*);							//释放planes资源

//sound:
VOID		play_backsound(GAME*);							//播放背景音乐

//control myplane:
VOID		move_myplane(WPARAM, HWND, MYPLANE*, GAME*);	//移动己方飞机，用上下左右，移动
BOOL		check_myplane_crash(MYPLANE*, PLANE*, GAME*);	//检查敌机和玩家飞机是否相撞
VOID		free_myplane(MYPLANE*);							//释放myplane资源

//bullet:
VOID		create_bullet(BULLET_LINK*, MYPLANE*, HBITMAP*);//生成子弹
VOID		draw_bullet(BULLET_LINK*, HDC, HDC);			//画出子弹
VOID		move_bullet(BULLET_LINK*);						//移动子弹
BULLET*		check_bullet_pos(BULLET_LINK*);					//检查子弹是否出界
BULLET*		check_bullet_info(BULLET_LINK*, PLANE*, GAME*);	//检查子弹是否击中敌机
VOID		free_bullet(BULLET_LINK*, BULLET*);				//释放子弹资源
VOID		free_allBullet(BULLET_LINK*);					//游戏重新开始时，释放所有子弹资源
#ifdef __cplusplus
}
#endif

#endif // __GAME_H__
