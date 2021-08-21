#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_demos/lv_demo.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define	LV_HOR_RES_MAX		800
#define LV_VER_RES_MAX		480
#define DISP_BUF_SIZE 		(LV_HOR_RES_MAX * LV_VER_RES_MAX)

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
    disp_drv.hor_res    = LV_HOR_RES_MAX;
    disp_drv.ver_res    = LV_VER_RES_MAX;
    lv_disp_drv_register(&disp_drv);

    /* Input device interface */
    evdev_init();

    lv_indev_drv_t indev_drv;                  	/*Descriptor of a input device driver*/
    lv_indev_drv_init(&indev_drv);             	/*Basic initialization*/
    indev_drv.type = LV_INDEV_TYPE_POINTER;    	/*Touch pad is a pointer-like device*/
    indev_drv.read_cb = evdev_read;      		/*Set your driver function*/
    lv_indev_drv_register(&indev_drv);         	/*Finally register the driver*/

    /*Create a Demo*/
//    lv_demo_widgets();
//    lv_demo_keypad_encoder();
//    lv_demo_stress();
//    lv_demo_benchmark();
    lv_demo_music();


    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_task_handler();
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
