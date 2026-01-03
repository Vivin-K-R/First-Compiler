#include <iostream>
#include <memory>
#include <vector>
//Lexer
//returns 0-255 for unknown character, else
//anyone below
enum Token{
    tok_eof=-1,
    tok_def=-2,
    tok_extern=-3,
    tok_identifier=-4,
    tok_number=-5,
};

static std::string identifierStr;
static double Numval;

static int gettok(){
    static int LastChar=' ';

    //white spaces need to be skipped
    while(isspace(LastChar)){
        LastChar = getchar();
    }

    //identifier
    if(isalpha(LastChar)){
        identifierStr = LastChar;
        while(isalnum(LastChar=getchar())){
            identifierStr += LastChar;
        }
        if(identifierStr == "def") return tok_def;
        if(identifierStr == "extern") return tok_extern;
        return tok_identifier;
    }

    //Numeric values
    if(isdigit(LastChar) || LastChar == '.'){
        std::string NumStr;
        do{
            NumStr += LastChar;
            LastChar = getchar();
        }while(isdigit(LastChar) || LastChar == '.');

        Numval = strtod(NumStr.c_str(),0);
        return tok_number;
    }

    //comments
    if(LastChar == '#'){
        do{
            LastChar = getchar();
        }while(LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        //continue read after comment
        if(LastChar != EOF) return gettok();
    }

    //final end of file checking
    if(LastChar == EOF) return tok_eof;

    //if curr char is not anything in language or we defined, then
    //return the ASCII
    int currChar = LastChar;
    LastChar = getchar();
    return currChar;
}

//Base class for every expression class (num,var,func)
class ExprAst{
    public:
        virtual ~ExprAst() = default;
};

class NumberExprAst : public ExprAst{
    double val;
    public:
        NumberExprAst(double val) : val(val){}
};

//parameter is a refernce, to avoid extra copy of passing string
class VariableExprAst : public ExprAst{
    std::string Name;
    public:
        VariableExprAst(const std::string &Name) : Name(Name){}
};

//To store expressions, Op for operators, Lhs,Rhs for oprands and sub expressions
class BinaryExprAst : public ExprAst{
    char Op;
    std::unique_ptr<ExprAst>Lhs,Rhs;
    public:
        BinaryExprAst(char Op,std::unique_ptr<ExprAst>Lhs,std::unique_ptr<ExprAst>Rhs) : Op(Op),Lhs(std::move(Lhs)),Rhs(std::move(Rhs)) {}
};

//vector to store pointerss of args of function
class CallExprAst : public ExprAst{
    std::string Calle;
    std::vector<std::unique_ptr<ExprAst>> Args;
    public:
        CallExprAst(const std::string &Calle,std::vector<std::unique_ptr<ExprAst>> Args) : Calle(Calle),Args(std::move(Args)) {}
};

//To store function prototype
//getter function returns as reference and const, caller cannot change string and function itself cannot change name
class PrototypeExprAst : public ExprAst{
    std::string Name;
    std::vector<std::string> Args;
    public:
        PrototypeExprAst(const std::string &Name,std::vector<std::string> Args) : Name(Name),Args(std::move(Args)){}
        const std::string &getName() const {return Name;}
};

//function definition (also includes prototype), so used previous Prototype class
class FunctionExprAst : public ExprAst{
    std::unique_ptr<PrototypeExprAst> Proto;
    std::unique_ptr<ExprAst> Body;
    public:
        FunctionExprAst(std::unique_ptr<PrototypeExprAst> Proto,std::unique_ptr<ExprAst> Body) : Proto(std::move(Proto)),Body(std::move(Body)) {}
};

//To read tokens from lexer and move for parsing
static int CurTok;
static int getNextToken(){
    return CurTok = gettok();
}

//helpers
std::unique_ptr<ExprAst> LogError(const char* S){
    fprintf(stderr,"Error : %s\n",S);
    return nullptr;
}
std::unique_ptr<PrototypeExprAst> LogErrorP(const char* S){
    LogError(S);
    return nullptr;
}

//Parser
//Number exprssion parsing
//NumberExpr = Number
static std::unique_ptr<ExprAst> ParseNumberExpr(){
    auto Result = std::make_unique<NumberExprAst>(Numval);
    return std::move(Result);
}

//Paranthesis
//ParseParanthExpr = ( Expr )
static std::unique_ptr<ExprAst> ParseParanthExpr(){
    getNextToken();//take (
    auto v = ParseExpression();  // reads expression inside ()
    if(!V) return nullptr;
    if(CurTok != ')') return LogError("expected ')'");
    getNextToken();//take )
    return V;
}

//identifier
// = identifier
// = identifier (Expr*)
static std::unique_ptr<ExprAst> ParseIdentifierExpr(){
    std::string IdName = identifierStr;
    getNextToken();//reading after the identifier
    if(CurTok != '(') return std::make_unique<VariableExprAst>(IdName);
    getNextToken();//executes to read next expr when encounterd a '('
    std::vector<std::unique_ptr<ExprAst>>Args;
    if(CurTok != ')'){
        while(true){
            if(auto Arg = ParseExpression())
                Args.push_back(std::move(Arg));
            else
                return nullptr;
            if(CurTok == ')') 
                break;
            if(CurTok != ',')
                return LogError("Expected ')' or '.' in argument list.");
            getNextToken(); // encountered ',' so moving to next expr
        }
    }
    getNextToken(); //encountered ')', moving to next
    return std::make_unique<CallExprAst>(IdName,std::move(Args));
}

//driver for parsing expr
static std::unique_ptr<ExprAst>ParseDriver(){
    switch (CurTok)
    {
    case tok_number:
        return ParseNumberExpr();
    case tok_identifier:
        return ParseIdentifierExpr();
    case '(':
        return ParseParanthExpr();
    default:
        return LogError("unkown token, expecting an expression.");
    }
}



