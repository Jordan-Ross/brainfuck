#include <iostream>
#include <cstring>
#include <fstream>
#include <stack>
#include <vector>

using std::ifstream;


class Instruction
{
    public:
    enum Type
    {
        INC, DEC, LS, RS, LOOPSTART, LOOPEND, GET, PRINT
    };    

    Instruction(Type t, int jmp = -1) : type(t), jump_to(jmp)
    {}

    Type type;

    int jump_to;
};



std::vector<Instruction> bfparse(ifstream& file) 
{
    std::vector<Instruction> program;
    std::stack<int> stack; 

    char c;
    int opening;

    while (file.get(c))
    {
        switch (c) 
        {
        case '+':
            program.push_back(Instruction(Instruction::INC));
            break;
        case '-':
            program.push_back(Instruction(Instruction::DEC));
            break;
        case '<':
            program.push_back(Instruction(Instruction::LS));
            break;
        case '>':
            program.push_back(Instruction(Instruction::RS));
            break;
        case '[':
            // This instruction is the start of a loop, but we need to know where to jump to,
            //  which is set once the closing ] is encountered
            program.push_back(Instruction(Instruction::LOOPSTART));
            stack.push(program.size() - 1);
            break;
        case ']':
            if (stack.size() == 0) 
            {
                // TODO: unbalenced parens
                throw;
            }
            opening = stack.top();
            // Set to jump to the corresponding [
            program.push_back(
                    Instruction(Instruction::LOOPEND, 
                    opening));
            // Set opening brace of set to jump to here in program
            program[opening].jump_to = program.size() - 1;
            stack.pop();
            break;
        case ',':
            program.push_back(Instruction(Instruction::GET));
            break;
        case '.':
            program.push_back(Instruction(Instruction::PRINT));
            break;
        default:
            break;
        }
    }
    return program;
}

int bfexecute(std::vector<Instruction> program) 
{
    const int ARR_LEN = 30000;
    char arr[ARR_LEN] = {};

    char c;
    int idx = 0;
    int jump_to;

    for(std::vector<Instruction>::iterator counter = program.begin(); 
        counter != program.end();) 
    {
        switch (counter->type) 
        {
        case Instruction::INC:
            arr[idx]++;
            counter++;
            break;
        case Instruction::DEC:
            arr[idx]--;
            counter++;
            break;
        case Instruction::LS:
            if (idx == 0) idx = ARR_LEN;
            else idx--;
            counter++;
            break;
        case Instruction::RS:
            if (idx == ARR_LEN) idx = 0;
            else idx++;
            counter++;
            break;
        case Instruction::LOOPSTART:
            if (arr[idx] == 0) 
            {
                jump_to = counter->jump_to;
                counter = program.begin();
                std::advance(counter, jump_to);
            }
            else 
            {
                counter++;
            }
            break;
        case Instruction::LOOPEND:
            if (arr[idx] != 0) 
            {
                jump_to = counter->jump_to;
                counter = program.begin();
                std::advance(counter, jump_to);
            }
            else 
            {
                counter++;
            }
            break;
        case Instruction::GET:
            arr[idx] = getchar();
            counter++;
            break;
        case Instruction::PRINT:
            std::cout << arr[idx];
            counter++;
            break;
        default:
            break;
        }
    }
    return 0;
}

int main(int argc, char** argv) 
{
    std::string infile = "";
    for (int k = 0; k < argc; k++) 
    {
        if (strcmp(argv[k], "-i") == 0 && k + 1 < argc) 
        {
            infile = argv[k+1];
        }
    }

    ifstream file(infile); 
    std::vector<Instruction> program = bfparse(file);
    file.close();

    int status = bfexecute(program);

    return status;
}

