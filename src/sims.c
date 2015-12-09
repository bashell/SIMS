#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sims.h"
#include "conio.h"

char account_path[N] = "../data/", infor_path[N] = "../data/";

/*
 * 系统初始化函数. 初始化账号链表和学生信息链表
 */
void system_init(const char *config_file_name, pUSR_ACCOUNT *pUsr_Account, pUSR_INFOR *pUsr_Infor) {
    FILE *fp = NULL, *fp_account = NULL, *fp_infor = NULL;
    char config_gets[N] = "";
    char account_left[N] = "", infor_left[N] = "";
    pUSR_ACCOUNT pUsr_Account_Cur = NULL, pUsr_Account_Tail = *pUsr_Account;
    pUSR_INFOR pUsr_Infor_Cur = NULL, pUsr_Infor_Tail = *pUsr_Infor;
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
        if(3 == read_num) {
            pUsr_Account_Tail -> pNext_Usr_Account = pUsr_Account_Cur;
            pUsr_Account_Tail = pUsr_Account_Cur;
        } else {
            free(pUsr_Account_Cur);
            pUsr_Account_Cur = NULL;
            break;
        }
    }
    fclose(fp_account);
    (*pUsr_Account) = (*pUsr_Account) -> pNext_Usr_Account;

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
        if(4 == read_num) {
            pUsr_Infor_Tail -> pNext_Usr_Infor = pUsr_Infor_Cur;
            pUsr_Infor_Tail = pUsr_Infor_Cur;
        } else {
            free(pUsr_Infor_Cur);
            pUsr_Infor_Cur = NULL;
            break;
        }
    }
    fclose(fp_infor);
    (*pUsr_Infor) = (*pUsr_Infor) -> pNext_Usr_Infor;
}

/*
 * 检查用户名和密码是否合法, 并确认当前账号的使用权限 (管理员 or 普通用户)  
 */
USR_ROLE role_confirm(pUSR_ACCOUNT pUsr_Account) {
    char usr_name[USR_NAME_LEN] = "", usr_password[USR_PWD_LEN] = "";
    char ch;
    int i;
    pUSR_ACCOUNT pUsr_Search = pUsr_Account;
    USR_ROLE role;

    // 输入用户名
    system("clear");
    printf("Enter the user's name(end with a '#')\nInput: ");
    fflush(stdin);
    fflush(stdout);
    fgets(usr_name, USR_NAME_LEN - 1, stdin);
    usr_name[USR_NAME_LEN] = '#';
    for(i = 0; i < USR_NAME_LEN; ++i)
        if(usr_name[i] == '#')
            usr_name[i] = '\0';
    
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
        }
    }
    if(role == -1) {
        system("clear");
        role = role_confirm(pUsr_Account);
    } else if(role == -2) {
        system("clear");
        printf("Goodbye!\n");
        sleep(2);
        exit(EXIT_SUCCESS);
    } else {
        return role;
    }
}

