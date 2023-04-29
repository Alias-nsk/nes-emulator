#include <assert.h>
#include <stdio.h>
#include "cpu_6502.h"

typedef enum {
    IMP,                // Implicit
    ABS, ABS_X, ABS_Y,  // Absolute
    IMM,                // Immediate
    IND, X_IND, IND_Y,  // Indirect
    REL,                // Relative
    ZPG, ZPG_X, ZPG_Y   // Zero page
} addr_mode_t;

typedef struct {
    uint8_t opcode;
    uint8_t cycles;
    addr_mode_t addr_mode;
} op_t;

typedef union {
    uint16_t w;
#ifdef CPU6502_BIG_ENDIAN
    struct { uint8_t h, l; } byte;
#else
    struct { uint8_t l, h; } byte;
#endif
} word_t;

#define OP_COUNT 256

/*
   cyles table taken from Marat Fayzullin
   https://fms.komkon.org/EMUL8/
*/
static uint8_t cycles[OP_COUNT] = {
  7,6,2,8,3,3,5,5,3,2,2,2,4,4,6,6,
  2,5,2,8,4,4,6,6,2,4,2,7,5,5,7,7,
  6,6,2,8,3,3,5,5,4,2,2,2,4,4,6,6,
  2,5,2,8,4,4,6,6,2,4,2,7,5,5,7,7,
  6,6,2,8,3,3,5,5,3,2,2,2,3,4,6,6,
  2,5,2,8,4,4,6,6,2,4,2,7,5,5,7,7,
  6,6,2,8,3,3,5,5,4,2,2,2,5,4,6,6,
  2,5,2,8,4,4,6,6,2,4,2,7,5,5,7,7,
  2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4,
  2,6,2,6,4,4,4,4,2,5,2,5,5,5,5,5,
  2,6,2,6,3,3,3,3,2,2,2,2,4,4,4,4,
  2,5,2,5,4,4,4,4,2,4,2,5,4,4,4,4,
  2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6,
  2,5,2,8,4,4,6,6,2,4,2,7,5,5,7,7,
  2,6,2,8,3,3,5,5,2,2,2,2,4,4,6,6,
  2,5,2,8,4,4,6,6,2,4,2,7,5,5,7,7
};

op_t ops[OP_COUNT] = {
    {0x00, 7, IMP}, {0x01, 6, X_IND}, {0x02, 2, IMP}, {0x03, 2, IMP}, {0x04, 2, IMP}, {0x05, 3, ZPG}, {0x06, 5, ZPG}, {0x07, 2, IMP}, {0x08, 3, IMP}, {0x09, 2, IMM}, {0x0a, 4, ABS}, \
    {0x0b, 6, ABS}, {0x0c, 2, IMP},
};

static uint16_t calculate_operand(cpu_6502_t* cpu, addr_mode_t addr_mode) {
    word_t operand;
    word_t pointer;
    switch (addr_mode) {
    case IMP:
        return 0; /* no operand used */
    case ABS:
        operand.byte.l = read_address(cpu->pc++);
        operand.byte.h = read_address(cpu->pc++);
        break;
    case ABS_X:
        operand.byte.l = read_address(cpu->pc++);
        operand.byte.h = read_address(cpu->pc++);
        operand.w += cpu->x;
        break;
    case ABS_Y:
        operand.byte.l = read_address(cpu->pc++);
        operand.byte.h = read_address(cpu->pc++);
        operand.w += cpu->y;
        break;
    case IMM:
        operand.byte.l = read_address(cpu->pc++);
        break;
    case IND:
        pointer.byte.l = read_address(cpu->pc++);
        pointer.byte.h = read_address(cpu->pc++);
        operand.byte.l = read_address(pointer.w);
        operand.byte.h = read_address(pointer.w + 1);
        break;
    case X_IND:
        assert(0 && "not implemented");
        break;
    case IND_Y:
        assert(0 && "not implemented");
        break;
    case REL:
        assert(0 && "not implemented");
        break;
    case ZPG:
        assert(0 && "not implemented");
        break;
    case ZPG_X:
        assert(0 && "not implemented");
        break;
    case ZPG_Y:
        assert(0 && "not implemented");
        break;
    default:
        assert(0 && "unknown addressing mode");
        break;
    }
    return operand.w;
}

static void execute_op(cpu_6502_t* cpu, op_t op, uint16_t address) {

}

/* TODO: TEMPORARY */
/* TODO: TEMPORARY */
/* TODO: TEMPORARY */
int exit_required = 0;

int run_6502(cpu_6502_t* cpu) {
    cpu->cycle_counter = cpu->interrupt_period;
    cpu->pc = cpu->initial_pc;

    uint8_t opcode;
    uint16_t address;
    op_t op;

    for (;;)
    {
        opcode = read_address(cpu->pc++);
        op = ops[opcode];
        address = calculate_operand(&cpu, op.addr_mode);
        execute_op(&cpu, op, address);

        /* TEMPORARY */
        if (cpu->pc >= 256) exit_required = 1;
        /* TEMPORARY */

        cpu->cycle_counter -= op.cycles;

        if (cpu->cycle_counter <= 0) {
            /* Check for interrupts and do other */
            /* cyclic tasks here                 */

            cpu->cycle_counter += cpu->interrupt_period;
            if (exit_required) break;
        }
    }

    return 0;

}

