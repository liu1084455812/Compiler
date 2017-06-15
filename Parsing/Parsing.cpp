#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define AnrrySize 1024
//ѧ������֪ʶ��<ABC />��ʾ�ñ�ǩ����Ϊ��

//���ԣ�
//֧�ֶ������ݶ���
//֧��ָ�붨��(����������� int** a;)
//֧�ֶ���ʱ��ֵ
//��䣺
//֧�ֱ������壬��������(��������ʱ��ֵ)����ֵ��䣬return��䣬�������㣨֧�����ȼ�����֧��FORѭ����֧��whileѭ����If��While��For��֧�ֶ�㣨�������㣩Ƕ��
//֧�ֿ������
//֧�ֿ����
//ȱ��:
//��֧��ȫ�ֱ���
//��֧������
//FORѭ��ÿ���ֺż�ֻ�ܷ�һ�����
//δʵ�ֱȽ����

//�����﷨�����ı�д����ԭ�ʷ����������⣺
//û�г����β��ӡ�#��������

//���﷨�����������в����ݲ��֣�
//����ʵ����IF_ELSE����XML�ж��ELSE��ǩ������ELSEIF��ǩ
//����ʵ���˱��ʽ�����ȼ������˺ܶ��ǩ

//ԭ��ܴ��ڵ����⣺
//ԭ�����IF�����﷨���������⣬���ڷ���if(a=1);ʱ�������ѡ����������ݡ�
//ԭ�����IF���ֻ��if(a=1){a=1;}else{a=1;}����ѡ��Ķε������{}��ʱ���������������
//ԭ��ܲ�֧��while���
//ԭ������﷨��������Ҫ���ܷ���<�������صĶ�������Ϊ��XML��<���ƻ���ߵı�ǩ���token.XML�������
//ԭ������﷨�������ܷ���==�ȸ��Ӳ�����
//ԭ���û��ʵ�ֳ˷������ο����ȼ������⣡
//ԭ����к�����β��}���ſ��ܳ����������
//ԭ�����FORѭ��ÿ���ֺż�ֻ�ܷ�һ�����
//ԭ�����δʵ�ֶ����Ǹ�ֵ

//C�����ķ���
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

//֧�����ȼ��ı��ʽ�ķ�
//Expression		 -> Term AssignmentTail 
//AssignmentTail	 -> = Term AssignmentTail | ExpressionTail | Empty
//ExpressionTail	 -> + Term ExpressionTail | - Term ExpressionTail | Empty
//Term				 -> Factor TermTail
//TermTail			 -> * Factor TermTail | / Factor TermTail | Empty
//Factor			 -> ( Expression ) | Number | ID


//��֧�ֱ��ʽ�����ȼ��ķ�
//EXPR				 -> FACTOR FLIST
//FACTOR			 -> ( Expression ) | Number | ID
//FLIST				 -> = FACTOR FLIST | + FACTOR FLIST | - FACTOR FLIST | * FACTOR FLIST | / FACTOR FLIST | Empty

//Token��������������
enum { const_i = 128, Float, Char, String, Operator, Separator, Id, KW };//��������
FILE* IN = NULL;				//�����ļ�ָ��
FILE* OUT = NULL;				//����ļ�ָ��
char Buffer[AnrrySize];			//IObuffer
char Value[AnrrySize];			//��ǰToken��Value
char Type[AnrrySize];			//��ǰToken��Type
int  PrefixBlank = 0;			//������ʱ��ǰ���ո���������Ϊ��λ

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

	fprintf(OUT, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");//����
	fprintf(OUT, "<ParserTree name=\"test.tree.xml\">\n");//����

	int line = 0;
	//���XML�汾��Ϣ��
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
	FindContent(Value);		//�ҵ�Token��Value
	fgets(Buffer, 1024, IN);//type
	strcpy(Type, Buffer);
	FindContent(Type);		//�ҵ�Token��Type
	fgets(Buffer, 1024, IN);//line
	fgets(Buffer, 1024, IN);//valid
	fgets(Buffer, 1024, IN);///token
}
void FindContent(char Anrry[]){//�ҵ���ǩ�е�����
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
			//���XML�汾��Ϣ��
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

	NextToken();//�����һ��������
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<FUNCTIONS>\n");
	while (Type[0] > 0) {//������һ��������
		GlobalDecl();
	}
	
	PrintBlank(PrefixBlank); fprintf(OUT, "<FUNCTIONS>\n");
	PrefixBlank--;
	PrintBlank(PrefixBlank);fprintf(OUT, "</Program>\n");
	PrefixBlank--;
}
//GlobalDecl		 -> Statements | FunctionDecl 
void GlobalDecl() {
	//ʶ�����ݣ� type { '*' } id 
	char DateType[AnrrySize]="void";//ȷ����������
	char ID[AnrrySize];

	if (strcmp(Type, "#") == 0){
		printf("Parsing Complete\n");
		Match("#");
		return;
	}

	PrefixTypeAndId(DateType, ID);//�ҵ�ǰ׺�������ͺͱ�ʶ��

	if (strcmp(Value,"(")==0) {//GlobalDecl		 -> FunctionDecl
		FunctionDecl(DateType, ID);
	}

	return;	
}
void FindType(char CurrentType[]){
	while (strcmp(Value, "*")==0 || strcmp(Value, "**")==0) {//ָ�붨��
		if (strcmp(Value, "*")){
			strcat(CurrentType, "*");
			Match("*");//ƥ��ָ��ǰ׺
		}
		else {
			strcat(CurrentType, "**");
			Match("**");//ƥ��ָ��ǰ׺
		}
	}
}
void PrintBlank(int PrefixBlank){
	for (int i = 0; i<PrefixBlank; i++){//���XML��Ϣ
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

	FindType(DateType);//�ж����������Ƿ���ָ��

	if (strcmp(Type, "identifier") == 0) {//��ǰ����ı�ʶ��
		strcpy(ID, Value);
		Match(Value);

	}
	else{//�Ƿ�����
		printf("Error Data  Definition\n");
		exit(-1);
	}
	return;
}
//FunctionDecl		 -> PrefixTypeAndId '(' ParameterDecl ')' '{' BodyDecl '}'
void FunctionDecl(char DateType[], char ID[]){
	//ʶ������:'(' ParameterDecl ')' '{' BodyDecl '}'
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
void Statement() {//���
	PrefixBlank++;
	PrintBlank(PrefixBlank); fprintf(OUT, "<STMT>\n");

	if (strcmp(Value,"return")==0) {//Statement->'return' Expression return���
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
	else {// ExpressionSatament���ʽ���
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

	char DateType[AnrrySize] = "void";//ȷ����������
	char ID[AnrrySize];
	PrefixTypeAndId(DateType, ID);//�ҵ�ǰ׺�������ͺͱ�ʶ��
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

	PrefixBlank++;//���return
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