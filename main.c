#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define programSwitch 1

//notice
//防守方變數安排 捕手-投手-一壘-二壘-三壘-游擊手-右外野-中外野-左外野 

//移動游標副程式 
void gotoxy(int xpos, int ypos)
{
  COORD scrn;
  HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
  scrn.X = xpos; scrn.Y = ypos;
  SetConsoleCursorPosition(hOuput,scrn); 
}

//---------------------------------struct code------------------------------------
struct player{
 //球員管理基本資料 
 float basNum, salary, year, attack, defend; //編號, 薪水, 球齡, 打擊率, 防守率
 //球賽及球對後續補增資料
 float hits, faceCount, playCount, losePoint; //安打數, 面對投手, 出戰局數, 自責失分(投手) 
 char name[20], defendLoc[20];      //姓名, 防守位
};

//define score struct
struct game{
 float year, date; //年份 日期 
 float scoreTeamA[12], teamANum, teamAHits, teamAPlayerDL[9], teamAPlayerAS[9], teamAPlayCount, teamALosePoint, teamAhits[9], teamAFaceCount[9]; //teamA 的11局分數 球隊編號 總打擊數 防手編號位置 攻擊編號 個打者安打數 面對投手數 出戰局數 自責失分 
 float scoreTeamB[12], teamBNum, teamBHits, teamBPlayerDL[9], teamBPlayerAS[9], teamBPlayCount, teamBLosePoint, teamBhits[9], teamBFaceCount[9]; //teamB 的11局分數 球隊編號 總打擊數 防手編號位置 攻擊編號 個打者安打數 面對投手數 出戰局數 自責失分 
 char  teamAName[20], teamBName[20];   //兩隊名字
};

struct team{
 char name[20], coach[20];      //姓名
 int teamNum, playerNum;  //紀錄總共有幾對 ,及每個球隊的球員數 
 struct player pry[20];   //球隊球員  
 char password[20];  //球隊密碼
 float ticketPrize, groundPrize, fieldCapacity, salaryDis[5]; //入場門票 主場費用 球場容納人數 分段薪資 
 float coachSalary, coachYear; //教練薪水, 資歷 
};

//define a global struct
struct team basTeam[100];
int teamNum=0, playerNum=0, pryTeamNum=0;

struct game basGame[100];
int gameNum=0;

//-------------------------------------file IO code-------------------------------

//save the new team & new players
void saveTeam(){
 FILE *fSave = fopen("player.dat", "w");
 int i,j; 

 //球隊密碼加密
 for(j=0;j<teamNum;j++){ 
  for(i=0;i<20;i++){
   switch(basTeam[j].password[i]){ 
    case 48 ... 57:
    case 65 ... 90:
    case 97 ... 122:
     basTeam[j].password[i]++;
     break;
   }
  }
 }
 
 //key the number of teams
 fprintf(fSave, "%d\n", teamNum);
 for(i=0;i<teamNum;i++){
  fprintf(fSave, "%s %d %s %s %.0f %.0f\n",
   basTeam[i].name, basTeam[i].playerNum, basTeam[i].password, 
   basTeam[i].coach, basTeam[i].coachSalary, basTeam[i].coachYear);
  fprintf(fSave, "%.0f %.0f %.0f %.0f %.0f %.0f %.0f %.0f\n",
   basTeam[i].salaryDis[0], basTeam[i].salaryDis[1], basTeam[i].salaryDis[2], basTeam[i].salaryDis[3], 
   basTeam[i].salaryDis[4], basTeam[i].ticketPrize, basTeam[i].groundPrize, basTeam[i].fieldCapacity);
  for(j=0;j<basTeam[i].playerNum;j++){
   fprintf(fSave, "%s %.0f %.0f %.0f %s %.2f %.2f %.0f %.0f %.0f %.0f\n", basTeam[i].pry[j].name, 
    basTeam[i].pry[j].basNum, basTeam[i].pry[j].salary, basTeam[i].pry[j].year,
    basTeam[i].pry[j].defendLoc, basTeam[i].pry[j].defend, basTeam[i].pry[j].attack,
    basTeam[i].pry[j].hits, basTeam[i].pry[j].faceCount, 
    basTeam[i].pry[j].playCount, basTeam[i].pry[j].losePoint);
  }
 }
 fclose(fSave);    
}

//read player and their team in player.dat
void readTeam(){
 FILE *fRead = fopen("player.dat", "r");
 int i, j;
 fscanf(fRead, "%d", &teamNum);
 for(i=0;i<teamNum;i++){
  fscanf(fRead, "%s %d %s %s %f %f\n", 
   &basTeam[i].name, &basTeam[i].playerNum, &basTeam[i].password, 
   &basTeam[i].coach, &basTeam[i].coachSalary, &basTeam[i].coachYear);
  fscanf(fRead, "%f %f %f %f %f %f %f %f\n",
   &basTeam[i].salaryDis[0], &basTeam[i].salaryDis[1], &basTeam[i].salaryDis[2], &basTeam[i].salaryDis[3], 
   &basTeam[i].salaryDis[4], &basTeam[i].ticketPrize, &basTeam[i].groundPrize, &basTeam[i].fieldCapacity);
  for(j=0;j<basTeam[i].playerNum;j++){
   //get the player data from player.dat
   fscanf(fRead, "%s %f %f %f %s %f %f %f %f %f %f", &basTeam[i].pry[j].name, 
    &basTeam[i].pry[j].basNum, &basTeam[i].pry[j].salary, &basTeam[i].pry[j].year,
    &basTeam[i].pry[j].defendLoc, &basTeam[i].pry[j].defend, 
    &basTeam[i].pry[j].attack, &basTeam[i].pry[j].hits, &basTeam[i].pry[j].faceCount, 
    &basTeam[i].pry[j].playCount, &basTeam[i].pry[j].losePoint);
   //if player's basNum =0 then reprsent deleting data don't read it
   if(basTeam[i].pry[j].basNum==0){j--;basTeam[i].playerNum--;}
  }
  if(basTeam[i].playerNum == -1){i--;teamNum--;}
 }

 //球隊密碼解密 
 for(j=0;j<teamNum;j++){ 
  for(i=0;i<20;i++){
   switch(basTeam[j].password[i]){ 
    case 48 ... 57:
    case 65 ... 90:
    case 97 ... 122:
     basTeam[j].password[i]--;
     break;
   }
  }
 }
 
 fclose(fRead);    
}

//save two teams score totally
void writeScore(){
 FILE *fSave = fopen("gameScore.dat", "w");
 int i ,j;
 saveTeam();
 readTeam();
 //write how many games in the ram
 fprintf(fSave, "%d\n", gameNum);
 for(i=0;i<=gameNum;i++){
  fprintf(fSave, "%.0f %.0f %.0f %s %.0f %.0f %s %.0f\n", 
   basGame[i].year, basGame[i].date, 
    basGame[i].teamANum, basGame[i].teamAName, basGame[i].teamAHits, 
    basGame[i].teamBNum, basGame[i].teamBName, basGame[i].teamBHits);
  
  for(j=0;j<12;j++)fprintf(fSave, "%.0f ", basGame[i].scoreTeamA[j]);
  for(j=0;j<9;j++)fprintf(fSave, "%.0f ", basGame[i].teamAPlayerAS[j]);
  for(j=0;j<9;j++)fprintf(fSave, "%.0f ", basGame[i].teamAPlayerDL[j]);
  for(j=0;j<9;j++)fprintf(fSave, "%.0f ", basGame[i].teamAhits[j]);
  for(j=0;j<9;j++)fprintf(fSave, "%.0f ", basGame[i].teamAFaceCount[j]);
  fprintf(fSave, "%.0f %.0f\n", basGame[i].teamAPlayCount, basGame[i].teamALosePoint);
  for(j=0;j<12;j++)fprintf(fSave, "%.0f ", basGame[i].scoreTeamB[j]);
  for(j=0;j<9;j++)fprintf(fSave, "%.0f ", basGame[i].teamBPlayerAS[j]);
  for(j=0;j<9;j++)fprintf(fSave, "%.0f ", basGame[i].teamBPlayerDL[j]);
  for(j=0;j<9;j++)fprintf(fSave, "%.0f ", basGame[i].teamBhits[j]);
  for(j=0;j<9;j++)fprintf(fSave, "%.0f ", basGame[i].teamBFaceCount[j]); 
  fprintf(fSave, "%.0f %.0f\n", basGame[i].teamBPlayCount, basGame[i].teamBLosePoint);
 }
 fclose(fSave);
}

//save two teams score totally
void readScore(){
 FILE *fRead = fopen("gameScore.dat", "r");
 int i ,j;
 readTeam();
 //write how many games in the ram
 fscanf(fRead, "%d\n", &gameNum);
 for(i=0;i<=gameNum;i++){
  fscanf(fRead, "%f %f %f %s %f %f %s %f\n", 
   &basGame[i].year, &basGame[i].date, 
    &basGame[i].teamANum, &basGame[i].teamAName, &basGame[i].teamAHits, 
    &basGame[i].teamBNum, &basGame[i].teamBName, &basGame[i].teamBHits);
  
  for(j=0;j<12;j++)fscanf(fRead, "%f ", &basGame[i].scoreTeamA[j]);
  for(j=0;j<9;j++)fscanf(fRead, "%f ", &basGame[i].teamAPlayerAS[j]);
  for(j=0;j<9;j++)fscanf(fRead, "%f ", &basGame[i].teamAPlayerDL[j]);
  for(j=0;j<9;j++)fscanf(fRead, "%f ", &basGame[i].teamAhits[j]);
  for(j=0;j<9;j++)fscanf(fRead, "%f ", &basGame[i].teamAFaceCount[j]);
  fscanf(fRead, "%f %f", &basGame[i].teamAPlayCount, &basGame[i].teamALosePoint);
  for(j=0;j<12;j++)fscanf(fRead, "%f ", &basGame[i].scoreTeamB[j]);
  for(j=0;j<9;j++)fscanf(fRead, "%f ", &basGame[i].teamBPlayerAS[j]);
  for(j=0;j<9;j++)fscanf(fRead, "%f ", &basGame[i].teamBPlayerDL[j]);
  for(j=0;j<9;j++)fscanf(fRead, "%f ", &basGame[i].teamBhits[j]);
  for(j=0;j<9;j++)fscanf(fRead, "%f ", &basGame[i].teamBFaceCount[j]); 
  fscanf(fRead, "%f %f", &basGame[i].teamBPlayCount, &basGame[i].teamBLosePoint);
 }
 fclose(fRead);
}

