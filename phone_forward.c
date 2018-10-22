#include "phone_forward.h"
#include <stdio.h>
#include <string.h>
#include "phone_forward_parser.h"

#define COUNT_OF_NUMBERS 12
#define CHAR_TO_NUMBER(number) ((int)number - (int)'0')
#define BASIC_ARRAY_LENGTH 25

/** @struct PhoneNumbers phone_forward.h
 * Implementacja struktury przechowującej numery telefonu
 */
struct PhoneNumbers {
	/**
    * Aktualna długość tablicy. 
    */
    size_t current_length;
    /**
    * Maksymalna długość tablicy.
    */
    size_t max_length;
    /**
    * Wskaźnik na tablicę napisów z numerami.
    */
    char **array_of_numbers;
};

/**
 * Struktura przechowująca przekierowania
 * od-do w formie trie.
 */
struct RedsFromTo {
    /**
    * Tablica wskaźników na synów.
    */
    struct RedsFromTo *children[COUNT_OF_NUMBERS];
    /**
    * wskaźnik na napis reprezentujący numer
    * na który jest przekierowanie.
    */
    char *redirection;
};

/**
 * Struktura przechowująca przekierowania 
 * do-od w formie trie.
 */
struct RedsToFrom {
    /**
    * Tablica wskaźników na synów.
    */
    struct RedsToFrom *children[COUNT_OF_NUMBERS];
    /**
    * Wskaźnik na przekierowania "od" w formie
    * struktury @p PhoneNumbers, gdyż może być
    * kilka róznych przekierowań na jeden numer.
    */
    struct PhoneNumbers *redirections;
};

/** @struct PhoneForward phone_forward.h
 * Implementacja struktury przechowującej przekierowania numerów telefonów
 */
struct PhoneForward {
    /** Wskaźnik na strukturę przechowującą
    * przekierowania od-do.
    */
    struct RedsFromTo *reds_from_to;
    /** Wskaźnik na strukturę przechowującą 
    * przekierowania do-od.
    */
    struct RedsToFrom *reds_to_from;
};

/**
 * typedef dla struktury RedsFromTo, aby unikac pisania slowa kluczowego "struct"
 */
typedef struct RedsFromTo RedsFromTo;

/**
 * typedef dla struktury RedsToFrom, aby unikac pisania slowa kluczowego "struct"
 */
typedef struct RedsToFrom RedsToFrom;

/** @brief Funkcja alokująca strukturę PhoneNumbers.
 * Funkcja alokuje pamięc i zwraca wskaźnik na strukturę
 * PhoneNumbers, która musi potem zostać zwolniona funkcją
 * @ref phnumDelete.
 * @return Wskaźnik na nowo utworzoną strukturę.
*/
PhoneNumbers *declare_phone_numbers() {
    PhoneNumbers *new = malloc(sizeof(PhoneNumbers));
    new->current_length = 0;
    new->max_length = BASIC_ARRAY_LENGTH;
    new->array_of_numbers = malloc(BASIC_ARRAY_LENGTH*sizeof(char*));
    for (int i = 0; i < BASIC_ARRAY_LENGTH; i++)
        new->array_of_numbers[i] = NULL;
    return new;
}

/** @brief Funkcja alokująca strukturę RedsFromTo.
 * Funkcja alokuje pamięc i zwraca wskaźnik na strukturę
 * RedsFromTo, która musi potem zostać zwolniona funkcją
 * @ref rftDelete.
 * @return Wskaźnik na nowo utworzoną strukturę.
*/
RedsFromTo *rftNew() {
    RedsFromTo *new = malloc(sizeof(RedsFromTo));
    if (new == NULL) return NULL;
    new->redirection = NULL;
    for (int i = 0; i < COUNT_OF_NUMBERS; i++)
        new->children[i] = NULL;
    return new;
}

/** @brief Funkcja alokująca strukturę RedsToFrom.
 * Funkcja alokuje pamięc i zwraca wskaźnik na strukturę
 * RedsToFrom, która musi potem zostać zwolniona funkcją
 * @ref rtfDelete.
 * @return Wskaźnik na nowo utworzoną strukturę.
*/
RedsToFrom *rtfNew() {
    RedsToFrom *new = malloc(sizeof(RedsToFrom));
    if (new == NULL) return NULL;
    new->redirections = NULL;
    for (int i = 0; i < COUNT_OF_NUMBERS; i++)
        new->children[i] = NULL;
    return new;
}

