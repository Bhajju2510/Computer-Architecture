#include <iostream>
#include<string>
#include <iomanip>
#include <fstream>
using namespace std;

int Regdst, jump, branch, memread, memtoreg, alu, memwrite, alusrc, regwrite, sel1, sel2, sel3, sel4, sel5;


uint8_t opcode(uint32_t instruction)
{
    uint8_t out = 0;
    out = (uint8_t)instruction & 0x0000007f;
    return out;
}
uint32_t imm(uint32_t instruction)
{
    uint32_t out;
    out = (instruction & 0b11111111111111111111000000000000) >> 12;
    return out;
}

uint8_t rd(uint32_t instruction)
{
    uint32_t out;
    out = (instruction & 0b00000000000000000000111110000000) >> 7;
    return (uint8_t)out;
}

uint8_t func3(uint32_t instruction)
{
    uint32_t out;
    out = (instruction & 0b00000000000000000111000000000000) >> 12;
    return (uint8_t)out;
}

uint8_t rs1(uint32_t instruction)
{
    uint32_t out;
    out = (instruction & 0b00000000000011111000000000000000) >> 15;
    return (uint8_t)out;
}

uint8_t rs2(uint32_t instruction)
{
    uint32_t out;
    out = (instruction & 0b00000001111100000000000000000000) >> 20;
    return (uint8_t)out;
}
uint8_t funct7(uint32_t instruction)
{
    uint32_t out;
    out = (instruction & 0b11111110000000000000000000000000) >> 25;
    return (uint8_t)out;
}

uint32_t imm12(uint32_t instruction)
{
    uint32_t out;
    out = (instruction & 0b11111111111100000000000000000000) >> 20;
    if ((out >> 11) == 1) {
        // handle the case when imm is negtive
        return (out | 0b11111111111111111111000000000000);
    }
    else {
        return out;
    }
}
uint32_t shamt(uint32_t instruction)
{
    uint32_t out;
    out = (instruction & 0b00000001111100000000000000000000) >> 20;
        return out;
   
}

uint32_t branch_imm(uint32_t instr)
{
    uint32_t imm = 0;
    uint8_t imm4to11 = rd(instr);
    uint8_t imm12to10to5 = funct7(instr);
    uint32_t imm0 = 0;
    uint32_t imm11 = (imm4to11 & 0b00000001) << 11;
    uint32_t imm12 = (imm12to10to5 & 0b01000000) << 6;
    uint32_t imm10to5 = (imm12to10to5 & 0b00111111) << 5;
    uint32_t imm4to1 = (imm4to11 & 0b00011110);
    imm = imm12 | imm11 | imm10to5 | imm4to1 | imm0;
    if ((imm12 >> 12) != 0)
    {
        imm = imm | 0xffffe000;  // add sign extension
    }
    return imm;
}
uint32_t jal_offset(uint32_t instr) {
    uint32_t offset = 0;
    uint32_t offset0 = 0;
    uint32_t offset20 = (instr & 0x80000000) >> 11;
    uint32_t offset10to1 = (instr & 0b01111111111000000000000000000000) >> 20;
    uint32_t offset11 = (instr & 0b00000000000100000000000000000000) >> 9;
    uint32_t offset19to12 = (instr & 0b00000000000011111111000000000000);
    offset = offset20 | offset19to12 | offset11 | offset10to1 | offset0;
    if ((offset20 >> 20) != 0) {
        offset = offset | 0xfff00000; // add sign extension
    }
    return offset;
}

