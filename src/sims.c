#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sims.h"
#include "conio.h"


pUSR_ACCOUNT pUsr_Account;
pUSR_INFOR pUsr_Infor;
USR_ROLE role;
SYSTEM_MENU_TYPE type;
char config_file_name[N] = "../data/configure.txt";
char account_path[N] = "../data/", infor_path[N] = "../data/";


/*
 * Initialize the account list and the information list.
 */
void system_init(const char *config_file_name, pUSR_ACCOUNT *ppUsr_Account, pUSR_INFOR *ppUsr_Infor) {
    FILE *fp = NULL, *fp_account = NULL, *fp_infor = NULL;
    char config_gets[N] = "";
    char account_left[N] = "", infor_left[N] = "";
    int read_num, cnt;
    pUSR_ACCOUNT pUsr_Account_Cur = NULL, pUsr_Account_Tail = *ppUsr_Account;
    pUSR_INFOR pUsr_Infor_Cur = NULL, pUsr_Infor_Tail = *ppUsr_Infor;

    // Get the contents of 'configure.txt' and complete two path files.
    fp = fopen(config_file_name, "rb");
    if(NULL == fp) {
        fprintf(stderr, "Configure file open error!\n");
        exit(EXIT_FAILURE);
    }
    while(fgets(config_gets, N, fp) != NULL) { }
    sscanf(config_gets, "%s %s", account_left, infor_left);
    strcat(account_path, account_left);
    strcat(infor_path, infor_left);
    fclose(fp);

    // Read the account information.
    fp_account = fopen(account_path, "rb");
    if(NULL == fp_account) {
        fprintf(stderr, "Account file open error!\n");
        exit(EXIT_FAILURE);
    }
    cnt = 0;
    while(1) {
        if(cnt == 0) {
            read_num = fscanf(fp_account, "%s %s %d", pUsr_Account_Tail -> usr_name, pUsr_Account_Tail -> usr_pwd,
                    &pUsr_Account_Tail -> usr_role);
            if(read_num == 3)
                ++cnt;
            else
                continue;
        }
        else if(cnt > 0) {
            pUsr_Account_Cur = (pUSR_ACCOUNT)calloc(1, sizeof(USR_ACCOUNT));
            read_num = fscanf(fp_account, "%s %s %d", pUsr_Account_Cur -> usr_name, pUsr_Account_Cur -> usr_pwd, 
                    &pUsr_Account_Cur -> usr_role);
            if(read_num == 3) {
                pUsr_Account_Tail -> pNext_Usr_Account = pUsr_Account_Cur;
                pUsr_Account_Tail = pUsr_Account_Cur;
            } else {
                free(pUsr_Account_Cur);
                pUsr_Account_Cur = NULL;
                break;
            }
        }
    }
    fclose(fp_account);

    // Read the student information.
    fp_infor = fopen(infor_path, "rb");
    if(NULL == fp_infor) {
        fprintf(stderr, "Student information file open error!\n");
        exit(EXIT_FAILURE);
    }
    cnt = 0;
    while(1) {
        if(cnt == 0) {
            read_num = fscanf(fp_infor, "%d %s %d %lf", &pUsr_Infor_Tail -> usr_id, pUsr_Infor_Tail -> usr_name,
                    &pUsr_Infor_Tail -> usr_course_id, &pUsr_Infor_Tail -> usr_course_score);
            if(read_num == 4)
                ++cnt;
            else
                continue;
        }
        else if(cnt > 0) {
            pUsr_Infor_Cur = (pUSR_INFOR)calloc(1, sizeof(USR_INFOR));
            read_num = fscanf(fp_infor, "%d %s %d %lf", &pUsr_Infor_Cur -> usr_id, pUsr_Infor_Cur -> usr_name, 
                    &pUsr_Infor_Cur -> usr_course_id, &pUsr_Infor_Cur -> usr_course_score);
            if(read_num == 4) {
                pUsr_Infor_Tail -> pNext_Usr_Infor = pUsr_Infor_Cur;
                pUsr_Infor_Tail = pUsr_Infor_Cur;
            } else {
                free(pUsr_Infor_Cur);
                pUsr_Infor_Cur = NULL;
                break;
            }
        }
    }
    fclose(fp_infor);
}


