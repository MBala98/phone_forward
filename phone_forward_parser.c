#include "phone_forward_parser.h"

#define BASIC_LENGTH_OF_ARRAY 100
#define BASIC_LENGTH_OF_NUMBER 8
#define BASIC_LENGTH_OF_WORD 10

/** @struct PfBase phone_forward_parser.h
 * Implementacja struktury przechowującej bazę przekierowań.
*/
struct PfBase {
    /**
    * Wskaźnik na napis reprezentujący nazwę bazy.
    */
	char *name;
    /**
    * Wskaźnik na strukturę PhoneForward.
    */
	PhoneForward *base;
};

/** @struct ArrayOfBases phone_forward_parser.h
 * Implementacja struktury przechowującej tablicę baz przekierowań.
*/
struct ArrayOfBases {
    /**
    * Aktualna długość tablicy. 
    */
    int current_length;
    /**
    * Maksymalna długość tablicy.
    */
	int max_length;
    /**
    * Wskaźnik na tablicę wskaźników do struktury PfBase.
    */
	PfBase **Array;
};

void clear(ArrayOfBases *AOB) {
    for (int i = 0; i < AOB->current_length; i++) {
        phfwdDelete(AOB->Array[i]->base);
        free(AOB->Array[i]->name);
        free(AOB->Array[i]);
    }
    free(AOB->Array);
    free(AOB);
}

void handle_error(int byte_number, ArrayOfBases *AOB, char *error_info) {
    fprintf(stderr, "%s %d\n", error_info, byte_number);
    clear(AOB);
    exit(1);
}

void print_numbers(PhoneNumbers const *pnum, int byte_numbers, ArrayOfBases *AOB, char *number) {
    size_t idx = 0;
    const char *num;
    while ((num = phnumGet(pnum, idx++)) != NULL)
        printf("%s\n", num);
    phnumDelete(pnum);
    if (idx == 1) {
        free(number);
        handle_error(byte_numbers, AOB, "ERROR ?");
    }
}

PfBase *create_base(const char *name) {
    PfBase *new = malloc(sizeof(PfBase));
    if (new == NULL) return NULL;
    new->name = malloc((strlen(name) +1 ) * sizeof(char));
    new->name = strcpy(new->name, name);
    new->base = phfwdNew();
    return new;
}

void move_array_right(PfBase *Array[], int index, int current_length) {
    PfBase *tmp = Array[index];
    for (int i = index + 1; i <= current_length; i++) {
        PfBase *tmp2 = Array[i];
        Array[i] = tmp;
        tmp = tmp2;
    }
}
 
void move_array_left(PfBase *Array[], int index, int current_length) {
    for (int i = index; i < current_length-1; i++) {
        Array[i] = Array[i+1];
    }
}

PfBase * insert_base(ArrayOfBases *AOB, int i, const char *base_name) { // inserting base with a given name into a given index of array of bases
    if (AOB->current_length == AOB->max_length) {
        PfBase ** tmp = AOB->Array;
        AOB->Array = realloc(AOB->Array, 2*AOB->max_length*sizeof(PfBase));
        if (AOB->Array == NULL) {
            free(tmp);
        	fprintf(stderr, "Błąd alokowania pamięci");
            exit(1);
        }
        AOB->max_length *= 2;
        for (int j = AOB->current_length; j < AOB->max_length; j++)
            AOB->Array[j] = NULL;
    }
    if (i != AOB->current_length) { // if index is something in the middle, then we need to move array, we dont need to do that if its the last element
         move_array_right(AOB->Array, i, AOB->current_length);
    }
    AOB->Array[i] = create_base(base_name);
    if (AOB->Array[i] == NULL) return NULL;
    AOB->current_length += 1;
    return AOB->Array[i];
}

PfBase *find_base(ArrayOfBases *AOB, int *index, char *base_name) { // looking for a base using binary search
    if (AOB->current_length == 0) {
        *index = 0;
        return NULL;
    }
    int i = 0;
    int j = AOB->current_length - 1;
    int m;
    while (i < j) {
        m = (i+j+1)/2;
        if (strcmp(AOB->Array[m]->name, base_name) <= 0) i = m;
        else j = m - 1;
    }
    m = strcmp(AOB->Array[i]->name, base_name); // using m so I dont need to declare another variable. We need this m in order not to calculate strcmp twice. 
    if (m == 0) return AOB->Array[i];
    else {
        if (i == 0 && m > 0) *index = i;
        else *index = i+1;
        return NULL;
    }
}

