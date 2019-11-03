/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

int state;

/***************************************************************/

void skipBlank() {
	while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_SPACE))
		readChar();
}

void skipComment() {
	// Thêm vào
	int state = 0;
	while ((currentChar != EOF) && (state < 2)) {
		switch (charCodes[currentChar]) {
		case CHAR_TIMES:
			state = 1;
			break;
		case CHAR_RPAR:
			if (state == 1)
				state = 2;
			else
				state = 0;
			break;
		default:
			state = 0;
		}
		readChar();
	}
	if (state != 2)
		error(ERR_ENDOFCOMMENT, lineNo, colNo);
	// Kết thúc
}

Token* readIdentKeyword(void) {
	// Thêm vào
	Token *token = makeToken(TK_NONE, lineNo, colNo);
	int count = 1;

	token->string[0] = toupper((char)currentChar);
	readChar();

	while ((currentChar != EOF) &&
		((charCodes[currentChar] == CHAR_LETTER) || (charCodes[currentChar] == CHAR_DIGIT))) {
		if (count <= MAX_IDENT_LEN) token->string[count++] = toupper((char)currentChar);
		readChar();
	}
	// Kiểm tra độ dài
	if (count > MAX_IDENT_LEN) {
		error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
		return token;
	}
	// Kết thúc nhập
	token->string[count] = '\0';
	token->tokenType = checkKeyword(token->string);

	if (token->tokenType == TK_NONE)
		token->tokenType = TK_IDENT;

	return token;
	// Kết thúc
}

Token* readNumber(void) {
	// TODO
	  // Thêm vào
	int count = 0;
	Token *token = makeToken(TK_NUMBER, lineNo, colNo);
	while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_DIGIT)) {
		token->string[count++] = (char)currentChar;
		readChar();
	}
	if (count > MAX_NUMBER_LEN) {
		error(ERR_NUMBERTOOLONG, token->lineNo, token->colNo);
		return token;
	}
	// Kết thúc nhập số
	token->string[count] = '\0';
	token->value = atoi(token->string);
	return token;
	// Kết thúc
}
// Doc hang ky tu
Token* readConstChar(void) {
	// Thêm vào
	Token *token = makeToken(TK_CHAR, lineNo, colNo);

	readChar();
	if (currentChar == EOF) {
		token->tokenType = TK_NONE;
		error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
		return token;
	}

	token->string[0] = currentChar;
	token->string[1] = '\0';
	token->value = currentChar;
	readChar();

	if (currentChar == EOF) {
		token->tokenType = TK_NONE;
		error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
		return token;
	}

	if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
		readChar();
		return token;
	}
	else {
		token->tokenType = TK_NONE;
		error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
		return token;
	}
	// Kết thúc
}

Token* getToken(void) {
	// Thêm vào
	Token *token;
	if (currentChar == EOF)
		return makeToken(TK_EOF, lineNo, colNo);

	switch (charCodes[currentChar]) {
	case CHAR_SPACE:
		state = 1;
		break;
	case CHAR_LPAR:
		state = 2;
		break;
	case CHAR_LETTER:
		state = 8;
		break;
	case CHAR_DIGIT:
		state = 10;
		break;
	case CHAR_PLUS:
		state = 12;
		break;
	case CHAR_MINUS:
		state = 13;
		break;
	case CHAR_TIMES:
		state = 14;
		break;
	case CHAR_SLASH:
		state = 15;
		break;
	case CHAR_GT:
		state = 22;
		break;
	case CHAR_LT:
		state = 25;
		break;
	case CHAR_EXCLAIMATION:
		state = 28;
		break;
	case CHAR_RPAR:
		state = 39;
		break;
	case CHAR_EQ:
		state = 16;
		break;
	case CHAR_COMMA:
		state = 17;
		break;
	case CHAR_SEMICOLON:
		state = 18;
		break;
	case CHAR_PERIOD:
		state = 19;
		break;
	case CHAR_COLON:
		state = 31;
		break;
	case CHAR_SINGLEQUOTE:
		state = 34;
		break;
	default:
		token = makeToken(TK_NONE, lineNo, colNo);
		error(ERR_INVALIDSYMBOL, lineNo, colNo);
		readChar();
		return token;
	}
	while (1) {
		switch (state) {
		case 1:
			skipBlank();
			return getToken();
		case 2:
			readChar();
			switch (charCodes[currentChar]) {
			case CHAR_PERIOD:
				state = 6;// lsel
				break;
			case CHAR_TIMES:
				state = 3;// skip comment
				break;
			default:
				state = 7;// lpar
				break;
			}
			break;
		case 3:
			skipComment();
			return getToken();
		case 6:
			token = makeToken(SB_LSEL, lineNo, colNo);
			return token;
		case 7:
			token = makeToken(SB_LPAR, lineNo, colNo);
			return token;
		case 8:
			return readIdentKeyword();
		case 10:
			return readNumber();
		case 12:
			token = makeToken(SB_PLUS, lineNo, colNo);
			readChar();
			return token;
		case 13:
			token = makeToken(SB_MINUS, lineNo, colNo);
			readChar();
			return token;
		case 14:
			token = makeToken(SB_TIMES, lineNo, colNo);
			readChar();
			return token;
		case 15:
			token = makeToken(SB_SLASH, lineNo, colNo);
			readChar();
			return token;
		case 22:
			readChar();
			switch (charCodes[currentChar]) {
			case CHAR_EQ:
				state = 23;// geq
				break;
			default:
				state = 24;// gt
				break;
			}
			break;
		case 23:
			token = makeToken(SB_GE, lineNo, colNo);
			readChar();
			return token;
		case 24:
			token = makeToken(SB_GT, lineNo, colNo);
			return token;
		case 25:
			readChar();
			switch (charCodes[currentChar]) {
			case CHAR_EQ:
				state = 26;// leq
				break;
			default:
				state = 27;// lt
				break;
			}
			break;
		case 26:
			token = makeToken(SB_LE, lineNo, colNo);
			readChar();
			return token;
		case 27:
			token = makeToken(SB_LT, lineNo, colNo);
			return token;
		case 28:
			readChar();
			switch (charCodes[currentChar]) {
			case CHAR_EQ:
				state = 29;// neq
				break;
			default:
				state = 30;// error
				break;
			}
			break;
		case 29:
			token = makeToken(SB_NEQ, lineNo, colNo);
			readChar();
			return token;
		case 30:
			token = makeToken(TK_NONE, lineNo, colNo);
			error(ERR_INVALIDSYMBOL, lineNo, colNo);
			readChar();
			return token;
		case 16:
			token = makeToken(SB_EQ, lineNo, colNo);
			readChar();
			return token;
		case 17:
			token = makeToken(SB_COMMA, lineNo, colNo);
			readChar();
			return token;
		case 18:
			token = makeToken(SB_SEMICOLON, lineNo, colNo);
			readChar();
			return token;
		case 19:
			readChar();
			switch (charCodes[currentChar]) {
			case CHAR_COMMA:
				state = 20;// rsel
				break;
			default:
				state = 21;// period
				break;
			}
			break;
		case 20:
			token = makeToken(SB_RSEL, lineNo, colNo);
			readChar();
			return token;
		case 21:
			token = makeToken(SB_PERIOD, lineNo, colNo);
			return token;
		case 31:
			readChar();
			switch (charCodes[currentChar]) {
			case CHAR_EQ:
				state = 32;// assign
				break;
			default:
				state = 33;// colon
				break;
			}
			break;
		case 32:
			token = makeToken(SB_ASSIGN, lineNo, colNo);
			readChar();
			return token;
		case 33:
			token = makeToken(SB_COLON, lineNo, colNo);
			return token;
		case 34:
			return readConstChar();
		default:
			break;
		}
	}
	// Kết thúc
}


