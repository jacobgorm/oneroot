#ifndef __IOH_H__
#define __IOH_H__

#include <stdint.h>

typedef struct ioh_event {
    volatile uint32_t state;
    void (*cb) (void *opaque);
    void *opaque;
} ioh_event;

int ioh_init(void);
int ioh_fd(void);
void ioh_event_init(ioh_event *event);
void ioh_event_set(ioh_event *event);
void ioh_event_reset(ioh_event *event);
void ioh_event_wait(ioh_event *event);
void ioh_event_close(ioh_event *event);
void ioh_event_set_callback(ioh_event *event, void (*cb) (void *opaque), void *opaque);

#endif /* __IOH_H__ */