PhoneForward *phfwdNew() {
	PhoneForward *new = malloc(sizeof(PhoneForward));
	if (new == NULL) return NULL;
    new->reds_from_to = rftNew();
    new->reds_to_from = rtfNew();
    if (new->reds_from_to == NULL || new->reds_to_from == NULL) 
        return NULL;
    return new;  
}

void phnumDelete(PhoneNumbers const *pnum) {
    if (pnum != NULL) {
        for (size_t i = 0; i < pnum->current_length; i++) 
            free(pnum->array_of_numbers[i]);
        free(pnum->array_of_numbers);
        free((void*)pnum);
    }
}

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p rft. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 * @param[in] rft – wskaźnik na usuwaną strukturę.
 */
void rftDelete(RedsFromTo *rft) {
    if (rft != NULL) {
        if (rft->redirection != NULL)
            free(rft->redirection);
        for (int i = 0; i < COUNT_OF_NUMBERS; i++)
            rftDelete(rft->children[i]);
        free(rft);
    }
}

/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywaną przez @p rtf. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 * @param[in] rtf – wskaźnik na usuwaną strukturę.
 */
void rtfDelete(RedsToFrom *rtf) {
    if (rtf != NULL) {
        phnumDelete(rtf->redirections);
        for (int i = 0; i < COUNT_OF_NUMBERS; i++)
            rtfDelete(rtf->children[i]);
        free(rtf);
    }
}

void phfwdDelete(PhoneForward *pf) {
    if (pf != NULL) {
        rftDelete(pf->reds_from_to);
        rtfDelete(pf->reds_to_from);
        free(pf);
    }
}


/**
 * Funkcja sprawdzająca czy napis jest numerem.
 * @param[in] num - wskaźnik na napis.
 * @return @p true jeśli napis jest numerem,
           @p false w przeciwnym razie.
*/
bool is_string_a_number(const char *num) {
    if (num == NULL) return false;
    int length = strlen(num);
    if (length == 0) return false;
    for(int i = 0; i < length; i++) 
        if (!is_number(num[i])) return false;
    return true;
}

/** Funkcja wyszukuje indeks w tablicy do wstawienia numeru.
 * Funkcja, za pomocą binary search, wyszukuje indeks w tablicy
 * numerów, na który można wstawić numer, tak, żeby zachować
 * posortowanie tablicy.
 * @param[in] pnum - wskaźnik na strukturę przechowującą
 *                  numery.
 * @param[in] num - wskaźnik na napis, który wstawiamy.
 * @return Indeks w który można wstawić numer.
*/
int find_index_to_insert_into(PhoneNumbers *pnum, char *num) {
    if (pnum->current_length == 0)
        return 0;
    int i = 0;
    int j = pnum->current_length -1;
    int m;
    while (i < j) {
        m = (i+j+1)/2;
        if (strcmp(pnum->array_of_numbers[m], num) <= 0)
            i = m;
        else
            j = m -1;
    }
    m = strcmp(pnum->array_of_numbers[i], num);
    if (m == 0)
        return -1;
    else if (i == 0 && m > 0)
        return i;
    else
        return i+1;
}

/** @brief Funkcja przesuwająca tablicę o jeden w prawo.
 * Funkcja przesuwa tablicę zawierającą wskaźniki na napisy
 * o jeden w prawo, aby można było wprowadzić do tej tablicy nowy napis
 * w odpowiednim miejscu.
 * @param[in] array - wskaźnik na tablicę.
 * @param[in] index - indeks od którego tablica ma zostać przesunięta w prawo.
 * @param[in] current_length - bieżąca długość tablicy.
*/
static void move_array_of_strings_right(char **array, int index, int current_length) {
    char *tmp = array[index];
    for (int i = index+1; i <= current_length; i++) {
        char *tmp2 = array[i];
        array[i] = tmp;
        tmp = tmp2;
    }
}

/** @brief Funkcja wstawiająca numer w tablicę numerów.
 * Funkcja wstawia numer @p num w tablicę numerów, tak,
 * żeby zachować posortowanie tablicy.
 * @param[in] pnum - wskaźnik na strukturę przechowującą
 *                  numery.
 * @param[in] num - wskaźnik na napis, który wstawiamy.
*/
void insert_into_array_of_numbers(PhoneNumbers *pnum, char *num) {
    int index = find_index_to_insert_into(pnum, num);
    if (index >=0) {
        if (pnum->current_length == pnum->max_length) {
            pnum->array_of_numbers = realloc(pnum->array_of_numbers, 2*pnum->max_length*sizeof(char*));
            pnum->max_length *= 2;
            for (size_t j = pnum->current_length; j < pnum->max_length; j++)
                pnum->array_of_numbers[j] = NULL;
        }
        if ((size_t)index != pnum->current_length)
            move_array_of_strings_right(pnum->array_of_numbers, index, pnum->current_length);
        pnum->array_of_numbers[index] = num;
        pnum->current_length += 1;
    }
    else if (num != NULL)
        free(num);
}

