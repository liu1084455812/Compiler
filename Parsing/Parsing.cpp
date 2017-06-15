#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define AnrrySize 1024
//学到了新知识，<ABC />表示该标签部分为空

//特性：
//支持多种数据定义
//支持指针定义(包括多层例如 int** a;)
//支持定义时赋值
//语句：
//支持变量定义，函数定义(包括定义时赋值)，赋值语句，return语句，四则运算（支持优先级），支持FOR循环，支持while循环，If、While、For均支持多层（超过两层）嵌套
//支持控制语句
//支持块语句
//缺陷:
//不支持全局变量
//不支持数组
//FOR循环每个分号间只能放一条语句
//未实现比较语句

//随着语法分析的编写发现原词法分析的问题：
//没有程序结尾添加“#”属性字

//本语法分析器与框架中不兼容部分：
//由于实现了IF_ELSE导致XML中多出ELSE标签，少了ELSEIF标签
//由于实现了表达式的优先级，多了很多标签

//原框架存在的问题：
//原框架中IF语句的语法分析有问题，当在分析if(a=1);时不能输出选择后的语句内容。
//原框架中IF语句只有if(a=1){a=1;}else{a=1;}这种选择的段的语句在{}中时，可以正常输出。
//原框架不支持while语句
//原框架中语法分析器需要不能分析<运算符相关的东西，因为在XML中<会破坏后边的标签造成token.XML输入错误
//原框架中语法分析不能分析==等复杂操作符
//原框架没有实现乘法，更何况优先级的问题！
//原框架中函数结尾的}符号可能出现输出错误
//原框架中FOR循环每个分号间只能放一条语句
//原框架中未实现定义是赋值

//C语言文法：
//BNF:
//Program			 ->	{ GlobalDecl }+
//GlobalDecl		 -> Statements | FunctionDecl 

//PrefixTypeAndId    -> type { '*' } id 

//FunctionDecl		 -> PrefixTypeAndId '(' ParameterDecl ')' '{' BodyDecl '}'

//ParameterDecl		 -> DefineExpression ParameterDeclTail | empty
//ParameterDeclTail	 -> ',' DefineExpression ParameterDeclTail | empty

//BodyDecl			 -> Statements

//Statements		 -> Statement Statements | Empty
//Statement			 -> IfStatement | WhileStatement | SonStatement | ReturnStatement | ExpressionSatament | DefineSatament | ForSatement
//ReturnStatement	 -> 'return' ExpressionSatament 
//ExpressionSatament -> Expression ;
//IfStatement		 -> 'if' '(' Expression ')' Statement ['else' Statement]
//WhileStatement     -> 'while' '(' Expression ')' Statement
//SonStatement		 -> '{' Statements '}'
//ForSatement		 -> 'for' '(' Expression ';'Expression';' Expression ')' Statement
//DefineSatament	 -> DefineExpression ;
//DefineExpression	 -> PrefixTypeAndId AssignmentTail

//支持优先级的表达式文法
//Expression		 -> Term AssignmentTail 
//AssignmentTail	 -> = Term AssignmentTail | ExpressionTail | Empty
//ExpressionTail	 -> + Term ExpressionTail | - Term ExpressionTail | Empty
//Term				 -> Factor TermTail
//TermTail			 -> * Factor TermTail | / Factor TermTail | Empty
//Factor			 -> ( Expression ) | Number | ID


//不支持表达式的优先级文法
//EXPR				 -> FACTOR FLIST
//FACTOR			 -> ( Expression ) | Number | ID
//FLIST				 -> = FACTOR FLIST | + FACTOR FLIST | - FACTOR FLIST | * FACTOR FLIST | / FACTOR FLIST | Empty

//Token类型属性字类型
enum { const_i = 128, Float, Char, String, Operator, Separator, Id, KW };//属性类型
FILE* IN = NULL;				//输入文件指针
FILE* OUT = NULL;				//输出文件指针
char Buffer[AnrrySize];			//IObuffer
char Value[AnrrySize];			//当前Token的Value
char Type[AnrrySize];			//当前Token的Type
int  PrefixBlank = 0;			//输出标记时的前导空格数以两个为单位

void NextToken();
void FindContent(char Anrry[]);
void Program();
void Match(char* Content);

