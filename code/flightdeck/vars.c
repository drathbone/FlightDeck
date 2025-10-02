#include "ui_generated_c.h"
#include <string.h>
#include <Arduino.h>

// v_com1_active
static char v_com1_active[16] = "123.456";
const char *get_var_v_com1_active() {
  return v_com1_active;
}
void set_var_v_com1_active(const char *value){
  if (!value) {
    v_com1_active[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_com1_active, value, sizeof(v_com1_active) - 1);
  v_com1_active[sizeof(v_com1_active) - 1] = '\0';
}

//v_com1_standby
static char v_com1_standby[16] = "123.456";
const char *get_var_v_com1_standby() {
  return v_com1_standby;
}
void set_var_v_com1_standby(const char *value){
  if (!value) {
    v_com1_standby[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_com1_standby, value, sizeof(v_com1_standby) - 1);
  v_com1_standby[sizeof(v_com1_standby) - 1] = '\0';
}

// v_com2_active
static char v_com2_active[16] = "123.456";
const char *get_var_v_com2_active() {
  return v_com2_active;
}
void set_var_v_com2_active(const char *value){
  if (!value) {
    v_com2_active[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_com2_active, value, sizeof(v_com2_active) - 1);
  v_com2_active[sizeof(v_com2_active) - 1] = '\0';
}

// v_com2_standby
static char v_com2_standby[16] = "123.456";
const char *get_var_v_com2_standby() {
  return v_com2_standby;
}
void set_var_v_com2_standby(const char *value){
  if (!value) {
    v_com2_standby[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_com2_standby, value, sizeof(v_com2_standby) - 1);
  v_com2_standby[sizeof(v_com2_standby) - 1] = '\0';
}

// v_nav1_active
static char v_nav1_active[16] = "123.456";
const char *get_var_v_nav1_active() {
  return v_nav1_active;
}
void set_var_v_nav1_active(const char *value){
  if (!value) {
    v_nav1_active[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_nav1_active, value, sizeof(v_nav1_active) - 1);
  v_nav1_active[sizeof(v_nav1_active) - 1] = '\0';
}

// v_nav1_standby
static char v_nav1_standby[16] = "123.456";
const char *get_var_v_nav1_standby() {
  return v_nav1_standby;
}
void set_var_v_nav1_standby(const char *value){
  if (!value) {
    v_nav1_standby[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_nav1_standby, value, sizeof(v_nav1_standby) - 1);
  v_nav1_standby[sizeof(v_nav1_standby) - 1] = '\0';
}

// v_nav2_active
static char v_nav2_active[16] = "123.456";
const char *get_var_v_nav2_active() {
  return v_nav2_active;
}
void set_var_v_nav2_active(const char *value){
  if (!value) {
    v_nav2_active[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_nav2_active, value, sizeof(v_nav2_active) - 1);
  v_nav2_active[sizeof(v_nav2_active) - 1] = '\0';
}

// v_nav2_standby
static char v_nav2_standby[16] = "123.456";
const char *get_var_v_nav2_standby() {
  return v_nav2_standby;
}
void set_var_v_nav2_standby(const char *value){
  if (!value) {
    v_nav2_standby[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_nav2_standby, value, sizeof(v_nav2_standby) - 1);
  v_nav2_standby[sizeof(v_nav2_standby) - 1] = '\0';
}

// v_aphdg
static char v_aphdg[16] = "120";
const char *get_var_v_aphdg() {
  return v_aphdg;
}
void set_var_v_aphdg(const char *value){
  if (!value) {
    v_aphdg[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_aphdg, value, sizeof(v_aphdg) - 1);
  v_aphdg[sizeof(v_aphdg) - 1] = '\0';
}

// v_apalt
static char v_apalt[16] = "10000";
const char *get_var_v_apalt() {
  return v_apalt;
}
void set_var_v_apalt(const char *value){
  if (!value) {
    v_apalt[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_apalt, value, sizeof(v_apalt) - 1);
  v_apalt[sizeof(v_apalt) - 1] = '\0';
}

// v_apvs
static char v_apvs[16] = "400";
const char *get_var_v_apvs() {
  return v_apvs;
}
void set_var_v_apvs(const char *value){
  if (!value) {
    v_apvs[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_apvs, value, sizeof(v_apvs) - 1);
  v_apvs[sizeof(v_apvs) - 1] = '\0';
}

// v_apias
static char v_apias[16] = "120";
const char *get_var_v_apias() {
  return v_apias;
}
void set_var_v_apias(const char *value){
  if (!value) {
    v_apias[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_apias, value, sizeof(v_apias) - 1);
  v_apias[sizeof(v_apias) - 1] = '\0';
}

static char v_simrate[16] = "x1";
const char *get_var_v_simrate(){
  return v_simrate;
}
void set_var_v_simrate(const char *value) {
  if (!value) {
    v_simrate[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_simrate, value, sizeof(v_simrate) - 1);
  v_simrate[sizeof(v_simrate) - 1] = '\0';
}


static char v_ground_speed[16] = "0 kts";
const char *get_var_v_ground_speed(){
  return v_ground_speed;
}
void set_var_v_ground_speed(const char *value) {
  if (!value) {
    v_ground_speed[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_ground_speed, value, sizeof(v_ground_speed) - 1);
  v_ground_speed[sizeof(v_ground_speed) - 1] = '\0';
}

static char v_parking_brake[16] = "SET";
const char *get_var_v_parking_brake(){
  return v_parking_brake;
}
void set_var_v_parking_brake(const char *value) {
  if (!value) {
    v_parking_brake[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_parking_brake, value, sizeof(v_parking_brake) - 1);
  v_parking_brake[sizeof(v_parking_brake) - 1] = '\0';
}

static char v_fuel_total[16] = "0 lbs";
const char *get_var_v_fuel_total(){
  return v_fuel_total;
}
void set_var_v_fuel_total(const char *value) {
  if (!value) {
    v_fuel_total[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_fuel_total, value, sizeof(v_fuel_total) - 1);
  v_fuel_total[sizeof(v_fuel_total) - 1] = '\0';
}

static char v_battery[16] = "24.7V";
const char *get_var_v_battery(){
  return v_battery;
}
void set_var_v_battery(const char *value) {
  if (!value) {
    v_battery[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_battery, value, sizeof(v_battery) - 1);
  v_battery[sizeof(v_battery) - 1] = '\0';
}

static char v_weight[16] = "5,200 lbs";
const char *get_var_v_weight(){
  return v_weight;
}
void set_var_v_weight(const char *value) {
  if (!value) {
    v_weight[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_weight, value, sizeof(v_weight) - 1);
  v_weight[sizeof(v_weight) - 1] = '\0';
}

static char v_temp[16] = "15°C";
const char *get_var_v_temp(){
  return v_temp;
}
void set_var_v_temp(const char *value) {
  if (!value) {
    v_temp[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_temp, value, sizeof(v_temp) - 1);
  v_temp[sizeof(v_temp) - 1] = '\0';
}

static char v_gpu_status[16] = "OFF";
const char *get_var_v_gpu_status(){
  return v_gpu_status;
}
void set_var_v_gpu_status(const char *value) {
  if (!value) {
    v_gpu_status[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_gpu_status, value, sizeof(v_gpu_status) - 1);
  v_gpu_status[sizeof(v_gpu_status) - 1] = '\0';
}

// Aircraft info page vars
static char v_ac_title[24] = "Full Aircraft Title";
const char *get_var_v_ac_title(){
  return v_ac_title;
}
void set_var_v_ac_title(const char *value) {
  if (!value) {
    v_ac_title[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_ac_title, value, sizeof(v_ac_title) - 1);
  v_ac_title[sizeof(v_ac_title) - 1] = '\0';
}


static char v_atc_model[24] = "Full ATC Model";
const char *get_var_v_atc_model(){
  return v_atc_model;
}
void set_var_v_atc_model(const char *value) {
  if (!value) {
    v_atc_model[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_atc_model, value, sizeof(v_atc_model) - 1);
  v_atc_model[sizeof(v_atc_model) - 1] = '\0';
}


static char v_icao_type[24] = "Full ICAO Type";
const char *get_var_v_icao_type(){
  return v_icao_type;
}
void set_var_v_icao_type(const char *value) {
  if (!value) {
    v_icao_type[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_icao_type, value, sizeof(v_icao_type) - 1);
  v_icao_type[sizeof(v_icao_type) - 1] = '\0';
}


static char v_pushback_state[16] = "ATTACHED";
const char *get_var_v_pushback_state(){
  return v_pushback_state;
}
void set_var_v_pushback_state(const char *value) {
  if (!value) {
    v_pushback_state[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_pushback_state, value, sizeof(v_pushback_state) - 1);
  v_pushback_state[sizeof(v_pushback_state) - 1] = '\0';
}


static char v_exit1_state[16] = "OFF";
const char *get_var_v_exit1_state(){
  return v_exit1_state;
}
void set_var_v_exit1_state(const char *value) {
  if (!value) {
    v_exit1_state[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_exit1_state, value, sizeof(v_exit1_state) - 1);
  v_exit1_state[sizeof(v_exit1_state) - 1] = '\0';
}


static char v_exit2_state[16] = "OFF";
const char *get_var_v_exit2_state(){
  return v_exit2_state;
}
void set_var_v_exit2_state(const char *value) {
  if (!value) {
    v_exit2_state[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_exit2_state, value, sizeof(v_exit2_state) - 1);
  v_exit2_state[sizeof(v_exit2_state) - 1] = '\0';
}


static char v_profile_name[17] = "test spad profile";
const char *get_var_v_profile_name(){
  return v_profile_name;
}
void set_var_v_profile_name(const char *value) {
  if (!value) {
    v_profile_name[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_profile_name, value, sizeof(v_profile_name) - 1);
  v_profile_name[sizeof(v_profile_name) - 1] = '\0';
}


static char v_last_rescan[17] = "10/05/25 11:54:03";
const char *get_var_v_last_rescan(){
  return v_last_rescan;
}
void set_var_v_last_rescan(const char *value) {
  if (!value) {
    v_last_rescan[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_last_rescan, value, sizeof(v_last_rescan) - 1);
  v_last_rescan[sizeof(v_last_rescan) - 1] = '\0';
}


static char v_subs_count[16] = "7";
const char *get_var_v_subs_count(){
  return v_subs_count;
}
void set_var_v_subs_count(const char *value) {
  if (!value) {
    v_subs_count[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_subs_count, value, sizeof(v_subs_count) - 1);
  v_subs_count[sizeof(v_subs_count) - 1] = '\0';
}


static char v_messages[16] = "messages";
const char *get_var_v_messages(){
  return v_messages;
}
void set_var_v_messages(const char *value) {
  if (!value) {
    v_messages[0] = '\0';
    return;
  }
  // Safe copy & truncate
  strncpy(v_messages, value, sizeof(v_messages) - 1);
  v_messages[sizeof(v_messages) - 1] = '\0';
}