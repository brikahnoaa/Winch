// ant.c - for working with antenna module
#include <com.h>
#include <ant.h>

AntInfo ant;

//
// turn on antenna module, wait until ant responds
// read depth
// sets: .depth
//
void antInit(void) {
  DBG0("\nantInit()")
} // antInit

//
// if td mode, check for data, else poke and wait.
// returns: depth
//
float antDepth(void) {
  return ant.depth;
}

//
// ??
//   null_mod, idle_mod, td_mod, gps_mod, call_mod, shut_mod
//
ModType antMode(ModType mod) {
  return mod;
}

// ??
void antStop() {}

// ??
bool antRising(void) {
  return true;
} // antRising

// ??
bool antDropping(void) {
  return true;
}

bool antSteady(void) {
  return true;
}
