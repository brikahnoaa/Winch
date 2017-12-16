// event.h

typedef enum {
  Null_ev=0,
  WinchStart_ev,
  WinchStop_ev,
  WisprDetect_ev,
  WisprSleep_ev,
  Send_ev,
  Call_ev,
  Sizeof_ev,
} Event;

void evDelay(Event event, int secs);
void evSetAt(Event event, time_t when);
void evClear(Event event);
Event evCheck(void);
void evDump();

