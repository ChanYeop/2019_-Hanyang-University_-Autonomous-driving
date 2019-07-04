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

#define LS_SIZE 128 //line scan ī�޶� ��ü ũ��(1�� ���� 128, 2�� ���� 256)
#define WINDOW_SIZE 5 //�̺��ϴ� window�� ũ��
#define FILTER_SIZE 9 //��� ���� window�� ũ��

#define DIFF_ARR_SIZE 		LS_SIZE - (WINDOW_SIZE - 1) //�̺е� array ũ�� : ī�޶� ��ü ũ�� - (window size - 1)
#define FILTERED_ARR_SIZE 	DIFF_ARR_SIZE - (FILTER_SIZE - 1) //���͸��� array ũ�� : �̺� array ũ�� - (filter window size - 1)
#define TWO_DIFF_ARR_SIZE 	FILTERED_ARR_SIZE - (WINDOW_SIZE - 1) //���� �̺е� array ũ�� : ���͸��� array ũ�� - (window size - 1)

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
