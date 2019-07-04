/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "InfineonRacer.h"
#include "Basic.h"
#define ABS(x) ( ((x)<0)?-(x):(x) ) //���밪

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
InfineonRacer_t IR_Ctrl  /**< \brief  global data */
      ={64, 64, FALSE};

float32 diffArr[DIFF_ARR_SIZE]; //adcResult�� �̺��� �� DIFF_THRESHOLD �ȳѴ� ���� 0���� ����� �����ϴ� array
float32 filteredArr[FILTERED_ARR_SIZE]; //diffArr�� ��� ���ͷ� ���͸��� �� �����ϴ� array
float32 twoDiffArr[TWO_DIFF_ARR_SIZE]; //filteredArr�� �̺��� �� �����ϴ� array
boolean conArr[TWO_DIFF_ARR_SIZE]; //twoDiffArr�� �� threshold(LINE_THRESHOLD)�� �̻��� ���� 1 �ƴϸ� 0(consider array)
int right_line = 128; //right_line�� ���� index ����
int left_line = 127; //left_line�� ���� index ����
uint32 left = 0;
uint32 right = 1;
int recently= 0;
int last = 0;
boolean school_zone = FALSE;
boolean crosswalk_ing = FALSE;
boolean check_line_left = FALSE;
boolean check_line_right = FALSE;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
void makeDiff(int lr); //window size�� �̺��Ͽ� �Ӱ谪(DIFF_THRESHOLD)���� clipping�� �� diffArr�� ����
void averageFilter(void); //filter window size�� ���͸��� �� filteredArr�� ����
void makeTwoDiff(void); //window size�� ���� �̺��� �� twoDiffArr�� ����
void considerLine(void); //twoDiffArr���� line���� �ǽɵǴ� index���� 1���� �ƴϸ� 0
void right_index_number(void); //filter�� ��ģ �����Ϳ��� line���ɼ� �����ϴ� index ��ȯ
void left_index_number(void);

void checking_crosswalk(void); // "school zone" or "racing zone" �Ǵ�

void final_right_line(void); // �������� �߽ɿ� ���� ����� ������ index�� right_line ������ ����
void final_left_line(void); // �������� �߽ɿ� ���� ����� ������ index�� left_line ������ ����
/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

//window size�� �̺��� �� diffArr�� ����
//+ �Ӱ谪(DIFF_THRESHOLD) �ȳѴ� �� 0���� ����
void makeDiff(int lr)
{
   for(int i = 0; i < DIFF_ARR_SIZE; i++)
   {
      diffArr[i] = ((float32)IR_LineScan.adcResult[lr][i + (WINDOW_SIZE - 1)] - (float32)IR_LineScan.adcResult[lr][i]) / WINDOW_SIZE;
      diffArr[i] = (ABS(diffArr[i]) < DIFF_THRESHOLD) ? 0 : diffArr[i];
   }
}

//��� ����. filter window size�� ���͸��� �� filteredArr�� ����
void averageFilter(void)
{
   int sum;
   for(int i = 0; i < FILTERED_ARR_SIZE; i++)
   {
      sum = 0;
      for(int j = 0; j < FILTER_SIZE; j++)
      {
         sum += diffArr[i + j];
      }
      filteredArr[i] = (float32)sum / FILTER_SIZE;
   }
}

//window size�� ���� �̺��� �� twoDiffArr�� ����
void makeTwoDiff(void)
{
   for(int i = 0; i < TWO_DIFF_ARR_SIZE; i++)
   {
      twoDiffArr[i] = (filteredArr[i + (WINDOW_SIZE - 1)] - filteredArr[i]) / WINDOW_SIZE;
   }
}

//twoDiffArr���� line���� �ǽɵǴ� index���� 1���� �ƴϸ� 0
void considerLine(void)
{
   for(int i = 0; i < TWO_DIFF_ARR_SIZE; i++)
   {
      if(twoDiffArr[i] > LINE_THRESHOLD)
         conArr[i] = TRUE;
      else
         conArr[i] = FALSE;
   }
}

void right_index_number(void)
{
   right_line = 128;

   for(int i = 0; i < LS_SIZE; i++)
   {
      if(IR_LineScan.line[right][i] == 4095)
      {
         right_line = i + 128;
         check_line_right = TRUE;
         check_line_left = FALSE;
         break;
      }
   }
   check_line_right = FALSE;
   check_line_left = FALSE;
}

