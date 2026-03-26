/*
 * ============================================================
 *  Three-Address Code (TAC) Generator
 *  Usage:  ./tac_gen <input_file>
 *
 *  Reads a source file and generates:
 *    1) TAC  - fully labelled, with conditional/unconditional jumps
 *    2) Quadruple table  - (op, arg1, arg2, result)
 *    3) Triple table     - (op, arg1, arg2) with back-references
 *
 *  Supported constructs:
 *    - Assignments          x = expr
 *    - Arithmetic           + - * /
 *    - Relational           < > <= >= == !=
 *    - Logical              && ||  (in conditions)
 *    - if / else if / else
 *    - while loops
 *    - for loops            (init ; cond ; incr)
 *    - break
 *    - 1-D array access     a[i]
 *    - 2-D array access     M[i][j]  (row-major flattening)
 *    - Arbitrarily nested loops / if-else blocks
 *    - Single-line // comments
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <iomanip>
#include <cstdlib>

// ===================================================================
//  TOKEN
// ===================================================================

enum TokKind
{
    T_ID,
    T_NUM,
    T_ASSIGN,
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_LT,
    T_GT,
    T_LE,
    T_GE,
    T_EQ,
    T_NEQ,
    T_AND,
    T_OR,
    T_NOT,
    T_LPAREN,
    T_RPAREN,
    T_LBRACKET,
    T_RBRACKET,
    T_LBRACE,
    T_RBRACE,
    T_SEMI,
    T_COMMA,
    T_IF,
    T_ELSE,
    T_WHILE,
    T_FOR,
    T_BREAK,
    T_EOF
};

struct Token
{
    TokKind kind;
    std::string val;
    int line;
};

// ===================================================================
//  LEXER
// ===================================================================

class Lexer
{
    std::string src;
    std::size_t pos = 0;
    int line = 1;

    bool peek2(char a, char b) const
    {
        return pos + 1 < src.size() && src[pos] == a && src[pos + 1] == b;
    }

    void skipWS()
    {
        while (pos < src.size())
        {
            char c = src[pos];
            if (c == '\n')
            {
                ++line;
                ++pos;
            }
            else if (std::isspace((unsigned char)c))
            {
                ++pos;
            }
            else if (peek2('/', '/'))
            {
                while (pos < src.size() && src[pos] != '\n')
                    ++pos;
            }
            else
                break;
        }
    }

    static TokKind keyword(const std::string &s)
    {
        if (s == "if")
            return T_IF;
        if (s == "else")
            return T_ELSE;
        if (s == "while")
            return T_WHILE;
        if (s == "for")
            return T_FOR;
        if (s == "break")
            return T_BREAK;
        return T_ID;
    }

public:
    explicit Lexer(std::string source) : src(std::move(source)) {}

    Token next()
    {
        skipWS();
        if (pos >= src.size())
            return {T_EOF, "", line};

        char c = src[pos];

        // integer literal
        if (std::isdigit((unsigned char)c))
        {
            std::string num;
            while (pos < src.size() && std::isdigit((unsigned char)src[pos]))
                num += src[pos++];
            return {T_NUM, num, line};
        }

        // identifier or keyword
        if (std::isalpha((unsigned char)c) || c == '_')
        {
            std::string id;
            while (pos < src.size() &&
                   (std::isalnum((unsigned char)src[pos]) || src[pos] == '_'))
                id += src[pos++];
            return {keyword(id), id, line};
        }

        // two-char operators (must come before single-char)
        if (peek2('<', '='))
        {
            pos += 2;
            return {T_LE, "<=", line};
        }
        if (peek2('>', '='))
        {
            pos += 2;
            return {T_GE, ">=", line};
        }
        if (peek2('=', '='))
        {
            pos += 2;
            return {T_EQ, "==", line};
        }
        if (peek2('!', '='))
        {
            pos += 2;
            return {T_NEQ, "!=", line};
        }
        if (peek2('&', '&'))
        {
            pos += 2;
            return {T_AND, "&&", line};
        }
        if (peek2('|', '|'))
        {
            pos += 2;
            return {T_OR, "||", line};
        }

        ++pos;
        switch (c)
        {
        case '=':
            return {T_ASSIGN, "=", line};
        case '+':
            return {T_PLUS, "+", line};
        case '-':
            return {T_MINUS, "-", line};
        case '*':
            return {T_STAR, "*", line};
        case '/':
            return {T_SLASH, "/", line};
        case '<':
            return {T_LT, "<", line};
        case '>':
            return {T_GT, ">", line};
        case '!':
            return {T_NOT, "!", line};
        case '(':
            return {T_LPAREN, "(", line};
        case ')':
            return {T_RPAREN, ")", line};
        case '[':
            return {T_LBRACKET, "[", line};
        case ']':
            return {T_RBRACKET, "]", line};
        case '{':
            return {T_LBRACE, "{", line};
        case '}':
            return {T_RBRACE, "}", line};
        case ';':
            return {T_SEMI, ";", line};
        case ',':
            return {T_COMMA, ",", line};
        default:
            std::cerr << "Unknown character '" << c
                      << "' at line " << line << '\n';
            return {T_EOF, "", line};
        }
    }

    std::vector<Token> tokenise()
    {
        std::vector<Token> v;
        Token t;
        do
        {
            t = next();
            v.push_back(t);
        } while (t.kind != T_EOF);
        return v;
    }
};

// ===================================================================
//  TAC INSTRUCTION
// ===================================================================
//
//  op        arg1       arg2     result      meaning
//  ----------------------------------------------------------------
//  "label"   -          -        -           LABEL:  (pseudo)
//  "="       rhs        -        dest        dest = rhs
//  binop     lhs        rhs      dest        dest = lhs op rhs
//  unaryop   operand    -        dest        dest = op operand
//  "[]r"     array      index    dest        dest = array[index]
//  "[]w"     array      index    rhs         array[index] = rhs
//  "goto"    -          -        target      goto TARGET
//  "if False" cond       -        target      if False cond goto TARGET

struct Instr
{
    std::string label; // used only when op == "label"
    std::string op, arg1, arg2, result;

    // Render as readable TAC text (label pseudo-instructions are handled outside)
    std::string str() const
    {
        if (op == "goto")
            return "goto " + result;
        if (op == "if False")
            return "if False " + arg1 + " goto " + result;
        if (op == "=")
            return result + " = " + arg1;
        if (op == "[]r")
            return result + " = " + arg1 + "[" + arg2 + "]";
        if (op == "[]w")
            return arg1 + "[" + arg2 + "] = " + result;
        if (!arg2.empty())
            return result + " = " + arg1 + " " + op + " " + arg2;
        if (!arg1.empty())
            return result + " = " + op + " " + arg1;
        return op;
    }
};

// ===================================================================
//  CODE GENERATOR  (recursive-descent parser + TAC emitter)
// ===================================================================

class CodeGen
{
    std::vector<Token> toks;
    std::size_t cur = 0;
    std::vector<Instr> code;
    int tempCnt = 0;
    int labelCnt = 0;
    std::stack<std::string> breakTarget; // innermost loop exit label

    // -- token helpers --------------------------------------------
    Token &peek() { return toks[cur]; }
    bool at(TokKind k) { return peek().kind == k; }
    Token consume() { return at(T_EOF) ? toks[cur] : toks[cur++]; }
    bool match(TokKind k)
    {
        if (at(k))
        {
            consume();
            return true;
        }
        return false;
    }

    void expect(TokKind k, const char *what)
    {
        if (!match(k))
        {
            std::cerr << "Parse error at line " << peek().line
                      << ": expected " << what
                      << ", got '" << peek().val << "'\n";
            std::exit(1);
        }
    }

    // -- name factories -------------------------------------------
    std::string freshTemp() { return "t" + std::to_string(++tempCnt); }
    std::string freshLabel() { return "L" + std::to_string(++labelCnt); }

    // -- emitters -------------------------------------------------
    void emit(const std::string &op,
              const std::string &a1 = "",
              const std::string &a2 = "",
              const std::string &res = "")
    {
        code.push_back({"", op, a1, a2, res});
    }
    void emitLabel(const std::string &lbl)
    {
        code.push_back({lbl, "label", "", "", ""});
    }

    // -- expressions (operator-precedence grammar) ----------------
    //
    //  Expr    -> Or
    //  Or      -> And ( '||' And )*
    //  And     -> Rel ( '&&' Rel )*
    //  Rel     -> Add ( relop Add )*
    //  Add     -> Mul ( ('+' | '-') Mul )*
    //  Mul     -> Unary ( ('*' | '/') Unary )*
    //  Unary   -> ('-' | '!') Unary  |  Primary
    //  Primary -> NUM | ID ('[' Expr ']' ('[' Expr ']')?)? | '(' Expr ')'

    std::string parseExpr() { return parseOr(); }

    std::string parseOr()
    {
        std::string lhs = parseAnd();
        while (at(T_OR))
        {
            consume();
            std::string rhs = parseAnd();
            std::string t = freshTemp();
            emit("||", lhs, rhs, t);
            lhs = t;
        }
        return lhs;
    }

    std::string parseAnd()
    {
        std::string lhs = parseRel();
        while (at(T_AND))
        {
            consume();
            std::string rhs = parseRel();
            std::string t = freshTemp();
            emit("&&", lhs, rhs, t);
            lhs = t;
        }
        return lhs;
    }

    std::string parseRel()
    {
        std::string lhs = parseAdd();
        for (;;)
        {
            std::string op;
            switch (peek().kind)
            {
            case T_LT:
                op = "<";
                break;
            case T_GT:
                op = ">";
                break;
            case T_LE:
                op = "<=";
                break;
            case T_GE:
                op = ">=";
                break;
            case T_EQ:
                op = "==";
                break;
            case T_NEQ:
                op = "!=";
                break;
            default:
                return lhs;
            }
            consume();
            std::string rhs = parseAdd();
            std::string t = freshTemp();
            emit(op, lhs, rhs, t);
            lhs = t;
        }
    }

    std::string parseAdd()
    {
        std::string lhs = parseMul();
        while (at(T_PLUS) || at(T_MINUS))
        {
            std::string op = consume().val;
            std::string rhs = parseMul();
            std::string t = freshTemp();
            emit(op, lhs, rhs, t);
            lhs = t;
        }
        return lhs;
    }

    std::string parseMul()
    {
        std::string lhs = parseUnary();
        while (at(T_STAR) || at(T_SLASH))
        {
            std::string op = consume().val;
            std::string rhs = parseUnary();
            std::string t = freshTemp();
            emit(op, lhs, rhs, t);
            lhs = t;
        }
        return lhs;
    }

    std::string parseUnary()
    {
        if (at(T_MINUS))
        {
            consume();
            std::string t = freshTemp();
            emit("-", parsePrimary(), "", t);
            return t;
        }
        if (at(T_NOT))
        {
            consume();
            std::string t = freshTemp();
            emit("!", parsePrimary(), "", t);
            return t;
        }
        return parsePrimary();
    }

    std::string parsePrimary()
    {
        if (at(T_LPAREN))
        {
            consume();
            std::string v = parseExpr();
            expect(T_RPAREN, ")");
            return v;
        }
        if (at(T_NUM))
            return consume().val;

        if (at(T_ID))
        {
            std::string name = consume().val;

            if (at(T_LBRACKET))
            {
                consume();
                std::string idx1 = parseExpr();
                expect(T_RBRACKET, "]");

                // 2-D: M[i][j] -> offset = i*cols + j
                if (at(T_LBRACKET))
                {
                    consume();
                    std::string idx2 = parseExpr();
                    expect(T_RBRACKET, "]");

                    std::string tRow = freshTemp(); // tRow = i * cols
                    std::string tOff = freshTemp(); // tOff = tRow + j
                    std::string tVal = freshTemp(); // tVal = name[tOff]
                    emit("*", idx1, "cols", tRow);
                    emit("+", tRow, idx2, tOff);
                    emit("[]r", name, tOff, tVal);
                    return tVal;
                }

                // 1-D: a[i]
                std::string t = freshTemp();
                emit("[]r", name, idx1, t);
                return t;
            }
            return name;
        }

        std::cerr << "Unexpected token '" << peek().val
                  << "' at line " << peek().line << '\n';
        std::exit(1);
    }

    // -- statements -----------------------------------------------

    // Block = '{' stmt* '}' | single-stmt
    void parseBlock()
    {
        if (at(T_LBRACE))
        {
            consume();
            while (!at(T_RBRACE) && !at(T_EOF))
                parseStmt();
            expect(T_RBRACE, "}");
        }
        else
        {
            parseStmt();
        }
    }

    // if '(' cond ')' block ( 'else' ( 'if' ... | block ) )?
    void parseIf()
    {
        consume(); // eat 'if'
        expect(T_LPAREN, "(");
        std::string cond = parseExpr();
        expect(T_RPAREN, ")");

        std::string lFalse = freshLabel(); // destination when condition false
        std::string lEnd = freshLabel();   // end of entire if-else chain

        emit("if False", cond, "", lFalse);
        parseBlock(); // true-branch

        if (at(T_ELSE))
        {
            consume();                  // eat 'else'
            emit("goto", "", "", lEnd); // skip else after true-branch
            emitLabel(lFalse);          // false-branch starts here
            if (at(T_IF))
                parseIf(); // else-if (recursive)
            else
                parseBlock();
            emitLabel(lEnd);
        }
        else
        {
            emitLabel(lFalse); // no else; false falls through
        }
    }

    // while '(' cond ')' block
    void parseWhile()
    {
        consume(); // eat 'while'

        std::string lStart = freshLabel(); // loop condition check
        std::string lEnd = freshLabel();   // loop exit

        breakTarget.push(lEnd);

        emitLabel(lStart);
        expect(T_LPAREN, "(");
        std::string cond = parseExpr();
        expect(T_RPAREN, ")");

        emit("if False", cond, "", lEnd); // exit when condition false
        parseBlock();                     // loop body
        emit("goto", "", "", lStart);     // back-edge
        emitLabel(lEnd);

        breakTarget.pop();
    }

    // for '(' init ';' cond ';' incr ')' block
    void parseFor()
    {
        consume(); // eat 'for'
        expect(T_LPAREN, "(");

        // init (optional)
        if (!at(T_SEMI))
            parseAssign();
        expect(T_SEMI, ";");

        std::string lCond = freshLabel(); // condition test
        std::string lBody = freshLabel(); // body start
        std::string lIncr = freshLabel(); // increment
        std::string lEnd = freshLabel();  // loop exit

        breakTarget.push(lEnd);

        emitLabel(lCond);

        // condition (optional -- defaults to true)
        std::string cond = "1";
        if (!at(T_SEMI))
            cond = parseExpr();
        expect(T_SEMI, ";");

        emit("if False", cond, "", lEnd);
        emit("goto", "", "", lBody); // cond true -> body

        // Save the increment token range, skip past ')'
        std::size_t incrStart = cur;
        {
            int depth = 1;
            while (cur < toks.size() && depth > 0)
            {
                if (toks[cur].kind == T_LPAREN)
                    ++depth;
                if (toks[cur].kind == T_RPAREN)
                    --depth;
                ++cur;
            }
            // cur now points one past the closing ')'
        }

        // Body
        emitLabel(lBody);
        parseBlock();

        // Increment (emit at lIncr, parsed from saved position)
        emitLabel(lIncr);
        std::size_t afterBody = cur;
        cur = incrStart;
        if (!at(T_RPAREN))
            parseAssign();
        cur = afterBody;

        emit("goto", "", "", lCond); // back to condition
        emitLabel(lEnd);

        breakTarget.pop();
    }

    // Parse one assignment statement (or bare expression).
    // Handles: x = expr | a[i] = expr | M[i][j] = expr | expr
    void parseAssign()
    {
        if (!at(T_ID))
        {
            parseExpr();
            return;
        }

        std::string name = consume().val;

        // Array write
        if (at(T_LBRACKET))
        {
            consume();
            std::string idx1 = parseExpr();
            expect(T_RBRACKET, "]");

            // 2-D write: M[i][j] = expr
            if (at(T_LBRACKET))
            {
                consume();
                std::string idx2 = parseExpr();
                expect(T_RBRACKET, "]");
                expect(T_ASSIGN, "=");
                std::string rhs = parseExpr();

                std::string tRow = freshTemp();
                std::string tOff = freshTemp();
                emit("*", idx1, "cols", tRow);
                emit("+", tRow, idx2, tOff);
                emit("[]w", name, tOff, rhs);
                return;
            }

            // 1-D write: a[i] = expr
            expect(T_ASSIGN, "=");
            std::string rhs = parseExpr();
            emit("[]w", name, idx1, rhs);
            return;
        }

        // Simple assignment: x = expr
        if (at(T_ASSIGN))
        {
            consume();
            std::string rhs = parseExpr();
            emit("=", rhs, "", name);
            return;
        }

        // Not an assignment -- rewind and parse as an expression
        --cur;
        parseExpr();
    }

    void parseStmt()
    {
        switch (peek().kind)
        {
        case T_IF:
            parseIf();
            break;
        case T_WHILE:
            parseWhile();
            break;
        case T_FOR:
            parseFor();
            break;
        case T_BREAK:
            consume();
            if (!breakTarget.empty())
                emit("goto", "", "", breakTarget.top());
            else
                std::cerr << "Warning: 'break' outside loop at line "
                          << peek().line << '\n';
            match(T_SEMI);
            break;
        case T_LBRACE:
            parseBlock();
            break;
        case T_SEMI:
            consume();
            break; // empty statement
        default:
            parseAssign();
            match(T_SEMI);
            break;
        }
    }

public:
    explicit CodeGen(std::vector<Token> tokens) : toks(std::move(tokens)) {}

    void run()
    {
        while (!at(T_EOF))
            parseStmt();
    }

    const std::vector<Instr> &instructions() const { return code; }
};

// ===================================================================
//  OUTPUT UTILITIES
// ===================================================================

static std::string pad(const std::string &s, int w)
{
    std::string r = s;
    if ((int)r.size() > w)
        r = r.substr(0, w); // truncate if too long
    while ((int)r.size() < w)
        r += ' ';
    return r;
}

// Print a table border row.
// style: 0 = top (+++), 1 = middle (+++), 2 = bottom (+++)
static void hRule(int style, const std::vector<int> &widths)
{
    const char *L[] = {"+", "+", "+"};
    const char *M[] = {"+", "+", "+"};
    const char *R[] = {"+", "+", "+"};
    std::cout << L[style];
    for (std::size_t i = 0; i < widths.size(); ++i)
    {
        for (int j = 0; j < widths[i] + 2; ++j)
            std::cout << "-";
        std::cout << (i + 1 < widths.size() ? M[style] : R[style]);
    }
    std::cout << '\n';
}

// -------------------------------------------------------------------
//  1. TAC (fully labelled)
// -------------------------------------------------------------------

static void printTAC(const std::vector<Instr> &code)
{
    std::cout << "\n+==============================================================+\n"
                 "|              THREE-ADDRESS CODE  (Labelled Form)            |\n"
                 "+==============================================================+\n\n";

    int seq = 0;
    for (const auto &ins : code)
    {
        if (ins.op == "label")
        {
            if (seq > 0)
                std::cout << '\n'; // blank line before label
            std::cout << ins.label << ":\n";
        }
        else
        {
            std::cout << "  [" << std::setw(3) << std::setfill('0') << seq
                      << std::setfill(' ') << "]   " << ins.str() << '\n';
            ++seq;
        }
    }
    std::cout << '\n';
}

// -------------------------------------------------------------------
//  2. Quadruple table  (op, arg1, arg2, result)
// -------------------------------------------------------------------

static void printQuadruple(const std::vector<Instr> &code)
{
    std::cout << "\n+==============================================================+\n"
                 "|                      QUADRUPLE TABLE                        |\n"
                 "+==============================================================+\n";

    const int WI = 4, WO = 10, WA = 14, WB = 14, WR = 16;
    hRule(0, {WI, WO, WA, WB, WR});
    std::cout << "| " << pad("#", WI) << " | "
              << pad("Op", WO) << " | "
              << pad("Arg1", WA) << " | "
              << pad("Arg2", WB) << " | "
              << pad("Result", WR) << " |\n";
    hRule(1, {WI, WO, WA, WB, WR});

    int n = 0;
    for (const auto &ins : code)
    {
        if (ins.op == "label")
            continue;
        std::cout << "| " << pad(std::to_string(n++), WI) << " | "
                  << pad(ins.op, WO) << " | "
                  << pad(ins.arg1, WA) << " | "
                  << pad(ins.arg2, WB) << " | "
                  << pad(ins.result, WR) << " |\n";
    }
    hRule(2, {WI, WO, WA, WB, WR});
    std::cout << '\n';
}

// -------------------------------------------------------------------
//  3. Triple table  (op, arg1, arg2)  -- no explicit result field
//     Temporaries produced at index N are referenced as (N).
// -------------------------------------------------------------------

struct Triple
{
    std::string op, arg1, arg2;
};

static std::string resolveTriple(const std::string &arg,
                                 const std::map<std::string, int> &t2i)
{
    auto it = t2i.find(arg);
    return (it != t2i.end()) ? "(" + std::to_string(it->second) + ")" : arg;
}

static void printTriple(const std::vector<Instr> &code)
{
    std::cout << "\n+==============================================================+\n"
                 "|                       TRIPLE TABLE                          |\n"
                 "+==============================================================+\n";

    std::vector<Triple> triples;
    std::map<std::string, int> t2i; // temp name -> triple index

    for (const auto &ins : code)
    {
        if (ins.op == "label")
            continue;

        Triple tr;
        tr.op = ins.op;
        tr.arg1 = resolveTriple(ins.arg1, t2i);
        tr.arg2 = resolveTriple(ins.arg2, t2i);

        int idx = (int)triples.size();
        triples.push_back(tr);

        // Only temporaries (names starting with 't' followed by digits)
        // carry implicit results; user variables do NOT.
        if (!ins.result.empty() && ins.result[0] == 't' && std::isdigit((unsigned char)ins.result[1]))
            t2i[ins.result] = idx;
    }

    const int WI = 4, WO = 10, WA = 18, WB = 18;
    hRule(0, {WI, WO, WA, WB});
    std::cout << "| " << pad("#", WI) << " | "
              << pad("Op", WO) << " | "
              << pad("Arg1", WA) << " | "
              << pad("Arg2", WB) << " |\n";
    hRule(1, {WI, WO, WA, WB});

    for (int i = 0; i < (int)triples.size(); ++i)
    {
        std::cout << "| " << pad(std::to_string(i), WI) << " | "
                  << pad(triples[i].op, WO) << " | "
                  << pad(triples[i].arg1, WA) << " | "
                  << pad(triples[i].arg2, WB) << " |\n";
    }
    hRule(2, {WI, WO, WA, WB});
    std::cout << '\n';
}

// ===================================================================
//  MAIN
// ===================================================================

int main(int argc, char *argv[])
{
    std::cout << "\n+==============================================================+\n"
                 "|       TAC GENERATOR  -  Compiler Design Lab                 |\n"
                 "|  Output: TAC (labelled)  |  Quadruple Table  |  Triple Table|\n"
                 "+==============================================================+\n";

    if (argc < 2)
    {
        std::cerr << "\nUsage:  " << argv[0] << " <input_file>\n\n"
                                                "Example:\n"
                                                "  "
                  << argv[0] << " input.txt\n\n"
                                "The input file should contain a C-like program.\n"
                                "Supported: assignments, if/else-if/else, while, for,\n"
                                "           break, 1-D/2-D arrays, nested loops, // comments.\n\n";
        return 1;
    }

    // Read source file
    std::ifstream fin(argv[1]);
    if (!fin)
    {
        std::cerr << "Error: cannot open '" << argv[1] << "'\n";
        return 1;
    }
    std::string src((std::istreambuf_iterator<char>(fin)),
                    std::istreambuf_iterator<char>());

    // Echo the source
    std::cout << "\nSource file : " << argv[1] << "\n";
    std::cout << "--------------------------------------------------------------\n";
    std::cout << src;
    if (!src.empty() && src.back() != '\n')
        std::cout << '\n';
    std::cout << "--------------------------------------------------------------\n";

    // Lex
    Lexer lex(src);
    auto tokens = lex.tokenise();

    // Parse + generate TAC
    CodeGen gen(std::move(tokens));
    gen.run();
    const auto &code = gen.instructions();

    if (code.empty())
    {
        std::cerr << "\nNothing to generate -- check the input file for syntax errors.\n";
        return 1;
    }

    // Output all three representations
    printTAC(code);
    printQuadruple(code);
    printTriple(code);

    return 0;
}