//clear IO File content
void clearData(char *dataName){
 FILE *fClear = fopen(dataName, "w");     
 fprintf(fClear, "");
 fclose(fClear);
}

//-------------------------------partition Small Function Code-----------------------------

//-------------------------------show to screen function-------------------------------------

//show the data so far and sort them
//if flag =1 then something may not to show if flag =2 then not show pause
void showOneTeamTable(int teamNumber, int flag){
 struct player bubbleTmp;
 int i,j;
 //bubble sort the team baseball number
 for(i=basTeam[teamNumber].playerNum-1;i>=0;i--){
  for(j=0;j<i;j++){
   if(basTeam[teamNumber].pry[j].basNum > basTeam[teamNumber].pry[j+1].basNum){
    bubbleTmp = basTeam[teamNumber].pry[j];
    basTeam[teamNumber].pry[j] = basTeam[teamNumber].pry[j+1];   
    basTeam[teamNumber].pry[j+1] = bubbleTmp;
   }
  }
 }
 //show the player in the team
 if(flag)printf("\t\t\tMember Management System\n");
 printf("Team :%s\tplayers :%d\n", basTeam[teamNumber].name, basTeam[teamNumber].playerNum);
 printf("---------------------------------------------------------------------------\n姓名\t編號\t年薪(萬元)\t球齡\t守備位置\t防守率\t打擊率\n");
 for(j=0;j<basTeam[teamNumber].playerNum;j++){
  printf("%s\t%.0f\t%.0f\t\t%.0f\t%s\t\t%.2f\t%.2f\n", 
   basTeam[teamNumber].pry[j].name,basTeam[teamNumber].pry[j].basNum, basTeam[teamNumber].pry[j].salary, basTeam[teamNumber].pry[j].year,
   basTeam[teamNumber].pry[j].defendLoc, basTeam[teamNumber].pry[j].defend, basTeam[teamNumber].pry[j].attack);       
 }
 if(flag==1)system("pause");   
}

//print struct player type data to scream
//印出一位球員資料 
void print2Scream(struct player pry){
 printf("%s\t%.0f\t%.0f\t\t%.0f\t%s\t\t%.2f\t%.2f\n", 
   pry.name, pry.basNum, pry.salary, pry.year, pry.defendLoc, pry.defend, pry.attack);     
}

void coachData(int teamNumber){
 printf("\nname:%s\tyear:%.0f\tsalary:%.0f\n", basTeam[teamNumber].coach, basTeam[teamNumber].coachYear, basTeam[teamNumber].coachSalary);   
}

//顯示單局賽程分數 
void showGameScore(int innings, int upOrDown){
 int i, j;
 char status[2][5] = {"up", "down"};
 
 if(innings>=0)
  printf("Game Number: %d\t\tGame Record System\tinnings:(%d, %s)\n", gameNum+1, innings+1, status[upOrDown]);
 else 
  printf("Game Number: %d\t\tGame Record System\tTotal Score\n", gameNum+1);
  
 for(j=0;j<2;j++){
  if(j==0)
   printf("Team\t\t1\t2\t3\t4\t5\t6\n-----------------------------------------------------------");
  else
   printf("\n\nTeam\t\t7\t8\t9\t10\t11\tT\n-----------------------------------------------------------");

  printf("\n%-10s", basGame[gameNum].teamAName);
  for(i=0+j*6;i<j*6+6;i++)printf("\t%.0f", basGame[gameNum].scoreTeamA[i]);
  printf("\n%-10s", basGame[gameNum].teamBName);
  for(i=0+j*6;i<j*6+6;i++)printf("\t%.0f", basGame[gameNum].scoreTeamB[i]);
 }
 printf("\n");
}

//--------------------------------------------初始化各項參數值------------------------------ 
void initValue(int team){
  int i;
  strcpy(basTeam[team].password, basTeam[team].name);
  strcat(basTeam[team].password, "12345"); 
  basTeam[team].coachYear = -1;
  basTeam[team].ticketPrize = 0;
  basTeam[team].groundPrize = 0;
  basTeam[team].fieldCapacity = 0;
  for(i=0;i<5;i++)basTeam[team].salaryDis[i] = 1000 + i * 500;            
} 
//--------------------------------------------待刪副程式區-------------------------------- 
void initMoney(){
 int i;
 readTeam();
 for(i=0;i<teamNum;i++)initValue(i);
 saveTeam();   
}

//----------------------------------other small partition function-----------------------
//但斷該浮點數是否有小數點 有責傳回0 無則傳出該浮點數整數部分 
int judgeNumber(float num){
 int intNum;
 intNum = num;
 if(num == intNum && num >0)
  return num;
 else
  return 0;    
}

//輸入密碼 並判斷是否與被判斷密碼相符 副程式 
int checkPasswd(int tN){
 int i=0, flag=0;
 char passwd[20];
 flag = 1;    
 printf("password:");
 while(flag){
  fflush(stdin);
  passwd[i] = getch();
  switch(passwd[i]){
   //若key = enter 則跳出迴圈  
   case '\r':
    flag = 0;
    break;
   //若key=backspace 則退一位 清空 在退一位 
   case '\b':
    if(i>0){
     i--;
     printf("\b \b");
    }
    break;
   default:
    putch('*');
    i++;
    break;     
  }
  //組合字元陣列為字串 
  passwd[i] = '\0';       
 }
 //判斷是否相符 若相符則傳回1 否則0 
 if(strcmp(passwd, basTeam[tN].password))
  return 0;
 else
  return 1;
}

//修改球員資料副程式 
void modifyFunction(int number, int flag){
 char cRec;
 while(flag){
  system("cls");
  printf("Mode :Modify\t\tMember Management System\tTeam :%s\nplease fill in the player's data below\n", 
   basTeam[pryTeamNum].name);
  //詢問要修改該球員哪一部份資料 並判斷是否為有效數值 
  printf("which part of player do you want to modify?\n1.Name\t2.Baseball Number\t3.Salary\t4.Year\n5.defendLoc\t6.finish and return\nchoose one:"); 
  fflush(stdin);
  cRec = getch();
  if(cRec<54){gotoxy(0, 5);printf("-------Original Data--------------------Input Data-----------------\n");}
  switch(cRec){
   case '1':
    printf("\t%s\t\t\t\t", basTeam[pryTeamNum].pry[number].name);
    fflush(stdin);
    scanf("%s", &basTeam[pryTeamNum].pry[number].name);
    break;
   case '2':
    printf("\t%.0f\t\t\t\t", basTeam[pryTeamNum].pry[number].basNum);
    fflush(stdin);
    scanf("%f", &basTeam[pryTeamNum].pry[number].basNum);
    break;
   case '3':
    printf("\t%.0f\t\t\t\t", basTeam[pryTeamNum].pry[number].salary);
    fflush(stdin);
    scanf("%f", &basTeam[pryTeamNum].pry[number].salary);
    break;
   case '4':
    printf("\t%.0f\t\t\t\t", basTeam[pryTeamNum].pry[number].year);
    fflush(stdin);
    scanf("%f", &basTeam[pryTeamNum].pry[number].year);
    break;
   case '5':
    printf("\t%s\t\t\t\t", basTeam[pryTeamNum].pry[number].defendLoc);
    fflush(stdin);
    scanf("%s", &basTeam[pryTeamNum].pry[number].defendLoc);
    break;
   case '6':
    flag = 0;
    break;
   default:
    system("PAUSE");            
   }
   if(cRec<54){
    printf("Data Modify Completely!!!\n");
    system("PAUSE");     
   }                            
 }    
}

//chose =1 modify, chose =0 delete
//更新球員資料副程式 
void updateData(int chose, int fixNumber[], int size){
 int i, flag=1;
 float fRec;
 char mode[10]; strcpy(mode, (chose==1)?"Modify":"Delete");
 system("cls");
 printf("Mode :%s\t\tMember Management System\tTeam :%s\nplease fill in the player's data below\n", 
   mode ,basTeam[pryTeamNum].name);
 //size >1 represent not only one data
 if(size>1){
  printf("Which data do you want to modify?\n");
  //show the team players
  printf("------------------------------------------------------------------------------\n\t姓名\t編號\t薪水\t\t球齡\t守備位置\t防守率\t打擊率\n");
  for(i=0;i<size;i++){
   printf("data %d:\t%s\t%.0f\t%.0f\t\t%.0f\t%s\t\t%.2f\t%.2f\n", i+1,
   basTeam[pryTeamNum].pry[fixNumber[i]].name,basTeam[pryTeamNum].pry[fixNumber[i]].basNum, 
   basTeam[pryTeamNum].pry[fixNumber[i]].salary, basTeam[pryTeamNum].pry[fixNumber[i]].year,
   basTeam[pryTeamNum].pry[fixNumber[i]].defendLoc, basTeam[pryTeamNum].pry[fixNumber[i]].defend, 
   basTeam[pryTeamNum].pry[fixNumber[i]].attack);
  }
  while(flag){
   printf("Choose one:");
   fflush(stdin);
   scanf("%f", &fRec);
   if(judgeNumber(fRec)){
    flag=0;
   }else{
    printf("Invalid number!!");
    system("pause");
   }      
  }
  
  if(chose){
   modifyFunction(fixNumber[judgeNumber(fRec)-1], 1);  
  }else{
   //baseball Number =0 represent the data is being deleted!!!
   basTeam[pryTeamNum].pry[fixNumber[judgeNumber(fRec)-1]].basNum = 0;
   printf("Data delete complete!!!"); 
   system("pause");
  }     
 }else{
        
  if(chose){
   modifyFunction(fixNumber[judgeNumber(fRec)], 1);
  }else{
   basTeam[pryTeamNum].pry[fixNumber[size-1]].basNum = 0;
   printf("Data delete complete!!!");
   system("pause");
  }       
 }
}

