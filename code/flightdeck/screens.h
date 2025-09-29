#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *pg_home;
    lv_obj_t *pg_radios;
    lv_obj_t *pg_autopilot;
    lv_obj_t *radios;
    lv_obj_t *obj0;
    lv_obj_t *autopilot;
    lv_obj_t *obj1;
    lv_obj_t *sim_controls;
    lv_obj_t *obj2;
    lv_obj_t *ground_services;
    lv_obj_t *obj3;
    lv_obj_t *engine_management;
    lv_obj_t *obj4;
    lv_obj_t *lighting;
    lv_obj_t *obj5;
    lv_obj_t *com1;
    lv_obj_t *obj6;
    lv_obj_t *obj7;
    lv_obj_t *obj8;
    lv_obj_t *l_com1_active;
    lv_obj_t *l_com1_standby;
    lv_obj_t *com2;
    lv_obj_t *obj9;
    lv_obj_t *obj10;
    lv_obj_t *l_com2_standby;
    lv_obj_t *l_com2_active;
    lv_obj_t *obj11;
    lv_obj_t *nav1;
    lv_obj_t *obj12;
    lv_obj_t *obj13;
    lv_obj_t *l_nav1_standby;
    lv_obj_t *l_nav1_active;
    lv_obj_t *obj14;
    lv_obj_t *nav2;
    lv_obj_t *obj15;
    lv_obj_t *obj16;
    lv_obj_t *l_nav2_standby;
    lv_obj_t *l_nav2_active;
    lv_obj_t *obj17;
    lv_obj_t *home;
    lv_obj_t *ap_modes;
    lv_obj_t *b_ap;
    lv_obj_t *obj18;
    lv_obj_t *b_fd;
    lv_obj_t *obj19;
    lv_obj_t *b_hdg;
    lv_obj_t *obj20;
    lv_obj_t *b_nav;
    lv_obj_t *obj21;
    lv_obj_t *b_alt;
    lv_obj_t *obj22;
    lv_obj_t *b_vs;
    lv_obj_t *obj23;
    lv_obj_t *b_apr;
    lv_obj_t *obj24;
    lv_obj_t *ap_hdg;
    lv_obj_t *obj25;
    lv_obj_t *obj26;
    lv_obj_t *ap_alt;
    lv_obj_t *obj27;
    lv_obj_t *obj28;
    lv_obj_t *ap_vs;
    lv_obj_t *obj29;
    lv_obj_t *obj30;
    lv_obj_t *ap_ias;
    lv_obj_t *obj31;
    lv_obj_t *obj32;
    lv_obj_t *ap_controls;
    lv_obj_t *home_1;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_PG_HOME = 1,
    SCREEN_ID_PG_RADIOS = 2,
    SCREEN_ID_PG_AUTOPILOT = 3,
};

void create_screen_pg_home();
void tick_screen_pg_home();

void create_screen_pg_radios();
void tick_screen_pg_radios();

void create_screen_pg_autopilot();
void tick_screen_pg_autopilot();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif


#endif /*EEZ_LVGL_UI_SCREENS_H*/