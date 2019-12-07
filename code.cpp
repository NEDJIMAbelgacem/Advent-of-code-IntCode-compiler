#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <fstream>

using namespace std;

#define assert(err) if(!err) exit(-1)


enum TokenType {Number, OpenParens, CloseParens, Operator, InterMark, TwoPoints};
enum InstrCode {ADD = 1, MULT = 2, IN = 3, OUT = 4, JMPNZ = 5, JMPZ = 6, LT = 7, EQL = 8, DIV = 9, SUB = 10, MOD = 11,
                GT = 12, LTEQL = 13, GTEQL = 14, HALT = 99};
enum AddressMode {Position = 0, Immediate = 1, Offset = 2};

std::string instruction_mnemonic(int instr) {
    switch(instr % 100) {
        case ADD: return "ADD";
        case MULT: return "MULT";
        case IN: return "IN";
        case OUT: return "OUT";
        case JMPNZ: return "JMPNZ";
        case JMPZ: return "JMPZ";
        case LT: return "LT";
        case EQL: return "EQL";
        case HALT: return "HALT";
        case MOD: return "MOD";
        case GT: return "GT";
        case LTEQL: return "LTEQL";
        case GTEQL: return "GTEQL";
    }
    return "";
}

int instr_length(int instr) {
    switch(instr % 100) {
            case MULT:
            case ADD:
            case LT:
            case EQL:
            case DIV:
            case SUB:
            case GT:
            case LTEQL:
            case GTEQL:
                return 4;
            case JMPNZ:
            case JMPZ:
                return 3;
            case IN:
            case OUT: return 2;
            case HALT: return 1;
        }
}

std::string token_type_str(TokenType t) {
    switch(t) {
        case Number: return "Number";
        case OpenParens: return "CloseParens";
        case Operator: return "Operator";
        case InterMark: return "InterMark";
        case TwoPoints: return "TwoPoints";
    }
    return "";
}

AddressMode instr_addressing_mode(int instr, int operand) {
    return (AddressMode)(instr / (operand == 1 ? 100 : operand == 2 ? 1000 : 10000) % 10);
}

int instruction_code(InstrCode instr, AddressMode op1 = Position, AddressMode op2 = Position, AddressMode op3 = Position) {
    return instr % 100 + op1 * 100 + op2 * 1000 + op3 * 10000;
}

int get_operator_pos(int instr, int op, int instr_ptr, std::vector<int>& input) {
    int adr_mode = instr / (op == 1 ? 100 : op == 2 ? 1000 : 10000) % 10;
    switch (adr_mode) {
        case Position: return input[instr_ptr + op];
        case Immediate: return instr_ptr + op;
        case Offset: return instr_ptr + op + input[instr_ptr + op];
    }
    return -1;
}

int run_program(std::vector<int>& input) {
    int instr_ptr = 0;
    while (instr_ptr < input.size()) {
        int opcode = input[instr_ptr];
        // std::cout << "running " << instruction_mnemonic(opcode) << std::endl;
        int op1 = get_operator_pos(opcode, 1, instr_ptr, input);
        int op2 = get_operator_pos(opcode, 2, instr_ptr, input);
        int op3 = get_operator_pos(opcode, 3, instr_ptr, input);
        bool halt = false;
        
        switch (opcode % 100) {
            case ADD:
                input[op3] = input[op1] + input[op2];
                break;
            case MULT:
                input[op3] = input[op1] * input[op2];
                break;
            case IN:
                int in;
                std::cout << "IN : ";
                std::cin >> in;
                input[op1] = in;
                break;
            case OUT:
                std::cout << "OUT : " << input[op1] << std::endl;
                break;
            case JMPNZ:
                if (input[op1]) instr_ptr = op2 - 3;
                break;
            case JMPZ:
                if (!input[op1]) instr_ptr = op2 - 3;
                break;
            case LT:
                input[op3] = input[op1] < input[op2];
                break;
            case EQL:
                input[op3] = input[op1] == input[op2];
                break;
            case GT:
                input[op3] = input[op1] > input[op2];
                break;
            case GTEQL:
                input[op3] = input[op1] >= input[op2];
                break;
            case LTEQL:
                input[op3] = input[op1] <= input[op2];
                break;
            case HALT:
                // std::cout << "HALTING" << std::endl;
                halt = true;
                break;
            // Extended Stuff
            case DIV:
                input[op3] = input[op1] / input[op2];
                break;
            case SUB:
                input[op3] = input[op1] - input[op2];
                break;
            default:
                halt = true;
                break;
        }
        if (halt) break;
        instr_ptr += instr_length(opcode);
    }
    // if (i != 99) std::cerr << "Error at " << i << std::endl;
    return input[0];
}

