#pragma once
#ifndef __GAME_H__
#define __GAME_H__

#ifdef __cplusplus
extern C
#endif


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







//enime:
void				updatePlaneInfo(BOOL, UINT);				//跟新敌机信息
void				checkPlaneInfo();							//检查敌机信息
BOOL				checkPlanePos();							//检查飞机位置，判断是否有飞机到达屏幕底部


#ifdef __cplusplus
extern C
#endif

#endif