/** @brief Funkcja wyszukująca indeks numeru w tablicy.
 * Funkcja wyszukuje napis @p num w tablicy numerów, korzystając
 * z binary search. Jeśli numeru nie ma w tablicy, funkcja zwraca -1.
 *  * @param[in] pnum - wskaźnik na strukturę przechowującą
 *                  numery.
 * @param[in] num - wskaźnik na napis, którego szukamy.
 * @return Indeks numeru w tablicy, albo -1 jeśli tego numeru w tablicy nie ma.
*/
static int find_index_of_number(PhoneNumbers *pnum, char const *num) {
    if (pnum->current_length == 0) return -1;
    int i = 0;
    int j = pnum->current_length - 1;
    int m;
    while (i < j) {
        m = (i+j+1)/2;
        if (strcmp(pnum->array_of_numbers[m], num) <= 0) i = m;
        else j = m - 1;
    }
    if (strcmp(pnum->array_of_numbers[i], num) == 0) return i;
    else return -1;
}

/** @brief Funkcja przesuwająca tablicę o jeden w lewo.
 * Funkcja przesuwa tablicę zawierającą wskaźniki na napisy
 * o jeden w lewo, aby zachować jedność tablicy po usunięciu jednego elementu
 * ze środka.
 * @param[in] array - wskaźnik na tablicę.
 * @param[in] index - indeks od którego tablica ma zostać przesunięta w lewo.
 * @param[in] current_length - bieżąca długość tablicy.
*/
static void move_array_of_strings_left(char **array, int index, int current_length) {
    for (int i = index; i < current_length -1; i++)
        array[i] = array[i+1];
}

/** @brief Funkcja usuwająca przekierowanie z tablicy przekierowań.
 * Funkcja usuwa numer @p num z tablicy numerów.
 * Numer ten reprezentuje przekierowanie "od".
 * @param[in] pnum - wskaźnik na strukturę przechowującą
 *                  numery.
 * @param[in] num - wskaźnik na napis, który usuwamy.
*/
void delete_redirection(PhoneNumbers *pnum, char const *num) {
    int index = find_index_of_number(pnum, num);
    if (index >= 0) {
        free(pnum->array_of_numbers[index]);
        pnum->array_of_numbers[index] = NULL;
        if ((size_t)index < pnum->current_length -1) {
            move_array_of_strings_left(pnum->array_of_numbers, index, pnum->current_length);
            pnum->array_of_numbers[pnum->current_length-1] = NULL;
        }
        pnum->current_length -= 1;

    }
}

/** @brief Funkcja usuwająca przekierowanie ze struktury RedsToFrom.
 * Funkcja usuwa przekierowanie na numer @p num z numeru @p num2.
 * @param[in] rtf - wskaźnik na strukturę przechowującą przekierowania do-od.
 * @param[in] num - wskaźnik na przekierowanie "do".
 * @parm[in] num2 - wskaźnik na przekierowanie "od".
*/
void removeFromRTF(RedsToFrom *rtf, char const *num, char const *num2) {
    int length = strlen(num);
    int index;
    for (int i = 0; i < length; i++) {
        index = CHAR_TO_NUMBER(num[i]);
        rtf = rtf->children[index];
    }
    delete_redirection(rtf->redirections, num2);
    if (rtf->redirections->current_length == 0) {
        phnumDelete(rtf->redirections);
        rtf->redirections = NULL;
    }
}