struct Node {
    std::string content;
    std::vector<Node> children;

    Node() { }
    Node(std::string _content) : content(_content) { }
    Node(std::string _content, std::vector<Node> _children) : content(_content), children(_children) { }

    int evaluate() {
        if (content == "*") {
            return children[0].evaluate() * children[1].evaluate();
        } else if (content == "/") {
            return children[0].evaluate() / children[1].evaluate();
        } else if (content == "+") {
            return children[0].evaluate() + children[1].evaluate();
        } else if (content == "%") {
            return children[0].evaluate() % children[1].evaluate();
        } else if (content == "<") {
            return children[0].evaluate() < children[1].evaluate();
        } else if (content == ">") {
            return children[0].evaluate() > children[1].evaluate();
        } else if (content == "<=") {
            return children[0].evaluate() <= children[1].evaluate();
        } else if (content == ">=") {
            return children[0].evaluate() >= children[1].evaluate();
        } else if (content == "-") {
            if (children.size() == 1) return -children[0].evaluate();
            return children[0].evaluate() - children[1].evaluate();
        } else if (content == "?:") {
            int op1 = children[0].evaluate();
            if (op1 != 0) return children[2].evaluate();
            return children[1].evaluate();
        }
        return stoi(content);
    }

    void print() {
        std::vector<Node> nodes{*this};
        while (nodes.size()) {
            std::vector<Node> next_nodes;
            for (Node& n : nodes) {
                std::cout << n.content << " ";
                next_nodes.insert(next_nodes.begin(), n.children.begin(), n.children.end());
            }
            nodes = next_nodes;
            std::cout << std::endl;
        }
    }

    std::vector<int> assemble() {
        auto get_instr = [](std::string opr) -> InstrCode {
            if (opr == "*") return MULT;
            if (opr == "/") return DIV;
            if (opr == "+") return ADD;
            if (opr == "-") return SUB;
            if (opr == "%") return MOD;
            if (opr == "<") return LT;
            if (opr == "==") return EQL;
            if (opr == ">") return GT;
            if (opr == ">=") return GTEQL;
            if (opr == "<=") return LTEQL;
            return HALT;
        };
        std::vector<int> children_borders;
        std::vector<int> children_assembly;
        if (content == "?:") {
            std::vector<int> instruction1 {instruction_code(JMPNZ, Offset, Offset), 0, 0};
            std::vector<int> instruction2 {instruction_code(ADD, Immediate, Offset, Offset), 0, 0, 0};
            std::vector<int> instruction3 {instruction_code(JMPNZ, Immediate, Offset), 1, 0};
            std::vector<int> op1 = children[0].assemble();
            std::vector<int> op2 = children[1].assemble();
            std::vector<int> op3 = children[2].assemble();
            children_assembly.insert(children_assembly.end(), op1.begin(), op1.end());

            instruction1[1] = -2;
            instruction1[2] = op2.size() + 8;
            children_assembly.insert(children_assembly.end(), instruction1.begin(), instruction1.end());

            children_assembly.insert(children_assembly.end(), op2.begin(), op2.end());

            instruction2[2] = -3;
            instruction2[3] = op3.size() + 3;
            children_assembly.insert(children_assembly.end(), instruction2.begin(), instruction2.end());

            instruction3[2] = op3.size() + 1;
            children_assembly.insert(children_assembly.end(), instruction3.begin(), instruction3.end());
            children_assembly.insert(children_assembly.end(), op3.begin(), op3.end());
            return children_assembly;
        }
        if (content[0] <= '9' && content[0] >= '0') {
            return {instruction_code(ADD, Immediate, Immediate, Offset), 0, stoi(content), 0};
        }

        if (content == "-" && children.size() == 1) {
            std::vector<int> operand = children[0].assemble();
            children_assembly.insert(children_assembly.end(), operand.begin(), operand.end());
            std::vector<int> instr = {instruction_code(SUB, Immediate, Offset, Offset), 0, -3, 0};
            children_assembly.insert(children_assembly.end(), instr.begin(), instr.end());
            return children_assembly;
        }

        for (Node& n : children) {
            std::vector<int> operand = n.assemble();
            children_assembly.insert(children_assembly.end(), operand.begin(), operand.end());
            children_borders.push_back(children_assembly.size() - 1);
        }

        int opr = instruction_code(get_instr(content), Offset, Offset, Offset);
        std::vector<int> operation {opr, 0, 0, 0};
        operation[1] = -children_assembly.size() + children_borders[0] - 1;
        operation[2] = -children_assembly.size() + children_borders[1] - 2;
        children_assembly.insert(children_assembly.end(), operation.begin(), operation.end());
        return children_assembly;
    }
};