/*
 * Check the validity of 'usr_name' and 'usr_password' to determine the access right of current account.
 */
USR_ROLE role_confirm(pUSR_ACCOUNT pUsr_Account) {
    char ch, usr_name[USR_NAME_LEN] = "", usr_password[USR_PWD_LEN] = "";
    int i, flag;
    pUSR_ACCOUNT pUsr_Search = pUsr_Account;
    USR_ROLE role;

    // Input usr_name
    system("clear");
    printf("Enter the user's name(end with a '#')\nInput: ");
    fflush(stdin);
    fflush(stdout);
    fgets(usr_name, USR_NAME_LEN - 1, stdin);
    usr_name[USR_NAME_LEN] = '#';
    for(i = 0; i < USR_NAME_LEN; ++i)
        if(usr_name[i] == '#')
            usr_name[i] = '\0';

    // Input usr_password
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

    // match what we get with the legal ones.
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
        fflush(stdout);
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
    }
    if(flag == -1) {
        system("clear");
        role = role_confirm(pUsr_Account);
    } 
    if(flag == -2) {
        system("clear");
        printf("Goodbye!\n");
        fflush(stdout);
        sleep(3);
        exit(EXIT_SUCCESS);
    } 
    return role;
}


/*
 * Show the corresponding system menu according to the 'usr_role'
 */
SYSTEM_MENU_TYPE show_system_menu(USR_ROLE role) {
    SYSTEM_MENU_TYPE type = 0;
    char ch, acc_name[USR_NAME_LEN] = "";
    int i, flag;

    switch(role) 
    {
        // Administrator
        case 1:
            system("clear");
            printf("********************************************************************************\n");
            printf("*****************Student  Information  Management  System***********************\n");
            printf("********************************************************************************\n");
            printf("\n\n");
            printf("                          1. search  student  information\n");
            printf("                          2. add     student  information\n");
            printf("                          3. delete  student  information\n");
            printf("                          4. update  student  information\n");
            printf("                          5. add     user     account\n");
            printf("                          6. update  user     account\n");
            printf("                          7. delete  user     account\n");
            printf("                          8. search  user     account\n");
            printf("                          9. exit\n");
            printf("\nselect a number: ");
            fflush(stdout);
            while(fflush(stdin), (ch = mygetch()) != EOF) {
                if(ch == '1') {  // type is 101 if the administrator wants to search.
                    type = 101;  
                    break;
                }
                if(ch == '2') {
                    add_usr_infor(pUsr_Infor, role);
                    show_system_menu(role);
                    break;
                }
                if(ch == '3') {
                    delete_usr_infor(pUsr_Infor, role);
                    show_system_menu(role);
                    break;
                }
                if(ch == '4') {
                    update_usr_infor(pUsr_Infor, role);
                    show_system_menu(role);
                    break;
                }
                if(ch == '5') {
                    add_usr_account(pUsr_Account, role);
                    show_system_menu(role);
                    break;
                }
                if(ch == '6') {
                    update_usr_account(pUsr_Account, role);
                    show_system_menu(role);
                    break;
                }
                if(ch == '7') {
                    delete_usr_account(pUsr_Account, role);
                    show_system_menu(role);
                    break;
                }
                if(ch == '8') {
                    system("clear");
                    printf("Enter the name(end with a '#')\nInput: ");
                    fflush(stdout);
                    fflush(stdin);
                    fgets(acc_name, USR_NAME_LEN - 1, stdin);
                    acc_name[USR_NAME_LEN - 1] = '#';
                    for(i = 0; i < USR_NAME_LEN - 1; ++i)
                        if(acc_name[i] == '#')
                            acc_name[i] = '\0';
                    search_usr_account(pUsr_Account, acc_name);
                    printf("\n\n\nPress any key to return.");
                    fflush(stdout);
                    while(fflush(stdin), (ch = mygetch()) != EOF) 
                        show_system_menu(role);  // press any key to return
                    break; 
                }
                if(ch == '9') {
                    printf("\n\n\nDo you really want to exit?(Y/N): ");
                    fflush(stdout);
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
                        fflush(stdout);
                        sleep(2);
                        exit(EXIT_SUCCESS);
                    }
                    if(flag == -2) {
                        printf("\nselect a number: ");
                        fflush(stdout);
                    }
                }
            }
            break;

            // User
        case 2:
            system("clear");
            printf("********************************************************************************\n");
            printf("*****************Student  Information  Management  System***********************\n");
            printf("********************************************************************************\n");
            printf("\n\n");
            printf("                          1. search  student  information\n");
            printf("                          9. exit\n");
            printf("\nselect a number: ");
            fflush(stdout);
            while(fflush(stdin), (ch = mygetch()) != EOF) {
                if(ch == '1') {  // type is 102 if an user wants to search.
                    type = 102;  
                    break;
                } 
                if(ch == '9') {
                    printf("\n\n\nDo you really want to exit?(Y/N): ");
                    fflush(stdout);
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
                        fflush(stdout);
                        sleep(2);
                        exit(EXIT_SUCCESS);
                    }
                    if(flag == -2) {
                        printf("\nselect a number: ");
                        fflush(stdout);
                    }
                }
            }
            break;
    }

    return type;
}


