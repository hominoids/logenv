
#include <stdint.h>

int mcp9808_open(void);
float mcp9808_read(void);

extern char mcp9808_iic_dev[];
extern uint16_t mcp9808_iic_addr;
extern uint16_t mcp9808_in;