/** @brief Funkcja dodająca przekierowanie do struktury RedsFromTo.
 * Funkcja dodaje przekierowanie z numeru @p from na numer @p to
 * do struktury RedsFromTo.
 * @param[in] rft - wskaźnik na strukturę do której dodajemy przekierowanie.
 * @param[in] from - wskaźnik na napis reprezentujący numer z którego jest
 *                   przekierowanie.
 * @param[in] to - wskaźnik na napis reprezentujący numer na który jest
                   przekierowanie.
 * @param[in] length1 - długość numeru @p from.
 * @param[in] length2 - długość numeru @p to.
*/
void addToRFT(RedsFromTo *rft, RedsToFrom *rtf, char const *from, char const *to, int length1, int length2) {
// length1 is length of num1 and length2 is length of num2
    int index;
    for (int i = 0; i < length1; i++) {
        index = CHAR_TO_NUMBER(from[i]);
        if (rft->children[index] == NULL)
            rft->children[index] = rftNew();
        rft = rft->children[index];
    }
    if (rft->redirection != NULL) {
        removeFromRTF(rtf, rft->redirection, from);
        free(rft->redirection);
    }
    rft->redirection = malloc(sizeof(char) * (length2+1));
    strcpy(rft->redirection, to);
}


/** @brief Funkcja dodająca przekierowanie do struktury RedsToFrom.
 * Funkcja dodaje przekierowanie z numeru @p from na numer @p to
 * do struktury RedsToFrom.
 * @param[in] rtf - wskaźnik na strukturę do której dodajemy przekierowanie.
 * @param[in] from - wskaźnik na napis reprezentujący numer z którego jest
 *                   przekierowanie.
 * @param[in] to - wskaźnik na napis reprezentujący numer na który jest
                   przekierowanie.
 * @param[in] length1 - długość numeru @p from.
 * @param[in] length2 - długość numeru @p to.
*/
void addToRTF(RedsToFrom *rtf, char const *from, char const *to, int length1, int length2) {
    int index;
    for (int i = 0; i < length2; i++) {
        index = CHAR_TO_NUMBER(to[i]);
        if (rtf->children[index] == NULL)
            rtf->children[index] = rtfNew();
        rtf = rtf->children[index];
    }
    if (rtf->redirections == NULL)
        rtf->redirections = declare_phone_numbers();
    char *number = malloc((length1+1)*sizeof(char));
    strcpy(number, from);
    insert_into_array_of_numbers(rtf->redirections, number);
}

bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if (strcmp(num1, num2) == 0 || !is_string_a_number(num1) || !is_string_a_number(num2) || pf == NULL) 
        return false;
    int length1 = strlen(num1);
    int length2 = strlen(num2);
    addToRFT(pf->reds_from_to, pf->reds_to_from, num1, num2, length1, length2);
    addToRTF(pf->reds_to_from, num1, num2, length1, length2);   
    return true;
}


/** @brief Funkcja usuwająca przekierowania ze struktury RedsFromTo.
 * Funkcja usuwa wszystkie przekierowania o prefiksie @p num ze struktury RedsFromTo.
 * Funkcja dodatkowo uaktualnia napis @p num, tak aby rekurencyjne wywołania funkcji
 * usuwały numery z poprawnym prefiksem.
 * @param[in] rft - wskaźnik na strukturę przechowującą przekierowania od-do.
 * @param[in] rtf - wskaźnik na strukturę przechowującą przekierowania do-od.
 * @param[in] current_index - aktualna długość @p num.
 * @param[in] max_index - maksymalna możliwa długość @p num.
 * @param[in] num - wskaźnik na prefiks.
 * @return Wskaźnik na uaktualniony napis @p num.
*/
char *removeFromRFT(RedsFromTo *rft, RedsToFrom *rtf, int current_index, int max_index, char *num) {
    if (rft != NULL) {
        if (current_index == max_index-2) {
            num = realloc(num, (2*max_index+1));
            max_index *= 2;
        }
        if (rft->redirection != NULL) {
            removeFromRTF(rtf, rft->redirection, num);
            free(rft->redirection);
            rft->redirection = NULL;
        }
        for (int i = 0; i < COUNT_OF_NUMBERS; i++) {
		    num[current_index] = i + '0';
            num[current_index+1] = '\0';
            num = removeFromRFT(rft->children[i], rtf, current_index+1, max_index, num);
        }
    }
    return num;
}
    
  
void phfwdRemove(PhoneForward *pf, char const *num) {
    if (pf != NULL && is_string_a_number(num)) {
        int length = strlen(num);
        int index;
        RedsFromTo *rft = pf->reds_from_to;
        RedsToFrom *rtf = pf->reds_to_from;
        for (int i = 0; i < length; i++) {
            index = CHAR_TO_NUMBER(num[i]);
            if (rft->children[index] != NULL)
            	rft = rft->children[index];
            else
                return;
        }
	char *number = malloc(sizeof(char)*(2*length+1));
	strcpy(number, num);
    number = removeFromRFT(rft, rtf, length, 2*length +2, number);
    free(number);
    }
}
    
