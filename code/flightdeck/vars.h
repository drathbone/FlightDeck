#ifndef EEZ_LVGL_UI_VARS_H
#define EEZ_LVGL_UI_VARS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// enum declarations



// Flow global variables

enum FlowGlobalVariables {
    FLOW_GLOBAL_VARIABLE_V_COM1_ACTIVE = 0,
    FLOW_GLOBAL_VARIABLE_V_COM1_STANDBY = 1,
    FLOW_GLOBAL_VARIABLE_V_COM2_ACTIVE = 2,
    FLOW_GLOBAL_VARIABLE_V_COM2_STANDBY = 3,
    FLOW_GLOBAL_VARIABLE_V_NAV1_ACTIVE = 4,
    FLOW_GLOBAL_VARIABLE_V_NAV1_STANDBY = 5,
    FLOW_GLOBAL_VARIABLE_V_NAV2_ACTIVE = 6,
    FLOW_GLOBAL_VARIABLE_V_NAV2_STANDBY = 7,
    FLOW_GLOBAL_VARIABLE_V_APHDG = 8,
    FLOW_GLOBAL_VARIABLE_V_APALT = 9,
    FLOW_GLOBAL_VARIABLE_V_APVS = 10,
    FLOW_GLOBAL_VARIABLE_V_APIAS = 11,
    FLOW_GLOBAL_VARIABLE_V_SIMRATE = 12,
    FLOW_GLOBAL_VARIABLE_V_GROUND_SPEED = 13,
    FLOW_GLOBAL_VARIABLE_V_PARKING_BRAKE = 14,
    FLOW_GLOBAL_VARIABLE_V_FUEL_TOTAL = 15,
    FLOW_GLOBAL_VARIABLE_V_BATTERY = 16,
    FLOW_GLOBAL_VARIABLE_V_WEIGHT = 17,
    FLOW_GLOBAL_VARIABLE_V_TEMP = 18,
    FLOW_GLOBAL_VARIABLE_V_GPU_STATUS = 19
};

// Native global variables

extern const char *get_var_v_com1_active();
extern void set_var_v_com1_active(const char *value);
extern const char *get_var_v_com1_standby();
extern void set_var_v_com1_standby(const char *value);
extern const char *get_var_v_com2_active();
extern void set_var_v_com2_active(const char *value);
extern const char *get_var_v_com2_standby();
extern void set_var_v_com2_standby(const char *value);
extern const char *get_var_v_nav1_active();
extern void set_var_v_nav1_active(const char *value);
extern const char *get_var_v_nav1_standby();
extern void set_var_v_nav1_standby(const char *value);
extern const char *get_var_v_nav2_active();
extern void set_var_v_nav2_active(const char *value);
extern const char *get_var_v_nav2_standby();
extern void set_var_v_nav2_standby(const char *value);
extern const char *get_var_v_aphdg();
extern void set_var_v_aphdg(const char *value);
extern const char *get_var_v_apalt();
extern void set_var_v_apalt(const char *value);
extern const char *get_var_v_apvs();
extern void set_var_v_apvs(const char *value);
extern const char *get_var_v_apias();
extern void set_var_v_apias(const char *value);
extern const char *get_var_v_simrate();
extern void set_var_v_simrate(const char *value);
extern const char *get_var_v_ground_speed();
extern void set_var_v_ground_speed(const char *value);
extern const char *get_var_v_parking_brake();
extern void set_var_v_parking_brake(const char *value);
extern const char *get_var_v_fuel_total();
extern void set_var_v_fuel_total(const char *value);
extern const char *get_var_v_battery();
extern void set_var_v_battery(const char *value);
extern const char *get_var_v_weight();
extern void set_var_v_weight(const char *value);
extern const char *get_var_v_temp();
extern void set_var_v_temp(const char *value);
extern const char *get_var_v_gpu_status();
extern void set_var_v_gpu_status(const char *value);


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/