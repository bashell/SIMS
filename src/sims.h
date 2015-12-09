#ifndef SIMS_H
#define SIMS_H

#define USR_NAME_LEN 10
#define USR_PWD_LEN 10
#define N 1024

typedef int USR_ROLE;
typedef int USR_ACCOUNT_NUM;
typedef int USR_INFOR_NUM;
typedef int SYSTEM_MENU_TYPE;
typedef int SEARCH_ID;

typedef struct usr_account_tag {
    char usr_name[USR_NAME_LEN];
    char usr_pwd[USR_PWD_LEN];
    USR_ROLE usr_role;
    struct usr_account_tag *pNext_Usr_Account;
} USR_ACCOUNT, *pUSR_ACCOUNT;

typedef struct usr_infor_tag {
   int usr_id;
   char usr_name[USR_NAME_LEN];
   int usr_course_id;
   double usr_course_score;
   struct usr_infor_tag *pNext_Usr_Infor;
} USR_INFOR, *pUSR_INFOR;

void system_init(const char *config_file_name, pUSR_ACCOUNT *pUsr_Account, pUSR_INFOR *pUsr_Infor);
USR_ROLE role_confirm(pUSR_ACCOUNT pUsr_Account);
SYSTEM_MENU_TYPE show_system_menu(USR_ROLE role);
SYSTEM_MENU_TYPE show_search_menu(SYSTEM_MENU_TYPE type, USR_ROLE role, pUSR_INFOR pUsr_Infor);
void search_usr_infor_by_name(pUSR_INFOR pUsr_Infor_List, char *search_name);
void search_usr_infor_by_id(pUSR_INFOR pUsr_Infor_List, SEARCH_ID search_id);


#endif  // SIMS_H