//判斷是否與資料庫內球隊相符 
int judgeTeamName(char name[20]){
 int i, count=0;
 for(i=0;i<teamNum;i++){
  if(strcmp(name, basTeam[i].name)==0)count++;
 }
 if(count>0)return count; else return 0;    
}

//但斷是否與該球隊之球員相符 
int judgePlayerNumber(int team, float inputF){
 int i, j, count=0;
 for(j=0;j<playerNum+1;j++)
  if(basTeam[team].pry[j].basNum == inputF)count++;
 if(count)return 0; else return 1;    
}

//選擇欲管理之球隊副程式 (含新增球隊功能) 
void teamSelect(int flag, int chose){
 char tmpTeamName[20], escAns, *tmp;
 int queFalg=1, i, flagPass, count, pY;
 char s[2][10] = {"Member", "Team"}; 
 while(flag){
  //設定輸入密碼欄位之Y座標量 
  pY = teamNum + 3;
  //選擇球隊 
  flagPass = 1;
  while(flagPass){
   pryTeamNum = -1;
   system("cls");
   printf("\t\t\t%s Management System\n", s[chose]);
   //若資料庫中有球隊, 則記錄該球隊之編號 若沒有球隊,則自動新增一球隊 
   if(teamNum>0){
    printf("The Existed Team Below:\n");
    for(i=0;i<teamNum;i++)
     printf("%s\n", basTeam[i].name);
   }else{
    printf("There are none data in disk. Please create a new team!!!\n");      
   }
   printf("Choose one or create a new one :");
   fflush(stdin);
   scanf("%s", &tmpTeamName);
   //teamNum=0 means no data in player.dat
   if(teamNum==0){
    printf("Add a new team : %s\n\n", tmpTeamName);
    strcpy(basTeam[teamNum].name, tmpTeamName);
    pryTeamNum = 0;
    teamNum = 1;
    flagPass = 0;
    //新球隊初始設定 
    initValue(pryTeamNum);      
   }else{    
    for(i=0;i<teamNum;i++){
     if(strcmp(tmpTeamName, basTeam[i].name)==0)pryTeamNum=i;}
    
    //no data found but not empty in player.dat means we have to add a new team 
    if(pryTeamNum==-1){
     teamNum++; pryTeamNum=teamNum-1; flagPass = 0; 
     strcpy(basTeam[pryTeamNum].name, tmpTeamName);
     printf("Add a new team : %s\n\n", tmpTeamName);
     //新球隊初始設定 
     initValue(pryTeamNum);
     break;
    } 
    
    //若輸入為原資料庫中舊有球隊 則要求輸入該隊密碼 並允許三次失誤 若超過三次則跳出選擇此球隊 
    count=3;
    while(count>0 && flagPass==1){
     if(checkPasswd(pryTeamNum)){
      flagPass = 0;
      count = -1;    
     }else{
      count--;
      printf("\npassword incorrect!!! trying times left:%d\n", count);
      pY+=2;    
     }
    }
    
   }
  }
  
  //詢問是否真的要管理此球隊 否則跳回繼續選擇其他球隊 
  queFalg = 1;
  while(queFalg){
   fflush(stdin);
   printf("\nWould you like to manage players in team %s?(Press y or n): ", basTeam[pryTeamNum].name);
   fflush(stdin);
   scanf("%c", &escAns);
   switch(escAns){
    case 'y':
     flag = 0;
     queFalg=0; 
     break;
    case 'n':
     queFalg=0;
     break;
    default:
     printf("Please press key y or key n!!!");
     system("PAUSE");
     break;     
   }
  }
  
 }  
}

//show the scores record of the two teams completely for a game
void showTotalGameRecord(int gameNumber){
 showGameScore(-1 ,0);
 printf("Team %s total hits: %.0f\tTeam %s total hits: %.0f\n",
  basGame[gameNumber].teamAName, basGame[gameNumber].teamAHits, 
  basGame[gameNumber].teamBName, basGame[gameNumber].teamBHits);     
}

//自動分配球員 
void autoDisPry(int teamNumber){
 
}

//手動分配球員 
//atOrde :判斷是攻擊隊還是防守隊
void manualDisPry(int teamNumber, int atOrde){
 int disCount=0, i, flag, chose[20], j, initFalg = 1;
 char status[2][20] = {"Attack", "Defend"};
 float n[9], pryAryTmp[9];
 
 //defend character
 char defS[9][10] = {
  "catcher", "pitcher", "baseFir", "baseSec" ,"baseTrd" ,"Free" ,"offOut" ,"midOut","leftOut"};  
 //attacke character
 char attS[9][10] = {
  "Hitter1", "Hitter2", "Hitter3", "Hitter4", "Hitter5", "Hitter6", "Hitter7", "Hitter8", "Hitter9"};       
 
 char s[9][10];
 
 if(atOrde==0)
  for(i=0;i<9;i++)strcpy(s[i], attS[i]); 
 else 
  for(i=0;i<9;i++)strcpy(s[i], defS[i]);
 
 //顯示 分配球員 
 disCount=0;
 while(disCount<9){
  system("cls");
  //print the existed player which were not chosen to be the player
  printf("Team :%s\t\tGame Player Distribute\t\tStatus :%s\n", basTeam[teamNumber].name, status[atOrde]);
  printf("---------------------------------------------------------------------------\n姓名\t編號\t薪水\t\t球齡\t守備位置\t防守率\t打擊率\n");
  //所有球員的迴圈 
  for(j=0;j<basTeam[teamNumber].playerNum;j++){
   flag = 1;
   if(initFalg){
    //顯示所有球員 
    printf("%s\t%.0f\t%.0f\t\t%.0f\t%s\t\t%.2f\t%.2f\n", 
     basTeam[teamNumber].pry[j].name,basTeam[teamNumber].pry[j].basNum, basTeam[teamNumber].pry[j].salary, basTeam[teamNumber].pry[j].year,
     basTeam[teamNumber].pry[j].defendLoc, basTeam[teamNumber].pry[j].defend, basTeam[teamNumber].pry[j].attack);       
   }else{
    //倘若有了已選球員,則不顯示 
    for(i=0;i<disCount;i++){
     if(n[i] == basTeam[teamNumber].pry[j].basNum)flag=0; 
    }   
    if(flag)
     printf("%s\t%.0f\t%.0f\t\t%.0f\t%s\t\t%.2f\t%.2f\n", 
     basTeam[teamNumber].pry[j].name,basTeam[teamNumber].pry[j].basNum, basTeam[teamNumber].pry[j].salary, basTeam[teamNumber].pry[j].year,
     basTeam[teamNumber].pry[j].defendLoc, basTeam[teamNumber].pry[j].defend, basTeam[teamNumber].pry[j].attack);   
   }
  }
  
  flag=1;
  while(flag){
   printf("please input %s's back number:", s[disCount]);
   fflush(stdin);
   scanf("%f", &n[disCount]);
   //判斷輸入支球員編號是否存在於球隊中, 有則儲存該球員之陣列序號 
   for(i=0;i<basTeam[teamNumber].playerNum;i++)
     if(n[disCount]==basTeam[teamNumber].pry[i].basNum){ 
      pryAryTmp[disCount] = i;  
      flag = 0;
     } 
   if(flag){
    printf("Invalid number!!!");
    system("pause");
   }
  }
  
  //核對是否與前面輸入球員衝突 
  flag=1;
  for(i=0;i<disCount;i++)
   if(n[disCount] == n[i]){
    printf("The same back number !!!");
    system("pause");
    flag = 0;
    break;
   } 
  
  initFalg = 0;
  if(flag)disCount++;       
 }
 
 //save the value to the array
 if(basGame[gameNum].teamANum == teamNumber){
  if(atOrde==0)
   for(i=0;i<9;i++)basGame[gameNum].teamAPlayerAS[i] = pryAryTmp[i];
  else
   for(i=0;i<9;i++)basGame[gameNum].teamAPlayerDL[i] = pryAryTmp[i]; 
 }else{
  if(atOrde==0)
   for(i=0;i<9;i++)basGame[gameNum].teamBPlayerAS[i] = pryAryTmp[i];
  else
   for(i=0;i<9;i++)basGame[gameNum].teamBPlayerDL[i] = pryAryTmp[i];       
 }
 system("cls"); 
}