class Parser {
    std::vector<std::pair<TokenType, std::string>> tokens;
    int tokens_ptr = 0;
    Node root;
    std::set<std::string> boolean_operators {"==", "<=", ">=", ">", "<"};
public:
    Parser(std::vector<std::pair<TokenType, std::string>> _tokens) : tokens(_tokens) {
        root = parse_S();
    }

    int consume(TokenType type) {
        if (tokens[tokens_ptr].first != type) {
            std::cerr << "Unexpected token " << tokens[tokens_ptr].second << std::endl;
            exit(-1);
        }
        tokens_ptr++;
        return true;
    }

    Node getASTroot() {
        return root;
    }

    // Grammar
    // E -> E + T | E - T | T
    // T -> T * B | T / B | T % n | B
    // B -> F | F < F | | F > F | F >= F | F <= F | F == F
    // F -> -A | A
    // A -> (E) | -F | n

    Node parse_S() {
        // std::cout << "parse_S" << std::endl;
        Node op1 = parse_E();
        if (tokens_ptr >= tokens.size()) return op1;
        consume(InterMark);
        Node op2 = parse_E();
        consume(TwoPoints);
        Node op3 = parse_E();
        return Node("?:", std::vector<Node>{op1, op2, op3});
    }

    Node parse_E() {
        // std::cout << "parse_E " << std::endl;
        Node n1 = parse_T();
        while (tokens[tokens_ptr].first == Operator && (tokens[tokens_ptr].second == "+" || tokens[tokens_ptr].second == "-")) {
            std::string opr = tokens[tokens_ptr].second;
            consume(Operator);
            Node n2 = parse_T();
            Node n3(opr, std::vector<Node> { n1, n2 });
            n1 = n3;
        }
        return n1;
    }

    Node parse_T() {
        // std::cout << "parse_T" << std::endl;
        Node n1 = parse_B();
        while (tokens[tokens_ptr].first == Operator && (tokens[tokens_ptr].second == "*" || tokens[tokens_ptr].second == "/")) {
            std::string opr = tokens[tokens_ptr].second;
            consume(Operator);
            Node n2 = parse_B();
            Node n3(opr, std::vector<Node> { n1, n2 });
            n1 = n3;
        }
        if (tokens[tokens_ptr].first == Operator && tokens[tokens_ptr].second == "%") {
            consume(Operator);
            Node n2(tokens[tokens_ptr].second);
            consume(Number);
            Node n3("%", std::vector<Node> { n1, n2 });
            n1 = n3;
        }
        return n1;
    }

    Node parse_B() {
        Node n1 = parse_F();
        if (tokens[tokens_ptr].first == Operator && boolean_operators.find(tokens[tokens_ptr].second) != boolean_operators.end()) {
            std::string opr = tokens[tokens_ptr].second;
            consume(Operator);
            Node n2 = parse_F();
            Node n3(opr, std::vector<Node> { n1, n2 });
            n1 = n3;
        }
        return n1;
    }

