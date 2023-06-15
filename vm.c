#include <stdio.h>
#include <stdlib.h>

#define MAX_STACK_SIZE 1000
#define MAX_INPUT_SIZE 100

int stack[MAX_STACK_SIZE];
int PC = 0, BP = 499, SP = 500;
int eop = 1;

int OP, L, M;



int base(int l, int base);
const char *opcode_name(int op);

// Print the current state of the virtual machine
void print_current_state(int op, int l, int m, int prev_PC) {
    int i, count;

    printf("%s %d %d \t%d %d %d \t", opcode_name(op), l, m, PC, BP, SP);

    count = 0;
    for (i = 500; i >= SP && count < 7; i--) {
        printf("%d ", stack[i]);
        count++;
    }

    if (BP < 500) {
        printf("| ");
        int temp_BP = BP;
        count = 0;
        while (temp_BP < 500 && count < 7) {
            printf("%d ", stack[temp_BP]);
            temp_BP++;
            count++;
        }
    }

    printf("\n");
}

// Returns the string representation of the opcode
const char *opcode_name(int op) {
    switch (op) {
        case 1: return "LIT";
        case 2: return "OPR";
        case 3: return "LOD";
        case 4: return "STO";
        case 5: return "CAL";
        case 6: return "INC";
        case 7: return "JMP";
        case 8: return "JPC";
        case 9: return "SYS";
        default: return "UNKNOWN";
    }
}

// Execute the arithmetic operation specified by 'm'
void execute_arithmetic_operation(int m) {
    switch (m) {
        case 0:  // RTN
            PC = stack[BP - 2];
            SP = BP + 1;
            BP = stack[BP - 1];
            break;
        case 1:  // NEG
            stack[SP] = -stack[SP];
            PC++;
            break;
        case 2:  // ADD
            SP++;
            stack[SP] = stack[SP] + stack[SP - 1];
            PC++;
            break;
        case 3:  // SUB
            SP++;
            stack[SP] = stack[SP] - stack[SP - 1];
            PC++;
            break;
        case 4:  // MUL
            SP++;
            stack[SP] = stack[SP] * stack[SP - 1];
            PC++;
            break;
        case 5:  // DIV
            SP++;
            stack[SP] = stack[SP] / stack[SP - 1];
            PC++;
            break;
        case 6:  // ODD
            stack[SP] = stack[SP] % 2;
            PC++;
            break;
        case 7:  // MOD
            SP++;
            stack[SP] = stack[SP] % stack[SP - 1];
            PC++;
            break;
        case 8:  // EQL
            SP++;
            stack[SP] = stack[SP] == stack[SP - 1];
            PC++;
            break;
        case 9:  // NEQ
            SP++;
            stack[SP] = stack[SP] != stack[SP - 1];
            PC++;
            break;
        case 10:  // LSS
            SP++;
            stack[SP] = stack[SP] < stack[SP - 1];
            PC++;
            break;
        case 11:  // LEQ
            SP++;
            stack[SP] = stack[SP] <= stack[SP - 1];
            PC++;
            break;
        case 12:  // GTR
            SP++;
            stack[SP] = stack[SP] > stack[SP - 1];
            PC++;
            break;
        case 13:  // GEQ
            SP++;
            stack[SP] = stack[SP] >= stack[SP - 1];
            PC++;
            break;
    }
}

// Fetch and execute the instruction
void fetch_and_execute() {
    int prev_PC = PC;

    // Fetch the next instruction
    switch (OP) {
        case 1:  // LIT
            SP--;
            stack[SP] = M;
            PC = prev_PC + 1;
            break;
        case 2:  // OPR
            execute_arithmetic_operation(M);
            break;
        case 3:  // LOD
            SP--;
            stack[SP] = stack[base(L, BP) - M];
            PC = prev_PC + 1;
            break;
        case 4:  // STO
            stack[base(L, BP) - M] = stack[SP];
            SP++;
            PC = prev_PC + 1;
            break;
        case 5:  // CAL
            stack[SP - 1] = base(L, BP);  // Static Link (SL)
            stack[SP - 2] = BP;           // Dynamic Link (DL)
            stack[SP - 3] = prev_PC + 1;  // Return Address (RA)
            BP = SP - 1;
            PC = M;
            break;
        case 6:  // INC
            SP -= M;
            PC = prev_PC + 1;
            break;
        case 7:  // JMP
            PC = M;
            break;
        case 8:  // JPC
            if (stack[SP] == 0) {
                PC = M;
            } else {
                PC = prev_PC + 1;
            }
            SP++;
            break;
        case 9:  // SYS
            switch (M) {
                case 1:  // Write the top stack element to the screen
                    printf("\nOutput result is: %d\n", stack[SP]);
                    SP++;
                    break;
                case 2:  // Read in input from the user and store it on top of the stack
                    printf("\nPlease Enter an Integer: ");
                    scanf("%d", &stack[SP - 1]);
                    break;
                case 3:  // End of program (Set "eop" flag to zero)
                    eop = 0;
                    break;
            }
            PC = prev_PC + 1;
            break;
        default:
            printf("Error: Invalid opcode.\n");
            exit(1);
    }
    // Print the current state after executing the instruction
    print_current_state(OP, L, M, prev_PC);
}

// Get the base for L lexicographical levels down
int base(int l, int base) {
    while (l > 0) {
        base = stack[base - 1];
        l--;
    }
    return base;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Missing input file.\n");
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        printf("Error: Unable to open input file.\n");
        return 1;
    }

    printf("instruction; PC BP SP; stack | activation record\n");
    printf("Initial values: 0 499 500\n");

    while (eop && fscanf(input, "%d %d %d", &OP, &L, &M) == 3) {
        fetch_and_execute();
        //print_current_state(OP, L, M, PC);
    }

    fclose(input);
    return 0;
}