/* 
 * Show the corresponding search menu according to the 'usr_role'
 */
SYSTEM_MENU_TYPE show_search_menu(SYSTEM_MENU_TYPE type, USR_ROLE role, pUSR_INFOR pUsr_Infor) {
    char ch, stu_name[USR_NAME_LEN] = "";
    int i, id;
    pUSR_INFOR pSearch_all = pUsr_Infor;
    SYSTEM_MENU_TYPE type_tmp;

    switch(type)
    {
        // Administrator, which has 4 options.
        case 101:
            system("clear");
            printf("0. search all\n");
            printf("1. search by name\n");
            printf("2. search by id\n");
            printf("3. return\n");
            fflush(stdout);
            while(fflush(stdin), (ch = mygetch()) != EOF) {
                if(ch == '0') {  // search all
                    system("clear");
                    printf("Id\tName\tCourse\tScore\n");
                    while(pSearch_all) {
                        printf("%d\t%s\t%d\t%lf\n", pSearch_all -> usr_id, pSearch_all -> usr_name,
                                pSearch_all -> usr_course_id, pSearch_all -> usr_course_score);
                        pSearch_all = pSearch_all -> pNext_Usr_Infor;
                    }
                    printf("\n\n\nPress any key to continue.");
                    fflush(stdout);
                    while(fflush(stdin), (ch = mygetch()) != EOF)
                        show_search_menu(type, role, pUsr_Infor);  // press any key to return
                    break;
                }
                if(ch == '1') {  // search by name
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
                    printf("\n\n\nPress any key to return.");
                    fflush(stdout);
                    while(fflush(stdin), (ch = mygetch()) != EOF) 
                        show_search_menu(type, role, pUsr_Infor);  // press any key to return
                    break; 
                }
                if(ch == '2') {  // search by ID
                    system("clear");
                    printf("Enter the id: ");
                    fflush(stdout);
                    fflush(stdin);
                    scanf("%d", &id);
                    search_usr_infor_by_id(pUsr_Infor, id);
                    printf("\n\n\nPress any key to return.");
                    fflush(stdout);
                    getchar();
                    while(fflush(stdin), (ch = mygetch()) != EOF)
                        show_search_menu(type, role, pUsr_Infor);  // press any key to return 
                    break;
                }
                if(ch == '3') {
                    type_tmp = show_system_menu(role);
                    show_search_menu(type_tmp, role, pUsr_Infor);
                    break;
                }
            }
            break;    

            // User, which has 3 options.
        case 102:
            system("clear");
            printf("1. search by name\n");
            printf("2. search by id\n");
            printf("3. return\n");
            fflush(stdout);
            while(fflush(stdin), (ch = mygetch()) != EOF) {
                if(ch == '1') {  // search by name
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
                    printf("\n\n\nPress any key to return.");
                    fflush(stdout);
                    while(fflush(stdin), (ch = mygetch()) != EOF) 
                        show_search_menu(type, role, pUsr_Infor);  // press any key to return
                    break; 
                }
                if(ch == '2') {  // search by id
                    system("clear");
                    printf("Enter the id: ");
                    fflush(stdout);
                    fflush(stdin);
                    scanf("%d", &id);
                    search_usr_infor_by_id(pUsr_Infor, id);
                    printf("\n\n\nPress any key to return.");
                    fflush(stdout);
                    getchar();
                    while(fflush(stdin), (ch = mygetch()) != EOF)
                        show_search_menu(type, role, pUsr_Infor);  // press any key to return 
                    break;
                }
                if(ch == '3') {  // return
                    type_tmp = show_system_menu(role);
                    show_search_menu(type_tmp, role, pUsr_Infor);
                    break;
                }
            }
            break;
    }
}


