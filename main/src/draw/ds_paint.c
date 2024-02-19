/******************************************************************************
* | File      	:   GUI_Paint.c
* | Author      :   Waveshare electronics
* | Function    :	Achieve drawing: draw points, lines, boxes, circles and
*                   their size, solid dotted line, solid rectangle hollow
*                   rectangle, solid circle hollow circle.
* | Info        :
*   Achieve display characters: Display a single character, string, number
*   Achieve time display: adaptive size display time minutes and seconds
*----------------
* |	This version:   V3.1
* | Date        :   2020-07-08
* | Info        :
* -----------------------------------------------------------------------------
* V3.1(2020-07-08):
* 1.Change: Paint_SetScale(UBYTE scale)
*		 Add scale 7 for 5.65f e-Parper
* 2.Change: Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color)
*		 Add the branch for scale 7
* 3.Change: Paint_Clear(UWORD Color)
*		 Add the branch for scale 7
*
* -----------------------------------------------------------------------------
* V3.0(2019-04-18):
* 1.Change: 
*    Paint_DrawPoint(..., DOT_STYLE DOT_STYLE)
* => Paint_DrawPoint(..., DOT_STYLE Dot_Style)
*    Paint_DrawLine(..., LINE_STYLE Line_Style, DOT_PIXEL Dot_Pixel)
* => Paint_DrawLine(..., DOT_PIXEL Line_width, LINE_STYLE Line_Style)
*    Paint_DrawRectangle(..., DRAW_FILL Filled, DOT_PIXEL Dot_Pixel)
* => Paint_DrawRectangle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
*    Paint_DrawCircle(..., DRAW_FILL Draw_Fill, DOT_PIXEL Dot_Pixel)
* => Paint_DrawCircle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Filll)
*
* -----------------------------------------------------------------------------
* V2.0(2018-11-15):
* 1.add: Paint_NewImage()
*    Create an image's properties
* 2.add: Paint_SelectImage()
*    Select the picture to be drawn
* 3.add: Paint_SetRotate()
*    Set the direction of the cache    
* 4.add: Paint_RotateImage() 
*    Can flip the picture, Support 0-360 degrees, 
*    but only 90.180.270 rotation is better
* 4.add: Paint_SetMirroring() 
*    Can Mirroring the picture, horizontal, vertical, origin
* 5.add: Paint_DrawString_CN() 
*    Can display Chinese(GB1312)   
*
* ----------------------------------------------------------------------------- 
* V1.0(2018-07-17):
*   Create library
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documnetation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to  whom the Software is
* furished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
******************************************************************************/

#include "esp_log.h"
#include "ds_paint.h"
#include "ds_font.h"
#include "ds_screen.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "ds_paint";

PAINT Paint;

uint8_t UTF8toUnicode(uint8_t *ch, uint16_t *_unicode)
{
    uint8_t *p = NULL ,n = 0;
    uint32_t e = 0;
    p = ch;
    if(1)//p == NULL
    {
            if(*p >= 0xfc)
            {
                    /*6:<11111100>*/
                    e  = (p[0] & 0x01) << 30;
                    e |= (p[1] & 0x3f) << 24;
                    e |= (p[2] & 0x3f) << 18;
                    e |= (p[3] & 0x3f) << 12;
                    e |= (p[4] & 0x3f) << 6;
                    e |= (p[5] & 0x3f);
                    n = 6;
            }
            else if(*p >= 0xf8)
            {
                    /*5:<11111000>*/
                    e = (p[0] & 0x03) << 24;
                    e |= (p[1] & 0x3f) << 18;
                    e |= (p[2] & 0x3f) << 12;
                    e |= (p[3] & 0x3f) << 6;
                    e |= (p[4] & 0x3f);
                    n = 5;
            }
            else if(*p >= 0xf0)
            {
                    /*4:<11110000>*/
                    e = (p[0] & 0x07) << 18;
                    e |= (p[1] & 0x3f) << 12;
                    e |= (p[2] & 0x3f) << 6;
                    e |= (p[3] & 0x3f);
                    n = 4;
            }
            else if(*p >= 0xe0)
            {
                    /*3:<11100000>*/
                    e = (p[0] & 0x0f) << 12;
                    e |= (p[1] & 0x3f) << 6;
                    e |= (p[2] & 0x3f);
                    n = 3;
            }
            else if(*p >= 0xc0)
            {
                    /*2:<11000000>*/
                    e = (p[0] & 0x1f) << 6;
                    e |= (p[1] & 0x3f);
                    n = 2;
            }
            else
            {
                    e = p[0];           
                    n = 1;
            }
            *_unicode = e;
    }
    return n;
}