void Program();
void GlobalDecl();
void FindType(char CurrentType[]);
void PrintBlank(int PrefixBlank);
void PrefixTypeAndId(char DateType[], char ID[]);
void FunctionDecl(char DateType[], char ID[]);
void ParameterDecl();
void ParameterDeclTail();
void BodyDecl();
void Statements();
void Statement();
void Expression();
void AssignmentTail();
void ExpressionTail();
void Term();
void Factor();
void TermTail();
void ReturnStatement();
void ExpressionSatament();
void IfStatement();
void WhileStatement();
void SonStatement();
void DefineSatament();
void DefineExpression();
void ForStatement();
void DefineSatamentTail();


int main(int argc, char* argv[]){
	PrefixBlank = 0;
	if (argc != 3){
		printf("IO Files Error");
		return -1;
	}
	IN = fopen(argv[1], "r+");
	OUT = fopen(argv[2], "w+");

	fprintf(OUT, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");//待改
	fprintf(OUT, "<ParserTree name=\"test.tree.xml\">\n");//待改

	int line = 0;
	//清除XML版本信息等
	fgets(Buffer, 1024, IN);
	fgets(Buffer, 1024, IN);
	fgets(Buffer, 1024, IN);
	
	
	Program();

	printf("\n");
	fprintf(OUT, "</ParserTree>\n");
	fclose(OUT);
	fclose(IN);
}
void NextToken(){
	
	fgets(Buffer, 1024, IN);//token
	fgets(Buffer, 1024, IN);//number
	fgets(Buffer, 1024, IN);//value
	strcpy(Value, Buffer);
	FindContent(Value);		//找到Token的Value
	fgets(Buffer, 1024, IN);//type
	strcpy(Type, Buffer);
	FindContent(Type);		//找到Token的Type
	fgets(Buffer, 1024, IN);//line
	fgets(Buffer, 1024, IN);//valid
	fgets(Buffer, 1024, IN);///token
}
void FindContent(char Anrry[]){//找到标签中的内容
	char *Start, *End;
	bool FindStart = 0, FindEnd = 0;
	char *content;

	for (int i = 0; Anrry[i] != 0; i++){
		if (FindStart == 0 && Anrry[i] == '>'){
			FindStart = 1;
			Start = &Anrry[i] + 1;
		}
		if (FindStart ==1 && FindEnd == 0 && Anrry[i] == '<'& Anrry[i+1]=='/'){
			FindEnd = 1;
			End = &Anrry[i];
			break;
		}
	}
	content = Start;
	for (int i = 0; 1; i++){
		if (content <= End){
			Anrry[i] = *content;
			content++;
		}
		if (content == End){
			Anrry[i + 1] = 0;
			break;
		}
	}
}
void Match(char* Content) {
	if (strcmp(Value,Content)==0) {
		if (strcmp(Value, "#") == 0){
			//清除XML版本信息等
			fgets(Buffer, 1024, IN);
			fgets(Buffer, 1024, IN);
			fgets(Buffer, 1024, IN);
			Type[0] = 0;
			return;
		}
		NextToken();
	}
	else {
		printf("Match Error");
		exit(-1);
	}
}

//Program			 ->	{ GlobalDecl }+
void Program() {
	PrefixBlank++;
	PrintBlank(PrefixBlank);fprintf(OUT, "<Program>\n");

	NextToken();//获得下一个属性字
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<FUNCTIONS>\n");
	while (Type[0] > 0) {//存在下一个属性字
		GlobalDecl();
	}
	
	PrintBlank(PrefixBlank); fprintf(OUT, "<FUNCTIONS>\n");
	PrefixBlank--;
	PrintBlank(PrefixBlank);fprintf(OUT, "</Program>\n");
	PrefixBlank--;
}
//GlobalDecl		 -> Statements | FunctionDecl 
void GlobalDecl() {
	//识别内容： type { '*' } id 
	char DateType[AnrrySize]="void";//确定数据类型
	char ID[AnrrySize];

	if (strcmp(Type, "#") == 0){
		printf("Parsing Complete\n");
		Match("#");
		return;
	}

	PrefixTypeAndId(DateType, ID);//找到前缀定义类型和标识符

	if (strcmp(Value,"(")==0) {//GlobalDecl		 -> FunctionDecl
		FunctionDecl(DateType, ID);
	}

	return;	
}
void FindType(char CurrentType[]){
	while (strcmp(Value, "*")==0 || strcmp(Value, "**")==0) {//指针定义
		if (strcmp(Value, "*")){
			strcat(CurrentType, "*");
			Match("*");//匹配指针前缀
		}
		else {
			strcat(CurrentType, "**");
			Match("**");//匹配指针前缀
		}
	}
}
void PrintBlank(int PrefixBlank){
	for (int i = 0; i<PrefixBlank; i++){//输出XML信息
		fprintf(OUT, "  ");
	}
}
//PrefixTypeAndId    -> type { '*' } id
void PrefixTypeAndId(char DateType[], char ID[]){
	if (strcmp(Value, "int")==0) {
		strcpy(DateType, "int");
		Match("int");
	}
	else if (strcmp(Value, "char")==0) {
		strcpy(DateType, "char");
		Match("char");
	}
	else if (strcmp(Value, "float")==0) {
		strcpy(DateType, "float");
		Match("float");
	}
	else if (strcmp(Value, "double")==0) {
		strcpy(DateType, "double");
		Match("double");
	}

	FindType(DateType);//判断数据类型是否是指针

	if (strcmp(Type, "identifier") == 0) {//当前定义的标识符
		strcpy(ID, Value);
		Match(Value);

	}
	else{//非法定义
		printf("Error Data  Definition\n");
		exit(-1);
	}
	return;
}
//FunctionDecl		 -> PrefixTypeAndId '(' ParameterDecl ')' '{' BodyDecl '}'
void FunctionDecl(char DateType[], char ID[]){
	//识别内容:'(' ParameterDecl ')' '{' BodyDecl '}'
	PrefixBlank++;
	PrintBlank(PrefixBlank);fprintf(OUT, "<FUNCTION>\n");

	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<TYPE>\n");

	PrefixBlank++; 
	PrintBlank(PrefixBlank); fprintf(OUT, "<keyword>"); fprintf(OUT, DateType); fprintf(OUT, "</keyword>\n");
	PrefixBlank--;
	PrintBlank(PrefixBlank); fprintf(OUT, "</TYPE>\n");

	PrintBlank(PrefixBlank); fprintf(OUT, "<identifier>"); fprintf(OUT, ID); fprintf(OUT, "</identifier>\n");
	Match("("); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, "("); fprintf(OUT, "</separator>\n");
	ParameterDecl();
	Match(")"); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ")"); fprintf(OUT, "</separator>\n");
	PrintBlank(PrefixBlank); fprintf(OUT, "<FUNC_BODY>\n");
	
	PrefixBlank++;
	Match("{"); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, "{"); fprintf(OUT, "</separator>\n");
	BodyDecl();
	Match("}"); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, "}"); fprintf(OUT, "</separator>\n");
	PrefixBlank--;

	PrintBlank(PrefixBlank); fprintf(OUT, "</FUNC_BODY>\n");
	PrefixBlank--;

	PrintBlank(PrefixBlank);fprintf(OUT, "</FUNCTION>\n");
	PrefixBlank--;
}
//ParameterDecl		 -> DefineExpression ParameterDeclTail | empty
void ParameterDecl(){
	if (strcmp(Value, ")") != 0){//ParameterDecl		 -> PrefixTypeAndId ParameterDeclTail
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<ARGS>\n");
		PrefixBlank--;
		DefineExpression();
		ParameterDeclTail();
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "</ARGS>\n");
		PrefixBlank--;
		return;
	}
	else{//ParameterDecl		 -> empty
		PrintBlank(PrefixBlank); fprintf(OUT, "<ARGS />\n");
		return;
	}
}
//ParameterDeclTail	 -> ',' DefineExpression ParameterDeclTail | empty
void ParameterDeclTail(){
	if (strcmp(Value, ",") == 0){//ParameterDeclTail	 -> ',' type {'*'} id ParameterDeclTail
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<ALIST>\n");
		Match(","); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ","); fprintf(OUT, "</separator>\n");
		DefineExpression();
		ParameterDeclTail();
		
		PrintBlank(PrefixBlank); fprintf(OUT, "</ALIST>\n");
		PrefixBlank--;
		return;
	}
	else if (strcmp(Value, ")") == 0){//ParameterDeclTail	 -> empty
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<ALIST />\n");
		PrefixBlank--;
		return;
	}
	
}
//BodyDecl			 -> Statements
void BodyDecl(){
	//PrefixBlank++;
	//PrintBlank(PrefixBlank); fprintf(OUT, "<FUNC_BODY>\n");
	Statements();
	
	//PrintBlank(PrefixBlank); fprintf(OUT, "</FUNC_BODY>\n");
	//PrefixBlank--;
	return;
}
//Statements		 -> Statement Statements | Empty
void Statements(){
	
	if (strcmp(Value, "}") == 0){//Statements->Empty
		return;
	}
	else {//Statements-> Statement Statements
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<STMTS>\n");

		Statement();
		Statements();

		PrintBlank(PrefixBlank); fprintf(OUT, "</STMTS>\n");
		PrefixBlank--;
	}
}
//Statement			 -> IfStatement | WhileStatement | SonStatement | ReturnStatement | ExpressionSatament | DefineSatament | ForSatement
void Statement() {//语句
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<STMT>\n");

	if (strcmp(Value,"return")==0) {//Statement->'return' Expression return语句
		ReturnStatement();
	}
	else if (strcmp(Value, "if") == 0){
		IfStatement();
	}
	else if (strcmp(Value, "{") == 0){
		SonStatement();
	}
	else if (strcmp(Value, "while") == 0){
		WhileStatement();
	}
	else if (strcmp(Value, "for") == 0){
		ForStatement();
	}
	else if (strcmp(Type, "keyword") == 0){
		DefineSatament();
	}
	else {// ExpressionSatament表达式语句
		ExpressionSatament();
	}
	
	PrintBlank(PrefixBlank); fprintf(OUT, "</STMT>\n");
	PrefixBlank--;
}
//ForSatement		 -> 'for' '(' Expression ';'Expression';' Expression ')' Statement
void ForStatement(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<FOR_STMT>\n");
	Match("for"); PrintBlank(PrefixBlank); fprintf(OUT, "<keyword>"); fprintf(OUT, "for"); fprintf(OUT, "</keyword>\n");
	Match("("); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, "("); fprintf(OUT, "</separator>\n");
	Expression();
	Match(";"); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ";"); fprintf(OUT, "</separator>\n");
	Expression();
	Match(";"); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ";"); fprintf(OUT, "</separator>\n");
	Expression();
	Match(")"); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ")"); fprintf(OUT, "</separator>\n");
	Statement();
	PrintBlank(PrefixBlank); fprintf(OUT, "<FOR_STMT>\n");
	PrefixBlank--;
}
//DefineSatament	 -> DefineExpression DefineSatamentTail
void DefineSatament(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<DEFIN_STMT>\n");
	DefineExpression();
	DefineSatamentTail();
	PrintBlank(PrefixBlank); fprintf(OUT, "</DEFIN_STMT>\n");
	PrefixBlank--;
}
//DefineSatamentTail -> , ID AssignmentTail DefineSatamentTail | ;
void DefineSatamentTail(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<DEFIN_STMTLIST>\n");

	if (strcmp(Value, ";")==0){
		PrefixBlank++;
		Match(";"); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ";"); fprintf(OUT, "</separator>\n");
		PrefixBlank--;
	}
	else if (strcmp(Value, ",") == 0){
		PrefixBlank++;
		Match(","); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ","); fprintf(OUT, "</separator>\n");
		PrintBlank(PrefixBlank); fprintf(OUT, "<identifier>"); fprintf(OUT,Value); fprintf(OUT, "</identifier>\n");
		Match(Value);
		AssignmentTail();
		DefineSatamentTail();
		PrefixBlank--;
	}
	PrintBlank(PrefixBlank); fprintf(OUT, "</DEFIN_STMTLIST>\n");
	PrefixBlank--;
}
//DefineExpression	 -> PrefixTypeAndId AssignmentTail
void DefineExpression(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<DEFIN_EXPR>\n");

	char DateType[AnrrySize] = "void";//确定数据类型
	char ID[AnrrySize];
	PrefixTypeAndId(DateType, ID);//找到前缀定义类型和标识符
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<TYPE>\n");
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<keyword>"); fprintf(OUT, DateType); fprintf(OUT, "</keyword>\n");
	PrefixBlank--;
	PrintBlank(PrefixBlank); fprintf(OUT, "</TYPE>\n");
	PrintBlank(PrefixBlank); fprintf(OUT, "<identifier>"); fprintf(OUT, ID); fprintf(OUT, "</identifier>\n");
	PrefixBlank--;
	AssignmentTail();

	PrintBlank(PrefixBlank); fprintf(OUT, "</DEFIN_EXPR>\n");
	PrefixBlank--;
}
//IfStatement		 -> 'if' '(' Expression ')' Statement ['else' Statement]
void IfStatement(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<IF_STMT>\n");
	
	PrefixBlank++;
	Match("if"); PrintBlank(PrefixBlank); fprintf(OUT, "<keyword>"); fprintf(OUT, "if"); fprintf(OUT, "</keyword>\n");
	Match("("); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, "("); fprintf(OUT, "</separator>\n");

	PrefixBlank--;
	Expression();
	PrefixBlank++;

	Match(")"); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ")"); fprintf(OUT, "</separator>\n");
	Statement();
	
	if (strcmp(Value, "else") == 0){//IfStatement		 -> 'if' '(' Expression ')' Statements 'else' Statements
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<ELSE>\n");
		Match("else"); PrintBlank(PrefixBlank); fprintf(OUT, "<keyword>"); fprintf(OUT, "else"); fprintf(OUT, "</keyword>\n");
		Statement();

		PrintBlank(PrefixBlank); fprintf(OUT, "<ELSE>\n");
		PrefixBlank--;
	}
	else;//IfStatement		 -> 'if' '(' Expression ')' Statement

	PrefixBlank--;

	PrintBlank(PrefixBlank); fprintf(OUT, "</IF_STMT>\n");
	PrefixBlank--;
}
//SonStatement		 -> '{' Statements '}'
void SonStatement(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<SON_STMT>\n");

	PrefixBlank++;
	Match("{"); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, "{"); fprintf(OUT, "</separator>\n");
	Statements();
	Match("}"); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, "}"); fprintf(OUT, "</separator>\n");
	PrefixBlank--;

	PrintBlank(PrefixBlank); fprintf(OUT, "<SON_STMT>\n");
	PrefixBlank--;
}
//WhileStatement     -> 'while' '(' Expression ')' Statement
void WhileStatement(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<WHILE_STMT>\n");

	PrefixBlank++;
	Match("while"); PrintBlank(PrefixBlank); fprintf(OUT, "<keyword>"); fprintf(OUT, "while"); fprintf(OUT, "</keyword>\n");
	Match("("); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, "("); fprintf(OUT, "</separator>\n");
	Expression();
	Match(")"); PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ")"); fprintf(OUT, "</separator>\n");
	Statement();
	PrefixBlank--;

	PrintBlank(PrefixBlank); fprintf(OUT, "<WHILE_STMT>\n");
	PrefixBlank--;
}
//ReturnStatement	 -> 'return' ExpressionSatament 
void ReturnStatement(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<RET_STMT>\n");

	PrefixBlank++;//输出return
	PrintBlank(PrefixBlank); fprintf(OUT, "<keyword>"); fprintf(OUT, "return"); fprintf(OUT, "</keyword>\n");
	Match("return");
	PrefixBlank--;

	if (strcmp(Value, ";") != 0){
		ExpressionSatament();
	}

	PrintBlank(PrefixBlank); fprintf(OUT, "</RET_STMT>\n");
	PrefixBlank--;
}
//ExpressionSatament -> Expression ;
void ExpressionSatament(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<EXPR_STMT>\n");

	if (strcmp(Value, ";") != 0){
		Expression();
	}

	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ";"); fprintf(OUT, "</separator>\n");
	Match(";");
	PrefixBlank--;
	PrintBlank(PrefixBlank); fprintf(OUT, "<\EXPR_STMT>\n");
	PrefixBlank--;
}
//Expression		 -> Term AssignmentTail 
void Expression(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<EXPR>\n");
	
	Term();
	AssignmentTail();

	PrintBlank(PrefixBlank); fprintf(OUT, "</EXPR>\n");
	PrefixBlank--;
}
//AssignmentTail-> = Term AssignmentTail | ExpressionTail | Empty
void AssignmentTail(){
	
	if (strcmp(Value, "+") == 0 || strcmp(Value, "-") == 0){//AssignmentTail-> ExpressionTail
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<AssignmentLIST>\n");
		ExpressionTail();
		PrintBlank(PrefixBlank); fprintf(OUT, "</AssignmentLIST>\n");
		PrefixBlank--;
	}
	else if (strcmp(Value, "=") == 0){//AssignmentTail-> = Term AssignmentTail 
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<AssignmentLIST>\n");
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<operator>"); fprintf(OUT, "="); fprintf(OUT, "</operator>\n");
		PrefixBlank--;
		Match("=");
		Term();
		AssignmentTail();
		PrintBlank(PrefixBlank); fprintf(OUT, "</AssignmentLIST>\n");
		PrefixBlank--;

	}
	else;//AssignmentTail-> Empty
	
}
//ExpressionTail	 -> + Term ExpressionTail | - Term ExpressionTail | Empty
void ExpressionTail(){
	
	if (strcmp(Value, "+") == 0){//ExpressionTail	 -> + Term ExpressionTail
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<EXPRLIST>\n");

		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<operator>"); fprintf(OUT, "+"); fprintf(OUT, "</operator>\n");
		PrefixBlank--;
		Match("+");
		Term();
		ExpressionTail();

		PrintBlank(PrefixBlank); fprintf(OUT, "</EXPRLIST>\n");
		PrefixBlank--;
	}
	else if (strcmp(Value, "-") == 0) {//ExpressionTail	 -> - Term ExpressionTail
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<EXPRLIST>\n");

		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<operator>"); fprintf(OUT, "-"); fprintf(OUT, "</operator>\n");
		PrefixBlank--;
		Match("-");
		Term();
		ExpressionTail();

		PrintBlank(PrefixBlank); fprintf(OUT, "</EXPRLIST>\n");
		PrefixBlank--;
	}
	else;//ExpressionTail	 -> Empty
	
}
//Term				 -> Factor TermTail
void Term(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<Term>\n");

	Factor();
	TermTail();

	PrintBlank(PrefixBlank); fprintf(OUT, "</Term>\n");
	PrefixBlank--;
}
//TermTail			 -> * Factor TermTail | / Factor TermTail | Empty
void TermTail(){
	
	if (strcmp(Value, "*") == 0){//TermTail			 -> * Factor TermTail
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<TermLIST>\n");
		
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<operator>"); fprintf(OUT, "*"); fprintf(OUT, "</operator>\n");
		PrefixBlank--;
		Match("*");
		Factor();
		TermTail();

		PrintBlank(PrefixBlank); fprintf(OUT, "</TermLIST>\n");
		PrefixBlank--;
	}
	else if (strcmp(Value, "/") == 0) {//TermTail			 -> / Factor TermTail
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<TermLIST>\n");

		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<operator>"); fprintf(OUT, "/"); fprintf(OUT, "</operator>\n");
		PrefixBlank--;
		Match("/");
		Factor();
		TermTail();

		PrintBlank(PrefixBlank); fprintf(OUT, "</TermLIST>\n");
		PrefixBlank--;
	}
	else;//TermTail			 ->	Empty

	
}
//Factor			 -> ( Expression ) | Number | ID
void Factor(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<Factor>\n");
	if (strcmp(Value, "(") == 0){
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, "("); fprintf(OUT, "</separator>\n");
		Match("(");

		Expression();

		PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ")"); fprintf(OUT, "</separator>\n");
		PrefixBlank--;
		Match(")");
	}
	else if (strcmp(Type, "const_i") == 0){
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<const_i>"); fprintf(OUT, Value); fprintf(OUT, "</const_i>\n");
		PrefixBlank--;
		Match(Value);
	}
	else if (strcmp(Type, "identifier") == 0){
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<identifier>"); fprintf(OUT, Value); fprintf(OUT, "</identifier>\n");
		PrefixBlank--;
		Match(Value);
	}
	else {
		printf("Expression Error");
	}
	PrintBlank(PrefixBlank); fprintf(OUT, "</Factor>\n");
	PrefixBlank--;
}