void display(uint32_t instruction)
{
    uint8_t op = opcode(instruction);
    int a, b, c,d,e, imm1;
    Regdst = jump = branch = memread = memtoreg = alu = memwrite = alusrc = regwrite = sel1 = sel2 = sel3 = sel4 = sel5 = 0;
    
    uint8_t  f3 = func3(instruction);
    uint8_t f7 = funct7(instruction);
    switch (op)
    {
    case 51://///////////////////////////////////////R type
        cout << "R type instruction" << endl;

        a = rs1(instruction);
        b = rs2(instruction);
        c = rd(instruction);
        if (f3 == 0b00000000 && f7 == 0b00000000)
        {
            cout << "ADD " << "R" << c << " R" << a << " R" << b << endl;

        }
        else if (f3 == 0b00000000 && f7 == 0b00100000)
        {
            cout << "SUB " << "R" << c << " R" << a << " R" << b << endl;

        }
        else if (f3 == 0b00000001 && f7 == 0b00000000)
        {
            cout << "SLL " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000010 && f7 == 0b00000000)
        {
            cout << "SLT " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000011 && f7 == 0b00000000)
        {
            cout << "SLTU " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000100 && f7 == 0b00000000)
        {
            cout << "XOR " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000101 && f7 == 0b00000000)
        {
            cout << "SRL " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000101 && f7 == 0b00100000)
        {
            cout << "SRA " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000110 && f7 == 0b00000000)
        {
            cout << "OR " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000000 && f7 == 0b00000001)
        {
            cout << "MUL " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000001 && f7 == 0b00000001)
        {
            cout << "MULH " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000010 && f7 == 0b00000001)
        {
            cout << "MULHSU " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000011 && f7 == 0b00000001)
        {
            cout << "MULHU " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000100 && f7 == 0b00000001)
        {
            cout << "DIV " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000101 && f7 == 0b00000001)
        {
            cout << "DIVU " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000110 && f7 == 0b00000001)
        {
            cout << "REM " << "R" << c << " R" << a << " R" << b << endl;
        }
        else if (f3 == 0b00000111 && f7 == 0b00000001)
        {
            cout << "REMU " << "R" << c << " R" << a << " R" << b << endl;
        }
        Regdst = jump = alu = regwrite = 1;
       

        cout << "Reg Dest:" << Regdst << "\t" << endl;
        cout << "Jump:" << jump << "\t" << endl;
        cout << "Branch:" << branch << "\t" << endl;
        cout << "Memory Read:" << memread << "\t" << endl;
        cout << "Memory to register:" << memtoreg << "\t" << endl; 
        cout << "Alu op:" << alu << "\t" << endl;;
        cout << "Memory Write:" << memwrite << "\t" << endl;
        cout << "Alu src:" << alusrc << "\t" << endl;
        cout << "Reg Write:" << regwrite << "\t" << endl;
        cout << "Sel 1:" << sel1 << "\t" << endl;
        cout << "Sel 2:" << sel2 << "\t" << endl;
        cout << "Sel 3:" << sel3<< "\t" << endl;
        cout << "Sel 4:" << sel4 << "\t" << endl;
        cout << "Sel 5:" << sel5 << "\t" << endl;;
        cout << "------------------------------------------" << endl;
        break;
    case 19:
        cout << "I type instruction" << endl;
        a = rs1(instruction);
        b = imm12(instruction);
        c = rd(instruction);
        d = shamt(instruction);
        if (f3 == 0b00000000)
        {
            cout << "ADDI " << "R" << c << " R" << a << " " << b << endl;

        }
        else if (f3 == 0b00000010 )
        {
            cout << "SLTI " << "R" << c << " R" << a << " " << b << endl;
        }
        else if (f3 == 0b00000011 )
        {
            cout << "SLTUI " << "R" << c << " R" << a << " " << b << endl;
        }
        else if (f3 == 0b00000100 )
        {
            cout << "XORI " << "R" << c << " R" << a << " " << b << endl;
        }
        else if (f3 == 0b00000110 )
        {
            cout << "ORI " << "R" << c << " R" << a << " " << b << endl;
        }
        else if (f3 == 0b00000111 )
        {
            cout << "ANDI " << "R" << c << " R" << a << " " << b << endl;
        }
        else if (f3 == 0b00000111 && f7==0b00000000)
        {
            cout << "SLLI " << "R" << c << " R" << a << " " << d << endl;
        }
        else if (f3 == 0b00000101 && f7 == 0b00000000)
        {
            cout << "SRLI " << "R" << c << " R" << a << " " << d << endl;
        }
        else if (f3 == 0b00000101 && f7 == 0b00100000)
        {
            cout << "SRAI " << "R" << c << " R" << a << " " << d << endl;
        }

        Regdst = jump = alu =alusrc= regwrite = 1;


        cout << "Reg Dest:" << Regdst << "\t" << endl;
        cout << "Jump:" << jump << "\t" << endl;
        cout << "Branch:" << branch << "\t" << endl;
        cout << "Memory Read:" << memread << "\t" << endl;
        cout << "Memory to register:" << memtoreg << "\t" << endl;
        cout << "Alu op:" << alu << "\t" << endl;;
        cout << "Memory Write:" << memwrite << "\t" << endl;
        cout << "Alu src:" << alusrc << "\t" << endl;
        cout << "Reg Write:" << regwrite << "\t" << endl;
        cout << "Sel 1:" << sel1 << "\t" << endl;
        cout << "Sel 2:" << sel2 << "\t" << endl;
        cout << "Sel 3:" << sel3 << "\t" << endl;
        cout << "Sel 4:" << sel4 << "\t" << endl;
        cout << "Sel 5:" << sel5 << "\t" << endl;;
        cout << "------------------------------------------" << endl;

        break;
    case 3:
        cout << "L type instruction" << endl;
        a = rs1(instruction);
        b = imm12(instruction);
        c = rd(instruction);
        
        if (f3 == 0b00000000)
        {
            cout << "LB " << "R" << c << " R" << a << " " << b << endl;

        }
        else if (f3 == 0b00000001)
        {
            cout << "LH " << "R" << c << " R" << a << " " << b << endl;
        }
        else if (f3 == 0b00000010)
        {
            cout << "LW " << "R" << c << " R" << a << " " << b << endl;
        }
        else if (f3 == 0b00000100)
        {
            cout << "LBU " << "R" << c << " R" << a << " " << b << endl;
        }
        else if (f3 == 0b00000101)
        {
            cout << "LHU " << "R" << c << " R" << a << " " << b << endl;
        }
        Regdst = jump =memread=memtoreg= alu = regwrite = 1;


        cout << "Reg Dest:" << Regdst << "\t" << endl;
        cout << "Jump:" << jump << "\t" << endl;
        cout << "Branch:" << branch << "\t" << endl;
        cout << "Memory Read:" << memread << "\t" << endl;
        cout << "Memory to register:" << memtoreg << "\t" << endl;
        cout << "Alu op:" << alu << "\t" << endl;;
        cout << "Memory Write:" << memwrite << "\t" << endl;
        cout << "Alu src:" << alusrc << "\t" << endl;
        cout << "Reg Write:" << regwrite << "\t" << endl;
        cout << "Sel 1:" << sel1 << "\t" << endl;
        cout << "Sel 2:" << sel2 << "\t" << endl;
        cout << "Sel 3:" << sel3 << "\t" << endl;
        cout << "Sel 4:" << sel4 << "\t" << endl;
        cout << "Sel 5:" << sel5 << "\t" << endl;;
        cout << "------------------------------------------" << endl;
        break;
    case 35:
        cout << "S type instruction" << endl;
        a = rs1(instruction);
        c = rs2(instruction);
        f7 = f7 << 5;
        e = f7 + rd(instruction);

        if (f3 == 0b00000000)
        {
            cout << "SB " << "R" << a << " R" << c << " " << e<< endl;

        }
        else if (f3 == 0b00000001)
        {
            cout << "SH " << "R" << a << " R" << c << " " << e << endl;
        }
        else if (f3 == 0b00000010)
        {
            cout << "SW " << "R" << a << " R" << c << " " << e << endl;
        }
        jump = memwrite =alu = regwrite = 1;


        cout << "Reg Dest:" << Regdst << "\t" << endl;
        cout << "Jump:" << jump << "\t" << endl;
        cout << "Branch:" << branch << "\t" << endl;
        cout << "Memory Read:" << memread << "\t" << endl;
        cout << "Memory to register:" << memtoreg << "\t" << endl;
        cout << "Alu op:" << alu << "\t" << endl;;
        cout << "Memory Write:" << memwrite << "\t" << endl;
        cout << "Alu src:" << alusrc << "\t" << endl;
        cout << "Reg Write:" << regwrite << "\t" << endl;
        cout << "Sel 1:" << sel1 << "\t" << endl;
        cout << "Sel 2:" << sel2 << "\t" << endl;
        cout << "Sel 3:" << sel3 << "\t" << endl;
        cout << "Sel 4:" << sel4 << "\t" << endl;
        cout << "Sel 5:" << sel5 << "\t" << endl;;
        cout << "------------------------------------------" << endl;
        break;

    case 99:
        cout << "SB type instruction" << endl;
        a = rs1(instruction);
        c = rs2(instruction);
      
        e = branch_imm(instruction);

        if (f3 == 0b00000000)
        {
            cout << "BEQ " << "R" << a << " R" << c << " " << e << endl;

        }
        else if (f3 == 0b00000001)
        {
            cout << "BNE " << "R" << a << " R" << c << " " << e << endl;
        }
        else if (f3 == 0b00000100)
        {
            cout << "BLT " << "R" << a << " R" << c << " " << e << endl;
        }
        else if (f3 == 0b00000101)
        {
            cout << "BGE " << "R" << a << " R" << c << " " << e << endl;
        }
        else if (f3 == 0b00000110)
        {
            cout << "BLTU " << "R" << a << " R" << c << " " << e << endl;
        }
        else if (f3 == 0b00000111)
        {
            cout << "BGEU " << "R" << a << " R" << c << " " << e << endl;
        }
        jump =branch= alu = regwrite = 1;


        cout << "Reg Dest:" << Regdst << "\t" << endl;
        cout << "Jump:" << jump << "\t" << endl;
        cout << "Branch:" << branch << "\t" << endl;
        cout << "Memory Read:" << memread << "\t" << endl;
        cout << "Memory to register:" << memtoreg << "\t" << endl;
        cout << "Alu op:" << alu << "\t" << endl;;
        cout << "Memory Write:" << memwrite << "\t" << endl;
        cout << "Alu src:" << alusrc << "\t" << endl;
        cout << "Reg Write:" << regwrite << "\t" << endl;
        cout << "Sel 1:" << sel1 << "\t" << endl;
        cout << "Sel 2:" << sel2 << "\t" << endl;
        cout << "Sel 3:" << sel3 << "\t" << endl;
        cout << "Sel 4:" << sel4 << "\t" << endl;
        cout << "Sel 5:" << sel5 << "\t" << endl;;
        cout << "------------------------------------------" << endl;
        break;
    case 55:
        c = rd(instruction);
        imm1 = imm(instruction);
        cout << "U type instruction" << endl;
        cout << "LUI " << " R" << c << " " << imm1<< endl;
        Regdst = jump  = regwrite =sel4=1;


        cout << "Reg Dest:" << Regdst << "\t" << endl;
        cout << "Jump:" << jump << "\t" << endl;
        cout << "Branch:" << branch << "\t" << endl;
        cout << "Memory Read:" << memread << "\t" << endl;
        cout << "Memory to register:" << memtoreg << "\t" << endl;
        cout << "Alu op:" << alu << "\t" << endl;;
        cout << "Memory Write:" << memwrite << "\t" << endl;
        cout << "Alu src:" << alusrc << "\t" << endl;
        cout << "Reg Write:" << regwrite << "\t" << endl;
        cout << "Sel 1:" << sel1 << "\t" << endl;
        cout << "Sel 2:" << sel2 << "\t" << endl;
        cout << "Sel 3:" << sel3 << "\t" << endl;
        cout << "Sel 4:" << sel4 << "\t" << endl;
        cout << "Sel 5:" << sel5 << "\t" << endl;;
        cout << "------------------------------------------" << endl;
     break;
    case 23:
        c = rd(instruction);
        imm1 = imm(instruction);
        cout << "U type instruction" << endl;
        cout << "AUIPC" << " R" << c << " " << imm1 << endl;
        Regdst = jump = alu = regwrite = sel2=sel3= 1;


        cout << "Reg Dest:" << Regdst << "\t" << endl;
        cout << "Jump:" << jump << "\t" << endl;
        cout << "Branch:" << branch << "\t" << endl;
        cout << "Memory Read:" << memread << "\t" << endl;
        cout << "Memory to register:" << memtoreg << "\t" << endl;
        cout << "Alu op:" << alu << "\t" << endl;;
        cout << "Memory Write:" << memwrite << "\t" << endl;
        cout << "Alu src:" << alusrc << "\t" << endl;
        cout << "Reg Write:" << regwrite << "\t" << endl;
        cout << "Sel 1:" << sel1 << "\t" << endl;
        cout << "Sel 2:" << sel2 << "\t" << endl;
        cout << "Sel 3:" << sel3 << "\t" << endl;
        cout << "Sel 4:" << sel4 << "\t" << endl;
        cout << "Sel 5:" << sel5 << "\t" << endl;;
        cout << "------------------------------------------" << endl;
        break;
    case 111:
        c = rd(instruction);
        imm1 = jal_offset(instruction);
        cout << "UJ type instruction" << endl;
        cout << "JAL " << " R" << c << " " << imm1 << endl;
        Regdst = sel5 = 1;


        cout << "Reg Dest:" << Regdst << "\t" << endl;
        cout << "Jump:" << jump << "\t" << endl;
        cout << "Branch:" << branch << "\t" << endl;
        cout << "Memory Read:" << memread << "\t" << endl;
        cout << "Memory to register:" << memtoreg << "\t" << endl;
        cout << "Alu op:" << alu << "\t" << endl;;
        cout << "Memory Write:" << memwrite << "\t" << endl;
        cout << "Alu src:" << alusrc << "\t" << endl;
        cout << "Reg Write:" << regwrite << "\t" << endl;
        cout << "Sel 1:" << sel1 << "\t" << endl;
        cout << "Sel 2:" << sel2 << "\t" << endl;
        cout << "Sel 3:" << sel3 << "\t" << endl;
        cout << "Sel 4:" << sel4 << "\t" << endl;
        cout << "Sel 5:" << sel5 << "\t" << endl;;
        cout << "------------------------------------------" << endl;
        break;

    case 103:
        a = rs1(instruction);
        b = imm12(instruction);
        c = rd(instruction);
        cout << "I type instruction" << endl;
        cout << "JALR " << "R" << c << " R" << a << " " << b << endl;
        Regdst = jump = alu =alusrc= regwrite =sel1= 1;


        cout << "Reg Dest:" << Regdst << "\t" << endl;
        cout << "Jump:" << jump << "\t" << endl;
        cout << "Branch:" << branch << "\t" << endl;
        cout << "Memory Read:" << memread << "\t" << endl;
        cout << "Memory to register:" << memtoreg << "\t" << endl;
        cout << "Alu op:" << alu << "\t" << endl;;
        cout << "Memory Write:" << memwrite << "\t" << endl;
        cout << "Alu src:" << alusrc << "\t" << endl;
        cout << "Reg Write:" << regwrite << "\t" << endl;
        cout << "Sel 1:" << sel1 << "\t" << endl;
        cout << "Sel 2:" << sel2 << "\t" << endl;
        cout << "Sel 3:" << sel3 << "\t" << endl;
        cout << "Sel 4:" << sel4 << "\t" << endl;
        cout << "Sel 5:" << sel5 << "\t" << endl;;
        cout << "------------------------------------------" << endl;
        break;
     default:
        cout << "Invalid Input.....please try again" << endl;
        break;
    }
}

