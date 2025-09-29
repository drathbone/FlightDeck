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
    FLOW_GLOBAL_VARIABLE_V_APIAS = 11
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


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_VARS_H*/