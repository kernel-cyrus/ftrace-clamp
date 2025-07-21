#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define FTRACE_ROOT "/sys/kernel/debug/tracing"
#define FTRACE_READ_SIZE 4096
#define ERR_EXIT 1

static void echo(const char *path, const char *content)
{
    int ret;

    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror(path);
        exit(ERR_EXIT);
    }

    ret = write(fd, content, strlen(content));
    if (ret < 0) {
        perror("ERROR: Failed to write.");
        close(fd);
        exit(ERR_EXIT);
    }

    close(fd);
}

static void append(const char *path, const char *content)
{
    int ret;

    int fd = open(path, O_WRONLY | O_APPEND);
    if (fd < 0) {
        perror(path);
        exit(ERR_EXIT);
    }

    ret = write(fd, content, strlen(content));
    if (ret < 0) {
        perror("ERROR: Failed to write.");
        close(fd);
        exit(ERR_EXIT);
    }

    ret = write(fd, "\n", 1);
    if (ret != 1) {
        perror("ERROR: Failed to write.");
        close(fd);
        exit(ERR_EXIT);
    }

    close(fd);
}

static void clear(const char *path)
{
    int fd = open(path, O_WRONLY | O_TRUNC);
    if (fd < 0) {
        perror(path);
        exit(ERR_EXIT);
    }

    close(fd);
}

void ftrace_start(void)
{
    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d", getpid());
    echo(FTRACE_ROOT "/set_ftrace_pid", pid_str);
    echo(FTRACE_ROOT "/set_event_pid", pid_str);
    echo(FTRACE_ROOT "/tracing_on", "1");
}

void ftrace_stop(void)
{
    echo(FTRACE_ROOT "/tracing_on", "0");
    clear(FTRACE_ROOT "/set_ftrace_pid");
    clear(FTRACE_ROOT "/set_event_pid");
}

void ftrace_clear(void)
{
    echo(FTRACE_ROOT "/trace", "\n");
}

void ftrace_set_tracer(const char *tracer)
{
    echo(FTRACE_ROOT "/current_tracer", tracer);
}

void ftrace_dump(void)
{
    char buffer[FTRACE_READ_SIZE];

    FILE *fp = fopen(FTRACE_ROOT "/trace", "r");
    if (!fp) {
        perror("fopen trace");
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        fputs(buffer, stdout);
    }

    fclose(fp);
}

void ftrace_set_event(const char *event)
{
    echo(FTRACE_ROOT "/set_event", event);
}

void ftrace_add_event(const char *event)
{
    append(FTRACE_ROOT "/set_event", event);
}

void ftrace_reset(void)
{
    echo(FTRACE_ROOT "/tracing_on", "0");
    clear(FTRACE_ROOT "/trace");
    clear(FTRACE_ROOT "/set_ftrace_pid");
    clear(FTRACE_ROOT "/set_event_pid");
    clear(FTRACE_ROOT "/set_event");
}

#ifdef BUILD_AS_EXECUTABLE

int main(void) {

    ftrace_reset();
    ftrace_set_tracer("nop");
    ftrace_add_event("sched:sched_switch");
    ftrace_add_event("sched:sched_wakeup");
    ftrace_start();

    sleep(1);

    ftrace_stop();
    ftrace_dump();
    ftrace_reset();

    return 0;
}

#endif