//賽前初始化參數 
void gameInitialization(){
 int flag, i; 
 char tmpStr[20];
 
 basGame[gameNum].teamANum = -1;
 basGame[gameNum].teamBNum = -1;
 
 //input the date of the game
 flag = 1;
 while(flag){   
  printf("\t\t\tGame Management System\n");
  printf("Input the game of the date (form:YYYY MMDD)");
  scanf("%f %f", &basGame[gameNum].year, &basGame[gameNum].date);
  if(judgeNumber(basGame[gameNum].year) && judgeNumber(basGame[gameNum].date)){
   flag = 0;
  }else{
   printf("Invalid Number!!!");      
  }
 }
 
 //show existed team and choose one to be the teamA
 for(i=0;i<teamNum;i++) printf("%s(%d)\n", basTeam[i].name, basTeam[i].playerNum);
 
 //要求輸入球隊A 
 flag = 1;
 while(flag){
  printf("\nChoose two teams->TeamA:");
  fflush(stdin);
  scanf("%s", &basGame[gameNum].teamAName);
  //核對清單中是否有找到與輸入球隊名字相符  
  for(i=0;i<teamNum;i++){
   if(strcmp(basTeam[i].name, basGame[gameNum].teamAName)==0)
     if(basTeam[i].playerNum >= 9)basGame[gameNum].teamANum = i; 
  }
  //若無責印出錯誤訊息 並要求使用者在輸入一次
  if(basGame[gameNum].teamANum != -1)
   flag = 0;
  else
   printf("Invalid Value or the number of the player in the team are not enough!!"); 
 }
 
 system("cls");
 printf("\t\t\tGame Management System\n");
 //show existed team and choose one to be the teamB
 for(i=0;i<teamNum;i++) 
  if(strcmp(basTeam[i].name, basGame[gameNum].teamAName))printf("%s(%d)\n", basTeam[i].name, basTeam[i].playerNum);
 
 //要求輸入球隊B 
 flag = 1;
 while(flag){
  printf("\nChoose two teams->TeamB:");
  fflush(stdin);
  scanf("%s", &basGame[gameNum].teamBName);
  //判斷是否與球隊A名字相衝突 
  if(strcmp(basGame[gameNum].teamBName, basGame[gameNum].teamAName)){  
   //核對清單中是否有找到與輸入球隊名字相符
   for(i=0;i<teamNum;i++){
    if(strcmp(basTeam[i].name, basGame[gameNum].teamBName)==0)
     if(basTeam[i].playerNum >= 9)basGame[gameNum].teamBNum = i; 
   }
   //若無責印出錯誤訊息 並要求使用者在輸入一次
   if(basGame[gameNum].teamBNum != -1)
    flag = 0;
   else
    printf("Invalid Value or the number of the player in the team are not enough!!");
  }else{
   printf("Same name with TeamA!!!");      
  } 
 }
 system("cls");
  
 //distribute the players for attack and defend
 manualDisPry(basGame[gameNum].teamANum, 0);
 manualDisPry(basGame[gameNum].teamANum, 1);
 manualDisPry(basGame[gameNum].teamBNum, 0);
 manualDisPry(basGame[gameNum].teamBNum, 1);

 //reset scorePoints of the two teams
 for(i=0;i<12;i++){
  basGame[gameNum].scoreTeamA[i] = 0;
  basGame[gameNum].scoreTeamB[i] = 0; 
 }
 
 //reset the hits of two teams
 basGame[gameNum].teamAHits = 0;
 basGame[gameNum].teamBHits = 0; 
}

//紀錄賽程 
//if upOrDown 0 :up ,1 :down 
int gaming(int innings, int hitNum ,int upOrDown){
 int killNum=0, flag=1, i;
 float hits, score;
 int hitterNum[9] ,gameTeamNumber, pitcherTeam, pitcher;
 
 if(upOrDown == 0){
  gameTeamNumber =  basGame[gameNum].teamANum;
  for(i=0;i<9;i++)hitterNum[i] = basGame[gameNum].teamAPlayerAS[i];
  pitcherTeam =  basGame[gameNum].teamBNum;
  pitcher = basGame[gameNum].teamBPlayerDL[1];
 }else{
  gameTeamNumber =  basGame[gameNum].teamBNum;
  for(i=0;i<9;i++)hitterNum[i] = basGame[gameNum].teamBPlayerAS[i];
  pitcherTeam =  basGame[gameNum].teamANum;
  pitcher = basGame[gameNum].teamAPlayerDL[1];
 }
 
 //該投手之出戰局數+1
 if(upOrDown) 
  basGame[gameNum].teamAPlayCount++;
 else
  basGame[gameNum].teamBPlayCount++;
  
 basTeam[pitcherTeam].pry[pitcher].playCount++;
  
 //hitter's value 
 while(killNum<3){
  showGameScore(innings, upOrDown);
  flag=1;
  while(flag){
   //顯示打者球號 姓名 並詢問打出安打還是被OUT 
   printf("Hitter: %.0f %s Input(0. Out 1. Hit):", basTeam[gameTeamNumber].pry[hitterNum[hitNum]].basNum, basTeam[gameTeamNumber].pry[hitterNum[hitNum]].name);
   fflush(stdin);
   scanf("%f", &hits);
   if(hits==1 || hits==0){
    //改球員之面對投手數+1
    if(upOrDown) 
     basGame[gameNum].teamAFaceCount[hitNum]++;
    else
     basGame[gameNum].teamBFaceCount[hitNum]++;
    basTeam[gameTeamNumber].pry[hitterNum[hitNum]].faceCount++;
    
    if(hits){
     //該球員之安打數+1
     if(upOrDown)
      basGame[gameNum].teamAhits[hitNum]++; 
     else
      basGame[gameNum].teamBhits[hitNum]++; 
      
     basTeam[gameTeamNumber].pry[hitterNum[hitNum]].hits++;
     
     //該隊總安打數+1 
     if(upOrDown == 0)
      basGame[gameNum].teamAHits++; 
     else
      basGame[gameNum].teamBHits++;
      
    }else{
     killNum++; 
    }
    flag = 0;
   }else{
    printf("Invalid Number!!!");
    system("pause");     
   }
  }
  hitNum++;
  if(hitNum==9)hitNum=0;
  system("cls");
 }
 
 //attack team's score
 flag=1;
 while(flag){ 
  showGameScore(innings, upOrDown);
  printf("Input Attack Team: %s score in innings %d :", basTeam[gameTeamNumber].name, innings+1);       
  fflush(stdin);
  scanf("%f", &score);
  if(judgeNumber(score) || score==0){
   
   //計算該局投手的防守率
   if(upOrDown)
    basGame[gameNum].teamALosePoint++;
   else
    basGame[gameNum].teamBLosePoint++;
    
   basTeam[pitcherTeam].pry[pitcher].losePoint += score;
   basTeam[pitcherTeam].pry[pitcher].defend = basTeam[pitcherTeam].pry[pitcher].losePoint * 9 / basTeam[pitcherTeam].pry[pitcher].playCount;
   
   printf("%d\n", innings);
   if(upOrDown == 0){
    basGame[gameNum].scoreTeamA[innings] = score;
    basGame[gameNum].scoreTeamA[11] += score;
   }else{
    basGame[gameNum].scoreTeamB[innings] = score;
    basGame[gameNum].scoreTeamB[11] += score;   
   }
   flag = 0;     
  }else{
   printf("Invalid Number!!!");
   system("pause");      
  }
  system("cls");
 }
 return hitNum;    
}

//將輸入的薪水階層存入 並分配薪水給各隊球員 
void updateSalary(){
 int i, j;
 for(i=0;i<teamNum;i++){
  for(j=0;j<basTeam[i].playerNum;j++){
   if(basTeam[i].pry[j].attack >= 0 && basTeam[i].pry[j].attack <= 0.2){
    basTeam[i].pry[j].salary = basTeam[i].salaryDis[0];     
   }else if(basTeam[i].pry[j].attack > 0.2 && basTeam[i].pry[j].attack <= 0.4){
    basTeam[i].pry[j].salary = basTeam[i].salaryDis[1];     
   }else if(basTeam[i].pry[j].attack > 0.4 && basTeam[i].pry[j].attack <= 0.6){
    basTeam[i].pry[j].salary = basTeam[i].salaryDis[2];     
   }else if(basTeam[i].pry[j].attack > 0.6 && basTeam[i].pry[j].attack <= 0.8){
    basTeam[i].pry[j].salary = basTeam[i].salaryDis[3];     
   }else if(basTeam[i].pry[j].attack > 0.8 && basTeam[i].pry[j].attack <= 1){
    basTeam[i].pry[j].salary = basTeam[i].salaryDis[4];     
   }
  }       
 }
 printf("Salary Distribution Update Successfully!!!\n"); system("pause");   
}
	
//-------------------------------partition Big Function Code-----------------------------

//add a new player(fun1-1)
void newPlayer(int flag){
 int bNFlag=1, pYFlag=1, pMFlag=1, pDFlag=1, pAFlag=1, queFalg=1, teamFlag=1;
 float tmpBasNum;
 struct player nPlayer;
 char escAns, tmpTeamName[20];
 //giving the init number
 nPlayer.attack =0;
 nPlayer.defend = 0;
 nPlayer.hits = 0; 
 nPlayer.faceCount = 0; 
 nPlayer.playCount = 0; 
 nPlayer.losePoint = 0;
 
 playerNum = basTeam[pryTeamNum].playerNum; 
 //input a team name ,and it will judge if the name existed.
    
 //if flag =0 then finish the data filling
 while(flag){
  bNFlag=1; pYFlag=1; pMFlag=1; pDFlag=1; pAFlag=1; queFalg=1;
  system("cls");
  showOneTeamTable(pryTeamNum, 2);
  //name
  printf("Name :");
  scanf("%s", &nPlayer.name); 
   
  //baseball Number
  while(bNFlag){
   printf("Player's Baseball Number :");
   fflush(stdin);
   scanf("%f", &tmpBasNum);     
   if(judgeNumber(tmpBasNum) && judgePlayerNumber(pryTeamNum ,tmpBasNum)){
    nPlayer.basNum = tmpBasNum;
    bNFlag=0;
   }else{
    if(judgeNumber(tmpBasNum) == 0)printf("Invalid number!!");
    if(judgePlayerNumber(pryTeamNum, tmpBasNum) == 0)printf("There's already the same player number!!");
    system("pause");
   }
  }
  
  //salary
  while(pMFlag){
   printf("Player's Salary :");
   fflush(stdin);
   scanf("%f", &nPlayer.salary);
   if(judgeNumber(nPlayer.salary)){
    pMFlag=0;
   }else{
    printf("Invalid number!!");
    system("pause");
   }
  }
  
  //year
  while(pYFlag){
   printf("Playing years :");
   fflush(stdin);
   scanf("%f", &nPlayer.year);
   if(judgeNumber(nPlayer.year)){
    pYFlag=0;
   }else{
    printf("Invalid number!!");
    system("pause");
   }
  }

  //Defend Location
  printf("Defend Location :");
  scanf("%s", &nPlayer.defendLoc);
  
  nPlayer.defend = 0;
  nPlayer.attack = 0;
    
  //save the data to the total stuct
  basTeam[pryTeamNum].pry[playerNum] = nPlayer;
 
  printf("player's data in %s builds sucessfully!!\n", basTeam[pryTeamNum].name);
  playerNum++;
  
  while(queFalg){
   printf("\nContinue adding player at %s?(Press y or n): ", basTeam[pryTeamNum].name);
   fflush(stdin);
   scanf("%c", &escAns);
   switch(escAns){
    case 'n':
     flag = 0;
     queFalg=0;
     break;
    case 'y':
     queFalg=0;
     break;
    default:
     printf("Please press key y or key n!!!");
     system("PAUSE");
     break;     
   }
   basTeam[pryTeamNum].playerNum = playerNum;
  }
 }
}

