/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "InfineonRacer.h"
#include "Basic.h"
#define ABS(x) ( ((x)<0)?-(x):(x) ) //절대값

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

float32 diffArr[DIFF_ARR_SIZE]; //adcResult를 미분한 후 DIFF_THRESHOLD 안넘는 값은 0으로 만들어 저장하는 array
float32 filteredArr[FILTERED_ARR_SIZE]; //diffArr를 평균 필터로 필터링한 값 저장하는 array
float32 twoDiffArr[TWO_DIFF_ARR_SIZE]; //filteredArr를 미분한 값 저장하는 array
boolean conArr[TWO_DIFF_ARR_SIZE]; //twoDiffArr값 중 threshold(LINE_THRESHOLD)값 이상인 값은 1 아니면 0(consider array)
int right_line = 128; //right_line의 시작 index 저장
int left_line = 127; //left_line의 시작 index 저장
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
void makeDiff(int lr); //window size로 미분하여 임계값(DIFF_THRESHOLD)으로 clipping한 값 diffArr에 저장
void averageFilter(void); //filter window size로 필터링한 값 filteredArr에 저장
void makeTwoDiff(void); //window size로 이중 미분한 값 twoDiffArr에 저장
void considerLine(void); //twoDiffArr에서 line으로 의심되는 index에는 1삽입 아니면 0
void right_index_number(void); //filter를 거친 데이터에서 line가능성 존재하는 index 반환
void left_index_number(void);

void checking_crosswalk(void); // "school zone" or "racing zone" 판단

void final_right_line(void); // 우측에서 중심에 제일 가까운 데이터 index룰 right_line 변수에 저장
void final_left_line(void); // 좌측에서 중심에 제일 가까운 데이터 index룰 left_line 변수에 저장
/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

//window size로 미분한 값 diffArr에 저장
//+ 임계값(DIFF_THRESHOLD) 안넘는 거 0으로 해줌
void makeDiff(int lr)
{
   for(int i = 0; i < DIFF_ARR_SIZE; i++)
   {
      diffArr[i] = ((float32)IR_LineScan.adcResult[lr][i + (WINDOW_SIZE - 1)] - (float32)IR_LineScan.adcResult[lr][i]) / WINDOW_SIZE;
      diffArr[i] = (ABS(diffArr[i]) < DIFF_THRESHOLD) ? 0 : diffArr[i];
   }
}

//평균 필터. filter window size로 필터링한 값 filteredArr에 저장
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

//window size로 이중 미분한 값 twoDiffArr에 저장
void makeTwoDiff(void)
{
   for(int i = 0; i < TWO_DIFF_ARR_SIZE; i++)
   {
      twoDiffArr[i] = (filteredArr[i + (WINDOW_SIZE - 1)] - filteredArr[i]) / WINDOW_SIZE;
   }
}

//twoDiffArr에서 line으로 의심되는 index에는 1삽입 아니면 0
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
   /* IR_LineScan.adcResult 의 정보를 읽어들여서
    * IR_Ctrl.Ls0Margin, IR_Ctrl.Ls1Margin 정보를 계산한다
    */
   final_left_line(); // 좌측에서 중심에 제일 가까운 데이터 index룰 left_line 변수에 저장
   final_right_line(); // 우측에서 중심에 제일 가까운 데이터 index룰 right_line 변수에 저장
   checking_crosswalk();

   if(school_zone) // 속도 제한 구역이면 1을 반환
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
   else // 속도 제한 구역이 아니면 0을 반환
   {
      // 레이싱 구역 코드
   }
   // 위 두줄의 결과값으로 left_line 변수와 right_line 변수 값이 생성




}

void InfineonRacer_control(void){

}
