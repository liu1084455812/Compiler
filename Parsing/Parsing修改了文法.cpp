#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define AnrrySize 1024
//学到了新知识，<ABC />表示该标签部分为空

//特性：
//支持多种数据定义
//支持指针定义(包括多层例如 int** a;)
//支持全局变量
//不支持数组
//不支持定义时赋值
//语句：
//支持变量定义，函数定义，赋值语句，return语句，四则运算（支持优先级）
//不支持控制语句
//FOR循环每个分号间只能放一条语句

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

//C语言文法：
//EBNF:
//Program			 ->	{ GlobalDecl }+
//GlobalDecl		 -> VariableDecl | FunctionDecl
//注意这里两部分有相同前缀，所以判断进入哪个部分的时候应该在识别ID之后
//VariableDecl		 ->	type { '*' } id { ',' {'*'} id } ';'
//FunctionDecl		 ->	type { '*' } id '(' ParameterDecl ')' '{' BodyDecl '}'
//ParameterDecl		 ->	type { '*' } id { ',' type { '*' } id }
//BodyDecl			 ->	{ VariableDecl } { Statement }
//Statement			 -> IfStatement | WhileStatement | '{' Statement '}'| 'return' Expression | Expression ';'
//IfStatement		 -> 'if' '(' Expression ')' Statement ['else' Statement]
//WhileStatement     -> 'while' '(' Expression ')' Statement

//其实可以将所有的EBNF转化为BNF，但由于表述过于麻烦这里只将有必要的Statement详细展开。

//BNF:
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

//不支持表达式的优先级文法
//EXPR				 -> FACTOR FLIST
//FACTOR			 -> ( Expression ) | Number | ID
//FLIST				 -> = FACTOR FLIST | + FACTOR FLIST | - FACTOR FLIST | * FACTOR FLIST | / FACTOR FLIST | Empty

//支持优先级的表达式文法
//Expression		 -> Term AssignmentTail 
//AssignmentTail-> = Term AssignmentTail | ExpressionTail
//ExpressionTail	 -> + Term ExpressionTail | - Term ExpressionTail | Empty
//Term				 -> Factor TermTail
//TermTail			 -> * Factor TermTail | / Factor TermTail | Empty
//Factor			 -> ( Expression ) | Number | ID

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
void VariableDecl(char DateType[], char ID[]);
void VariableTail(char DateType[]);
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