/*mix player function(fun 1:search 0:delete 2:modify fun1-2 fun1-3 fun1-4)
this function only appropiate with only delete one data, that function modify can works and
search for multiple players*/
void mul3_1Player(int flag, int fun){
 struct player tmpPry; 
 int loopFlag=1, findFlag=1, i=0, j, dataFind=0, fixNumber[basTeam[pryTeamNum].playerNum];
 float chose;
 char mode[3][10] = {"Delete", "Search", "Modify"}, cRec;
 
 while(flag){
  dataFind=0;
  system("cls");
  printf("Mode :%s\t\tMember Management System\tTeam :%s\n",mode[fun] ,basTeam[pryTeamNum].name);
  printf("What information about the player you know?\n");
  //訊問知道愈做修改的球員哪部分的資訊 
  printf("1.name\t2.Baseball Number\t3.Defend Location\t4.return\nchoose one:");
  fflush( stdin );
  cRec = getch();
  if(cRec > 48 && cRec< 53 && fun != 1){gotoxy(0, 3);showOneTeamTable(pryTeamNum, 0);}
  switch(cRec){
    case '1':
     if(fun==1)gotoxy(0, 3);
     printf("input Name:");
     scanf("%s", &tmpPry.name);            
     //if data and dataBase are the same , then don't save to temp array, if not then save 
     for(i=0;i<basTeam[pryTeamNum].playerNum;i++){
      if(strcmp(tmpPry.name, basTeam[pryTeamNum].pry[i].name)==0){
       //print the data that was found
       if(fun==1){
        if(dataFind==0)printf("姓名\t編號\t年薪(萬元)\t球齡\t守備位置\t防守率\t打擊率\n");
        print2Scream(basTeam[pryTeamNum].pry[i]);
       }
       fixNumber[dataFind] = i; dataFind++;      
      }
     }
     break;
    case '2':
     if(fun==1)gotoxy(0, 3);
     while(findFlag){
      printf("Player's Number :");
      fflush(stdin);
      scanf("%f", &tmpPry.basNum);
      if(judgeNumber(tmpPry.basNum)){
       findFlag=0; 
      }else{
        printf("Invalid number!!");
        system("pause");
      }
     }
     //判斷並記錄該球員之球號 若為搜尋模式則印出該球員 
     for(i=0;i<basTeam[pryTeamNum].playerNum;i++){
      if(tmpPry.basNum == basTeam[pryTeamNum].pry[i].basNum){
       if(fun==1){
        if(dataFind==0)printf("姓名\t編號\t年薪(萬元)\t球齡\t守備位置\t防守率\t打擊率\n");
        print2Scream(basTeam[pryTeamNum].pry[i]);
       }
	   fixNumber[dataFind] = i; dataFind++;      
      }
     }  
     break;
    case '3':
     if(fun==1)gotoxy(0, 3);
     printf("input Defend Location :");
     fflush( stdin );
     scanf("%s", &tmpPry.defendLoc);             
     //if data and dataBase are the same , then don't save to temp array, if not then save
     for(i=0;i<basTeam[pryTeamNum].playerNum;i++){
      if(strcmp(tmpPry.defendLoc, basTeam[pryTeamNum].pry[i].defendLoc)==0){
       if(fun==1){
        if(dataFind==0)printf("姓名\t編號\t年薪(萬元)\t球齡\t守備位置\t防守率\t打擊率\n");
        print2Scream(basTeam[pryTeamNum].pry[i]);
       } 
       fixNumber[dataFind] = i; dataFind++;      
      }
     }
     break;
    case '4':
     loopFlag = 0;
     flag=0;
     dataFind = 1;
     break;
    default:
     dataFind = 1;
     break;            
   }
   if(dataFind!=0){
    // modify and delete function
    if(fun == 0 && flag != 0){
     updateData(0, fixNumber, dataFind);
     chose =5;
     flag=0;
    }else if(fun == 2 && flag != 0){
     updateData(1, fixNumber, dataFind);
     flag=0;
     chose =5;
    }
   }else{
    printf("No data found!!!"); 
    system("PAUSE");    
   }
 }
}

//baseball game program(fun2-1)
void gameFun(int flag){
 int innings = 0, hitANum=0, hitBNum=0, choseFlag;
 gameNum++; 
 
 while(flag){
  //set the defend team and attack team
  gameInitialization();      
  
  while(innings<12){
   //On half board
   hitANum = gaming(innings, hitANum, 0);
   //Below half board
   hitBNum = gaming(innings, hitBNum, 1);
   
   //if score has the difference in board > 8 then exit the game
   switch(innings){
    case 8:
    case 9:
     if(basGame[gameNum].scoreTeamA[11] !=  basGame[gameNum].scoreTeamB[11])innings = 12;
     break;
    default:
     break;       
   }
   innings++;     
  }
  
  //show the whole game record result
  system("cls");
  showTotalGameRecord(gameNum);
  //詢問守否要存取該場球賽 
  choseFlag =1;
  while(choseFlag){
   printf("Save the game?(Press y or n):");
   fflush(stdin);
   switch(getchar()){
    case 'Y':
    case 'y':
     choseFlag = 0;
     writeScore();
     break;
    case 'N':
    case 'n':
     gameNum--;
     choseFlag = 0;
     break;
    default:
     printf("Invalid Chosen!!!");                  
   }
  }
  
  //ask if continue a new game if no then return to menu
  choseFlag =1;
  while(choseFlag){
   printf("Continue to add a new game?(Press y or n):");
   fflush(stdin);
   switch(getchar()){
    case 'Y':
    case 'y':
     choseFlag = 0;
     gameNum++;
     system("cls");
     break;
    case 'N':
    case 'n':
     choseFlag = 0;
     flag = 0;
     system("cls");
     break;
    default:
     printf("Invalid Chosen!!!");                  
   }
  }
 }
}

//baseball game program(fun2-2)
void searchGame(int flag){
 int i, mon, day, choseFlag;
 float fRec;
 while(flag){
  printf("\t\t\tGame Record System\tSearch\n");
  printf("What games do you want to know?\n\n");
  for(i=0;i<=gameNum;i++){
   mon = basGame[i].date / 100;
   day = (int)basGame[i].date % 100;
   printf("%d. Date: %.0f/%d/%d %s vs %s (%.0f : %.0f)\n",
    i+1, basGame[i].year, mon, day, basGame[i].teamAName, basGame[i].teamBName,
    basGame[i].scoreTeamA[11], basGame[i].scoreTeamB[11]);      
  } 
  
  choseFlag = 1;
  while(choseFlag){
   printf("Choose one :");
   fflush(stdin);
   scanf("%f", &fRec);
   fRec--;
   if(judgeNumber(fRec) || fRec == 0){
    choseFlag = 0;
   }else{
    printf("Invalid Number!!!");
    system("pause");      
   }
  }
 
  system("cls");
  showTotalGameRecord((int)fRec);
  printf("\nSearch Result...");
  system("pause");
  
  //ask if continue a new game if no then return to menu
  choseFlag =1;
  while(choseFlag){
   printf("Continue to search game?(Press y or n):");
   fflush(stdin);
   switch(getchar()){
    case 'Y':
    case 'y':
     choseFlag = 0;
     system("cls");
     break;
    case 'N':
    case 'n':
     choseFlag = 0;
     flag = 0;
     system("cls");
     break;
    default:
     printf("Invalid Chosen!!!");                  
   }
  }
 }    
}