/*
 * Search by name
 */
void search_usr_infor_by_name(pUSR_INFOR pUsr_Infor_List, char *search_name) {
    pUSR_INFOR pSearch = pUsr_Infor_List;
    while(pSearch) {
        if(strcmp(pSearch -> usr_name, search_name) == 0) {
            system("clear");
            printf("Id\tName\tCourse\tScore\n");
            printf("%d\t%s\t%d\t%lf\n", pSearch -> usr_id, pSearch -> usr_name, 
                    pSearch -> usr_course_id, pSearch -> usr_course_score);
            fflush(stdout);
            break;
        }
        pSearch = pSearch -> pNext_Usr_Infor;
    }
    if(NULL == pSearch) {
        system("clear");
        printf("Not Found!\n");
        fflush(stdout);
        sleep(2);
    }
}


/*
 * Search by ID
 */
void search_usr_infor_by_id(pUSR_INFOR pUsr_Infor_List, SEARCH_ID search_id) {
    pUSR_INFOR pSearch = pUsr_Infor_List;
    while(pSearch) {
        if(pSearch -> usr_id == search_id) {
            system("clear");
            printf("Id\tName\tCourse\tScore\n");
            printf("%d\t%s\t%d\t%lf\n", pSearch -> usr_id, pSearch -> usr_name, 
                    pSearch -> usr_course_id, pSearch -> usr_course_score);
            fflush(stdout);
            break;
        }
        pSearch = pSearch -> pNext_Usr_Infor;
    }
    if(NULL == pSearch) {
        system("clear");
        printf("Not Found!\n");
        fflush(stdout);
        sleep(2);
    }
}


/*
 * Add a user information
 */