SYSTEM_MENU_TYPE show_system_menu(USR_ROLE role) {
    SYSTEM_MENU_TYPE type = 0;
    char ch;
    int i, flag;

    switch(role) 
    {
      // 管理员用户的界面
      case 1:
        {
            system("clear");
            printf("********************************************************************************\n");
            printf("*****************Student  Information  Management  System***********************\n");
            printf("********************************************************************************\n");
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
            while(fflush(stdin), (ch = mygetch()) != EOF) {
                if(ch == '1') {  // 管理员用户选择查询功能时, type的返回值为101
                    type = 101;  
                    break;
                }
                if(ch == '2') {
                    
                }
                if(ch == '3') {
                
                }
                if(ch == '4') {
                
                }
                if(ch == '5') {
                
                }
                if(ch == '6') {
                
                }
                if(ch == '7') {
                
                }
                if(ch == '8') {
                
                }
                if(ch == '9') {
                
                }
            }

            break;
        }

      // 普通用户的界面
      case 2:
        {
            system("clear");
            printf("********************************************************************************\n");
            printf("*****************Student  Information  Management  System***********************\n");
            printf("********************************************************************************\n");
            printf("\n\n");
            printf("                          1. search  student  information\n");
            printf("                          9. exit\n");
            printf("\nselect a number: ");
            while(fflush(stdin), (ch = mygetch()) != EOF) {
                if(ch == '1') {  // 普通账户选择查询功能时, type的返回值为102
                    type = 102;  
                    break;
                } 
                if(ch == '9') {
                    printf("\n\n\nDo you really want to exit?(Y/N): ");
                    while(fflush(stdin), (ch = mygetch()) != EOF) {
                        if(!(ch == 'y' || ch == 'Y' || ch == 'n' || ch == 'N'))
                            printf(" \b");
                        if(ch == 'y' || ch == 'Y') {
                            flag = -1;
                            break;
                        }
                        if(ch == 'n' || ch == 'N') {
                            flag = -2;
                            break;
                        }
                    }
                    if(flag == -1) {
                        system("clear");
                        printf("Goodbye!\n");
                        sleep(2);
                        exit(EXIT_SUCCESS);
                    }
                    if(flag == -2) 
                        printf("\nselect a number: ");
                }
            }
            break;
        }
    }
    return type;
}


/*
 * 根据不同的权限显示不同功能的search menu
 */
SYSTEM_MENU_TYPE show_search_menu(SYSTEM_MENU_TYPE type, USR_ROLE role, pUSR_INFOR pUsr_Infor) {
    char ch, stu_name[USR_NAME_LEN] = "";
    int i, id;
    pUSR_INFOR pSearch_all = pUsr_Infor;
    SYSTEM_MENU_TYPE type_tmp;

    switch(type)
    {
      // 管理员用户. 具有4个查询功能
      case 101:
        {
            system("clear");
            printf("0. search all\n");
            printf("1. search by name\n");
            printf("2. search by id\n");
            printf("3. return\n");
            while(fflush(stdin), (ch = mygetch()) != EOF) {
                if(ch == '0') {  // 查询所有学生的信息
                    system("clear");
                    printf("Id\tName\tCourse\tScore\n");
                    while(pSearch_all) {
                        printf("%d\t%s\t%d\t%lf\n", pSearch_all -> usr_id, pSearch_all -> usr_name,
                            pSearch_all -> usr_course_id, pSearch_all -> usr_course_score);
                        pSearch_all = pSearch_all -> pNext_Usr_Infor;
                    }
                    printf("\n\n\nPress any key to continue.");
                    while(fflush(stdin), (ch = mygetch()) != EOF)
                        show_search_menu(type, role, pUsr_Infor);  // 按任意键返回
                    break;
                }
                if(ch == '1') {  // 按名字查询
                    system("clear");
                    printf("Enter the name(end with a '#')\nInput: ");
                    fflush(stdout);
                    fflush(stdin);
                    fgets(stu_name, USR_NAME_LEN - 1, stdin);
                    stu_name[USR_NAME_LEN - 1] = '#';
                    for(i = 0; i < USR_NAME_LEN - 1; ++i)
                        if(stu_name[i] == '#')
                            stu_name[i] = '\0';
                    search_usr_infor_by_name(pUsr_Infor, stu_name);
                    while(fflush(stdin), (ch = mygetch()) != EOF) 
                        show_search_menu(type, role, pUsr_Infor);  // 按任意键返回
                    break; 
                }
                if(ch == '2') {  // 按学号查询
                    system("clear");
                    printf("Enter the id: ");
                    fflush(stdout);
                    fflush(stdin);
                    scanf("%d", &id);
                    search_usr_infor_by_id(pUsr_Infor, id);
                    while(fflush(stdin), (ch = mygetch()) != EOF)
                        show_search_menu(type, role, pUsr_Infor);  // 按任意键返回
                    break;
                }
                if(ch == '3') {
                    type_tmp = show_system_menu(role);
                    show_search_menu(type_tmp, role, pUsr_Infor);
                    break;
                }
            }
            break;    
        }
      
      // 普通账户. 只有3个查询功能
      case 102:
        {
            system("clear");
            printf("1. search by name\n");
            printf("2. search by id\n");
            printf("3. return\n");
            while(fflush(stdin), (ch = mygetch()) != EOF) {
                if(ch == '1') {  // 按名字查询
                    system("clear");
                    printf("Enter the name(end with a '#')\nInput: ");
                    fflush(stdout);
                    fflush(stdin);
                    fgets(stu_name, USR_NAME_LEN - 1, stdin);
                    stu_name[USR_NAME_LEN - 1] = '#';
                    for(i = 0; i < USR_NAME_LEN - 1; ++i)
                        if(stu_name[i] == '#')
                            stu_name[i] = '\0';
                    search_usr_infor_by_name(pUsr_Infor, stu_name);
                    while(fflush(stdin), (ch = mygetch()) != EOF) 
                        show_search_menu(type, role, pUsr_Infor);  // 按任意键返回
                    break; 
                }
                if(ch == '2') {  // 按学号查询
                    system("clear");
                    printf("Enter the id: ");
                    fflush(stdout);
                    fflush(stdin);
                    scanf("%d", &id);
                    search_usr_infor_by_id(pUsr_Infor, id);
                    while(fflush(stdin), (ch = mygetch()) != EOF)
                        show_search_menu(type, role, pUsr_Infor);  // 按任意键返回
                    break;
                }
                if(ch == '3') {
                    type_tmp = show_system_menu(role);
                    show_search_menu(type_tmp, role, pUsr_Infor);
                    break;
                }
            }
            break;
        }
    }
}

