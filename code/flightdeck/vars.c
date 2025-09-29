#include "vars.h"
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