// uint32_t utf8_to_unicode(const char* utf8) {
//     uint32_t unicode = 0;
//     int shift = 0;

//     while (*utf8) {
//         unsigned char byte = (unsigned char)(*utf8);
//         if ((byte & 0x80) == 0) {
//             // Single-byte UTF-8 character
//             unicode |= (uint32_t)byte << shift;
//             break;
//         } else if ((byte & 0xE0) == 0xC0) {
//             // Two-byte UTF-8 character
//             unicode |= (uint32_t)(byte & 0x1F) << shift;
//             shift += 6;
//         } else if ((byte & 0xF0) == 0xE0) {
//             // Three-byte UTF-8 character
//             unicode |= (uint32_t)(byte & 0x0F) << shift;
//             shift += 12;
//         } else if ((byte & 0xF8) == 0xF0) {
//             // Four-byte UTF-8 character
//             unicode |= (uint32_t)(byte & 0x07) << shift;
//             shift += 18;
//         } else {
//             // Invalid UTF-8 sequence
//             return 0;
//         }

//         utf8++;
//     }

//     return unicode;
// }

// int main() {
//     const char* utf8_string = "你好"; // Replace with your UTF-8 string
//     uint32_t unicode = utf8_to_unicode(utf8_string);
    
//     if (unicode != 0) {
//         printf("UTF-8: %s\nUnicode: U+%04X\n", utf8_string, unicode);
//     } else {
//         printf("Invalid UTF-8 sequence\n");
//     }

//     return 0;
// }

/******************************************************************************
function: Create Image
parameter:
    image   :   Pointer to the image cache
    width   :   The width of the picture
    Height  :   The height of the picture
    Color   :   Whether the picture is inverted
******************************************************************************/
void Paint_NewImage(UBYTE *image, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color)
{
    Paint.Image = NULL;
    Paint.Image = image;

    Paint.WidthMemory = Width;
    Paint.HeightMemory = Height;
    Paint.Color = Color;    
	Paint.Scale = 2;
		
    Paint.WidthByte = (Width % 8 == 0)? (Width / 8 ): (Width / 8 + 1);
    Paint.HeightByte = Height;    
//    printf("WidthByte = %d, HeightByte = %d\r\n", Paint.WidthByte, Paint.HeightByte);
//    printf(" EPD_WIDTH / 8 = %d\r\n",  122 / 8);
   
    Paint.Rotate = Rotate;
    Paint.Mirror = MIRROR_NONE;
    
    if(Rotate == ROTATE_0 || Rotate == ROTATE_180) {
        Paint.Width = Width;
        Paint.Height = Height;
    } else {
        Paint.Width = Height;
        Paint.Height = Width;
    }
}

/******************************************************************************
function: Select Image
parameter:
    image : Pointer to the image cache
******************************************************************************/
void Paint_SelectImage(UBYTE *image)
{
    Paint.Image = image;
}

/******************************************************************************
function: Select Image Rotate
parameter:
    Rotate : 0,90,180,270
******************************************************************************/
void Paint_SetRotate(UWORD Rotate)
{
    if(Rotate == ROTATE_0 || Rotate == ROTATE_90 || Rotate == ROTATE_180 || Rotate == ROTATE_270) {
        ESP_LOGI(TAG,"Set image Rotate %d\r\n", Rotate);
        Paint.Rotate = Rotate;
    } else {
        ESP_LOGI(TAG,"rotate = 0, 90, 180, 270\r\n");
    }
}

void Paint_SetScale(UBYTE scale)
{
    if(scale == 2){
        Paint.Scale = scale;
        Paint.WidthByte = (Paint.WidthMemory % 8 == 0)? (Paint.WidthMemory / 8 ): (Paint.WidthMemory / 8 + 1);
    }else if(scale == 4){
        Paint.Scale = scale;
        Paint.WidthByte = (Paint.WidthMemory % 4 == 0)? (Paint.WidthMemory / 4 ): (Paint.WidthMemory / 4 + 1);
    }else if(scale == 7){//Only applicable with 5in65 e-Paper
				Paint.Scale = scale;
				Paint.WidthByte = (Paint.WidthMemory % 2 == 0)? (Paint.WidthMemory / 2 ): (Paint.WidthMemory / 2 + 1);;
		}else{
        ESP_LOGE(TAG,"Set Scale Input parameter error\r\n");
        ESP_LOGE(TAG,"Scale Only support: 2 4 7\r\n");
    }
}
/******************************************************************************
function:	Select Image mirror
parameter:
    mirror   :Not mirror,Horizontal mirror,Vertical mirror,Origin mirror
******************************************************************************/
void Paint_SetMirroring(UBYTE mirror)
{
    if(mirror == MIRROR_NONE || mirror == MIRROR_HORIZONTAL || 
        mirror == MIRROR_VERTICAL || mirror == MIRROR_ORIGIN) {
        ESP_LOGI(TAG,"mirror image x:%s, y:%s\r\n",(mirror & 0x01)? "mirror":"none", ((mirror >> 1) & 0x01)? "mirror":"none");
        Paint.Mirror = mirror;
    } else {
        ESP_LOGE(TAG,"mirror should be MIRROR_NONE, MIRROR_HORIZONTAL, \
        MIRROR_VERTICAL or MIRROR_ORIGIN\r\n");
    }    
}

