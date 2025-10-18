
#include <stdint.h>

extern uint16_t sensor_in;
int mcp9808_open(void);
float mcp9808_read(void);
