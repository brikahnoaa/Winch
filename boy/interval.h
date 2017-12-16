// interval.h

typedef enum {
  Null_it=0,
  Watch_it=1,
  Power_it=1<<1,
  Winch_it=1<<2,
  Wispr_it=1<<3,
  Data_it=1<<4,
  Call_it=1<<5,
  } IntervalType;

void addIT(IntervalType intT, int secs);
static void removeIT(int id);
int checkIT(void);
int cancelIT(IntervalType intT);

