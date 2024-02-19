#include "ds_ui_menupage.h"
#include "ds_screen.h"
#include "ds_data_image.h"


void ds_ui_show_menu(void)
{
  EPD_Dis_Part(0, 0, gImage_main_page, 200, 200); // x,y,DATA,resolution 200*200

}
