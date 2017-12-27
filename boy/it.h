// it.h interval timer

typedef enum {
  null_it=0,
  sink_it,
  settle_it,
  winch_it,
  wispr_it,
  rest_it,
  raise_it,
  drop_it,
  } IntervalType;

void itAdd(IntervalType intT, int secs);
IntervalType itCheck(void);
void itCancel(IntervalType intT);
time_t itQuery(IntervalType intT);
static void itRemove(Interval* in);

