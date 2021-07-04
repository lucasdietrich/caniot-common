
#ifndef _ATTRIBUTES_H
#define _ATTRIBUTES_H

/*___________________________________________________________________________*/

#include <stdint.h>

#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "defines.h"
#include "types.h"
#include "config.h"

/*___________________________________________________________________________*/

#define ATTR_IDENTIFICATION 0
#define ATTR_SYSTEM 1
#define ATTR_CONFIG 2

#define ATTR_KEY_SECTION(key) ((uint8_t) (((uint16_t)key) >> 12))
#define ATTR_KEY_ATTR(key) ((uint8_t) ((((uint16_t)key) >> 4) & 0xFF))
#define ATTR_KEY_PART(key) ((uint8_t) (((uint16_t)key) & 0xF))
#define ATTR_KEY_OFFSET(key) (ATTR_KEY_PART(key) << 2)

#define ATTR_KEY(section, attr, part) ((section & 0xF) << 12 | (attr & 0xFF) << 4 | (part & 0xF))

/*___________________________________________________________________________*/

typedef enum : uint8_t
{
    WRITABLE = 0,
    RAM = 1 << 0,
    EEPROM = 1 << 1,
    PROGMEMORY = 1 << 2,
    READONLY = 1 << 3,
    PRIVATE = 1 << 4, // unimplemented
} section_option_t;

/*___________________________________________________________________________*/

typedef uint16_t key_t;

typedef union
{
    uint32_t u32;
    uint16_t u16;
    uint8_t u8;
} value_t;

typedef struct
{
    uint8_t section;
    section_option_t options;
    uint8_t offset;
    uint8_t read_size;
} attr_ref_t;

/*___________________________________________________________________________*/

struct attribute_t
{
    uint8_t offset;
    uint8_t size;
    uint8_t readonly;
    char name[30];
};

struct section_t
{
    uint8_t options;
    char name[15];
    const attribute_t * array;
    uint8_t array_size;
};

#define ATTRIBUTE(struct, readonly, name, param) \
    {                                            \
        (uint8_t) offsetof(struct, param),       \
            (uint8_t)sizeof(struct ::param),     \
            (uint8_t)readonly ? READONLY : 0,    \
            name                                 \
    }

#define SECTION(options, name, array) \
    {                                 \
        options,                      \
            name,                     \
            array,                     \
            ARRAY_SIZE(array)         \
    }

/*___________________________________________________________________________*/

#define KEY_ATTR_ABSTIME ATTR_KEY(ATTR_SYSTEM, 1, 0)

static const struct attribute_t identification_attr[] PROGMEM = {
    ATTRIBUTE(identification_t, READONLY, "nodeid", deviceid),
    ATTRIBUTE(identification_t, READONLY, "version", version),
    ATTRIBUTE(identification_t, READONLY, "name", name),
};

static const struct attribute_t system_attr[] PROGMEM = {
    ATTRIBUTE(system_t, READONLY, "uptime", uptime),
    ATTRIBUTE(system_t, WRITABLE, "abstime", abstime),
    ATTRIBUTE(system_t, READONLY, "calculated_abstime", calculated_abstime),
    ATTRIBUTE(system_t, READONLY, "uptime_shift", uptime_shift),
    ATTRIBUTE(system_t, READONLY, "last_telemetry", last_telemetry),
    ATTRIBUTE(system_t, READONLY, "received.total", stats.received.total),
    ATTRIBUTE(system_t, READONLY, "received.read_attribute", stats.received.read_attribute),
    ATTRIBUTE(system_t, READONLY, "received.write_attribute", stats.received.write_attribute),
    ATTRIBUTE(system_t, READONLY, "received.command", stats.received.command),
    ATTRIBUTE(system_t, READONLY, "received.request_telemetry", stats.received.request_telemetry),
    ATTRIBUTE(system_t, READONLY, "received.processed", stats.received.processed),
    ATTRIBUTE(system_t, READONLY, "received.query_failed", stats.received.query_failed),
    ATTRIBUTE(system_t, READONLY, "sent.total", stats.sent.total),
    ATTRIBUTE(system_t, READONLY, "sent.telemetry", stats.sent.telemetry),
    ATTRIBUTE(system_t, READONLY, "last_query_error", last_query_error),
    ATTRIBUTE(system_t, READONLY, "last_telemetry_error", last_telemetry_error),
    ATTRIBUTE(system_t, READONLY, "battery", battery),
};

static const struct attribute_t config_attr[] PROGMEM = {
    ATTRIBUTE(config_t, WRITABLE, "telemetry_period", telemetry_period),
};

static const struct section_t attr_sections[] PROGMEM = {
    SECTION(RAM | PROGMEMORY | READONLY, "identification", identification_attr),
    SECTION(RAM, "system", system_attr),
    SECTION(EEPROM, "configuration", config_attr)
};

/*___________________________________________________________________________*/

const uint8_t resolve_attribute(const key_t key, attr_ref_t *const p_attr_ref);

/*___________________________________________________________________________*/

#endif