void add_usr_infor(pUSR_INFOR pUsr_Infor_List, USR_ROLE role) {
    pUSR_INFOR pUsr_Infor_List_New = NULL;
    pUSR_INFOR pUsr_Infor_List_Pre = NULL;
    pUSR_INFOR pUsr_Infor_List_Cur = pUsr_Infor_List;
    SYSTEM_MENU_TYPE type_tmp;
    int has_same = 0;
    int add_id, add_course_id;
    char add_usr_name[USR_NAME_LEN] = "";
    double add_course_score;
    FILE *fp = NULL;

    system("clear");
    printf("Input the student's ID to add: ");
    fflush(stdin);
    fflush(stdout);
    scanf("%d", &add_id);

    // Check if there is the same ID.
    while(pUsr_Infor_List_Cur) {
        if(pUsr_Infor_List_Cur -> usr_id == add_id) {
            has_same = 1;
            break;
        }
        pUsr_Infor_List_Cur = pUsr_Infor_List_Cur -> pNext_Usr_Infor;
    }

    if(has_same) {  // the ID we want to add already exists in the list!
        system("clear");
        printf("The ID: '%d' exists!", add_id);
        fflush(stdout);
        sleep(2);
        return ;
    }
    else {  // we can add now
        system("clear");
        printf("Input new student's name, course_id and course_score: ");
        fflush(stdout);
        while(1) {
            fflush(stdin);
            if(3 == fscanf(stdin, "%s %d %lf", add_usr_name, &add_course_id, &add_course_score))
                break;
            else {
                system("clear");
                printf("Error! Input three items(i.e. name, course_id and course_score):\n");
                fflush(stdout);
            }
        }

        // Insert the new information node into the information list
        pUsr_Infor_List_New = (pUSR_INFOR)calloc(1, sizeof(USR_INFOR));
        pUsr_Infor_List_New -> usr_id = add_id;
        strcpy(pUsr_Infor_List_New -> usr_name, add_usr_name);
        pUsr_Infor_List_New -> usr_course_id = add_course_id;
        pUsr_Infor_List_New -> usr_course_score = add_course_score;
        pUsr_Infor_List_Cur = pUsr_Infor_List;
        while(pUsr_Infor_List_Cur) {
            if(pUsr_Infor_List_New -> usr_id > pUsr_Infor_List_Cur -> usr_id) {
                pUsr_Infor_List_Pre = pUsr_Infor_List_Cur;
                pUsr_Infor_List_Cur = pUsr_Infor_List_Cur -> pNext_Usr_Infor;
            } else {
                break;
            }
        }
        if(NULL == pUsr_Infor_List_Pre) {
            pUsr_Infor_List_New -> pNext_Usr_Infor = pUsr_Infor_List;
            pUsr_Infor_List = pUsr_Infor_List_New;
        } else {
            pUsr_Infor_List_New -> pNext_Usr_Infor = pUsr_Infor_List_Pre -> pNext_Usr_Infor;
            pUsr_Infor_List_Pre -> pNext_Usr_Infor = pUsr_Infor_List_New;
        }

        // write back
        pUsr_Infor_List_Cur = pUsr_Infor_List;
        fp = fopen(infor_path, "wb");
        if(NULL == fp) {
            fprintf(stderr, "Student information file open error!\n");
            exit(EXIT_FAILURE);
        }
        while(pUsr_Infor_List_Cur) {
            while(4 == fprintf(fp, "%d %s %d %lf", pUsr_Infor_List_Cur -> usr_id, pUsr_Infor_List_Cur -> usr_name,
                        pUsr_Infor_List_Cur -> usr_course_id, pUsr_Infor_List_Cur -> usr_course_score))
            { }
            fputc('\n', fp);
            pUsr_Infor_List_Cur = pUsr_Infor_List_Cur -> pNext_Usr_Infor;
        }
        fclose(fp);
        system("clear");
        printf("Add success!");
        fflush(stdout);
        sleep(2);
    }
}


/*
 * Delete a user information
 */
void delete_usr_infor(pUSR_INFOR pUsr_Infor_List, USR_ROLE role) {
    pUSR_INFOR pUsr_Infor_List_Cur = pUsr_Infor_List;
    pUSR_INFOR pUsr_Infor_List_Pre = NULL;
    int del_id, flag = 0;
    FILE *fp = NULL;

    system("clear");
    printf("Input the ID you want to delete: ");
    fflush(stdout);
    fflush(stdin);
    scanf("%d", &del_id);

    // check if there is the ID to delete.
    while(pUsr_Infor_List_Cur) {
        if(pUsr_Infor_List_Cur -> usr_id != del_id) {
            pUsr_Infor_List_Pre = pUsr_Infor_List_Cur;
            pUsr_Infor_List_Cur = pUsr_Infor_List_Cur -> pNext_Usr_Infor;
        } else {  // we find the node
            if(NULL == pUsr_Infor_List_Pre)  // It is the first node
                pUsr_Infor_List = pUsr_Infor_List -> pNext_Usr_Infor;
            else  // It is not the first node
                pUsr_Infor_List_Pre -> pNext_Usr_Infor = pUsr_Infor_List_Cur -> pNext_Usr_Infor;
            free(pUsr_Infor_List_Cur);
            pUsr_Infor_List_Cur = NULL;
            flag = 1;
            break;
        }
    }
    if(!flag) {  // Not found
        system("clear");
        printf("The ID: '%d' not found!", del_id);
        fflush(stdout);
        sleep(2);
        return ;
    }
    if(flag) {  // write back 
        fp = fopen(infor_path, "wb");
        if(NULL == fp) {
            fprintf(stderr, "Student information file open error!\n");
            exit(EXIT_FAILURE);
        }
        pUsr_Infor_List_Cur = pUsr_Infor_List;
        while(pUsr_Infor_List_Cur) {
            while(4 == fprintf(fp, "%d %s %d %lf", pUsr_Infor_List_Cur -> usr_id, pUsr_Infor_List_Cur -> usr_name,
                        pUsr_Infor_List_Cur -> usr_course_id, pUsr_Infor_List_Cur -> usr_course_score));
            { }
            fputc('\n', fp);
            pUsr_Infor_List_Cur = pUsr_Infor_List_Cur -> pNext_Usr_Infor;
        }
        fclose(fp);
        system("clear");
        printf("Delete success!");
        fflush(stdout);
        sleep(2);
    }
}


