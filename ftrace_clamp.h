#ifndef __FTRACE_CLAMP_H__
#define __FTRACE_CLAMP_H__

extern void ftrace_start(void);
extern void ftrace_stop(void);
extern void ftrace_clear(void);
extern void ftrace_reset(void);
extern void ftrace_set_tracer(const char *tracer);
extern void ftrace_set_event(const char *event);
extern void ftrace_add_event(const char *event);
extern void ftrace_dump(void);

#endif