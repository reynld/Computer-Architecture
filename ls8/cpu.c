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
void cpu_load(struct cpu *cpu, int argc, char *argv[] )
{
  if (argc < 2) {
    char data[DATA_LEN] = {
      // From print8.ls8
      0b10000010, // LDI R0,8
      0b00000000,
      0b00001000,
      0b01000111, // PRN R0
      0b00000000,
      0b00000001  // HLT
    };

    int address = 0;

    for (int i = 0; i < DATA_LEN; i++) {
      cpu->ram[address++] = data[i];
    }
  } else {

    // TODO: Replace this with something less hard-coded
    FILE *fp;
    char line[1024];

    fp = fopen(argv[1], "r");

    int count = 0;
    while (fgets(line, 1024, fp) != NULL) {
      char *ptr;
      if (strncmp(line, "#", 1)){
        unsigned char ret = strtoul(line, &ptr, 2);
        cpu_ram_write(cpu, count++, ret);
      }
    }
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
      cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];
      break;
    case ALU_AND:
      cpu->registers[regA] = cpu->registers[regA] & cpu->registers[regB];
      break;
    case ALU_OR:
      cpu->registers[regA] = cpu->registers[regA] | cpu->registers[regB];
      break;
    case ALU_XOR:
      cpu->registers[regA] = cpu->registers[regA] ^ cpu->registers[regB];
      break;
    case ALU_NOT:
      cpu->registers[regA] = ~cpu->registers[regA];
      cpu->pc = cpu->pc - 1;
      break;
    case ALU_SHL:
      cpu->registers[regA] = cpu->registers[regA] << cpu->registers[regB];
      break;
    case ALU_SHR:
      cpu->registers[regA] = cpu->registers[regA] >> cpu->registers[regB];
      break;
    case ALU_MOD:
      cpu->registers[regA] = cpu->registers[regA] % cpu->registers[regB];
      if (cpu->registers[regA] == 0) {
        printf("Error: MOD is equal to 0");
        cpu->ram[cpu->pc+3] = HLT;
      }
      break;
    
    case ALU_CMP:
      if (cpu->registers[regA] == cpu->registers[regB]) {
        cpu->fl = 1;
      } else {
        cpu->fl = 0;
      }
      break;
    // TODO: implement more ALU ops
  }
  cpu->pc = cpu->pc + 3;
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction
  cpu->registers[7] = 255;

  unsigned char operandA, operandB;
  unsigned char reg_num, val;

  while (running) {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    unsigned char current = cpu->ram[cpu->pc];

    // 2. Figure out how many operands this next instruction requires
    // 3. Get the appropriate value(s) of the operands following this instruction
    operandA = cpu_ram_read(cpu, cpu->pc + 1);
    operandB = cpu_ram_read(cpu, cpu->pc + 2);

    // 4. switch() over it to decide on a course of action.
    // 5. Do whatever the instruction should do according to the spec.
    switch(current) {
      case PUSH:
        cpu->registers[7]--;
        reg_num = cpu->ram[current + 1];
        val = cpu->registers[reg_num];
        cpu->ram[cpu->registers[7]] = val;
        cpu->pc = cpu->pc + 2;
        break;

      case POP:
        reg_num = cpu->ram[current + 1];
        cpu->registers[reg_num] = cpu->ram[cpu->registers[7]];
        cpu->registers[7]++;
        cpu->pc = cpu->pc + 2;
        break;

      case HLT:
        running = 0;
        break;

      case LDI:
        cpu->registers[operandA] = operandB;
        cpu->pc = cpu->pc + 3;
        break;

      case PRN:
        printf("%d\n", cpu->registers[operandA]);
        cpu->pc = cpu->pc + 2;
        break;

      case MUL:
        alu(cpu, ALU_MUL, operandA, operandB);
        break;

      case CMP:
        alu(cpu, ALU_CMP, operandA, operandB);
        break;

      case AND:
        alu(cpu, ALU_AND, operandA, operandB);
        break;

      case NOT:
        alu(cpu, ALU_NOT, operandA, operandB);
        break;

      case OR:
        alu(cpu, ALU_OR, operandA, operandB);
        break;

      case XOR:
        alu(cpu, ALU_XOR, operandA, operandB);
        break;

      case SHL:
        alu(cpu, ALU_SHL, operandA, operandB);
        break;

      case SHR:
        alu(cpu, ALU_SHR, operandA, operandB);
        break;

      case MOD:
        alu(cpu, ALU_MOD, operandA, operandB);
        break;

      case JMP:
        cpu->pc = cpu->registers[operandA];
        break;

      case JEQ:
        if (cpu->fl == 1) {
          cpu->pc = cpu->registers[operandA];
        } else {
          cpu->pc = cpu->pc + 2;
        }
        break;

      case JNE:
        if (cpu->fl == 0) {
          cpu->pc = cpu->registers[operandA];
          // cpu->pc = cpu->pc + 1;
        } else {
          cpu->pc = cpu->pc + 2;
        }
        break;

      default:
        break;
    }
    // 6. Move the PC to the next instruction.
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->pc = 0;
  cpu->fl = 0;
  // memset(cpu->ram, 0, sizeof(cpu));
  cpu->ram = calloc(256, sizeof(unsigned char));
  cpu->registers = calloc(8, sizeof(unsigned char));
  // memset(cpu->ram, 0, sizeof(unsigned char) * 256);
  // memset(cpu->registers, 0, sizeof(unsigned char) * 8);
}