/*
 * Update a user information (We can't modify the usr_id, but the other three elements)
 */
void update_usr_infor(pUSR_INFOR pUsr_Infor_List, USR_ROLE role) {
    pUSR_INFOR pUsr_Infor_List_Cur = pUsr_Infor_List;
    int id_for_search;
    int update_course_id;
    char update_usr_name[USR_NAME_LEN] = "";
    double update_course_score;
    FILE *fp = NULL;

    system("clear");
    printf("Input the student's ID to update: ");
    fflush(stdin);
    fflush(stdout);
    scanf("%d", &id_for_search);

    // Check if there is the id we want to update.
    while(pUsr_Infor_List_Cur) {
        if(pUsr_Infor_List_Cur -> usr_id == id_for_search)
            break;
        pUsr_Infor_List_Cur = pUsr_Infor_List_Cur -> pNext_Usr_Infor;
    }
    if(NULL == pUsr_Infor_List_Cur) {  // the ID doesn't exist
        system("clear");
        printf("The ID: '%d' doesn't exist!", id_for_search);
        fflush(stdout);
        sleep(2);
        return ;
    }
    else {  // we can update now
        system("clear");
        printf("Input the new information(i.e. name, course_id and course_score): ");
        fflush(stdout);
        while(1) {
            fflush(stdin);
            if(3 == fscanf(stdin, "%s %d %lf", update_usr_name, &update_course_id, &update_course_score))
                break;
            else {
                system("clear");
                printf("Error! Input three items(i.e. name, course_id and course_score):\n");
                fflush(stdout);
            }
        }
        strcpy(pUsr_Infor_List_Cur -> usr_name, update_usr_name);
        pUsr_Infor_List_Cur -> usr_course_id = update_course_id;
        pUsr_Infor_List_Cur -> usr_course_score = update_course_score;
        
        // write back
        pUsr_Infor_List_Cur = pUsr_Infor_List;
        fp = fopen(infor_path, "wb");
        if(NULL == fp) {
            fprintf(stderr, "Student information file open error!\n");
            exit(EXIT_FAILURE);
        }
        while(pUsr_Infor_List_Cur) {
            while(4 == fprintf(fp, "%d %s %d %lf", pUsr_Infor_List_Cur -> usr_id, pUsr_Infor_List_Cur -> usr_name,
                        pUsr_Infor_List_Cur -> usr_course_id, pUsr_Infor_List_Cur -> usr_course_score))
            { }
            fputc('\n', fp);
            pUsr_Infor_List_Cur = pUsr_Infor_List_Cur -> pNext_Usr_Infor;
        }
        fclose(fp);
        system("clear");
        printf("Update success!");
        fflush(stdout);
        sleep(2);
    }
}


/*
 * Add a user account
 */
