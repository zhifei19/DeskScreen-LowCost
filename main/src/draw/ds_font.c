/*
*---------------------------------------------------------------
*                        Lvgl Font Tool                         
*                                                               
* 注:使用unicode编码                                              
* 注:本字体文件由Lvgl Font Tool V0.4 生成                          
* 作者:阿里(qq:617622104)                                         
*---------------------------------------------------------------
*/

#include "esp_partition.h"
#include "esp_log.h"

#define FLASH_ADDR_TYPE         0x01
#define FLASH_ADDR_SUBTYPE      0x40

const static char* TAG = "ds_font";

typedef struct{
    uint16_t min;
    uint16_t max;
    uint8_t  bpp;
    uint8_t  reserved[3];
}x_header_t;
typedef struct{
    uint32_t pos;
}x_table_t;
typedef struct{
    uint8_t adv_w;
    uint8_t box_w;
    uint8_t box_h;
    int8_t  ofs_x;
    int8_t  ofs_y;
    uint8_t r;
}glyph_dsc_t;

static x_header_t __g_xbf_hd = {
    .min = 0x0020,
    .max = 0x9fa0,
    .bpp = 4,
};


static uint8_t __g_font_buf[242];//如bin文件存在SPI FLASH可使用此buff

esp_partition_t* partition_font = NULL;

static uint8_t *__user_font_getdata(int offset, int size){
    //如字模保存在SPI FLASH, SPIFLASH_Read(__g_font_buf,offset,size);
    //如字模已加载到SDRAM,直接返回偏移地址即可如:return (uint8_t*)(sdram_fontddr+offset);
    if(partition_font == NULL){
        partition_font = esp_partition_find_first(FLASH_ADDR_TYPE, FLASH_ADDR_SUBTYPE,NULL);
        if (partition_font == NULL)
        {
            ESP_LOGE(TAG,"Failed to open font file for reading");
            return NULL;
        }else
        {
            ESP_LOGI(TAG,"Successfully open font file for reading");
        }
    }
    esp_err_t err = esp_partition_read(partition_font,offset,__g_font_buf,size);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG,"Failed to reading font data %x",offset);
    }
    return __g_font_buf;
}


// static const uint8_t * __user_font_get_bitmap(const lv_font_t * font, uint32_t unicode_letter) {
//     if( unicode_letter>__g_xbf_hd.max || unicode_letter<__g_xbf_hd.min ) {
//         return NULL;
//     }
//     uint32_t unicode_offset = sizeof(x_header_t)+(unicode_letter-__g_xbf_hd.min)*4;
//     uint32_t *p_pos = (uint32_t *)__user_font_getdata(unicode_offset, 4);
//     if( p_pos[0] != 0 ) {
//         uint32_t pos = p_pos[0];
//         glyph_dsc_t * gdsc = (glyph_dsc_t*)__user_font_getdata(pos, sizeof(glyph_dsc_t));
//         return __user_font_getdata(pos+sizeof(glyph_dsc_t), gdsc->box_w*gdsc->box_h*__g_xbf_hd.bpp/8);
//     }
//     return NULL;
// }

static const uint8_t * user_font_get_bitmap(uint32_t unicode_letter,uint8_t *buf) {
    if( unicode_letter>__g_xbf_hd.max || unicode_letter<__g_xbf_hd.min ) {
        return NULL;
    }
    uint32_t unicode_offset = sizeof(x_header_t)+(unicode_letter-__g_xbf_hd.min)*4;
    uint32_t *p_pos = (uint32_t *)__user_font_getdata(unicode_offset, 4);
    if( p_pos[0] != 0 ) {
        uint32_t pos = p_pos[0];
        glyph_dsc_t * gdsc = (glyph_dsc_t*)__user_font_getdata(pos, sizeof(glyph_dsc_t));
        buf = __user_font_getdata(pos+sizeof(glyph_dsc_t), gdsc->box_w*gdsc->box_h*__g_xbf_hd.bpp/8);
        return buf;
    }
    return NULL;
}


static bool user_font_get_glyph_dsc(glyph_dsc_t * dsc_out, uint32_t unicode_letter) {
    if( unicode_letter>__g_xbf_hd.max || unicode_letter<__g_xbf_hd.min ) {
        return NULL;
    }
    uint32_t unicode_offset = sizeof(x_header_t)+(unicode_letter-__g_xbf_hd.min)*4;
    uint32_t *p_pos = (uint32_t *)__user_font_getdata(unicode_offset, 4);
    if( p_pos[0] != 0 ) {
        glyph_dsc_t * gdsc = (glyph_dsc_t*)__user_font_getdata(p_pos[0], sizeof(glyph_dsc_t));
        dsc_out->adv_w = gdsc->adv_w;
        dsc_out->box_h = gdsc->box_h;
        dsc_out->box_w = gdsc->box_w;
        dsc_out->ofs_x = gdsc->ofs_x;
        dsc_out->ofs_y = gdsc->ofs_y;
        // dsc_out->bpp   = __g_xbf_hd.bpp;
        return true;
    }
    return false;
}

void font_test(void)
{
    uint8_t buf[300];
    glyph_dsc_t dsc_out;
    uint8_t size;

    user_font_get_glyph_dsc(&dsc_out,0x0041);
    printf("0x0041 unicode height is %d, weight is %d\n",dsc_out.box_h,dsc_out.box_w);

    user_font_get_bitmap(0x0041,buf); //表示字符A
    printf("Unicode character 0x0041:\n");
    for(int row = 0; row<(dsc_out.box_h); row++)
    {
        for(int col = 0; col<(dsc_out.box_w); col++){
            printf("%x ",buf[row*dsc_out.box_w+col]);
        }
        printf("\n");
    }
}


//YouYuan,,-1
//字模高度：22
//XBF字体,外部bin文件
// lv_font_t myFont2 = {
//     .get_glyph_bitmap = __user_font_get_bitmap,
//     .get_glyph_dsc = __user_font_get_glyph_dsc,
//     .line_height = 22,
//     .base_line = 0,
// };