void Program() {
	PrefixBlank++;
	PrintBlank(PrefixBlank);fprintf(OUT, "<Program>\n");

	NextToken();//获得下一个属性字
	while (Type[0] > 0) {//存在下一个属性字
		GlobalDecl();
	}

	PrintBlank(PrefixBlank);fprintf(OUT, "</Program>\n");

	PrefixBlank--;
}
//GlobalDecl		 -> VariableDecl | FunctionDecl 
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
	else {//GlobalDecl		 -> VariableDecl
		VariableDecl(DateType, ID);
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
//VariableDecl		 -> PrefixTypeAndId VariableDeclTail
void VariableDecl(char DateType[], char ID[]){
	//识别内容:VariableDecl -> 
	PrefixBlank++;
	PrintBlank(PrefixBlank);fprintf(OUT, "<Variable>\n");

	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<TYPE>"); fprintf(OUT, DateType); fprintf(OUT, "</TYPE>\n");
	PrintBlank(PrefixBlank); fprintf(OUT, "<identifier>"); fprintf(OUT, ID); fprintf(OUT, "</identifier>\n");
	PrefixBlank--;
	
	if (strcmp(Value, ",") == 0){
		VariableTail(DateType);
	}

	PrintBlank(PrefixBlank); fprintf(OUT, "</Variable>\n");
	PrefixBlank--;
	

}
//VariableDeclTail	 -> ',' {'*'} id VariableDeclTail | ";"
void VariableTail(char DateType[]){
	
	if (strcmp(Value, ";") == 0) {//VariableDeclTail	 -> ";"
		Match(";");
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<VariableTail>\n");

		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ";"); fprintf(OUT, "</separator>\n");
		PrefixBlank--;

		PrintBlank(PrefixBlank); fprintf(OUT, "</VariableTail>\n");
		PrefixBlank--;
		return;
	}
	else if (strcmp(Value, ",") == 0){//VariableDeclTail	 -> ',' {'*'} id VariableDeclTail
		char CurrentType[AnrrySize];
		char NextID[AnrrySize];
		
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<VariableTail>\n");

		strcpy(CurrentType, DateType);//用于判断变量类型
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ","); fprintf(OUT, "</separator>\n");
		Match(",");//匹配","

		FindType(CurrentType);//判断数据类型

		if (strcmp(Type, "identifier") == 0) {//当前定义的标识符
			strcpy(NextID, Value);
			Match(Value);//匹配标识符
		}
		else{//非法定义
			printf("Error Data  Definition\n");
			exit(-1);
		}

		PrintBlank(PrefixBlank); fprintf(OUT, "<TYPE>"); fprintf(OUT, CurrentType); fprintf(OUT, "</TYPE>\n");
		PrintBlank(PrefixBlank); fprintf(OUT, "<identifier>"); fprintf(OUT, NextID); fprintf(OUT, "</identifier>\n");
		PrefixBlank--;

		PrintBlank(PrefixBlank); fprintf(OUT, "</VariableTail>\n");
		PrefixBlank--;
		VariableTail(DateType);
		return;
	}
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
//ParameterDecl		 -> PrefixTypeAndId ParameterDeclTail | empty
void ParameterDecl(){
	if (strcmp(Value, ")") != 0){//ParameterDecl		 -> PrefixTypeAndId ParameterDeclTail
		PrintBlank(PrefixBlank); fprintf(OUT, "<ARGS>\n");

		char DateType[AnrrySize] = "void";//确定数据类型
		char ID[AnrrySize];
		PrefixTypeAndId(DateType, ID);//找到前缀定义类型和标识符

		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<FARGS>\n");

		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<TYPE>\n"); 
		
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<keyword>"); fprintf(OUT, DateType); fprintf(OUT, "</keyword>\n");
		PrefixBlank--;
		
		PrintBlank(PrefixBlank); fprintf(OUT, "</TYPE>\n");
		PrintBlank(PrefixBlank); fprintf(OUT, "<identifier>"); fprintf(OUT, ID); fprintf(OUT, "</identifier>\n");
		PrefixBlank--;

		PrintBlank(PrefixBlank); fprintf(OUT, "</FARGS>\n");
		PrefixBlank--;

		if (strcmp(Value, ",") == 0){
			ParameterDeclTail();
		}

		PrintBlank(PrefixBlank); fprintf(OUT, "</ARGS>\n");
		return;
	}
	else{//ParameterDecl		 -> empty
		PrintBlank(PrefixBlank); fprintf(OUT, "<ARGS />\n");
		return;
	}
}
//ParameterDeclTail	 -> ',' type {'*'} id ParameterDeclTail | empty
void ParameterDeclTail(){
	if (strcmp(Value, ",") == 0){//ParameterDeclTail	 -> ',' type {'*'} id ParameterDeclTail
		PrintBlank(PrefixBlank); fprintf(OUT, "<ALIST>\n");

		PrefixBlank++;
		Match(",");
		PrintBlank(PrefixBlank); fprintf(OUT, "<separator>"); fprintf(OUT, ","); fprintf(OUT, "</separator>\n");

		char DateType[AnrrySize] = "void";//确定数据类型
		char ID[AnrrySize];
		PrefixTypeAndId(DateType, ID);//找到前缀定义类型和标识符
		PrintBlank(PrefixBlank); fprintf(OUT, "<FARGS>\n");

		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<TYPE>\n");

		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<keyword>"); fprintf(OUT, DateType); fprintf(OUT, "</keyword>\n");
		PrefixBlank--;

		PrintBlank(PrefixBlank); fprintf(OUT, "</TYPE>\n");
		PrintBlank(PrefixBlank); fprintf(OUT, "<identifier>"); fprintf(OUT, ID); fprintf(OUT, "</identifier>\n");
		PrefixBlank--;

		PrintBlank(PrefixBlank); fprintf(OUT, "</FARGS>\n");
		PrefixBlank--;

		if (strcmp(Value, ",") == 0){
			ParameterDeclTail();
		}

		return;
	}
	else if (strcmp(Value, ")") == 0){//ParameterDeclTail	 -> empty
		PrintBlank(PrefixBlank); fprintf(OUT, "<ALIST />\n");
		return;
	}
	
}
//BodyDecl			 ->	{ VariableDecl } { Statement }
void BodyDecl(){
	while (strcmp(Type, "keyword")==0 || strcmp(Value, "}")!=0){//VariableDecl//Statement
		if (strcmp(Type, "keyword") == 0 && strcmp(Value, "return") != 0){
			char DateType[AnrrySize] = "void";//确定数据类型
			char ID[AnrrySize];
			PrefixTypeAndId(DateType, ID);//找到前缀定义类型和标识符
			VariableDecl(DateType, ID);
		}
		else if (strcmp(Value, "}") != 0){
			Statements();
		}	
	} 
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
//Statement			 -> IfStatement | WhileStatement | SonStatement | ReturnStatement | ExpressionSatament
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
	else {// ExpressionSatament表达式语句
		ExpressionSatament();
	}
	
	PrintBlank(PrefixBlank); fprintf(OUT, "</STMT>\n");
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

	PrintBlank(PrefixBlank); fprintf(OUT, "<IF_STMT>\n");
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
//AssignmentTail-> = Term AssignmentTail | ExpressionTail
void AssignmentTail(){
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<EqualEXPRLIS>\n");
	if (strcmp(Value, "=") == 0){
		PrefixBlank++;
		PrintBlank(PrefixBlank); fprintf(OUT, "<operator>"); fprintf(OUT, "="); fprintf(OUT, "</operator>\n");
		PrefixBlank--;
		Match("=");

		Term();
		AssignmentTail();

	}
	else {
		ExpressionTail();
	}
	PrintBlank(PrefixBlank); fprintf(OUT, "</EqualEXPRLIS>\n");
	PrefixBlank--;
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