void add_usr_account(pUSR_ACCOUNT pUsr_Account_List, USR_ROLE role) {
    pUSR_ACCOUNT pUsr_Account_List_New = NULL;
    pUSR_ACCOUNT pUsr_Account_List_Cur = pUsr_Account_List;
    SYSTEM_MENU_TYPE type_tmp;
    int has_same = 0;
    int add_role;
    char add_account_name[USR_NAME_LEN] = "", add_account_password[USR_NAME_LEN] = "";
    FILE *fp = NULL;

    system("clear");
    printf("Input the account's name to add(Don't end with '#' this time): ");
    fflush(stdin);
    fflush(stdout);
    fscanf(stdin, "%s", add_account_name);

    // Check if there is the same name.
    while(pUsr_Account_List_Cur) {
        if(strcmp(pUsr_Account_List_Cur -> usr_name, add_account_name) == 0) {
            has_same = 1;
            break;
        }
        pUsr_Account_List_Cur = pUsr_Account_List_Cur -> pNext_Usr_Account;
    }

    if(has_same) {  // the name we want to add already exists in the list!
        system("clear");
        printf("The Name: '%s' exists!", add_account_name);
        fflush(stdout);
        sleep(2);
        return ;
    }
    else {  // we can add now
        system("clear");
        printf("Input new password and role: ");
        fflush(stdout);
        while(1) {
            fflush(stdin);
            if(2 == fscanf(stdin, "%s %d", add_account_password, &add_role))
                break;
            else {
                system("clear");
                printf("Error! Input two items(i.e. the password and role):\n");
                fflush(stdout);
            }
        }

        // Insert the new account node into the account list
        pUsr_Account_List_New = (pUSR_ACCOUNT)calloc(1, sizeof(USR_ACCOUNT));
        strcpy(pUsr_Account_List_New -> usr_name, add_account_name);
        strcpy(pUsr_Account_List_New -> usr_pwd, add_account_password);
        pUsr_Account_List_New -> usr_role = add_role;

        pUsr_Account_List_Cur = pUsr_Account_List;
        while(pUsr_Account_List_Cur -> pNext_Usr_Account)
            pUsr_Account_List_Cur = pUsr_Account_List_Cur -> pNext_Usr_Account;
        pUsr_Account_List_Cur -> pNext_Usr_Account = pUsr_Account_List_New;

        // write back
        pUsr_Account_List_Cur = pUsr_Account_List;
        fp = fopen(account_path, "wb");
        if(NULL == fp) {
            fprintf(stderr, "Account file open error!\n");
            exit(EXIT_FAILURE);
        }
        while(pUsr_Account_List_Cur) {
            while(3 == fprintf(fp, "%s %s %d", pUsr_Account_List_Cur -> usr_name, pUsr_Account_List_Cur -> usr_pwd,
                        pUsr_Account_List_Cur -> usr_role))
            { }
            fputc('\n', fp);
            pUsr_Account_List_Cur = pUsr_Account_List_Cur -> pNext_Usr_Account;
        }
        fclose(fp);
        system("clear");
        printf("Add success!");
        fflush(stdout);
        sleep(2);
    }
}


/*
 * Search a user account by its name
 */
void search_usr_account(pUSR_ACCOUNT pUsr_Account_List, char *search_name) {
    pUSR_ACCOUNT pSearch = pUsr_Account_List;
    while(pSearch) {
        if(strcmp(pSearch -> usr_name, search_name) == 0) {
            system("clear");
            printf("Acc\tPwd\tRole\n");
            printf("%s\t%s\t%d\n", pSearch -> usr_name, pSearch -> usr_pwd, pSearch -> usr_role);
            fflush(stdout);
            break;
        }
        pSearch = pSearch -> pNext_Usr_Account;
    }
    if(NULL == pSearch) {
        system("clear");
        printf("Not Found!\n");
        fflush(stdout);
        sleep(2);
    }
}


/*
 * Delete a user account
 */
void delete_usr_account(pUSR_ACCOUNT pUsr_Account_List, USR_ROLE role) {
    pUSR_ACCOUNT pUsr_Account_List_Cur = pUsr_Account_List;
    pUSR_ACCOUNT pUsr_Account_List_Pre = NULL;
    char del_name[USR_NAME_LEN] = "";
    int flag = 0;
    FILE *fp = NULL;

    system("clear");
    printf("Input the usr_name you want to delete: ");
    fflush(stdout);
    fflush(stdin);
    fscanf(stdin, "%s", del_name);

    // check if there is the usr_name to delete.
    while(pUsr_Account_List_Cur) {
        if(strcmp(pUsr_Account_List_Cur -> usr_name, del_name) != 0) {
            pUsr_Account_List_Pre = pUsr_Account_List_Cur;
            pUsr_Account_List_Cur = pUsr_Account_List_Cur -> pNext_Usr_Account;
        } else {  // we find the node
            if(NULL == pUsr_Account_List_Pre)  // It is the first node
                pUsr_Account_List = pUsr_Account_List -> pNext_Usr_Account;
            else  // It is not the first node
                pUsr_Account_List_Pre -> pNext_Usr_Account = pUsr_Account_List_Cur -> pNext_Usr_Account;
            free(pUsr_Account_List_Cur);
            pUsr_Account_List_Cur = NULL;
            flag = 1;
            break;
        }
    }
    if(!flag) {  // Not found
        system("clear");
        printf("The name: '%s' not found!", del_name);
        fflush(stdout);
        sleep(2);
        return ;
    }
    if(flag) {  // write back 
        fp = fopen(account_path, "wb");
        if(NULL == fp) {
            fprintf(stderr, "Account file open error!\n");
            exit(EXIT_FAILURE);
        }
        pUsr_Account_List_Cur = pUsr_Account_List;
        while(pUsr_Account_List_Cur) {
            while(3 == fprintf(fp, "%s %s %d", pUsr_Account_List_Cur -> usr_name, pUsr_Account_List_Cur -> usr_pwd,
                        pUsr_Account_List_Cur -> usr_role))
            { }
            fputc('\n', fp);
            pUsr_Account_List_Cur = pUsr_Account_List_Cur -> pNext_Usr_Account;
        }
        fclose(fp);
        system("clear");
        printf("Delete success!");
        fflush(stdout);
        sleep(2);
    }
}


