
#include <stdint.h>

int mcp9808_open(void);
float mcp9808_read(void);

extern uint16_t mcp9808_iic_addr;
extern uint16_t sensor_in;