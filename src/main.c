#include "sims.h"

USR_ACCOUNT pUsr_Account_Head;
USR_INFOR pUsr_Infor_Head;
pUSR_ACCOUNT pUsr_Account = &pUsr_Account_Head;
pUSR_INFOR pUsr_Infor = &pUsr_Infor_Head;
char config_file_name[N] = "../data/configure.txt";
USR_ROLE role;
SYSTEM_MENU_TYPE type;

int main()
{
    system_init(config_file_name, pUsr_Account, pUsr_Infor);
    role = role_confirm(pUsr_Account);    
    type = show_system_menu(role);
    return 0;
}
