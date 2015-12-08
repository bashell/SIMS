#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sims.h"
#include "conio.h"

extern pUSR_ACCOUNT pUsr_Account;
extern pUSR_INFOR pUsr_Infor;
extern USR_ROLE role;
extern SYSTEM_MENU_TYPE type;
char account_path[N] = "../data/", infor_path[N] = "../data/";

/*
 * 系统初始化函数. 初始化账号链表和学生信息链表
 */
void system_init(const char *config_file_name, pUSR_ACCOUNT pUsr_Account, pUSR_INFOR pUsr_Infor) {
    FILE *fp = NULL, *fp_account = NULL, *fp_infor = NULL;
    char config_gets[N] = "";
    char account_left[N] = "", infor_left[N] = "";
    //pUSR_ACCOUNT pUsr_Account_Cur = NULL, pUsr_Account_Tail = pUsr_Account;
    pUSR_ACCOUNT pUsr_Account_Head = (pUSR_ACCOUNT)calloc(1, sizeof(USR_ACCOUNT));
    pUSR_ACCOUNT pUsr_Account_Tail = (pUSR_ACCOUNT)calloc(1, sizeof(USR_ACCOUNT));
    pUSR_INFOR pUsr_Infor_Cur = NULL, pUsr_Infor_Tail = pUsr_Infor;
    int read_num, count;

    // 加载配置文件configure.txt中的信息, 补全路径
    fp = fopen(config_file_name, "rb");
    if(NULL == fp) {
        fprintf(stderr, "Config_file open error!\n");
        exit(EXIT_FAILURE);
    }
    while(fgets(config_gets, N, fp) != NULL) { }
    sscanf(config_gets, "%s %s", account_left, infor_left);
    strcat(account_path, account_left);
    strcat(infor_path, infor_left);
    fclose(fp);

    // 读取用户信息
    fp_account = fopen(account_path, "rb");
    if(NULL == fp_account) {
        fprintf(stderr, "Account information open error!\n");
        exit(EXIT_FAILURE);
    }
    count = 0;
    while(1) {
        pUsr_Account_Cur = (pUSR_ACCOUNT)calloc(1, sizeof(USR_ACCOUNT));
        read_num = fscanf(fp_account, "%s %s %d", pUsr_Account_Cur -> usr_name, pUsr_Account_Cur -> usr_pwd, 
                          &pUsr_Account_Cur -> usr_role);
        //printf("%s %s %d\n", pUsr_Account_Cur -> usr_name, pUsr_Account_Cur -> usr_pwd, pUsr_Account_Cur -> usr_role);
        //sleep(3);
        if(3 == read_num) {
            pUsr_Account_Tail -> pNext_Usr_Account = pUsr_Account_Cur;
            pUsr_Account_Tail = pUsr_Account_Cur;
        } else {
            free(pUsr_Account_Cur);
            //pUsr_Account_Cur = NULL;
            break;
        }
    }
    fclose(fp_account);
    pUsr_Account = pUsr_Account -> pNext_Usr_Account;
    printf("%s %s %d\n", pUsr_Account_Cur -> usr_name, pUsr_Account_Cur -> usr_pwd, pUsr_Account_Cur -> usr_role);
    sleep(3);

    // 读取学生信息
    fp_infor = fopen(infor_path, "rb");
    if(NULL == fp_infor) {
        fprintf(stderr, "Student information file open error!\n");
        exit(EXIT_FAILURE);
    }
    while(1) {
        pUsr_Infor_Cur = (pUSR_INFOR)calloc(1, sizeof(USR_INFOR));
        read_num = fscanf(fp_infor, "%d %s %d %lf", &pUsr_Infor_Cur -> usr_id, pUsr_Infor_Cur -> usr_name, 
                          &pUsr_Infor_Cur -> usr_course_id, &pUsr_Infor_Cur -> usr_course_score);
        //printf("%d %s\n", pUsr_Infor_Cur -> usr_id, pUsr_Infor_Cur -> usr_name);
        //sleep(3);
        if(4 == read_num) {
            pUsr_Infor_Tail -> pNext_Usr_Infor = pUsr_Infor_Cur;
            pUsr_Infor_Tail = pUsr_Infor_Cur;
        } else {
            free(pUsr_Infor_Cur);
            //pUsr_Infor_Cur = NULL;
            break;
        }
    }
    fclose(fp_infor);
//    printf("%s %s %d\n", pUsr_Account -> usr_name, pUsr_Account -> usr_pwd, pUsr_Account -> usr_role);
//    sleep(3);
}

