#include "cpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DATA_LEN 6

unsigned char cpu_ram_read(struct cpu *cpu, unsigned char index) 
{
  return cpu->ram[index];
}

void cpu_ram_write(struct cpu *cpu, int index, unsigned char val) 
{
  cpu->ram[index] = val;
}

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu)
{
  // char data[DATA_LEN] = {
  //   // From print8.ls8
  //   0b10000010, // LDI R0,8
  //   0b00000000,
  //   0b00001000,
  //   0b01000111, // PRN R0
  //   0b00000000,
  //   0b00000001  // HLT
  // };

  // int address = 0;

  // for (int i = 0; i < DATA_LEN; i++) {
  //   cpu->ram[address++] = data[i];
  // }

  // TODO: Replace this with something less hard-coded

  FILE *fp;
  char line[1024];

  fp = fopen("./examples/mult.ls8", "r");

  int count = 0;
  while (fgets(line, 1024, fp) != NULL) {
    char *ptr;
    unsigned char ret = strtoul(line, &ptr, 2);
    // printf("%u\n", ret);
    cpu_ram_write(cpu, count++, ret);
  }
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op) {
    case ALU_MUL:
      // TODO
      break;

    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  // #define LDI  0b10000010
  // #define HLT  0b00000001
  // #define PRN  0b01000111

  while (running) {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    unsigned char current = cpu->ram[cpu->pc];

    // 2. Figure out how many operands this next instruction requires
    // 3. Get the appropriate value(s) of the operands following this instruction
    unsigned char operandA = cpu_ram_read(cpu, cpu->pc + 1);
    unsigned char operandB = cpu_ram_read(cpu, cpu->pc + 2);

    // 0b10000010, // LDI R0,8
    // 0b00000000,
    // 0b00001000,
    // 0b01000111, // PRN R0
    // 0b00000000,
    // 0b00000001  // HLT

    // 4. switch() over it to decide on a course of action.
    // 5. Do whatever the instruction should do according to the spec.
    switch(current) {
      case HLT:
        running = 0;
        break;

      case LDI:
        cpu->registers[operandA] = operandB;
        cpu->pc = cpu->pc + 2;
        break;

      case MUL:
        // printf("A: %d\n", cpu->registers[operandA]);
        // printf("B: %d\n", cpu->registers[operandB]);
        cpu->registers[operandA] = cpu->registers[operandA] * cpu->registers[operandB];
        cpu->pc = cpu->pc + 2;
        break;
      
      case PRN:
        printf("%d\n", cpu->registers[operandA]);
        cpu->pc = cpu->pc + 1;
        break;

      default:
        break;
    }

    // 6. Move the PC to the next instruction.
    cpu->pc++;
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->pc = 0;
  // memset(cpu->ram, 0, sizeof(cpu));
  cpu->ram = calloc(256, sizeof(unsigned char));
  cpu->registers = calloc(8, sizeof(unsigned char));
  // memset(cpu->ram, 0, sizeof(unsigned char) * 256);
  // memset(cpu->registers, 0, sizeof(unsigned char) * 8);
}
