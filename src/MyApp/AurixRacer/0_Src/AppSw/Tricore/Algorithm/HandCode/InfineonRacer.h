#ifndef INFINEONRACER_H_
#define INFINEONRACER_H_


/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include <Ifx_Types.h>
#include "Configuration.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/
#define IR_getLs0Margin()		IR_Ctrl.Ls0Margin
#define IR_getLs1Margin()		IR_Ctrl.Ls1Margin

#define LS_SIZE 128 //line scan 카메라 전체 크기(1개 쓰면 128, 2개 쓰면 256)
#define WINDOW_SIZE 5 //미분하는 window의 크기
#define FILTER_SIZE 9 //평균 필터 window의 크기

#define DIFF_ARR_SIZE 		LS_SIZE - (WINDOW_SIZE - 1) //미분된 array 크기 : 카메라 전체 크기 - (window size - 1)
#define FILTERED_ARR_SIZE 	DIFF_ARR_SIZE - (FILTER_SIZE - 1) //필터링된 array 크기 : 미분 array 크기 - (filter window size - 1)
#define TWO_DIFF_ARR_SIZE 	FILTERED_ARR_SIZE - (WINDOW_SIZE - 1) //이중 미분된 array 크기 : 필터링된 array 크기 - (window size - 1)

#define DIFF_THRESHOLD 150
#define LINE_THRESHOLD 20

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/



/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/
typedef struct{
	sint32 Ls0Margin;
	sint32 Ls1Margin;
	boolean basicTest;
}InfineonRacer_t;

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
IFX_EXTERN InfineonRacer_t IR_Ctrl;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_EXTERN void InfineonRacer_init(void);
IFX_EXTERN void InfineonRacer_detectLane(void);
IFX_EXTERN void InfineonRacer_control(void);

#endif