/******************************************************************************
function: Draw Pixels
parameter:
    Xpoint : At point X
    Ypoint : At point Y
    Color  : Painted colors
******************************************************************************/
void Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color)
{
    if(Xpoint > Paint.Width || Ypoint > Paint.Height){
        ESP_LOGE(TAG,"Exceeding display boundaries\r\n");
        return;
    }      
    UWORD X, Y;

    switch(Paint.Rotate) {
    case 0:
        X = Xpoint;
        Y = Ypoint;  
        break;
    case 90:
        X = Paint.WidthMemory - Ypoint - 1;
        Y = Xpoint;
        break;
    case 180:
        X = Paint.WidthMemory - Xpoint - 1;
        Y = Paint.HeightMemory - Ypoint - 1;
        break;
    case 270:
        X = Ypoint;
        Y = Paint.HeightMemory - Xpoint - 1;
        break;
    default:
        return;
    }
    
    switch(Paint.Mirror) {
    case MIRROR_NONE:
        break;
    case MIRROR_HORIZONTAL:
        X = Paint.WidthMemory - X - 1;
        break;
    case MIRROR_VERTICAL:
        Y = Paint.HeightMemory - Y - 1;
        break;
    case MIRROR_ORIGIN:
        X = Paint.WidthMemory - X - 1;
        Y = Paint.HeightMemory - Y - 1;
        break;
    default:
        return;
    }

    if(X > Paint.WidthMemory || Y > Paint.HeightMemory){
        ESP_LOGE(TAG,"Exceeding display boundaries\r\n");
        return;
    }
    
    if(Paint.Scale == 2){
        UDOUBLE Addr = X / 8 + Y * Paint.WidthByte;
        UBYTE Rdata = Paint.Image[Addr];
        if(Color == BLACK)
            Paint.Image[Addr] = Rdata & ~(0x80 >> (X % 8));
        else
            Paint.Image[Addr] = Rdata | (0x80 >> (X % 8));
    }else if(Paint.Scale == 4){
        UDOUBLE Addr = X / 4 + Y * Paint.WidthByte;
        Color = Color % 4;//Guaranteed color scale is 4  --- 0~3
        UBYTE Rdata = Paint.Image[Addr];
        
        Rdata = Rdata & (~(0xC0 >> ((X % 4)*2)));
        Paint.Image[Addr] = Rdata | ((Color << 6) >> ((X % 4)*2));
    }else if(Paint.Scale == 7){
		UDOUBLE Addr = X / 2  + Y * Paint.WidthByte;
		UBYTE Rdata = Paint.Image[Addr];
		Rdata = Rdata & (~(0xF0 >> ((X % 2)*4)));//Clear first, then set value
		Paint.Image[Addr] = Rdata | ((Color << 4) >> ((X % 2)*4));
		//printf("Add =  %d ,data = %d\r\n",Addr,Rdata);
		}
}

/******************************************************************************
function: Clear the color of the picture
parameter:
    Color : Painted colors
******************************************************************************/
void Paint_Clear(UWORD Color)
{
	if(Paint.Scale == 2) {
		for (UWORD Y = 0; Y < Paint.HeightByte; Y++) {
			for (UWORD X = 0; X < Paint.WidthByte; X++ ) {//8 pixel =  1 byte
				UDOUBLE Addr = X + Y*Paint.WidthByte;
				Paint.Image[Addr] = Color;
			}
		}		
    }else if(Paint.Scale == 4) {
        for (UWORD Y = 0; Y < Paint.HeightByte; Y++) {
			for (UWORD X = 0; X < Paint.WidthByte; X++ ) {
				UDOUBLE Addr = X + Y*Paint.WidthByte;
				Paint.Image[Addr] = (Color<<6)|(Color<<4)|(Color<<2)|Color;
			}
		}		
	}else if(Paint.Scale == 7) {
		for (UWORD Y = 0; Y < Paint.HeightByte; Y++) {
			for (UWORD X = 0; X < Paint.WidthByte; X++ ) {
				UDOUBLE Addr = X + Y*Paint.WidthByte;
				Paint.Image[Addr] = (Color<<4)|Color;
			}
		}		
	}
}

