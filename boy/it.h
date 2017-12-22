// it.h interval timer

typedef enum {
  Null_it=0,
  Sink_it,
  Settle_it,
  Winch_it,
  Wispr_it,
  Rest_it,
  Raise_it,
  Drop_it,
  } IntervalType;

void itAdd(IntervalType intT, int secs);
IntervalType itCheck(void);
void itCancel(IntervalType intT);
time_t itQuery(IntervalType intT);
static void itRemove(Interval* in);