int find_index_of_base(ArrayOfBases *AOB, char *base_name) { // performing binary search on sorted array of bases in order to find correct index
    if (AOB->current_length == 0) return -1;
    int i = 0;
    int j = AOB->current_length - 1;
    int m;
    while (i < j) {
        m = (i+j+1)/2;
        if (strcmp(AOB->Array[m]->name, base_name) <= 0) i = m;
        else j = m - 1;
    }
    if (strcmp(AOB->Array[i]->name, base_name) == 0) return i;
    else return -1;
}

ArrayOfBases *initialize_array_of_bases() {
    ArrayOfBases *AOB = malloc(sizeof(ArrayOfBases));
    if (AOB == NULL) return NULL;
    AOB->max_length = BASIC_LENGTH_OF_ARRAY;
    AOB->current_length = 0;
    AOB->Array = malloc(BASIC_LENGTH_OF_ARRAY*sizeof(PfBase));
    for (int i = 0; i < BASIC_LENGTH_OF_ARRAY; i++) {
        AOB->Array[i] = NULL;
    }
    return AOB;
}

int delete_base(ArrayOfBases *AOB, char *base_name) { 
    int index = find_index_of_base(AOB, base_name);
    if (index < 0 || index >= AOB->current_length) return ERROR; // case when index was out of bounds
	phfwdDelete(AOB->Array[index]->base);
    free(AOB->Array[index]->name);
    free(AOB->Array[index]);
    AOB->Array[index] = NULL;
    if (index < AOB->current_length - 1) { // we need to move array if we are deleting something from the middle. If its last element then there is no such need
        move_array_left(AOB->Array, index, AOB->current_length);
        AOB->Array[AOB->current_length-1] = NULL;
    }
    AOB->current_length -= 1;
    return SUCCESS;
}
        
bool is_number(char x) {
     return ((x >= '0' && x <= '9') || x == ':' || x == ';');
}

int recognize_input(char sign) { // Checking if a 'sign' matches some enums
    if (isspace(sign)) return WHITE_SIGN;
    else if (is_number(sign)) return NUMBER;
    else if (sign == '$') return COMMENT;
    else if (sign == '?') return Q_MARK;
    else if (sign == '>') return LARGER_CHARACTER;
    else if (sign == 'D') return DEL_OPERATOR;
    else if (sign == 'N') return NEW_OPERATOR;
    else if (sign == '@') return AT;
    else return ERROR;
}

void error_eof(ArrayOfBases *AOB) {
    fprintf(stderr, "ERROR EOF\n");
    clear(AOB);
    exit(1);
}

char *read_whole_number(int *byte_number, char sign, int *following_sign) { // reading a number after we saw its first digit, which is 'sign' 
    int current_length = 0;
    int max_length = BASIC_LENGTH_OF_NUMBER;
    char *number = malloc((max_length+1)*sizeof(char));
    while (is_number(sign) && !feof(stdin)) { // reading all the digits
        if (current_length == max_length - 1) {
            max_length *= 2;
            number = realloc(number, (max_length + 1)*sizeof(char));
        }
        number[current_length] = sign;
        current_length++;
        sign = getchar();
    }
	*following_sign = recognize_input(sign); // checking what is the following sign after number so handle_input() knows how to behave next.
    number[current_length] = '\0';
    *byte_number += (current_length);
    return number;
}
    
char *find_number(int *byte_numbers, ArrayOfBases *AOB, int *following_sign, char *number) { // looking for a number without knowing its first character
    char sign;
    do {
        sign = getchar();
        *byte_numbers += 1;
        *following_sign = recognize_input(sign);
        if (*following_sign == COMMENT)
            handle_comment(AOB, byte_numbers);
    } while ((*following_sign == WHITE_SIGN || *following_sign == COMMENT) && !feof(stdin)); // deleting all comments all white signs in that loop
    if (feof(stdin)) {
	free(number);
        error_eof(AOB);
	    return NULL;
    }
    else if (!is_number(sign)) { // if a first found character other than white sign is not a digit, then its an error.
                                // It means that we can only use this function in specific context.
	free(number);
        handle_error(*byte_numbers, AOB, "ERROR");
        return NULL;
    }
    else return read_whole_number(byte_numbers, sign, following_sign); // returning a number with 'sign' as its first digit
}
        