//顯示球季紀錄表現(fun2-3)
void seasonTable(int flag){
 int gameYears[10], gameY, i, yNum=0, gameName[100], tNum=0, j, upRange, downRange, month, tFlag, checkFlag;
 float fRec;
 char escAns;
 //定義一個球員結構暫存 目的是利用所有已在球員資料顯示 並且計算其中之防禦率 打擊率 
 struct team tmpTeam[100];
 //副制所有球隊 球員資料 
 for(i=0;i<100;i++)tmpTeam[i] = basTeam[i];
 
 while(flag){
  system("cls");
  printf("\t\t\tGame Record System\tSeason Table\n");      
  
  //清空所有該死的參數值 
  for(i=0;i<teamNum;i++){
   for(j=0;j<tmpTeam[i].playerNum;j++){
    tmpTeam[i].pry[j].hits = 0;      
    tmpTeam[i].pry[j].faceCount = 0;
    tmpTeam[i].pry[j].playCount = 0;
    tmpTeam[i].pry[j].losePoint = 0;
    tmpTeam[i].pry[j].attack = 0;
    tmpTeam[i].pry[j].defend = 0;
   }
   gameName[i] = -1;       
  }
 
  //搜尋資料庫中存在年份
  for(i=0;i<=gameNum;i++){
   if(gameYears[yNum] != basGame[i].year){
    gameYears[yNum] = basGame[i].year;
    //列印偵查到的年份 
    printf("year: %d\n", gameYears[yNum]);
   }else{
    yNum++;      
   }
  }
 
  checkFlag = 1;
  while(checkFlag){
   printf("Choose one:"); 
   fflush(stdin);
   scanf("%f", &fRec);
   if(judgeNumber(fRec)){
    //檢查機制 
    tFlag = 0;
    for(i=0;i<yNum;i++)if(fRec==gameYears[i]){ tFlag = 1;break; } 
    if(tFlag){ 
     checkFlag = 0; 
     gameY = fRec; 
    }else{
     printf("None same data!!!");
     system("pause");
    }      
   }else{
    printf("Invalid Number!!!");
    system("pause");       
   }       
  }
 
  checkFlag = 1;
  while(checkFlag){
   //選擇上半季或下半季 
   printf("Up or down?(Up:0 Down:1)");
   fflush(stdin);
   scanf("%f", &fRec);
   if(fRec==0){
    upRange = 1; downRange = 6; checkFlag = 0;       
   }else if(fRec==1){
    upRange = 7; downRange = 12; checkFlag = 0;       
   }else{
    printf("Invalid Number!!!");
    system("pause");      
   }       
  }
  
 
  //搜尋資料庫中該年份上下半季存在的球隊 
  for(i=0;i<=gameNum;i++){
   if(basGame[i].year == gameY){
    month = basGame[i].date / 100;
    if(month >= upRange && month <= downRange){
     //掃描該隊友上場之球員 累加面對投手數 安打數 
     for(j=0;j<9;j++){
      tmpTeam[(int)basGame[i].teamANum].pry[(int)basGame[i].teamAPlayerAS[j]].hits += basGame[i].teamAhits[j];
      tmpTeam[(int)basGame[i].teamANum].pry[(int)basGame[i].teamAPlayerAS[j]].faceCount += basGame[i].teamAFaceCount[j];
      tmpTeam[(int)basGame[i].teamBNum].pry[(int)basGame[i].teamBPlayerAS[j]].hits += basGame[i].teamAhits[j];
      tmpTeam[(int)basGame[i].teamBNum].pry[(int)basGame[i].teamBPlayerAS[j]].faceCount += basGame[i].teamAFaceCount[j];
     }
     tmpTeam[(int)basGame[i].teamANum].pry[(int)basGame[i].teamAPlayerDL[1]].playCount += basGame[i].teamBPlayCount;
     tmpTeam[(int)basGame[i].teamANum].pry[(int)basGame[i].teamAPlayerDL[1]].losePoint += basGame[i].teamBLosePoint;
     tmpTeam[(int)basGame[i].teamBNum].pry[(int)basGame[i].teamBPlayerDL[1]].playCount += basGame[i].teamBPlayCount;
     tmpTeam[(int)basGame[i].teamBNum].pry[(int)basGame[i].teamBPlayerDL[1]].losePoint += basGame[i].teamBLosePoint;
    
     if(tNum==0){
      gameName[tNum] = basGame[i].teamANum;
      tNum++;       
     }
     //判斷是否與前面A球隊有重複 
     tFlag = 1;
     for(j=0;j<tNum;j++) if(gameName[j] == basGame[i].teamANum) {tFlag = 0; break;}
     if(tFlag){gameName[tNum] = basGame[i].teamANum; tNum++;}
     //判斷是否與前面B球隊有重複  
     tFlag = 1;
     for(j=0;j<tNum;j++) if(gameName[j] == basGame[i].teamBNum) {tFlag = 0; break;}
     if(tFlag){gameName[tNum] = basGame[i].teamBNum; tNum++;}
    }
   }       
  }
 
   //列印資料
  for(i=0;i<tNum;i++){
   printf("Team: %s\n", tmpTeam[gameName[i]].name); 
   printf("-------------------------------------------------------------------\n姓名      編號  總安打  面對投手  出戰局數  自責失分  防守率  打擊率\n"); 
   for(j=0;j<tmpTeam[gameName[i]].playerNum;j++){
    //計算該打者之防禦率(若面對投手數為0 則不計算) 打擊率(僅限投手) 
    if(tmpTeam[gameName[i]].pry[j].faceCount != 0)tmpTeam[gameName[i]].pry[j].attack = tmpTeam[gameName[i]].pry[j].hits / tmpTeam[gameName[i]].pry[j].faceCount; 
    if(tmpTeam[gameName[i]].pry[j].playCount != 0)tmpTeam[gameName[i]].pry[j].defend = 9 * tmpTeam[gameName[i]].pry[j].losePoint / tmpTeam[gameName[i]].pry[j].playCount;
    printf("%-10s%-6.0f%-8.0f%-10.0f%-10.0f%-10.0f%-8.2f%.2f\n", 
    tmpTeam[gameName[i]].pry[j].name, tmpTeam[gameName[i]].pry[j].basNum, tmpTeam[gameName[i]].pry[j].hits, tmpTeam[gameName[i]].pry[j].faceCount,
    tmpTeam[gameName[i]].pry[j].playCount, tmpTeam[gameName[i]].pry[j].losePoint, tmpTeam[gameName[i]].pry[j].defend, tmpTeam[gameName[i]].pry[j].attack);
   }
  }
 
  //ask twice
  checkFlag = 1;
  while(checkFlag){
   printf("Continue to show?(Press y or n):");
   fflush(stdin);
   scanf("%c", &escAns);
   switch(escAns){
    case 'Y':    
    case 'y':
     checkFlag=0;
     break;
    case 'n':
    case 'N':
     checkFlag=0;
     flag = 0;
     break;
    default:
     printf("Please press key y or key n!!!");
     system("PAUSE");
     break;     
   }      
  }      
 } 
}

//baseball team manage program(fun3-1)
//球隊密碼修改副程式 
void passwdModifyFun(int flag){
 char oldPass[20];
 int count, i, inFlag, pY = 1;
 while(flag){
  system("cls");
  printf("Team:%s\tTeam Management System\t\tPassword Modify\n", basTeam[pryTeamNum].name);
  //修改密碼前須輸入舊密碼 若三次錯誤則跳離該副程式 
  count = 3;
  while(count>0){
   printf("input old password:");
   //create a intput password
   inFlag = 1; i = 0;
   while(inFlag){
    fflush(stdin);
    oldPass[i] = getch();
    switch(oldPass[i]){
     case '\r':
      inFlag = 0;
      break;
     case '\b':
      if(i>0){
       i--;
       printf("\b \b");
      }
      break;
     default:
      putch('*');
      i++;
      break;     
    }
    oldPass[i] = '\0';       
   }
   //check the password
   //若核對為正確密碼 則允許輸入新密碼 
   if(strcmp(oldPass, basTeam[pryTeamNum].password)==0){
    pY++;
    printf("\nnew password:");
    //create a intput password
    inFlag = 1; i = 0;
    while(inFlag){
     fflush(stdin);
     oldPass[i] = getch();
     switch(oldPass[i]){
     case '\r':
      inFlag = 0;
      break;
     case '\b':
      if(i>0){
       i--;
       printf("\b \b");
      }
      break;
     default:
      putch('*');
      i++;
      break;     
    }
     oldPass[i] = '\0';       
    }
    //存入新密碼 
    strcpy(basTeam[pryTeamNum].password, oldPass);
    printf("\npassword modify completely!!!");
    system("pause");
    count = -1;
    flag = 0;
   }else{
    count--;
    printf("\npassword incorrect!!! trying times left:%d\n", count);
    pY += 2;      
   }
  }
  //如果三次都輸入錯誤則結束副程式 
  if(count==0)flag = 0;  
 } 
 saveTeam();
 readTeam();
}

//add a coach(fun3-2-1)
void newCoach(){
 int localFlag;
 float fRec;
 printf("Team:%s\t\tTeam Management System\n", basTeam[pryTeamNum].name);    

 printf("Input coach's name:");
 scanf("%s", &basTeam[pryTeamNum].coach);
         
 localFlag = 1;
 while(localFlag){
  printf("Input coach's salary:");      
  fflush(stdin);
  scanf("%f", &fRec); 
  if(judgeNumber(fRec) || fRec==0){
   localFlag = 0;
   basTeam[pryTeamNum].coachSalary = fRec;
  }else{
   printf("Invalid Number!!!");      
  }     
 }
 
 localFlag = 1;
 while(localFlag){
  printf("Input coach's Qualifications:");      
  fflush(stdin);
  scanf("%f", &fRec); 
  if(judgeNumber(fRec) || fRec==0){
   localFlag = 0;
   basTeam[pryTeamNum].coachYear = fRec;
  }else{
   printf("Invalid Number!!!");      
  }     
 }
 saveTeam();
 readTeam();
 printf("Coach Save sucessfully!!!");
 system("pause");
}