/*
 * 检查用户名和密码是否合法, 并确认当前账号的使用权限 (管理员 or 普通用户)  
 */
USR_ROLE role_confirm(pUSR_ACCOUNT pUsr_Account) {
    //printf("%s %s %d\n", pUsr_Account -> usr_name, pUsr_Account -> usr_pwd, pUsr_Account -> usr_role);
    //sleep(3);
    char usr_name[USR_NAME_LEN] = "", usr_password[USR_PWD_LEN] = "";
    char ch;
    int i;
    pUSR_ACCOUNT pUsr_Search = pUsr_Account;
    USR_ROLE role;

    // 输入用户名
    system("clear");
    printf("Enter usr_name: ");
    fflush(stdin);
    fflush(stdout);
    fgets(usr_name, USR_NAME_LEN, stdin);
    for(i = 0; i < USR_NAME_LEN; ++i)
        if(usr_name[i] == '\n')
            usr_name[i] == '\0';

    // 输入密码
    system("clear");
    printf("Enter usr_password: ");
    fflush(stdin);
    fflush(stdout);
    for(i = 0; (ch = mygetch()) != '\n'; ) {
        if(i == 0 && ch == 127) {  
            putchar('\a');
        } else if(i > 0 && i < USR_PWD_LEN && ch == 127) {
            usr_password[--i] = '\0';
            printf("\b \b");
        } else if(i == USR_PWD_LEN - 1 && ch != 127) {
            putchar('\a');
        } else {
            usr_password[i++] = ch;
            printf("*");
        }
    }
//    printf("\n"); 
//    printf("%s %s\n", usr_name, usr_password);
//    sleep(10);
    // 将得到的用户名与密码同系统中的数据进行匹配. 成功则返回相应的权限
    while(pUsr_Search) {
        if(strcmp(usr_name, pUsr_Search -> usr_name) == 0 && strcmp(usr_password, pUsr_Search -> usr_pwd) == 0) {
            role = pUsr_Search -> usr_role;
            break;
        } else {
            pUsr_Search = pUsr_Search -> pNext_Usr_Account;
        }
    }
    if(NULL == pUsr_Search) {
        printf("\nAccount or Password input error! Input again?(Y/N): ");
        while(fflush(stdin), (ch = mygetch()) != EOF) {
            if(!(ch == 'y' || ch == 'Y' || ch == 'n' || ch == 'N'))
                printf(" \b");
            if(ch == 'y' || ch == 'Y') {
                role = -1;
                break;
            }
            if(ch == 'n' || ch == 'N') {
                role = -2;
                break;
            }
            if(ch == 13)
                continue;
        }
    }
    if(role == -1) {
        system("clear");
        role = role_confirm(pUsr_Account);
    } else if(role == -2) {
        system("clear");
        printf("Goodbye!\n");
        sleep(1);
        exit(EXIT_SUCCESS);
    } else {
        return role;
    }
}

SYSTEM_MENU_TYPE show_system_menu(USR_ROLE role) {
    SYSTEM_MENU_TYPE type = 0;

    switch(role) {
    case 1:
        {
            system("clear");
            printf("*************************************************************************");
            printf("*****************Student  Information  Management  System****************");
            printf("*************************************************************************");
            printf("\n\n");
            printf("                          1. search  student  information\n");
            printf("                          2. add     student  information\n");
            printf("                          3. update  student  information\n");
            printf("                          4. delete  student  information\n");
            printf("                          5. add     user     account\n");
            printf("                          6. update  user     account\n");
            printf("                          7. delete  user     account\n");
            printf("                          8. search  user     account\n");
            printf("                          9. exit\n");
            printf("\nselect a number: ");

        }
    }
}
