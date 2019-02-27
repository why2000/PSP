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
FILE* read_fp;
FILE* write_fp;
//注册表及其他非语法树内容都是全局链表，可以用NULL检测首尾
RNL* rootRNL;
RNL* leaveRNL;
//注释信息
ANNOTATIONS* Annotations;
//预编译信息
PRECOMPILES* Precompiles;
//if标志(刚结束if，可以else)
int IF_STATUS;
//条件表达式标志(for/while/if)
int JUDGE_STATUS;
//错误标志（已废弃）
int ERROR_STATUS;
//管道
int* fpipe; 

int main(int argc, char* argv[]) {
    fpipe = (int*)malloc(sizeof(int)*2);
    int index;
    int* mainstatus = (int*)malloc(sizeof(int)*(argc));
    int failsum = 0;
    int totalnum = 0;
    for(index = 1; index < argc; index++){
        write_fp = stdout;
        if(strcmp(argv[index], "-o") == 0 || strcmp(argv[index], "--output") == 0 ){
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
//        anastart(argv[index]);//debug
        int ret = pipe(fpipe); 
        if (ret == -1) { 
            perror("pipe error\n"); 
            return 1; 
        }
        //这里冲一下缓冲区，防止子进程输出缓冲区导致加倍输出
        //逻辑: 第一次fork->子进程1缓冲->子进程1exit输出->父进程缓冲句子A->第二次fork->子进程2缓冲句子B->子进程2exit输出AB->父进程fclose输出A->得到ABA
        fflush(write_fp);
        pid_t id = fork(); 
        if (id == 0) 
        {
            close(fpipe[0]);
            int buf_result = anastart(argv[index]);
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
            sleep(5000);
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
            if (mainstatus[index] == 0){
                fprintf(write_fp, "Result: %s successfully analyzed!\n", argv[index]);
            }else{
                failsum++;
                fprintf(write_fp, "Result: Failed to analyze %s!\n", argv[index]);
            }
            //调研加倍输出的问题，原因是子进程把缓冲区输出了
//            FILE* p1 = NULL;
//            FILE* p2 = NULL;
//            FILE* p3 = NULL;
//            if(fopen("p1p.txt", "r") != NULL){
//                if(fopen("./p2p.txt", "r") != NULL){
//                    p3 = fopen("./p3p.txt", "w");
//                }else{
//                    p2 = fopen("./p2p.txt", "w");
//                }
//            }
//            else{
//                p1 = fopen("p1p.txt", "w");
//            }
//            if(p1)fprintf(p1,"0");
//            if(p2)fprintf(p2,"0");
//            if(p3)fprintf(p3,"0");
        } 
        else {
            perror("fork error\n");
            return 2; 
        }
        
    }
    fprintf(write_fp, "Analysis finished for %d files, %d succeeded, %d failed!\n", totalnum, (totalnum-failsum), failsum);
    if(totalnum < argc - 1 && write_fp != NULL){
        printf("Results are saved in %s.\n", argv[totalnum+2]);
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
    Annotations->next = NULL;
    Precompiles = (PRECOMPILES*)malloc(sizeof(PRECOMPILES));
    Precompiles->next = NULL;
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
    fflush(write_fp);
    fclose(read_fp);
    return ana_status;
}










