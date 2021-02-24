#ifndef _PRINT_ERROR_H
#define _PRINT_ERROR_H

typedef enum e_error_id { ERROR_ID_UNDEFINED, ERROR_ID_NO_RESOURCES,
                          ERROR_ID_NODE_EXISTS, ERROR_ID_BLOCK_EXISTS,
                          ERROR_ID_NODE_NOT_EXISTS, ERROR_ID_BLOCK_NOT_EXISTS,
                          ERROR_ID_CMD_NOT_FOUND } Error_ID;

void print_error(short error_id);

#endif // _PRINT_ERROR_H