//coach modifying function(fun3-2-2)
void modifyCoach(int flag){
 float tmp[2], tmpRec, inFlag;
 char cRec;
 tmp[0] = basTeam[pryTeamNum].coachSalary;
 tmp[1] = basTeam[pryTeamNum].coachYear;
 
 while(flag){
  system("cls");
  printf("Team:%s\t\tTeam Management System\n", basTeam[pryTeamNum].name);
  coachData(pryTeamNum);
  printf("Which one do you want to modify?\n1.name\t2.salary\t3.Qualifications\t4.return and finish modifying\n");
  printf("choose one:");
  fflush(stdin);
  cRec = getch();
  if(cRec<52){gotoxy(0, 5);printf("----------Original Data--------------------Input Data----------\n");}
  switch(cRec){
   case '1':
    gotoxy(10, 6); printf("%s", basTeam[pryTeamNum].coach);
    gotoxy(43, 6); fflush(stdin);
    scanf("%s", &basTeam[pryTeamNum].coach);
    break;
   case '2':
   case '3':
    gotoxy(10, 6); printf("%.0f", tmp[(int)cRec-50]);
    inFlag = 1;
    while(inFlag){
     gotoxy(43, 6);
     fflush(stdin);
     scanf("%f", &tmpRec);
     if(judgeNumber(tmpRec)){
      if(cRec==50)basTeam[pryTeamNum].coachSalary = tmpRec;
      else basTeam[pryTeamNum].coachYear = tmpRec;
      inFlag = 0;
     }else{
      gotoxy(43, 6); printf("          ");       
     }
    }
    break;
   case '4':
    flag = 0;
    gotoxy(0, 5);
    printf("Data Modify Completely!!!\n");
    system("pause");      
    break;
   default:
    system("pause");
  }
 } 
 saveTeam();
 readTeam();
}

//function deleting coach(fun3-2-3)
void deleteCoach(){
 int checkFlag;
 char escAns;
 printf("Team:%s\t\tTeam Management System\n", basTeam[pryTeamNum].name);
 
 checkFlag = 1;
 while(checkFlag){
  printf("Are you sure you want to delete the coach %s(Press y or n):", basTeam[pryTeamNum].coach);
  fflush(stdin);
  scanf("%c", &escAns);
  switch(escAns){
   case 'N':
   case 'n':
    checkFlag=0;
    break;
   case 'y':
   case 'Y':
    checkFlag=0;
    basTeam[pryTeamNum].coachYear = -1;
    printf("Delete Complete!!!");
    system("pause");
    break;
   default:
    printf("Please press key y or key n!!!");
    system("pause");
    break;     
  }       
 }
 saveTeam();
 readTeam();
}

//coach manage system(fun3-2)
void coachManagemant(int flag){
 char cRec;
 while(flag){
  system("cls");
  printf("Team:%s\t\tTeam Management System\n", basTeam[pryTeamNum].name);
  coachData(pryTeamNum);
  printf("1.new coach\n2.modify coach\n3.delete coach\n4.back to main menu\nchoose one:");
  fflush(stdin);
  cRec = getch();
  switch(cRec){
   case '1':
    system("cls");
    newCoach();
    system("cls");
    break;
   case '2':
    system("cls");
    modifyCoach(1);
    system("cls");
    break;
   case '3':
    system("cls");
    deleteCoach();
    system("cls");    
    break;
   case '4':
    flag = 0;
    break;
   default:
    system("pause");
    break;
  }             
 }
}

//distribute salary(fun3-3)
void salDis(int flag){
 int i, secCount, checkFlag;
 char escAns;
 float fRec, tmpSecSalary[5];
 while(flag){
  //judge modify or just show?
  system("cls");
  printf("Team:%s\t\tTeam Management System\n", basTeam[pryTeamNum].name);
  printf("ERA\t");
  for(i=0;i<5;i++)printf("%d%%~%d%%    ", 20*i, 20*(i+1));
  printf("\n\t");
  for(i=0;i<5;i++){
   printf("%-11.0f", basTeam[pryTeamNum].salaryDis[i]);
  }
  printf("\nModify:1 Return:0\nChoose one:");
  fflush(stdin);
  scanf("%f", &fRec);
  if(fRec==0){
   secCount = 5;
   checkFlag = 0;
   updateSalary();
   flag = 0;    
  }else if(fRec==1){
   secCount = 0; 
   checkFlag = 1; 
  }else{
   printf("Invalid Value!!!");
   secCount = 5;
   checkFlag = 0; 
  }
  
  //modify mode
  while(secCount<5){
   system("cls");
   printf("Team:%s\t\tTeam Management System\n", basTeam[pryTeamNum].name);
   printf("Input the salary in different section\n");
   printf("ERA-----------------Original Data-------------------Input Data-----------------\n");
   //output oringinal data
   for(i=0;i<5;i++){
    printf("%d%%~%d%%", 20*i, 20*(i+1));
    gotoxy(23, 3+i);
    printf("%.0f\n", basTeam[pryTeamNum].salaryDis[i]);      
   }
   for(i=0;i<secCount;i++){
    gotoxy(52, 3+i);
    printf("%.0f", tmpSecSalary[i]);
   }
   gotoxy(52, 3+secCount);
   fflush(stdin);
   scanf("%f", &tmpSecSalary[secCount]);
   if(judgeNumber(tmpSecSalary[secCount])){
    secCount++;     
   }   
  }
  
  //check mode
  while(checkFlag){
   gotoxy(0, 8);
   printf("Sure to save the distribution?(Press y or n): ");
   fflush(stdin);
   scanf("%c", &escAns);
   switch(escAns){
    case 'y':
     flag = 0;
     checkFlag=0;
     for(i=0;i<5;i++)basTeam[pryTeamNum].salaryDis[i] = tmpSecSalary[i];
     updateSalary();
     break;
    case 'n':
     checkFlag=0;
     break;
    default:
     printf("Please press key y or key n!!!");
     system("PAUSE");
     break;     
   }      
  }
 }
 saveTeam();
 readTeam();
}

//estimate how many events at least to play and will get the profit(fun3-4)
void estEvent(int flag){
 int checkFlag, i;
 char escAns, name[3][20] = {"Ticket Prize", "Ground Prize", "Field Capacity"}, cRec;
 float fRec[3], fRece;
 printf("Team:%s\t\tTeam Management System\n", basTeam[pryTeamNum].name);

 //判斷是否有已輸入資料 
 if(basTeam[pryTeamNum].ticketPrize == 0 && basTeam[pryTeamNum].fieldCapacity == 0 && basTeam[pryTeamNum].groundPrize == 0){
  i=0;
 }else{
  checkFlag = 1;
  while(checkFlag){
   printf("There's already existed data, do you want to modify?(Press y or n):");
   fflush(stdin);
   scanf("%c", &escAns);
   switch(escAns){
    case 'Y':    
    case 'y':
     checkFlag=0;
     i = 3;
     break;
    case 'n':
    case 'N':
     checkFlag=0;
     i = 0;
     break;
    default:
     printf("Please press key y or key n!!!");
     system("PAUSE");
     break;     
   }      
  }
 }
 
 //如果已且要修改有資料則修改 ,否則新增新的資料 
 if(i==3){
  checkFlag = 1;
  while(checkFlag){
   system("cls");
   printf("Team:%s\t\tTeam Management System\n", basTeam[pryTeamNum].name);
   printf("1.Ticket Prize\t2.Ground Prize\t3.Field Capacity\t4.finish and return\nchoose one:");
   fflush(stdin);
   cRec = getch();
   if(cRec<52)printf("-------Original Data--------------------Input Data-----------------\n"); 
   switch(cRec){
    case '1':
     printf("\t%.0f\t\t\t\t", basTeam[pryTeamNum].ticketPrize);
     fflush(stdin);
     scanf("%f", &basTeam[pryTeamNum].ticketPrize);
     break;
    case '2':
     printf("\t%.0f\t\t\t\t", basTeam[pryTeamNum].groundPrize);
     fflush(stdin);
     scanf("%f", &basTeam[pryTeamNum].groundPrize);
     break;
    case '3':
     printf("\t%.0f\t\t\t\t", basTeam[pryTeamNum].fieldCapacity);
     fflush(stdin);
     scanf("%f", &basTeam[pryTeamNum].fieldCapacity);
     break;
    case '4':
     checkFlag = 0;
     break;
    default:
     system("PAUSE");            
   }
  }         
 }else{
  while(i<3){
  checkFlag = 1;
   while(checkFlag){
    if(i==0)printf("%s(單位:元):", name[i]);
    if(i==1)printf("%s(單位:萬元):", name[i]);
    if(i==2)printf("%s(單位:萬人)", name[i]);
    fflush(stdin);
    scanf("%f", &fRec[i]);
    if(judgeNumber(fRec[i])){
     checkFlag = 0;      
     i++;
    }else{
     printf("Invalid Number!!!");      
    }
   }       
  }
  //save the input array
  basTeam[pryTeamNum].ticketPrize = fRec[0];
  basTeam[pryTeamNum].groundPrize = fRec[1];
  basTeam[pryTeamNum].fieldCapacity = fRec[2];       
 }
 
 //calculate the total salary of the all player in the team
 fRece = 0;
 for(i=0;i<basTeam[pryTeamNum].playerNum;i++){
  fRece += basTeam[pryTeamNum].pry[i].salary;       
 }
 
 fRece /= basTeam[pryTeamNum].ticketPrize * basTeam[pryTeamNum].fieldCapacity - basTeam[pryTeamNum].groundPrize; 
 i = fRece;
 printf("estimate games :%d", i+1);
 system("pause");
 
 saveTeam();
 readTeam();
}

//delete a team function(fun3-5)
float deleteTeam(){
 char escAns;
 int checkFlag, pY, count;
 
 printf("Team:%s\t\tTeam Management System\n", basTeam[pryTeamNum].name);
 
 //與該隊密碼核對 並允許三次失誤 若超過三次則跳出選擇此球隊 
 count=3; 
 pY = 1;
 while(count>0){
  if(checkPasswd(pryTeamNum)){
   count = -1;
   checkFlag = 1;    
  }else{
   count--;
   printf("\npassword incorrect!!! trying times left:%d\n", count);
   pY+=2;
   checkFlag = 0;    
  }
 }
 
 //ask twice
 while(checkFlag){
  system("cls");
  printf("Team:%s\t\tTeam Management System\n", basTeam[pryTeamNum].name);
  printf("Are you sure you want to delete team %s permanently?(Press y or n):", basTeam[pryTeamNum].name);
  fflush(stdin);
  scanf("%c", &escAns);
  switch(escAns){
   case 'Y':    
   case 'y':
    checkFlag=0;
    //標記為待刪球隊 
    basTeam[pryTeamNum].playerNum = -1;
    saveTeam();
    readTeam();
    saveTeam();
    readTeam(); 
    return 1;
    break;
   case 'n':
   case 'N':
    checkFlag=0;
    break;
   default:
    printf("Please press key y or key n!!!");
    system("PAUSE");
    break;     
  }      
 }  
}