/** @brief Tworzy nowe przekierowanie.
 * Funkcja bierze prefix @p to oraz sufix @p end i tworzy nowy string,
 * łączący oba numery.
 * @param[in] to - wskaźnik na napis reprezentujący prefix.
 * @param[in] end - wskaźnik na napis reprezentujący sufix.
 * @return Wskaźnik na napis reprezentujący połączone numery.
 */
char *create_redirection(char *to, const char *end) {
    int i = 0;
    int j = strlen(to);
    int size = strlen(end) + j;
    char *result = malloc((size + 1) * sizeof(char));
    while (i < j) {
        result[i] = to[i];
        i++;
    }
    while (i < size) {
        result[i] = end[i-j]; // i-j cause we need to start array "end" from 0
        i++;
    }
    result[i] = '\0';
    return result;
}

PhoneNumbers const * phfwdGet(PhoneForward *pf, char const *num) {
    RedsFromTo *rft = pf->reds_from_to;
    int length = strlen(num);
    int index;
    int i = 0;
    int end_of_redirection = 0;
    PhoneNumbers *pnum = declare_phone_numbers();
    if (!is_string_a_number(num))
        return pnum;
    bool max_redirection = false;
	char *candidate = NULL;
    while (!max_redirection && i < length) {
        index = CHAR_TO_NUMBER(num[i]);
        if (rft->children[index] == NULL)
            max_redirection = true;
        else {
            rft = rft->children[index];
            if (rft->redirection != NULL) {
                end_of_redirection = i;
                candidate = rft->redirection;
            }
        }
        i++;
    }
    if (candidate != NULL) 
        pnum->array_of_numbers[0] = create_redirection(candidate, &num[end_of_redirection+1]);
    else {
        pnum->array_of_numbers[0] = malloc((length+1)*sizeof(char));
        strcpy(pnum->array_of_numbers[0], num);
    }
    pnum->current_length += 1;
    return pnum;
}




char const * phnumGet(PhoneNumbers const *pnum, size_t idx) {   
        if (idx >= pnum->current_length)
            return NULL;
        else  
            return pnum->array_of_numbers[idx];
}
        

    
PhoneNumbers const * phfwdReverse(PhoneForward *pf, char const *num) {
    PhoneNumbers *pnum = declare_phone_numbers();
    if (!is_string_a_number(num))
        return pnum;
    RedsToFrom *rtf = pf->reds_to_from;
    size_t length = strlen(num);
    size_t i = 0;
    int index;
    char *number = malloc((length+1)*sizeof(char)); // used to insert "num" to pnum, need to copy here, because we do not copy in insert_into...
    strcpy(number, num);
    insert_into_array_of_numbers(pnum, number);
    while (rtf != NULL && i < length) {
        index = CHAR_TO_NUMBER(num[i]);
        rtf = rtf->children[index];
        if (rtf != NULL && rtf->redirections != NULL) {
                size_t j = 0;
                while (j < rtf->redirections->current_length) {
                    insert_into_array_of_numbers(pnum, create_redirection(rtf->redirections->array_of_numbers[j], &num[i+1]));
					j++;
                }
        }
        i++;
    }
    return pnum;
}

/** 
 * Funkcja sprawdza czy napis posiada jakąś cyfrę.
 * @param[in] string - wskaźnik na napis, który sprawdzamy.
 * @param[in] length - długość sprawdzanego napisu.
 * @return @p true jeśli napis posiada cyfrę, @p false w
 *                 przeciwnym wypadku.
*/
bool contains_number(char const *string, int length) {
	for (int i = 0; i < length; i++) {
		if (is_number(string[i]))
            return true;
    }
    return false;
}

/** 
 * Funkcja tworzy tablicę typu "bool", w której pod i-tym indeksem
 * jest wartość @p true, jeśli napis @p set zawiera cyfrę i, oraz @p false
 * w przeciwnym wypadku.
 * @param[in] set - wskaźnik na sprawdzany napis.
 * @param[in] length - długość sprawdzanego napisu.
 * @return Wskaźnik na nowo utworzoną tablicę typu "bool".
*/
bool *create_array_of_containing(char const *set, int length) {
    bool *result = malloc(COUNT_OF_NUMBERS*sizeof(bool));
    int index;
    for (int i = 0; i < COUNT_OF_NUMBERS; i++)
        result[i] = false;
    for (int i = 0; i < length; i++) {
        index = CHAR_TO_NUMBER(set[i]);
        if (index >= 0 && index < COUNT_OF_NUMBERS)
            result[index] = true;
    }
    return result;
}