/******************************************************************************
function: Clear the color of a window
parameter:
    Xstart : x starting point
    Ystart : Y starting point
    Xend   : x end point
    Yend   : y end point
    Color  : Painted colors
******************************************************************************/
void Paint_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color)
{
    UWORD X, Y;
    for (Y = Ystart; Y < Yend; Y++) {
        for (X = Xstart; X < Xend; X++) {//8 pixel =  1 byte
            Paint_SetPixel(X, Y, Color);
        }
    }
}

/******************************************************************************
function: Draw Point(Xpoint, Ypoint) Fill the color
parameter:
    Xpoint		: The Xpoint coordinate of the point
    Ypoint		: The Ypoint coordinate of the point
    Color		: Painted color
    Dot_Pixel	: point size
    Dot_Style	: point Style
******************************************************************************/
void Paint_DrawPoint(UWORD Xpoint, UWORD Ypoint, UWORD Color,
                     DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_Style)
{
    if (Xpoint > Paint.Width || Ypoint > Paint.Height) {
        ESP_LOGE(TAG,"Paint_DrawPoint Input exceeds the normal display range\r\n");
				printf("Xpoint = %d , Paint.Width = %d  \r\n ",Xpoint,Paint.Width);
				printf("Ypoint = %d , Paint.Height = %d  \r\n ",Ypoint,Paint.Height);
        return;
    }

    int16_t XDir_Num , YDir_Num;
    if (Dot_Style == DOT_FILL_AROUND) {
        for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++) {
                if(Xpoint + XDir_Num - Dot_Pixel < 0 || Ypoint + YDir_Num - Dot_Pixel < 0)
                    break;
                // printf("x = %d, y = %d\r\n", Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel);
                Paint_SetPixel(Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel, Color);
            }
        }
    } else {
        for (XDir_Num = 0; XDir_Num <  Dot_Pixel; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num <  Dot_Pixel; YDir_Num++) {
                Paint_SetPixel(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1, Color);
            }
        }
    }
}