//show the career record of the whole team(fun4-1)
void tCarRecord(int flag){
 int i, checkFlag, pName, tHit;
 char escAns;
 float fRec, batAvg;
 while(flag){
  tHit = 0; batAvg = 0;
  system("cls");
  printf("\t\tLeague Management System\n");
  for(i=0;i<teamNum;i++)printf("%d.%s\n", i+1, basTeam[i].name);
  
  checkFlag=1;
  while(checkFlag){
   printf("Choose one to show the career of team:");
   fflush(stdin);
   scanf("%f", &fRec);
   if(judgeNumber(fRec)){
    checkFlag = 0;
    pName = fRec-1;
   }else{
    printf("Invalid Number!!!");
    system("pause");     
   }      
  }
  
  for(i=0;i<basTeam[pName].playerNum;i++){
   batAvg += basTeam[pName].pry[i].attack;
   tHit += basTeam[pName].pry[i].hits; 
  }
  tHit/=basTeam[pName].playerNum;
  batAvg/=basTeam[pName].playerNum;
  
  system("cls");
  printf("\t\tLeague Management System\n");
  printf("Team:%s\n", basTeam[pName].name);
  printf("Number of players:%-19dTicket Price:%.0f\n", basTeam[pName].playerNum, basTeam[pName].ticketPrize);
  printf("Home Prices:%-25.0fMain Field Capacity:%.0f\n", basTeam[pName].groundPrize, basTeam[pName].fieldCapacity);
  printf("Total average hits:%-18dTotal batting average:%.2f\n", tHit, batAvg);
  
  checkFlag = 1;
  while(checkFlag){
   printf("Continue to show the information of the team?(Press y or n):");
   fflush(stdin);
   scanf("%c", &escAns);
   switch(escAns){
    case 'Y':    
    case 'y':
     checkFlag=0;
     break;
    case 'n':
    case 'N':
     checkFlag=0;
     flag = 0;
     break;
    default:
     printf("Please press key y or key n!!!");
     system("PAUSE");
     break;     
   }
  }
 }    
}

//show the career record of the whole team(fun4-2)
void pCarRecord(int flag){
 int i, checkFlag, pName, tHit;
 char escAns;
 float fRec, batAvg;
 while(flag){
  tHit = 0; batAvg = 0;
  system("cls");
  printf("\t\tLeague Management System\n");
  for(i=0;i<teamNum;i++)printf("%d.%s\n", i+1, basTeam[i].name);
  
  checkFlag=1;
  while(checkFlag){
   printf("Choose one to show the career of team:");
   fflush(stdin);
   scanf("%f", &fRec);
   if(judgeNumber(fRec)){
    checkFlag = 0;
    pName = fRec-1;
   }else{
    printf("Invalid Number!!!");
    system("pause");     
   }      
  }
  
  system("cls");
  printf("\t\tLeague Management System\n");
  printf("Team:%s\n", basTeam[pName].name);
  
  printf("--------------------------------------------------------------------------\n姓名      編號  球齡  總安打  面對投手  出戰局數  自責失分  防守率  打擊率\n"); 
  for(i=0;i<basTeam[pName].playerNum;i++){
   printf("%-10s%-6.0f%-6.0f%-8.0f%-10.0f%-10.0f%-10.0f%-8.2f%.2f\n", 
   basTeam[pName].pry[i].name, basTeam[pName].pry[i].basNum, basTeam[pName].pry[i].year, basTeam[pName].pry[i].hits, basTeam[pName].pry[i].faceCount,
   basTeam[pName].pry[i].playCount, basTeam[pName].pry[i].losePoint, basTeam[pName].pry[i].defend, basTeam[pName].pry[i].attack);
  }
  
  checkFlag = 1;
  while(checkFlag){
   printf("Continue to show the information of the team?(Press y or n):");
   fflush(stdin);
   scanf("%c", &escAns);
   switch(escAns){
    case 'Y':    
    case 'y':
     checkFlag=0;
     break;
    case 'n':
    case 'N':
     checkFlag=0;
     flag = 0;
     break;
    default:
     printf("Please press key y or key n!!!");
     system("PAUSE");
     break;     
   }
  }
 }    
}

//Schedule Arrangement(fun4-3)
void schArr(){
 int i, j, k, l;
 printf("\t\tLeague Management System\nYear:2014\n");
 k=1; l=1;
 for(i=teamNum-1;i>0;i--){
  for(j=0;j<i;j++){
   printf("%d.(%s v.s %s)\n", l, basTeam[j].name, basTeam[j+k].name);      
   l++;
  }
  k++;       
 }
 system("pause");    
}

//--------------------------------------judgeMode code------------------------------------

//when user choose fun1.(member Manage) it jumps to do the function here
void memberManagement(int flag){
 float fRec;
 char cRec;
 teamSelect(1, 0);
 system("cls");
 while(flag){          
  printf("\t\t\tMember Management System\tTeam :%s\n", basTeam[pryTeamNum].name);
  printf("1.new player\n2.delete player\n3.search player\n4.modify player\n5.show summary table of all players\n6.back to main menu\nchoose one:");
  fflush( stdin );
  cRec = getch();
  switch(cRec){
   case '1':
    system("cls");
    newPlayer(1);
    system("cls");   
    break;
   case '2':
    system("cls");
    mul3_1Player(1, 0); 
    system("cls");
    break;
   case '3':
    system("cls");
    mul3_1Player(1, 1);
    system("cls");
    break;
   case '4':
    system("cls");
    mul3_1Player(1, 2);
    system("cls");
    break;
   case '5':
    system("cls");
    showOneTeamTable(pryTeamNum, 1);
    system("cls");
    break;
   case '6':
    flag = 0;
    break;
   default:
    system("cls");  
  }
  saveTeam();
  readTeam();
 }
}

//when user choose fun2.(game Manage) it jumps to do the function here
void gameManagement(int flag){
 readScore();
 char rRec;
 while(flag){ 
  printf("\t\t\tGame Management System\n");
  printf("1.new game\n2.search game\n3.Seasons Table\n4.back to main menu\nchoose one:");
  fflush( stdin );
  rRec = getch();
  switch(rRec){
   case '1':
    system("cls");
    gameFun(1);
    system("cls");
    break;
   case '2':
    system("cls");
    searchGame(1);
    system("cls");
    break;
   case '3':
    system("cls");
    seasonTable(1);
    system("cls");
    break;
   case '4':
    flag = 0;
    break;
   default:
    system("cls");
    break;  
  }
 }
 writeScore();
}

//when user choose fun3.(team Manage) it jumps to do the function here
void teamManagement(int flag){
 float delFlag;
 char cRec;
 //choose the team that you want to manage
 teamSelect(1, 1);
 system("cls");
 delFlag = 0;
 while(flag){ 
  printf("Team:%s\t\tTeam Management System\n", basTeam[pryTeamNum].name);      
  printf("1.Password Modify\n2.Coach Database\n3.Salary Distribution\n4.Estimating Events\n5.team format\n6.back to main menu\nchoose one:");
  fflush(stdin);
  //如果DELfLAG並未被啟動 則保持繼續在該球隊活動 
  if(delFlag==0)cRec = getch(); else cRec = '6';
  switch(cRec){
   case '1':
    system("cls");
    passwdModifyFun(1);
    system("cls");
    break;
   case '2':
    system("cls");
    coachManagemant(1);
    system("cls");
    break;
   case '3':
    system("cls");
    salDis(1);
    system("cls");
    break;
   case '4':
    system("cls");
    estEvent(1);
    system("cls");
    break;
   case '5':
    system("cls");    
    delFlag = deleteTeam();
    system("cls");
    break;
   case '6':
    flag = 0;
    break;
   default:
    system("cls");
    break;
  }      
 }    
}

//league baseball management(fun4)
void leagueManagement(int flag){
 char cRec;
 while(flag){ 
  printf("\t\tLeague Management System\n");      
  printf("1.Team Career Record\n2.Player Career Record\n3.Schedule Arrangement\n4.back to main menu\nchoose one:");
  fflush(stdin);
  cRec = getch();
  switch(cRec){
   case '1':
    system("cls");
    tCarRecord(1);
    system("cls");
    break;
   case '2':
    system("cls");
    pCarRecord(1);
    system("cls");
    break;
   case '3':
    system("cls");
    schArr();
    system("cls");
    break;
   case '4':
    flag = 0;
    break;
   default:
    system("cls");
    break;
  }      
 }    
}

//when program start, jump to this loop and do the functions
void mainSwitchMode(int flag){
 readTeam();
 char cRec;
 while(flag){           
  printf("\t\t\tBaseball Management System\nNow here are functions available below\n");
  printf("1.member database\n2.game manage\n3.team manage\n4.league manage\n5.exit\nchoose one:");
  fflush( stdin );
  cRec = getch();
  switch(cRec){
   case '1':
    system("cls");
    memberManagement(1);
    system("cls");
    break;
   case '2':
    system("cls");
    gameManagement(1);
    system("cls");
    break;
   case '3':
    system("cls");
    teamManagement(1);
    system("cls");
    break;
   case '4':
    system("cls");
    leagueManagement(1);
    system("cls");
    break;
   case '5':
    flag=0;
    break;
   default:
    system("cls");
    break;
  }
 }
}

int main(int argc, char *argv[]){
  //start the system
  mainSwitchMode(programSwitch);
  //system("PAUSE");
  return 0;
}