/*
 * Update an account information (We can't modify the usr_name, but the other two elements)
 */
void update_usr_account(pUSR_ACCOUNT pUsr_Account_List, USR_ROLE role) {
    pUSR_ACCOUNT pUsr_Account_List_Cur = pUsr_Account_List;
    char name_for_search[USR_NAME_LEN] = "";
    char update_usr_pwd[USR_NAME_LEN] = "";
    USR_ROLE update_usr_role;
    FILE *fp = NULL;

    system("clear");
    printf("Input the account's name to update: ");
    fflush(stdin);
    fflush(stdout);
    fscanf(stdin, "%s", name_for_search);

    // Check if there is the name we want to update.
    while(pUsr_Account_List_Cur) {
        if(strcmp(pUsr_Account_List_Cur -> usr_name, name_for_search) == 0)
            break;
        pUsr_Account_List_Cur = pUsr_Account_List_Cur -> pNext_Usr_Account;
    }
    if(NULL == pUsr_Account_List_Cur) {  // the name doesn't exist
        system("clear");
        printf("The name: '%s' doesn't exist!", name_for_search);
        fflush(stdout);
        sleep(2);
        return ;
    }
    else {  // we can update now
        system("clear");
        printf("Input the new password and role: ");
        fflush(stdout);
        while(1) {
            fflush(stdin);
            if(2 == fscanf(stdin, "%s %d", update_usr_pwd, &update_usr_role))
                break;
            else {
                system("clear");
                printf("Error! Input two items(i.e. the password and role):\n");
                fflush(stdout);
            }
        }
        strcpy(pUsr_Account_List_Cur -> usr_pwd, update_usr_pwd);
        pUsr_Account_List_Cur -> usr_role = update_usr_role;
        
        // write back
        pUsr_Account_List_Cur = pUsr_Account_List;
        fp = fopen(account_path, "wb");
        if(NULL == fp) {
            fprintf(stderr, "Account file open error!\n");
            exit(EXIT_FAILURE);
        }
        while(pUsr_Account_List_Cur) {
            while(3 == fprintf(fp, "%s %s %d", pUsr_Account_List_Cur -> usr_name, pUsr_Account_List_Cur -> usr_pwd,
                        pUsr_Account_List_Cur -> usr_role))
            { }
            fputc('\n', fp);
            pUsr_Account_List_Cur = pUsr_Account_List_Cur -> pNext_Usr_Account;
        }
        fclose(fp);
        system("clear");
        printf("Update success!");
        fflush(stdout);
        sleep(2);
    }
}



int main()
{
    pUsr_Account = (pUSR_ACCOUNT)calloc(1, sizeof(USR_ACCOUNT));
    pUsr_Infor = (pUSR_INFOR)calloc(1, sizeof(USR_INFOR));

    system_init(config_file_name, &pUsr_Account, &pUsr_Infor);
    role = role_confirm(pUsr_Account);
    type = show_system_menu(role);
    show_search_menu(type, role, pUsr_Infor);

    return 0;
}