/******************************************************************************
function: Draw a line of arbitrary slope
parameter:
    Xstart ：Starting Xpoint point coordinates
    Ystart ：Starting Xpoint point coordinates
    Xend   ：End point Xpoint coordinate
    Yend   ：End point Ypoint coordinate
    Color  ：The color of the line segment
    Line_width : Line width
    Line_Style: Solid and dotted lines
******************************************************************************/
void Paint_DrawLine(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,
                    UWORD Color, DOT_PIXEL Line_width, LINE_STYLE Line_Style)
{
    if (Xstart > Paint.Width || Ystart > Paint.Height ||
        Xend > Paint.Width || Yend > Paint.Height) {
        ESP_LOGE(TAG,"Paint_DrawLine Input exceeds the normal display range\r\n");
        return;
    }

    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;
    int dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
    int dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

    // Increment direction, 1 is positive, -1 is counter;
    int XAddway = Xstart < Xend ? 1 : -1;
    int YAddway = Ystart < Yend ? 1 : -1;

    //Cumulative error
    int Esp = dx + dy;
    char Dotted_Len = 0;

    for (;;) {
        Dotted_Len++;
        //Painted dotted line, 2 point is really virtual
        if (Line_Style == LINE_STYLE_DOTTED && Dotted_Len % 3 == 0) {
            //Debug("LINE_DOTTED\r\n");
            Paint_DrawPoint(Xpoint, Ypoint, IMAGE_BACKGROUND, Line_width, DOT_STYLE_DFT);
            Dotted_Len = 0;
        } else {
            Paint_DrawPoint(Xpoint, Ypoint, Color, Line_width, DOT_STYLE_DFT);
        }
        if (2 * Esp >= dy) {
            if (Xpoint == Xend)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx) {
            if (Ypoint == Yend)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}

/******************************************************************************
function: Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the rectangle
******************************************************************************/
void Paint_DrawRectangle(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend,
                         UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
{
    if (Xstart > Paint.Width || Ystart > Paint.Height ||
        Xend > Paint.Width || Yend > Paint.Height) {
        ESP_LOGE(TAG,"Input exceeds the normal display range\r\n");
        return;
    }

    if (Draw_Fill) {
        UWORD Ypoint;
        for(Ypoint = Ystart; Ypoint < Yend; Ypoint++) {
            Paint_DrawLine(Xstart, Ypoint, Xend, Ypoint, Color , Line_width, LINE_STYLE_SOLID);
        }
    } else {
        Paint_DrawLine(Xstart, Ystart, Xend, Ystart, Color, Line_width, LINE_STYLE_SOLID);
        Paint_DrawLine(Xstart, Ystart, Xstart, Yend, Color, Line_width, LINE_STYLE_SOLID);
        Paint_DrawLine(Xend, Yend, Xend, Ystart, Color, Line_width, LINE_STYLE_SOLID);
        Paint_DrawLine(Xend, Yend, Xstart, Yend, Color, Line_width, LINE_STYLE_SOLID);
    }
}

/******************************************************************************
function: Use the 8-point method to draw a circle of the
            specified size at the specified position->
parameter:
    X_Center  ：Center X coordinate
    Y_Center  ：Center Y coordinate
    Radius    ：circle Radius
    Color     ：The color of the ：circle segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the Circle
******************************************************************************/
void Paint_DrawCircle(UWORD X_Center, UWORD Y_Center, UWORD Radius,
                      UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
{
    if (X_Center > Paint.Width || Y_Center >= Paint.Height) {
        ESP_LOGE(TAG,"Paint_DrawCircle Input exceeds the normal display range\r\n");
        return;
    }

    //Draw a circle from(0, R) as a starting point
    int16_t XCurrent, YCurrent;
    XCurrent = 0;
    YCurrent = Radius;

    //Cumulative error,judge the next point of the logo
    int16_t Esp = 3 - (Radius << 1 );

    int16_t sCountY;
    if (Draw_Fill == DRAW_FILL_FULL) {
        while (XCurrent <= YCurrent ) { //Realistic circles
            for (sCountY = XCurrent; sCountY <= YCurrent; sCountY ++ ) {
                Paint_DrawPoint(X_Center + XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//1
                Paint_DrawPoint(X_Center - XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//2
                Paint_DrawPoint(X_Center - sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//3
                Paint_DrawPoint(X_Center - sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//4
                Paint_DrawPoint(X_Center - XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//5
                Paint_DrawPoint(X_Center + XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//6
                Paint_DrawPoint(X_Center + sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);//7
                Paint_DrawPoint(X_Center + sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
            if (Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    } else { //Draw a hollow circle
        while (XCurrent <= YCurrent ) {
            Paint_DrawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, Line_width, DOT_STYLE_DFT);//1
            Paint_DrawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, Line_width, DOT_STYLE_DFT);//2
            Paint_DrawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, Line_width, DOT_STYLE_DFT);//3
            Paint_DrawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, Line_width, DOT_STYLE_DFT);//4
            Paint_DrawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, Line_width, DOT_STYLE_DFT);//5
            Paint_DrawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, Line_width, DOT_STYLE_DFT);//6
            Paint_DrawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, Line_width, DOT_STYLE_DFT);//7
            Paint_DrawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, Line_width, DOT_STYLE_DFT);//0

            if (Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    }
}

/******************************************************************************
function: Show English characters
parameter:
    Xpoint           ：X coordinate
    Ypoint           ：Y coordinate
    Acsii_Char       ：To display the English characters
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
// void Paint_DrawChar(UWORD Xpoint, UWORD Ypoint, const char Acsii_Char,
//                     sFONT* Font, UWORD Color_Foreground, UWORD Color_Background)
// {
//     UWORD Page, Column;

//     if (Xpoint > Paint.Width || Ypoint > Paint.Height) {
//         ESP_LOGE(TAG,"Paint_DrawChar Input exceeds the normal display range\r\n");
//         return;
//     }

//     uint32_t Char_Offset = (Acsii_Char - ' ') * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
//     const unsigned char *ptr = &Font->table[Char_Offset];

//     for (Page = 0; Page < Font->Height; Page ++ ) {
//         for (Column = 0; Column < Font->Width; Column ++ ) {

//             //To determine whether the font background color and screen background color is consistent
//             if (FONT_BACKGROUND == Color_Background) { //this process is to speed up the scan
//                 if (*ptr & (0x80 >> (Column % 8)))
//                     Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
//                     // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
//             } else {
//                 if (*ptr & (0x80 >> (Column % 8))) {
//                     Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Foreground);
//                     // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
//                 } else {
//                     Paint_SetPixel(Xpoint + Column, Ypoint + Page, Color_Background);
//                     // Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
//                 }
//             }
//             //One pixel is 8 bits
//             if (Column % 8 == 7)
//                 ptr++;
//         }// Write a line
//         if (Font->Width % 8 != 0)
//             ptr++;
//     }// Write all
// }

/******************************************************************************
function:	Display the string
parameter:
    Xstart           ：X coordinate
    Ystart           ：Y coordinate
    pString          ：The first address of the English string to be displayed
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
// void Paint_DrawString_EN(UWORD Xstart, UWORD Ystart, const char * pString,
//                          sFONT* Font, UWORD Color_Foreground, UWORD Color_Background)
// {
//     UWORD Xpoint = Xstart;
//     UWORD Ypoint = Ystart;

//     if (Xstart > Paint.Width || Ystart > Paint.Height) {
//         ESP_LOGE(TAG,"Paint_DrawString_EN Input exceeds the normal display range\r\n");
//         return;
//     }

//     while (* pString != '\0') {
//         //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
//         if ((Xpoint + Font->Width ) > Paint.Width ) {
//             Xpoint = Xstart;
//             Ypoint += Font->Height;
//         }

//         // If the Y direction is full, reposition to(Xstart, Ystart)
//         if ((Ypoint  + Font->Height ) > Paint.Height ) {
//             Xpoint = Xstart;
//             Ypoint = Ystart;
//         }
//         Paint_DrawChar(Xpoint, Ypoint, * pString, Font, Color_Background, Color_Foreground);

//         //The next character of the address
//         pString ++;

//         //The next word of the abscissa increases the font of the broadband
//         Xpoint += Font->Width;
//     }
// }


/******************************************************************************
function: Display the string
parameter:
    Xstart  ：X coordinate
    Ystart  ：Y coordinate
    pString ：The first address of the Chinese string and English
              string to be displayed
    Font    ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawString_CN(UWORD Xstart, UWORD Ystart, const char * pString,
                        UWORD Color_Foreground, UWORD Color_Background)
{
    const char* pstr = pString;
    int i;
    uint8_t  bitmap[300];
    i = 0;
    uint16_t char_cn_un = 0;
    
    //当前中文字数
    uint16_t cn_data_num = 0;
    uint16_t cn_box_w = 0;
    //当前英文字数
    uint16_t en_data_num = 0;
    uint16_t en_box_w = 0;
    while (*(pstr+i) != '\0')
    {
        memset(&bitmap,0,sizeof(bitmap));
        UTF8toUnicode((uint8_t*)pstr+i,&char_cn_un);
        uint8_t box_w = 0,box_h = 0;
        uint8_t offset_x = 0,offset_y = 0;
        int size = ds_get_bitmap(char_cn_un,bitmap,&box_w,&box_h,&offset_x,&offset_y);
            
        uint8_t page_len = box_h;  //box_h
        uint8_t colnum_len = box_w/2; //box_w/2
        printf("page_len %d  colnum_len %d\n",page_len,colnum_len); 
        for(uint8_t Page = 0; Page < page_len; Page ++) //0-127
        { 
            for (uint8_t Column = 0; Column < colnum_len; Column++) //0-1
            {
                uint8_t tmp = bitmap[Page*colnum_len + Column]; 
                // printf("%x ",tmp);  
                for(int index = 0;index < 8 ;index += 4){
                    if (((tmp >> index) & 0x0f) >= 1){
                        Paint_SetPixel( Xstart + 2*Column + (1- index/4) + (en_box_w + cn_box_w)+(en_data_num+cn_data_num)*3 + offset_x,Ystart+ Page -offset_y -page_len    , Color_Background);
                    }else{
                        Paint_SetPixel(  Xstart + 2*Column + (1- index/4) + (en_box_w + cn_box_w)+(en_data_num+cn_data_num)*3 + offset_x,Ystart+ Page - offset_y -page_len  , Color_Foreground);
                    }
                }
            }
            // printf("\n"); 
        }

        if (*(pstr+i) >= 128){
            //中文4byte
            i+=3;
            cn_data_num ++;
            cn_box_w += box_w;
        }else{
            i++;
            en_data_num ++;
            en_box_w += box_w;
        }
    }
}

//图片全刷
void ds_paint_image(void){
    unsigned int i;
    spi_send_cmd(0x10);
    for(i=0;i<ALLSCREEN_GRAGHBYTES;i++){
        spi_send_data(0x00);  
    }
    spi_send_cmd(0x13);
    for(i=0;i<ALLSCREEN_GRAGHBYTES;i++){
        spi_send_data(Paint.Image[i]);  
    }           
}

unsigned char gImage_last_page[ALLSCREEN_GRAGHBYTES];

void ds_paint_last_page(void){
    unsigned int i;
    for(i=0;i<ALLSCREEN_GRAGHBYTES;i++){
        spi_send_data(~gImage_last_page[i]);  
    }           
}

void ds_paint_image_new(){
    unsigned int i;
    for(i=0;i<ALLSCREEN_GRAGHBYTES;i++){
        spi_send_data(~Paint.Image[i]);  
    }       
}

void ds_paint_clean(void){
    unsigned int i;
    for(i=0;i<ALLSCREEN_GRAGHBYTES;i++){
        spi_send_data(0);  
    }           
}

void ds_paint_image_copy(){
    memcpy(gImage_last_page,Paint.Image,ALLSCREEN_GRAGHBYTES);
}

#define EPD_2IN9BC_WIDTH       200
#define EPD_2IN9BC_HEIGHT      200
#define IMAGE_SIZE (((EPD_2IN9BC_WIDTH % 8 == 0) ? (EPD_2IN9BC_WIDTH / 8 ) : (EPD_2IN9BC_WIDTH / 8 + 1)) * EPD_2IN9BC_HEIGHT)

void ds_ui_show_test(){

    uint8_t *m_custom_image;
    printf("IMAGE_SIZE %d\n",IMAGE_SIZE);
    if ((m_custom_image = (uint8_t *)malloc(IMAGE_SIZE)) == NULL) {
      printf("Failed to apply for black memory...\r\n");
      return ;
    }
    Paint_NewImage(m_custom_image, EPD_2IN9BC_WIDTH, EPD_2IN9BC_HEIGHT, 0, WHITE);
    Paint_SelectImage(m_custom_image);
    Paint_Clear(WHITE);


    // Paint_DrawCircle(150,150,10,BLACK,5,DRAW_FILL_FULL);
    // Paint_DrawString_CN(50, 80, "hello", WHITE,BLACK);
    // Paint_DrawString_CN(50, 110, "你好", WHITE,BLACK);


    // ds_screen_partial_display(0,151,0,151 ,ds_paint_clean,ds_paint_image_new); 
    EPD_HW_Init();                 // Electronic paper initialization
    EPD_WhiteScreen_ALL(Paint.Image); // Refresh the picture in full screen
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    EPD_DeepSleep(); // Sleep
    printf("ds_ui_show_test finished\n");

    // ds_paint_image_copy();
    free(m_custom_image);
}

/******************************************************************************
function:	Display nummber
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    Nummber          : The number displayed
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
// #define  ARRAY_LEN 255
// void Paint_DrawNum(UWORD Xpoint, UWORD Ypoint, int32_t Nummber,
//                    sFONT* Font, UWORD Color_Foreground, UWORD Color_Background)
// {

//     int16_t Num_Bit = 0, Str_Bit = 0;
//     uint8_t Str_Array[ARRAY_LEN] = {0}, Num_Array[ARRAY_LEN] = {0};
//     uint8_t *pStr = Str_Array;

//     if (Xpoint > Paint.Width || Ypoint > Paint.Height) {
//         ESP_LOGE(TAG,"Paint_DisNum Input exceeds the normal display range\r\n");
//         return;
//     }

//     //Converts a number to a string
//     do {
//         Num_Array[Num_Bit] = Nummber % 10 + '0';
//         Num_Bit++;
//         Nummber /= 10;
//     } while(Nummber);
    

//     //The string is inverted
//     while (Num_Bit > 0) {
//         Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
//         Str_Bit ++;
//         Num_Bit --;
//     }

//     //show
//     Paint_DrawString_EN(Xpoint, Ypoint, (const char*)pStr, Font, Color_Background, Color_Foreground);
// }

/******************************************************************************
function:	Display nummber (Able to display decimals)
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    Nummber          : The number displayed
    Font             ：A structure pointer that displays a character size
    Digit            : Fractional width
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
// void Paint_DrawNumDecimals(UWORD Xpoint, UWORD Ypoint, double Nummber,
//                     sFONT* Font, UWORD Digit, UWORD Color_Foreground, UWORD Color_Background)
// {
//     int16_t Num_Bit = 0, Str_Bit = 0;
//     uint8_t Str_Array[ARRAY_LEN] = {0}, Num_Array[ARRAY_LEN] = {0};
//     uint8_t *pStr = Str_Array;
// 	int temp = Nummber;
// 	float decimals;
// 	uint8_t i;
//     if (Xpoint > Paint.Width || Ypoint > Paint.Height) {
//         ESP_LOGE(TAG,"Paint_DisNum Input exceeds the normal display range\r\n");
//         return;
//     }

// 	if(Digit > 0) {		
// 		decimals = Nummber - temp;
// 		for(i=Digit; i > 0; i--) {
// 			decimals*=10;
// 		}
// 		temp = decimals;
// 		//Converts a number to a string
// 		for(i=Digit; i>0; i--) {
// 			Num_Array[Num_Bit] = temp % 10 + '0';
// 			Num_Bit++;
// 			temp /= 10;						
// 		}	
// 		Num_Array[Num_Bit] = '.';
// 		Num_Bit++;
// 	}

// 	temp = Nummber;
//     //Converts a number to a string
//     do {
//         Num_Array[Num_Bit] = temp % 10 + '0';
//         Num_Bit++;
//         temp /= 10;
//     } while(temp);

//     //The string is inverted
//     while (Num_Bit > 0) {
//         Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
//         Str_Bit ++;
//         Num_Bit --;
//     }

//     //show
//     Paint_DrawString_EN(Xpoint, Ypoint, (const char*)pStr, Font, Color_Background, Color_Foreground);
// }

/******************************************************************************
function:	Display time
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    pTime            : Time-related structures
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
// void Paint_DrawTime(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, sFONT* Font,
//                     UWORD Color_Foreground, UWORD Color_Background)
// {
//     uint8_t value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

//     UWORD Dx = Font->Width;

//     //Write data into the cache
//     Paint_DrawChar(Xstart                           , Ystart, value[pTime->Hour / 10], Font, Color_Background, Color_Foreground);
//     Paint_DrawChar(Xstart + Dx                      , Ystart, value[pTime->Hour % 10], Font, Color_Background, Color_Foreground);
//     Paint_DrawChar(Xstart + Dx  + Dx / 4 + Dx / 2   , Ystart, ':'                    , Font, Color_Background, Color_Foreground);
//     Paint_DrawChar(Xstart + Dx * 2 + Dx / 2         , Ystart, value[pTime->Min / 10] , Font, Color_Background, Color_Foreground);
//     Paint_DrawChar(Xstart + Dx * 3 + Dx / 2         , Ystart, value[pTime->Min % 10] , Font, Color_Background, Color_Foreground);
//     Paint_DrawChar(Xstart + Dx * 4 + Dx / 2 - Dx / 4, Ystart, ':'                    , Font, Color_Background, Color_Foreground);
//     Paint_DrawChar(Xstart + Dx * 5                  , Ystart, value[pTime->Sec / 10] , Font, Color_Background, Color_Foreground);
//     Paint_DrawChar(Xstart + Dx * 6                  , Ystart, value[pTime->Sec % 10] , Font, Color_Background, Color_Foreground);
// }

/******************************************************************************
function:	Display monochrome bitmap
parameter:
    image_buffer ：A picture data converted to a bitmap
info:
    Use a computer to convert the image into a corresponding array,
    and then embed the array directly into Imagedata.cpp as a .c file.
******************************************************************************/
void Paint_DrawBitMap(const unsigned char* image_buffer)
{
    UWORD x, y;
    UDOUBLE Addr = 0;

    for (y = 0; y < Paint.HeightByte; y++) {
        for (x = 0; x < Paint.WidthByte; x++) {//8 pixel =  1 byte
            Addr = x + y * Paint.WidthByte;
            Paint.Image[Addr] = (unsigned char)image_buffer[Addr];
        }
    }
}

/******************************************************************************
function:	paste monochrome bitmap to a frame buff
parameter:
    image_buffer ：A picture data converted to a bitmap
    xStart: The starting x coordinate
    yStart: The starting y coordinate
    imageWidth: Original image width
    imageHeight: Original image height
    flipColor: Whether the color is reversed
info:
    Use this function to paste image data into a buffer
******************************************************************************/
void Paint_DrawBitMap_Paste(const unsigned char* image_buffer, UWORD xStart, UWORD yStart, UWORD imageWidth, UWORD imageHeight, UBYTE flipColor)
{
    UBYTE color, srcImage;
    UWORD x, y;
    UWORD width = (imageWidth%8==0 ? imageWidth/8 : imageWidth/8+1);
    
    for (y = 0; y < imageHeight; y++) {
        for (x = 0; x < imageWidth; x++) {
            srcImage = image_buffer[y*width + x/8];
            if(flipColor)
                color = (((srcImage<<(x%8) & 0x80) == 0) ? 1 : 0);
            else
                color = (((srcImage<<(x%8) & 0x80) == 0) ? 0 : 1);
            Paint_SetPixel(x+xStart, y+yStart, color);
        }
    }
}