void search_usr_infor_by_name(pUSR_INFOR pUsr_Infor_List, char *search_name) {
    pUSR_INFOR pSearch = pUsr_Infor_List;
    while(pSearch) {
        if(strcmp(pSearch -> usr_name, search_name) == 0) {
            system("clear");
            printf("Id\tName\tCourse\tScore\n");
            printf("%d\t%s\t%d\t%lf\n", pSearch -> usr_id, pSearch -> usr_name, 
                    pSearch -> usr_course_id, pSearch -> usr_course_score);
            break;
        }
        pSearch = pSearch -> pNext_Usr_Infor;
    }
    if(NULL == pSearch) {
        system("clear");
        printf("Not Found!\n");
    }
    printf("\n\n\nPress any key to continue.");
}


void search_usr_infor_by_id(pUSR_INFOR pUsr_Infor_List, SEARCH_ID search_id) {
    pUSR_INFOR pSearch = pUsr_Infor_List;
    while(pSearch) {
        if(pSearch -> usr_id == search_id) {
            system("clear");
            printf("Id\tName\tCourse\tScore\n");
            printf("%d\t%s\t%d\t%lf\n", pSearch -> usr_id, pSearch -> usr_name, 
                    pSearch -> usr_course_id, pSearch -> usr_course_score);
            break;
        }
        pSearch = pSearch -> pNext_Usr_Infor;
    }
    if(NULL == pSearch) {
        system("clear");
        printf("Not Found!\n");
    }
    printf("\n\n\nPress any key to continue.");
    getchar();  /* 若不加此句, 则不会显示出查询结果, 为何? */
}



int main()
{
    pUSR_ACCOUNT pUsr_Account = (pUSR_ACCOUNT)calloc(1, sizeof(USR_ACCOUNT));
    pUSR_INFOR pUsr_Infor = (pUSR_INFOR)calloc(1, sizeof(USR_INFOR));
    USR_ROLE role;
    SYSTEM_MENU_TYPE type;
    char config_file_name[N] = "../data/configure.txt";

    system_init(config_file_name, &pUsr_Account, &pUsr_Infor);
    role = role_confirm(pUsr_Account);
    type = show_system_menu(role);
    show_search_menu(type, role, pUsr_Infor);

    return 0;
}
