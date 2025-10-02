#ifndef EEZ_LVGL_UI_IMAGES_H
#define EEZ_LVGL_UI_IMAGES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const lv_img_dsc_t img_autopilot_tile_48px;
extern const lv_img_dsc_t img_engine_fan_48px;
extern const lv_img_dsc_t img_ground_tile_48px;
extern const lv_img_dsc_t img_lights_tile_48px;
extern const lv_img_dsc_t img_radios_tile_48px;
extern const lv_img_dsc_t img_sim_pause_48px;
extern const lv_img_dsc_t img_home;
extern const lv_img_dsc_t img_active_pause_grey;
extern const lv_img_dsc_t img_sim_pause_grey;
extern const lv_img_dsc_t img_sim_rate_up_grey;
extern const lv_img_dsc_t img_sim_rate_down_grey;
extern const lv_img_dsc_t img_sim_rate_reset_grey;
extern const lv_img_dsc_t img_active_pause_red;
extern const lv_img_dsc_t img_sim_pause_red;
extern const lv_img_dsc_t img_sim_pause_amber;
extern const lv_img_dsc_t img_active_pause_amber;
extern const lv_img_dsc_t img_active_pause_cyan;
extern const lv_img_dsc_t img_sim_pause_cyan;
extern const lv_img_dsc_t img_sim_rate_down_cyan;
extern const lv_img_dsc_t img_sim_rate_up_cyan;
extern const lv_img_dsc_t img_sim_rate_reset_cyan;
extern const lv_img_dsc_t img_aircraft_cyan;

#ifndef EXT_IMG_DESC_T
#define EXT_IMG_DESC_T
typedef struct _ext_img_desc_t {
    const char *name;
    const lv_img_dsc_t *img_dsc;
} ext_img_desc_t;
#endif

extern const ext_img_desc_t images[22];


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_IMAGES_H*/