bool is_operator(char sign) {
    return (sign == '>' || sign == '?');
}

int search_for_operator(ArrayOfBases *AOB, int *byte_number, char *number) { // looking for '?' or '>' sign
    char sign;
    do {
        sign = getchar();
        *byte_number += 1;
    } while (isspace(sign) && !feof(stdin)); // deleting all white signs
    if (feof(stdin)) {
        error_eof(AOB);
        return -1;
    }
    else if (sign == '>')
        return LARGER_CHARACTER;
    else if (sign == '?')
        return Q_MARK;
    else if (sign == '$')
        return COMMENT;
    else {
        free(number);
        handle_error(*byte_number, AOB, "ERROR");
        return -1;
    }
}

bool is_comment(int enum_type) { // just checking if enum types are the one for comment
    return (enum_type == COMMENT || enum_type == COMMENT_ON || enum_type == COMMENT_ALMOST_DONE);
}


void handle_comment(ArrayOfBases *AOB, int *byte_number) {
    char sign;
    int type_of_comment = COMMENT;
    while(!feof(stdin) && is_comment(type_of_comment)) {
        *byte_number += 1;
        switch(type_of_comment) {
            case COMMENT: // case when we just started a potential comment with '$' sign
                sign = getchar();
                if (sign == '$')
                    type_of_comment = COMMENT_ON;
                else handle_error(*byte_number, AOB, "ERROR"); // thats what happens if there is no following '$' sign after the first one
                break;

            case COMMENT_ON: // processing comment
                sign = getchar();
                if (feof(stdin))
                    error_eof(AOB);
                if (sign == '$') // putting on alert if we see '$' sign
                    type_of_comment = COMMENT_ALMOST_DONE;
                break;

            case COMMENT_ALMOST_DONE:
                sign = getchar();
                if (sign == '$') { // thats a time to stop comment
                    type_of_comment = -1;
                }
                else type_of_comment = COMMENT_ON; // first '$' sign was false alarm
                break;
        }
    }
    if (feof(stdin) && (type_of_comment == COMMENT_ALMOST_DONE || type_of_comment == COMMENT_ON)) // case when input finished without finishing comment
        error_eof(AOB);
    
}

bool correct_ID(char sign) {
    return isalpha(sign) || is_number(sign);
}

char *get_string(ArrayOfBases *AOB, int *byte_numbers, int *type_of_input, size_t operator) {
    int current_length = 0;
    int max_length = BASIC_LENGTH_OF_WORD;
    char sign;
    do {
        sign = getchar();
        *byte_numbers += 1;
        *type_of_input = recognize_input(sign);
        if (*type_of_input == COMMENT)
            handle_comment(AOB, byte_numbers);
    } while ((*type_of_input == WHITE_SIGN || *type_of_input == COMMENT) && !feof(stdin)); // deleting all comments all white signs in that loop
    if (feof(stdin))
        error_eof(AOB); 
    if (is_number(sign)) { 
        if (operator == DEL_OPERATOR)
            return read_whole_number(byte_numbers, sign, type_of_input);
        else if (operator == NEW_OPERATOR)
            handle_error(*byte_numbers, AOB, "ERROR");
    }
    char *word = malloc((BASIC_LENGTH_OF_WORD+1)*sizeof(char));
    while (correct_ID(sign) && !feof(stdin)) { // reading whole ID
        if (current_length == max_length) {
            max_length *= 2;
            word = realloc(word, (max_length + 2)*sizeof(char));
        }
        word[current_length++] = sign;
        sign = getchar();
    }
    *byte_numbers += current_length;
    *type_of_input = recognize_input(sign);
    if (current_length == 0) { // thats case when there was no ID at all
        free(word);
        return NULL;
    }
    else {
        word[current_length] = '\0';
        return word;
    }
}

      
char *read_rest_of_operator(ArrayOfBases *AOB, int *byte_numbers, int *type_of_input) {
    int current_length = 0;
    int max_length = BASIC_LENGTH_OF_WORD;
    char sign;
    char *word = malloc((BASIC_LENGTH_OF_WORD+1)*sizeof(char));
    sign = getchar();
    *byte_numbers += 1;
    while (isalpha(sign) && !feof(stdin)) {
        if (current_length == max_length) {
            max_length *= 2;
            word = realloc(word, (max_length + 1)*sizeof(char));
        }
        word[current_length++] = sign;
        sign = getchar();
        *byte_numbers += 1;
    }
    if (feof(stdin)) {
        free(word);
        error_eof(AOB);
    }
    word[current_length] = '\0';
    *type_of_input = recognize_input(sign);
    return word;
}
    