void left_index_number(void)
{
   left_line = 127;

   for(int i = LS_SIZE-1; i >= 0; i--)
   {
      if(IR_LineScan.line[left][i] == 4095)
      {
         left_line = i;
         check_line_left = TRUE;
         check_line_right = FALSE;
         break;
      }
   }
   check_line_left = FALSE;
   check_line_right = FALSE;
}

void final_right_line(void)
{
   makeDiff(right);
   averageFilter();
   makeTwoDiff();
   considerLine();

   for(int i = 0; i < (WINDOW_SIZE - 1) + FILTER_SIZE / 2; i++)
   {
      IR_LineScan.line[right][i] = 0;
   }
   for(int i = (WINDOW_SIZE - 1) + FILTER_SIZE / 2; i < TWO_DIFF_ARR_SIZE + (WINDOW_SIZE - 1) + FILTER_SIZE / 2; i++)
   {
      IR_LineScan.line[right][i] = (conArr[i - ((WINDOW_SIZE - 1) + (FILTER_SIZE - 1) / 2)] == TRUE) ? 4095 : 0;
   }
   for(int i = TWO_DIFF_ARR_SIZE + (WINDOW_SIZE - 1) + FILTER_SIZE / 2; i < LS_SIZE; i++)
   {
      IR_LineScan.line[right][i] = 0;
   }
   right_index_number();
}

void final_left_line(void)
{
   makeDiff(left);
   averageFilter();
   makeTwoDiff();
   considerLine();

   for(int i = 0; i < (WINDOW_SIZE - 1) + FILTER_SIZE / 2; i++)
   {
      IR_LineScan.line[left][i] = 0;
   }
   for(int i = (WINDOW_SIZE - 1) + FILTER_SIZE / 2; i < TWO_DIFF_ARR_SIZE + (WINDOW_SIZE - 1) + FILTER_SIZE / 2; i++)
   {
      IR_LineScan.line[left][i] = (conArr[i - ((WINDOW_SIZE - 1) + (FILTER_SIZE - 1) / 2)] == TRUE) ? 4095 : 0;
   }
   for(int i = TWO_DIFF_ARR_SIZE + (WINDOW_SIZE - 1) + FILTER_SIZE / 2; i < LS_SIZE; i++)
   {
      IR_LineScan.line[left][i] = 0;
   }
   left_index_number();
}

void checking_crosswalk(void)
{
   recently = right_line - left_line;

   if((last != 0) && ((last - recently) >= 10) && (crosswalk_ing != TRUE)){
      if(school_zone == FALSE){
         school_zone = TRUE;
      }
      else{
         school_zone = FALSE;
      }
      crosswalk_ing = TRUE;
   }
   else if((recently - last) >= 5 && crosswalk_ing == TRUE)
        crosswalk_ing = FALSE;

   last = recently;
}

void InfineonRacer_init(void){

}

int cnt =0;
boolean left1 = FALSE;
boolean left2 = FALSE;
boolean right1 = FALSE;
boolean right2 = FALSE;


void InfineonRacer_detectLane(void){
   /* IR_LineScan.adcResult �� ������ �о�鿩��
    * IR_Ctrl.Ls0Margin, IR_Ctrl.Ls1Margin ������ ����Ѵ�
    */
   final_left_line(); // �������� �߽ɿ� ���� ����� ������ index�� left_line ������ ����
   final_right_line(); // �������� �߽ɿ� ���� ����� ������ index�� right_line ������ ����
   checking_crosswalk();

   if(school_zone) // �ӵ� ���� �����̸� 1�� ��ȯ
   {
      if(check_line_left){
         cnt++;
         left1 = TRUE;
         right1 = FALSE;
      }
      else if(check_line_right){
         cnt++;
         right1 = TRUE;
         left1 = FALSE;
      }
      else
         cnt =0;
      if(cnt >= 10){
         if(left1 == TRUE && right1 == FALSE){
            left2 = TRUE;
            right2 = FALSE;
         }
         else if(right1 == TRUE && left1 == TRUE){
            right2 = TRUE;
            left2 = FALSE;
         }
      }
   }
   else // �ӵ� ���� ������ �ƴϸ� 0�� ��ȯ
   {
      // ���̽� ���� �ڵ�
   }
   // �� ������ ��������� left_line ������ right_line ���� ���� ����




}

void InfineonRacer_control(void){

}
