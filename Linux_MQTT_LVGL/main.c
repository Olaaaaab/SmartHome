#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "ui/ui.h"
#include "mqtt/mqtt_iot.h"

#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define DISP_BUF_SIZE (1024 * 600)

pthread_t discon_t;

// 断开和mqtt服务器连接的线程入口函数
static void *mqtt_disconnect_t(void* argv)
{
    while(1)
    {
        char ch;
        ch = getchar();
        if(ch=='Q' || ch=='q')
        {
            printf("Try to exit mqtt task\n");
            if(mqtt_disconnect() == EXIT_SUCCESS)   break;
           
        }
    }
//    isConnected = false;
    pthread_exit(&discon_t);	// 退出线�?    return NULL;
}

void set_temp_humi_data(uint16_t value)
{
    uint8_t temp_value = (value>>8)&0xFF;
    uint8_t humi_value = value&0xFF;
    
    lv_slider_set_value(ui_Slider1, temp_value, LV_ANIM_OFF);
    lv_slider_set_value(ui_Slider2, humi_value, LV_ANIM_OFF);
}

int main(void)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 1024;
    disp_drv.ver_res    = 600;
    lv_disp_drv_register(&disp_drv);

    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
    indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    /*This function will be called periodically (by the library) to get the mouse position and state*/
    indev_drv_1.read_cb = evdev_read;
    lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);


    /*Set a cursor for the mouse*/
//    LV_IMG_DECLARE(mouse_cursor_icon)
//    lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
//    lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
//    lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/


    /*Create a Demo*/
//    lv_demo_widgets();
    ui_init();

    // 成功建立客户端和服务器的连接且订阅主题后才创建断开连接的线�?    
    if(mqtt_iot() == 0)
    {
//        isConnected = true;
        pthread_create(&discon_t, 0, mqtt_disconnect_t, NULL);
    }

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