int max(int a, int b) {
    if (a > b) return a;
    else return b;
}

size_t count_digits(char *set) {
    int length = strlen(set);
    int counter = 0;
    for (int i = 0; i < length; i++) 
        if (is_number(set[i]))
            counter++;
    return counter;
}


void handle_input(ArrayOfBases *AOB, PfBase *current_base) {
    PfBase *tmp;
    tmp = NULL;
    char *number, *number2, *word;
    number = number2 = word = NULL;
    int type_of_input, byte_number, index, current_byte_number;
    index = 0;
    char sign = getchar();
    type_of_input = recognize_input(sign);
    byte_number = 1;
	while(!feof(stdin)) {
        switch(type_of_input) {

            case WHITE_SIGN:
                sign = getchar();
                type_of_input = recognize_input(sign);
                byte_number++;
                break;

            case COMMENT:
                handle_comment(AOB, &byte_number);
                sign = getchar();
                byte_number++;
                type_of_input = recognize_input(sign);
                break;

            case NUMBER:
                number = read_whole_number(&byte_number, sign, &type_of_input); // reading whole number that just started
                while (type_of_input == WHITE_SIGN || type_of_input == COMMENT) { // loop in order to delete all comments and white signs
                    if (type_of_input == WHITE_SIGN)
                        type_of_input = search_for_operator(AOB, &byte_number, number);
                    else if (type_of_input == COMMENT) {
                        handle_comment(AOB, &byte_number);
                        sign = getchar();
                        byte_number++;
                        type_of_input = recognize_input(sign);
                    }
                }
                switch (type_of_input) { // switch for few cases we can have after a number

                    case Q_MARK: // case when '?' is after the number
                        if (current_base == NULL) {
                            free(number);
                            handle_error(byte_number, AOB, "ERROR ?");
                        }
                        else {
                            print_numbers(phfwdGet(current_base->base, number), byte_number, AOB, number);
                            sign = getchar();
                            type_of_input = recognize_input(sign);
                            byte_number++;
                            free(number);
                            number = NULL;
                        }
                        break;
                    
                    case LARGER_CHARACTER: // case when '>' is after the number
                        if (current_base == NULL) { // if we have no current base then we cant perform the '>' operator
                            free(number);
                            handle_error(byte_number, AOB, "ERROR >");
                        }
                        else {
                            current_byte_number = byte_number; // used to give correct byte number in case it goes wrong
                            number2 = find_number(&byte_number, AOB, &type_of_input, number);
                            if (!phfwdAdd(current_base->base, number, number2)) { // This case means that adding redirection went wrong
                                free(number);
                                free(number2);
                                handle_error(current_byte_number, AOB, "ERROR >");
                            }
                            free(number);
                            free(number2);
                            number = number2 = NULL;
                        }
                        break;
                
                    default: // every other case is wrong
                        free(number);
                        handle_error(byte_number, AOB, "ERROR");
                        break;
                }
                break;

            case NEW_OPERATOR: // we see that there's a 'N' letter
                word = read_rest_of_operator(AOB, &byte_number, &type_of_input);
                if (strcmp(word, "EW") != 0) { // need to check if its really the 'NEW' expression
                    free(word);
                    handle_error(byte_number, AOB, "ERROR");
                }
                else {
                    if (type_of_input == COMMENT) { // case when we have comment right after NEW
                        handle_comment(AOB, &byte_number);
                    }
                    free(word);
                    word = get_string(AOB, &byte_number, &type_of_input, NEW_OPERATOR); // Getting next string after NEW command
                    if (word == NULL) { // case when there is no following sings to read
			            handle_error(byte_number, AOB, "ERROR");
		            }
                    else if (is_number(word[0])) {
                        handle_error(byte_number-1, AOB, "ERROR");
                    }
                    if (strcmp(word, "NEW") == 0 || strcmp(word, "DEL") == 0) { // cant have that ID
                        free(word);
                        handle_error(byte_number, AOB, "ERROR");
                    }
                    tmp = find_base(AOB, &index, word);
                    if (tmp == NULL) { // if there's no base with ID we just received
                        
                        tmp = insert_base(AOB, index, word);
                        if (tmp == NULL) { // memory error
                            free(word);
                            handle_error(byte_number, AOB, "MEMORY ERROR");
                        }
                        else current_base = tmp;
                    }
                    else current_base = tmp; // if there already is base with such ID, we just take it
                    free(word);
                    word = NULL;
                }
                break;
                        
                
            case DEL_OPERATOR: // seeing a 'D' letter in input
                current_byte_number = byte_number; // Using it in order to call ERROR DEL with that number
                word = read_rest_of_operator(AOB, &byte_number, &type_of_input);
                if (strcmp(word, "EL") != 0) { // need to check if its really 'DEL'
                    free(word);
                    handle_error(byte_number, AOB, "ERROR");
                }
                else {
                        if (type_of_input == COMMENT) { // case when there is comment right after DEL
                        handle_comment(AOB, &byte_number);
                    }
                    free(word);
                    word = get_string(AOB, &byte_number, &type_of_input, DEL_OPERATOR); // getting ID of base we have to delete
                    if (word == NULL) { // case when there is no following sings to read
			            handle_error(byte_number, AOB, "ERROR");
		            }
                    if (current_base != NULL && strcmp(current_base->name, word) == 0) // case when we delete a current base
                        current_base = NULL;
                    if (is_number(word[0]) )
                        phfwdRemove(current_base->base, word);
                    else if (delete_base(AOB, word) == ERROR) { // if deleting goes wrong
                        free(word);
                        handle_error(current_byte_number, AOB, "ERROR DEL");
                    }
                    free(word); // if deleting goes fine we just continue
                }
                break;

            case Q_MARK: // This is case when '?' is before the number
                if (current_base != NULL) { // if current base is NULL then this operation is wrong
                    do {
                        sign = getchar();
                        byte_number++;
                        type_of_input = recognize_input(sign);
                        if (type_of_input == COMMENT)
                            handle_comment(AOB, &byte_number);
                        } while ((type_of_input == WHITE_SIGN || type_of_input == COMMENT) && !feof(stdin)); // deleting all comments all white signs in that loop
                    if (feof(stdin)) //if we get to the end of the file before finding number
                        error_eof(AOB);
                    else if (!is_number(sign)) //if there is other sing than number and white sign after '?' operator
                        handle_error(byte_number, AOB, "ERROR");
                    else {
                        number = read_whole_number(&byte_number, sign, &type_of_input); // reading the number
                        print_numbers(phfwdReverse(current_base->base, number), byte_number, AOB, number); // performing phfwdReverse
                        free(number);
                        number = NULL;
                    }
                    
                }
                else {
                    free(number);
                    handle_error(byte_number, AOB, "ERROR ?");
                }
                break;

            case AT:
                if (current_base != NULL) {
                    if (feof(stdin)) //if we get to the end of the file before finding number
                        error_eof(AOB);
                    else {
                        number = get_string(AOB, &byte_number, &type_of_input, AT);
                        printf("%zu\n", phfwdNonTrivialCount(current_base->base, number, max(0, count_digits(number) - 12)));
                        free(number);
                        number = NULL;
                    }
                }   
                else {
                    free(number);
                    handle_error(byte_number, AOB, "ERROR @");
                }
                break;         




            default:
                handle_error(byte_number, AOB, "ERROR");
                break;

            
                
        }
    
    }
}

        
