    Node parse_F() {
        // std::cout << "parse_F" << std::endl;
        if (tokens[tokens_ptr].second == "-") {
            consume(Operator);
            return Node("-", std::vector<Node> { parse_A() });
        }
        return parse_A();
    }

    Node parse_A() {
        // std::cout << "parse_A" << std::endl;
        if (tokens[tokens_ptr].first == Number) {
            std::string n = tokens[tokens_ptr].second;
            consume(Number);
            return Node(n);
        }
        if (tokens[tokens_ptr].second == "-") {
            consume(Operator);
            return Node("-", std::vector<Node> { parse_F() });
        }
        consume(OpenParens);
        Node n = parse_S();
        consume(CloseParens);
        return n;
    }

};

std::vector<int> convert_expression(std::string expr) {
    std::set<char> space_chars = {'\t', '\n', '\b', ' '};
    int input_ptr = 0;
    auto get_next_token = [&]() -> std::pair<TokenType, std::string> {
        if (expr[input_ptr] == '(') {
            input_ptr++;
            return {OpenParens, "("};
        }
        if (expr[input_ptr] == ')') {
            input_ptr++;
            return {CloseParens, ")"};
        }
        if (expr[input_ptr] == '?') {
            input_ptr++;
            return {InterMark, "?"};
        }
        if (expr[input_ptr] == ':') {
            input_ptr++;
            return {TwoPoints, ":"};
        }
        if (input_ptr + 1 < expr.size() && expr[input_ptr] == '=' && expr[input_ptr + 1] == '=') {
            input_ptr += 2;
            return {Operator, "=="};
        }
        if (expr[input_ptr] > '9' || expr[input_ptr] < '0') {
            std::string opr { expr[input_ptr] };
            input_ptr++;
            if (input_ptr < expr.size() && (opr == "<" || opr == ">") && expr[input_ptr] == '=') {
                opr.push_back('=');
                input_ptr++;
            }
            return {Operator, opr};
        }
        int i = input_ptr;
        std::string t;
        while (input_ptr < expr.size() && expr[input_ptr] <= '9' && expr[input_ptr] >= '0') {
            t.push_back(expr[input_ptr]);
            ++input_ptr;
        }
        return {Number, t};
    };
    std::vector<std::pair<TokenType, std::string>> tokens;
    while (input_ptr < expr.size()) {
        tokens.push_back(get_next_token());
        while (input_ptr < expr.size() && space_chars.find(expr[input_ptr]) != space_chars.end()) input_ptr++;
    }
    // for (auto [a, b] : tokens) std::cout << b << ", ";
    // std::cout << std::endl;
    Parser p(tokens);
    Node ast = p.getASTroot();
    std::vector<int> assembly = ast.assemble();
    assembly.push_back(99);
    return assembly;
}

std::vector<int> erase_offset_mode(const std::vector<int>& program) {
    std::vector<int> prg = program;
    int instr_ptr = 0;
    while (instr_ptr < prg.size()) {
        int opcode = prg[instr_ptr];
        std::vector<AddressMode> ops = {
            instr_addressing_mode(opcode, 1),
            instr_addressing_mode(opcode, 2),
            instr_addressing_mode(opcode, 3)
        };
        for (int i = 1; i <= 3; ++i) {
            if (ops[i - 1] == Offset) {
                prg[instr_ptr + i] += instr_ptr + i;
                ops[i - 1] = Position;
            }
        }
        prg[instr_ptr] = instruction_code((InstrCode)(prg[instr_ptr] % 100), ops[0], ops[1], ops[2]);
        instr_ptr += instr_length(opcode);
    }
    return prg;
}

int main() {
    std::string expression;
    std::cout << "Input expression : ";
    std::getline(std::cin, expression);
    std::vector<int> program = convert_expression(expression);
    program = erase_offset_mode(program);
    std::cout << "Assembled program memry: " << std::endl;
    for (int i : program) std::cout << i << " ";
    std::cout << std::endl;
    run_program(program);
    std::cout << "Program memory after execution : " << std::endl;
    for (int i : program) std::cout << i << " ";
    std::cout << std::endl;
    std::cout << "Expression result (end of input before HALT instruction): " << std::endl;
    std::cout << program[program.size() - 2] << std::endl;
}