/******************************************************************/

void printToken(Token *token) {

	printf("%d-%d:", token->lineNo, token->colNo);

	switch (token->tokenType) {
	case TK_NONE: printf("TK_NONE\n"); break;
	case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
	case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
	case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
	case TK_EOF: printf("TK_EOF\n"); break;

	case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
	case KW_CONST: printf("KW_CONST\n"); break;
	case KW_TYPE: printf("KW_TYPE\n"); break;
	case KW_VAR: printf("KW_VAR\n"); break;
	case KW_INTEGER: printf("KW_INTEGER\n"); break;
	case KW_CHAR: printf("KW_CHAR\n"); break;
	case KW_ARRAY: printf("KW_ARRAY\n"); break;
	case KW_OF: printf("KW_OF\n"); break;
	case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
	case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
	case KW_BEGIN: printf("KW_BEGIN\n"); break;
	case KW_END: printf("KW_END\n"); break;
	case KW_CALL: printf("KW_CALL\n"); break;
	case KW_IF: printf("KW_IF\n"); break;
	case KW_THEN: printf("KW_THEN\n"); break;
	case KW_ELSE: printf("KW_ELSE\n"); break;
	case KW_WHILE: printf("KW_WHILE\n"); break;
	case KW_DO: printf("KW_DO\n"); break;
	case KW_FOR: printf("KW_FOR\n"); break;
	case KW_TO: printf("KW_TO\n"); break;

	case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
	case SB_COLON: printf("SB_COLON\n"); break;
	case SB_PERIOD: printf("SB_PERIOD\n"); break;
	case SB_COMMA: printf("SB_COMMA\n"); break;
	case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
	case SB_EQ: printf("SB_EQ\n"); break;
	case SB_NEQ: printf("SB_NEQ\n"); break;
	case SB_LT: printf("SB_LT\n"); break;
	case SB_LE: printf("SB_LE\n"); break;
	case SB_GT: printf("SB_GT\n"); break;
	case SB_GE: printf("SB_GE\n"); break;
	case SB_PLUS: printf("SB_PLUS\n"); break;
	case SB_MINUS: printf("SB_MINUS\n"); break;
	case SB_TIMES: printf("SB_TIMES\n"); break;
	case SB_SLASH: printf("SB_SLASH\n"); break;
	case SB_LPAR: printf("SB_LPAR\n"); break;
	case SB_RPAR: printf("SB_RPAR\n"); break;
	case SB_LSEL: printf("SB_LSEL\n"); break;
	case SB_RSEL: printf("SB_RSEL\n"); break;
	}
}

int scan(char *fileName) {
	Token *token;
	state = 0;
	if (openInputStream(fileName) == IO_ERROR)
		return IO_ERROR;

	token = getToken();

	while (token->tokenType != TK_EOF) {
		state = 0;
		printToken(token);
		free(token);
		token = getToken();
	}

	free(token);
	closeInputStream();
	return IO_SUCCESS;
}

/******************************************************************/

int main() {
	 char *fileName = "example2.kpl";
	
	 if (scan(fileName) == IO_ERROR) {
		printf("Can\'t read input file!\n");
		return -1;
	 }
	 system("pause");
	 return 0;
}



