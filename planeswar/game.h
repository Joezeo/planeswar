/**********************************************
				Editor:Joezeo
		  E-mail:joezeo@outlook.com

**********************************************/
#ifndef __GAME_H__
#define __GAME_H__

#include "windows.h"
#include "resource.h"
#include "assert.h"
#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"winmm.lib")

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
VOID			//初始化所有变量	
init_items(LPARAM, HWND, HBITMAP**, BULLET_LINK*, MYPLANE**, PLANE**, GAME*, HBITMAP*);

VOID			//游戏结束时释放资源
free_items(HBITMAP*, BULLET_LINK*, MYPLANE*, PLANE*);						

static VOID		//重新开始游戏，重设变量
restart_game(HWND, BULLET_LINK*, MYPLANE*, GAME*);							


//scence:
VOID			//分场景画出界面	
draw_senceUI(HDC, HBITMAP*, BULLET_LINK*, MYPLANE*, PLANE*, GAME*, HBITMAP*);	

static VOID		//画背景
draw_background(HDC, HDC, HBITMAP*, BITMAP, GAME*, HBITMAP*);					

static VOID		//画场景一：WELCOME
draw_senceI(HDC, HDC, HBITMAP*, BITMAP);										

static VOID		//画场景二：RUNING
draw_senceII(HDC, HDC, HBITMAP*, BITMAP, BULLET_LINK*, MYPLANE*, PLANE*, GAME*);

static VOID		//画场景三：OVER
draw_senceIII(HDC, HDC, HBITMAP*, BITMAP, GAME*);								


//message event:
VOID			//计时器事件	
proc_timer(HWND, BULLET_LINK*, MYPLANE*, PLANE*, GAME*, HBITMAP*);		

VOID			//鼠标左键按下事件
proc_lButtondown(HWND, LPARAM, BULLET_LINK*, MYPLANE*, GAME*, HBITMAP*);

static BOOL		//检测开始按钮是否被按下
check_button_start(POINT, HBITMAP*);					

static BOOL		//检测重新开始按钮是否被按下
check_button_restart(POINT, HBITMAP*);					

static BOOL		//检测退出按钮是否被按下
check_button_exit(POINT, HBITMAP*);						

static BOOL		//检测暂停/开始按钮是否被按下
check_button_pause_play(POINT, HBITMAP*);				

static BOOL		//检测音乐开关按钮是否被按下
check_button_sound_on_off(POINT, HBITMAP*);				


//enime:
VOID			//跟新敌机信息
update_plane_info(BOOL, UINT, PLANE*, GAME*);	

static VOID		//检查敌机信息
check_plane_info(PLANE*, GAME*);				

static VOID		//检查飞机位置，判断是否有飞机到达屏幕底部，如是，update_plane_info
check_plane_pos(PLANE*, GAME*);					

static VOID		//释放planes资源
free_planes(PLANE*);							


//sound:
VOID			//播放背景音乐
play_backsound(GAME*);							


//control myplane:
VOID			//移动己方飞机，用上下左右，移动
move_myplane(WPARAM, HWND, MYPLANE*, GAME*);	

static BOOL		//检查敌机和玩家飞机是否相撞
check_myplane_crash(MYPLANE*, PLANE*, GAME*);	

static VOID		//释放myplane资源
free_myplane(MYPLANE*);							


//bullet:
static VOID		//生成子弹
create_bullet(BULLET_LINK*, MYPLANE*, HBITMAP*);

static VOID		//画出子弹
draw_bullet(BULLET_LINK*, HDC, HDC);			

static VOID		//移动子弹
move_bullet(BULLET_LINK*);						

static BULLET*	//检查子弹是否出界
check_bullet_pos(BULLET_LINK*);					

static BULLET*	//检查子弹是否击中敌机
check_bullet_info(BULLET_LINK*, PLANE*, GAME*);	

static VOID		//释放子弹资源	
free_bullet(BULLET_LINK*, BULLET*);				

static VOID		//游戏重新开始时，释放所有子弹资源
free_allBullet(BULLET_LINK*);					


#ifdef __cplusplus
}
#endif

#endif // __GAME_H__