/**
 * Funkcja oblicza ilość możliwych cyfr w numerze.
 * Inaczej mowiąc, wynik tej funkcji to ilość róznych cyfr
 * w napisie "set" przekazywanym do funkcji @ref phfwdNonTrivialCount.
 * @param[in] array - tablica typu "bool" zawierająca pod
 * i-tym indeksem @p true, jeśli cyfra jest "legalna" oraz @p false
 * w przeciwnym wypadku.
 * @return Ilość możliwych cyfr.
*/
size_t how_many_possible_numbers(bool *array) {
    size_t result = 0;
    for (int i = 0; i < COUNT_OF_NUMBERS; i++)
        if (array[i])
            result++;
    return result;
}

/**
 * Funkcja do szybkiego potęgowania.
 * @param[in] n - podstawa potęgi.
 * @param[in] a - wykładnik potęgi.
 * @return Wynik potęgowania.
*/
size_t quick_exp(int n, int a) {
    if (a == 0)
        return 1;
    else {
        size_t tmp = quick_exp(n, a/2);
        if (a % 2 == 1)
            return n*tmp*tmp;  
        else
            return tmp*tmp;
    }
}
   
/**
 * Funkcja tworzy tablicę liczb, z których może składać się numer w funkcji
 * @ref phfwdNonTrivialCount. 
 * @param[in] array - tablica typu "bool" zawierająca pod
 * i-tym indeksem @p true, jeśli cyfra jest "legalna" oraz @p false
 * w przeciwnym wypadku.
 * @param[in] number_of_possible_numbers - liczba możliwych cyfr,
 * będzie to też oczywiście długość zwracanej tablicy liczb.
 * @return Tablica możliwych liczb w numerze.
*/
int *array_of_possible_numbers(bool *array, int number_of_possible_numbers) {
    int *result = malloc(number_of_possible_numbers*sizeof(int));
    int j = 0;
    for (int i = 0; i < COUNT_OF_NUMBERS; i++) {
        if (array[i]) {
            result[j] = i;
            j++;
        }
    }     
    return result;
}

/** @brief Funkcja do obliczania ilości możliwych numerów.
 * Funkcja rekurencyjnie oblicza ilość możliwych numerów, korzystając
 * z tablicy liczb utworzonej w @ref array_of_possible_numbers.
 * @param[in] - wskaźnik na strukturę RedsToFrom, zawierającą przekierowania do-od.
 * @param[in] level - aktualna głębokość rekurencji.
 * @param[in] len - dopuszczalna długość numeru.
 * @param[in] legal_numbers - tablica możliwych liczb.
 * @param[in] number_of_poss_numbers - liczba możliwych liczb, inaczej
 * mówiąc długość tablicy @p legal_numbers.
 * @return Ilość możliwych numerów.
*/
size_t calculate_possible_numbers_rec(RedsToFrom *rtf, size_t level, size_t len, int *legal_numbers, size_t number_of_poss_numbers) {
    if (rtf != NULL) {
        size_t result = 0;
        if (rtf->redirections != NULL && level <= len)
            result += quick_exp(number_of_poss_numbers, len-level);
        else if (level < len) {
            for (size_t i = 0; i < number_of_poss_numbers; i++) 
                result += calculate_possible_numbers_rec(rtf->children[legal_numbers[i]], level+1, len, legal_numbers, number_of_poss_numbers);
        }
        return result;
    }
    else
        return 0;
}

size_t phfwdNonTrivialCount(struct PhoneForward *pf, char const *set, size_t len) {
    if (pf == NULL || set == NULL || len == 0)
        return 0;
    int length = strlen(set);
    if (length == 0 || !contains_number(set, length))
        return 0;
    size_t result = 0;
    RedsToFrom *rtf = pf->reds_to_from;
    bool *array_of_containing = create_array_of_containing(set, length);
    size_t number_of_possible_numbers = how_many_possible_numbers(array_of_containing);
    int *array_of_numbers = array_of_possible_numbers(array_of_containing, number_of_possible_numbers);
    for (size_t j = 0; j < number_of_possible_numbers; j++) 
        result += calculate_possible_numbers_rec(rtf->children[array_of_numbers[j]], 1, len, array_of_numbers, number_of_possible_numbers);
    free(array_of_numbers);
    free(array_of_containing);
    return result;
}








































    
