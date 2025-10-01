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
    lv_obj_t *pg_sim_controls;
    lv_obj_t *pg_lights;
    lv_obj_t *pg_ground_services;
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
    lv_obj_t *b_bc;
    lv_obj_t *obj33;
    lv_obj_t *b_yd;
    lv_obj_t *obj34;
    lv_obj_t *b_lvl;
    lv_obj_t *obj35;
    lv_obj_t *b_vnav;
    lv_obj_t *obj36;
    lv_obj_t *home_1;
    lv_obj_t *pause;
    lv_obj_t *active_pause;
    lv_obj_t *obj37;
    lv_obj_t *obj38;
    lv_obj_t *none;
    lv_obj_t *none_1;
    lv_obj_t *none_2;
    lv_obj_t *none_3;
    lv_obj_t *none_4;
    lv_obj_t *none_5;
    lv_obj_t *none_6;
    lv_obj_t *sim_rate;
    lv_obj_t *obj39;
    lv_obj_t *l_simrate;
    lv_obj_t *b_sim_rate_inc;
    lv_obj_t *b_sim_rate_dec;
    lv_obj_t *b_sim_rate_reset;
    lv_obj_t *none_7;
    lv_obj_t *none_8;
    lv_obj_t *home_2;
    lv_obj_t *btn_landing;
    lv_obj_t *lbl_landing;
    lv_obj_t *btn_recog;
    lv_obj_t *lbl_recog;
    lv_obj_t *btn_taxi;
    lv_obj_t *lbl_taxi;
    lv_obj_t *btn_wing;
    lv_obj_t *lbl_wing;
    lv_obj_t *btn_nav;
    lv_obj_t *lbl_nav;
    lv_obj_t *btn_logo;
    lv_obj_t *lbl_logo;
    lv_obj_t *btn_strobe;
    lv_obj_t *lbl_strobe;
    lv_obj_t *btn_cabin;
    lv_obj_t *lbl_cabin;
    lv_obj_t *btn_beacon;
    lv_obj_t *lbl_beacon;
    lv_obj_t *btn_all_on;
    lv_obj_t *lbl_all_on;
    lv_obj_t *btn_panel;
    lv_obj_t *lbl_panel;
    lv_obj_t *btn_all_off;
    lv_obj_t *lbl_all_off;
    lv_obj_t *home_3;
    lv_obj_t *con_ground_speed;
    lv_obj_t *lbl_ground_speed;
    lv_obj_t *lbl_ground_speed_val;
    lv_obj_t *con_parking_brake;
    lv_obj_t *lbl_parking_brake;
    lv_obj_t *lbl_parking_brake_val;
    lv_obj_t *con_fuel_total;
    lv_obj_t *lbl_fuel_total;
    lv_obj_t *lbl_fuel_total_val;
    lv_obj_t *btn_pushback;
    lv_obj_t *lbl_pushback;
    lv_obj_t *btn_refuel;
    lv_obj_t *lbl_refuel;
    lv_obj_t *btn_gpu;
    lv_obj_t *lbl_gpu;
    lv_obj_t *btn_doors;
    lv_obj_t *lbl_doors;
    lv_obj_t *btn_de_ice;
    lv_obj_t *lbl_de_ice;
    lv_obj_t *btn_autostart;
    lv_obj_t *lbl_autostart;
    lv_obj_t *con_status_bar;
    lv_obj_t *lbl_battery;
    lv_obj_t *lbl_battery_val;
    lv_obj_t *lvl_div1;
    lv_obj_t *lbl_weight;
    lv_obj_t *lbl_weight_val;
    lv_obj_t *lvl_div2;
    lv_obj_t *lbl_temp;
    lv_obj_t *lbl_temp_val;
    lv_obj_t *lvl_div3;
    lv_obj_t *lbl_gpu_status;
    lv_obj_t *lbl_gpu_status_val;
    lv_obj_t *home_4;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_PG_HOME = 1,
    SCREEN_ID_PG_RADIOS = 2,
    SCREEN_ID_PG_AUTOPILOT = 3,
    SCREEN_ID_PG_SIM_CONTROLS = 4,
    SCREEN_ID_PG_LIGHTS = 5,
    SCREEN_ID_PG_GROUND_SERVICES = 6,
};

void create_screen_pg_home();
void tick_screen_pg_home();

void create_screen_pg_radios();
void tick_screen_pg_radios();

void create_screen_pg_autopilot();
void tick_screen_pg_autopilot();

void create_screen_pg_sim_controls();
void tick_screen_pg_sim_controls();

void create_screen_pg_lights();
void tick_screen_pg_lights();

void create_screen_pg_ground_services();
void tick_screen_pg_ground_services();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif


#endif /*EEZ_LVGL_UI_SCREENS_H*/