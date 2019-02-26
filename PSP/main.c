//
//  reader.c
//  PSP
//
//  Created by 伍瀚缘 on 2019/2/18.
//  Copyright © 2019年 伍瀚缘. All rights reserved.
//

//记得在所有errorfound(0);之前free所有函数中创建的堆
//2.23晚:理论上实现了类型不一致检测（未测试）
//2.24晚:注意栈链表一定要保证前后各有一空位，否则会导致NULL指针在栈释放后被覆盖写入值，无法检测首尾
//2.25晚:实现了单目运算符/宏定义
//2.26晚:实现了多进程，报错时无需free

#include"main.h"
ulong line_num;
char token_name[MAX_TOKEN_SIZE];
//全局链表可以用NULL检测首尾
RNL* rootRNL;
RNL* leaveRNL;
FILE* read_fp;
FILE* write_fp;
ANNOTATIONS* Annotations;
PRECOMPILES* Precompiles;
int IF_STATUS;
int JUDGE_STATUS;
int ERROR_STATUS;
int* fpipe; 

int main(int argc, char* argv[]) {
    fpipe = (int*)malloc(sizeof(int)*2);
    int index;
    int* mainstatus = (int*)malloc(sizeof(int)*(argc));
    int failsum = 0;
    int totalnum = 0;
    for(index = 1; index < argc; index++){
        write_fp = stdout;
        if(strcmp(argv[index], "-o") == 0 || strcmp(argv[index], "-output") == 0 ){
            index++;
            if((write_fp = fopen(argv[index], "w")) == NULL){
                printf("Error! Failed to open output file.\n");
                return 1;
            }
            
            break;
        }
        totalnum++;
    }
    for (index = 1; index <= totalnum; index++){
        int ret = pipe(fpipe); 
        if (ret == -1) { 
            perror("pipe error\n"); 
            return 1; 
        }
        pid_t id = fork(); 
        if (id == 0) 
        {
            close(fpipe[0]);
            int buf_result = anastart(argv[index]);
//            printf("childres:%d\n",buf_result);
            if(buf_result == 0){
                char* child = "0";
                write(fpipe[1], child, strlen(child) + 1); 
                sleep(2);
                exit(0);
            }
            else{
                char* child = "1";
                write(fpipe[1], child, strlen(child) + 1); 
                sleep(2);
                exit(0);
            }
        }
        else if (id>0) {
            wait(0);
            close(fpipe[1]);  
            char msg[100];  
            memset(msg,'\0',sizeof(msg));  
            ssize_t s  =  read(fpipe[0],  msg,  sizeof(msg));    
            if(s>0)    
            {  
                msg[s - 1]  =  '\0';    
            }    
            if(strcmp(msg, "0") == 0){
                mainstatus[index] = 0;
            }
            else{
                mainstatus[index] = 1;
            }
        } 
        else {
            perror("fork error\n");
            return 2; 
        }
        if (mainstatus[index] == 0){
            fprintf(write_fp, "Result: %s successful analyzed!\n", argv[index]);
        }else{
            failsum++;
            fprintf(write_fp, "Result: Failed to analyze %s!\n", argv[index]);
        }
    }
    fprintf(write_fp, "Analysis finished for %d files, %d succeeded, %d failed!\n", totalnum, (totalnum-failsum), failsum);
    if(totalnum < argc - 1 && write_fp != NULL){
        printf("Results are saved in %s.", argv[totalnum+2]);
    }
    if(write_fp != stdout){
        fclose(write_fp);
    }
    free(mainstatus);
    mainstatus = NULL;
}

int anastart(const char* filename){
    //0为成功
    int ana_status = 1;
    line_num = 1;
    read_fp = NULL;
    IF_STATUS = 0;
    JUDGE_STATUS = 0;
    ERROR_STATUS = 0;
//    int curstatus;
    if((read_fp = fopen(filename, "r")) == NULL){
        printf("Error! Failed to open %s.\n", filename);
        return 1;
    }
    rootRNL = (RNL*)malloc(sizeof(RNL));
    rootRNL->FPL = NULL;
    rootRNL->next = (RNL*)malloc(sizeof(RNL));
    rootRNL->next->value_type = EMPTY_TOKEN;
    rootRNL->next->var_type = STARTOFSTA;
    rootRNL->next->last = rootRNL;
    rootRNL->last = (RNL*)malloc(sizeof(RNL));
    rootRNL->last->value_type = EMPTY_TOKEN;
    rootRNL->last->var_type = ENDOFSTA;
    rootRNL->last->next = rootRNL;
    rootRNL->value_type = EMPTY_TOKEN;
    rootRNL->var_type = ENDOFSTA;
    leaveRNL = rootRNL;
    fprintf(write_fp, "文件\"%s\"语法解析结果:\n", filename);
    Annotations = (ANNOTATIONS*)malloc(sizeof(ANNOTATIONS));
    Precompiles = (PRECOMPILES*)malloc(sizeof(PRECOMPILES));
    PRECOMPILES* Precompiles_root = Precompiles;
    ANNOTATIONS* Annotations_root = Annotations;
    EDL* EDL_cur = ExtDefList();
    if (EDL_cur && EDL_cur->ED_kind != ENDOFSTA && !ERROR_STATUS){
        fprintf(write_fp, "预编译信息:\n");
        PREoutput(Precompiles_root);
        fprintf(write_fp, "注释信息:\n");
        ANNoutput(Annotations_root);
        fprintf(write_fp, "语法树:\n");
        EDLoutput(EDL_cur);
        clear_RNL(rootRNL);
        free(rootRNL);
        ana_status = 0;
    }else{
        ana_status = 1;
    }
    fclose(read_fp);
    return ana_status;
}