int binaryToDecimal(string n)
{
    string num = n;
    int dec_value = 0;

    // Initializing base value to 1, i.e 2^0
    int base = 1;

    int len = num.length();
    for (int i = len - 1; i >= 0; i--) {
        if (num[i] == '1')
            dec_value += base;
        base = base * 2;
    }

    return dec_value;
}

/*int main()
{
    char ch;
    char c[32];
    int j;
    ifstream infile;
    string filename = "";
    cout << "Enter the file name to open"<<endl;
    cin >> filename;
    infile.open(filename);
    while (!infile.is_open())
    {
        cout << "Sorry the file did not open try again or type cancel" << endl;
        cin >> filename;
            if (filename == "cancel")
            {
                cout << "Cancelled" << endl;
                break;
            }
            else
            {
                infile.open(filename);
            }
    }
    if (infile.is_open())
    {
        while (!infile.eof())
        {
            infile >> ch;	//reading from file
            if (ch != '\n')
            {
                c[j] = ch;
            }
            else
            {
                j = 0;
                int i;
                string s = "";
                for (i = 0; i < 32; i++)
                {
                    s = s + c[i];
                }
                int instruction = (uint32_t)binaryToDecimal(s);

                display(instruction);

            }
        }
    }
    //close the file
    infile.close();


    return 0;


}*/
int main()
{
    uint32_t instruction;
    string arr[] = { "00000000010100011000010000110011","01000000010000101000000010110011","00000000001000011100000010110011","00000000100000111110011110110011","00000000010100110111001000110011","00000000010100100000010010010011","00000000010100101010101000010011","00000000001000011100000010010011","00000000100000111110011110010011","00000000010100110111001000010011","00000000010100010000000110100011","0000000100011001001001000100011","000001000110010010001000000011","000000000001000000001000000011","00000000010100101000000001100011","00000000011101001001000001100011","11111110100000110100111111100011","11111110100001010101111111100011","00000000000000000011000110110111","00000000000000000111001110010111" };
    for (int i = 0; i < 20; i++)
    {
        instruction = (uint32_t)binaryToDecimal(arr[i]);
        display(instruction);
     }
    return 0;




}
        
        
        
        
  