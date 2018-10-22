/** @file
 * Biblioteka parsera do obsługi programu "Telefony"
 *
 * @author Maciej Bala <m.bala2@student.uw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 30.05.2018
 */

#ifndef _PHONE_FORWARD_PARSER_H_
#define _PHONE_FORWARD_PARSER_H_

#include <stdio.h>
#include <stdbool.h>
#include "phone_forward.h"
#include <string.h>
#include <ctype.h>

/** 
 * Struktura przechowująca bazę przekierowań.
*/
struct PfBase;

/**
 * Struktura przechowująca tablicę baz przekierowań.
*/
struct ArrayOfBases;

/**
 * typedef dla struktury ArrayOfBases, aby unikac pisania slowa kluczowego "struct"
*/
typedef struct ArrayOfBases ArrayOfBases;

/**
 * typedef dla struktury PfBase, aby unikac pisania slowa kluczowego "struct"
*/
typedef struct PfBase PfBase;

/**
 * Enumerator ułatwiający czytanie wejścia.
*/
enum State_of_program {COMMENT, WHITE_SIGN, NUMBER, Q_MARK, LARGER_CHARACTER, ID, NEW_OPERATOR, DEL_OPERATOR, ERROR, SUCCESS, COMMENT_ON, COMMENT_ALMOST_DONE, AT};

/** @brief Funkcja do czysczenia pamięci po strukturze ArrayOfBases
 * Usuwa pamięć zaalokowaną przez @ref initialize_array_of_bases.
 * @param[in] AOB - wskaźnik na usuwaną strukturę. 
*/
void clear(ArrayOfBases *AOB);

/** @brief Funkcja zajmująca się obsługą błędów programu.
 * Funkcja czyści pamięć zaalokowaną przez program
 * i wypisuje odpowiedni komunikat wraz z liczbą
 * wczytanych dotąd bajtów.
 * @param[in] byte_number - liczba wczytanych dotąd bajtów.
 * @param[in] AOB - wskaźnik na strukturę ArrayOfBases.
 * @param[in] error_info - wskaźnik na komunikat który ma zostać wypisany.
*/
void handle_error(int byte_number, ArrayOfBases *AOB, char *error_info);

/** @brief Funkcja wypisująca numery na standardowe wyjście.
 * Funkcja wypisuje numery, każdy od nowej linii, 
 * które są przechowywane w pnum. Może też obsłużyć błąd
 * i zakończyć program, jeśli pnum jest puste.
 * @param[in] pnum - Wskaźnik na strukturę PhoneNumbers, z której będziemy wypisywać numery.
 * @param[in] byte_numbers - liczba wczytanych dotąd bajtów.
 * @param[in] AOB - wskaźnik na strukturę ArrayOfBases. 
 * @param[in] number - wskaźnik na napis zaalakowany wcześniej.
 * Używany jest on do zwolnienia pamięci w przypadku błędu.
*/ 
void print_numbers(PhoneNumbers const *pnum, int byte_numbers, ArrayOfBases *AOB, char *number);

/** @brief Funkcja tworząca bazę.
 * Funkcja tworzy bazę o podanej nazwie i zwraca wskaźnik na nią.
 * @param[in] name - wskaźnik na napis reprezentujący nazwę.
 * @return Wskaźnik na zaalokowaną strukturę PfBase, lub Null
	   jeśli nie udało się zaalokować pamięci.
*/
PfBase *create_base(const char *name);

/** @brief Funkcja przesuwająca tablicę o jeden w prawo.
 * Funkcja przesuwa tablicę zawierającą wskaźniki na strukturę PfBase
 * o jeden w prawo, aby można było wprowadzić do tej tablicy nowy wskaźnik
 * w odpowiednim miejscu.
 * @param[in] Array - wskaźnik na tablicę.
 * @param[in] index - indeks od którego tablica ma zostać przesunięta w prawo.
 * @param[in] current_length - bieżąca długość tablicy.
*/
void move_array_right(PfBase *Array[], int index, int current_length);

/** @brief Funkcja przesuwająca tablicę o jeden w lewo.
 * Funkcja przesuwa tablicę zawierającą wskaźniki na strukturę PfBase
 * o jeden w lewo, aby zachować jedność tablicy po usunięciu jednego elementu
 * ze środka.
 * @param[in] Array - wskaźnik na tablicę.
 * @param[in] index - indeks od którego tablica ma zostać przesunięta w lewo.
 * @param[in] current_length - bieżąca długość tablicy.
*/
void move_array_left(PfBase *Array[], int index, int current_length);

/** @brief Funkcja wstawiająca bazę w tablicę baz.
 * Funkcja tworzy i wstawia bazę o nazwie base_name na miejscu
 * danym przez parametr i.
 * @param[in] AOB - wskaźnik na strukturę ArrayOfBases, do której wstawiamy bazę.
 * @param[in] i - indeks na którym wstawiamy bazę.
 * @param[in] base_name - nazwa bazy, którą mamy wstawić.
 * @return Wskaźnik na nowo utworzoną i wstawioną bazę.
*/
PfBase *insert_base(ArrayOfBases *AOB, int i, const char *base_name);

/** @brief Funkcja znajdująca bazę o podanej nazwie.
 * Funkcja wyszukuje bazę o podanej nazwie w tablicy baz i zwraca wskaźnik na nią,
 * jednocześnie przekazując jej indeks poprzez wskaźnik na niego.
 * @param[in] AOB - wskaźnik na strukturę przechowującą tablicę baz.
 * @param[out] index - wskaźnik na indeks bazy, którą odnajdziemy.
 * @param[in] base_name - nazwa bazy, której szukamy.
 * @return Wskaźnik na odnalezioną bazę, albo NULL jeśli danej bazy nie ma w tablicy.
*/
PfBase *find_base(ArrayOfBases *AOB, int *index, char *base_name);

/** @brief Funkcja wyszukuje indeks bazy o podanej nazwie.
 * Funkcja wyszukuje w tablicy baz bazę o podanej nazwie
 * i zwraca jej indeks.
 * @param[in] AOB - wskaźnik na strukturę przechowującą tablicę baz.
 * @param[in] base_name - nazwa bazy, którą wyszukujemy.
 * @return indeks odnalezionej bazy, albo -1 jeśli nie udało się 
 *         odnaleźć bazy o podanej nazwie.
*/
int find_index_of_base(ArrayOfBases *AOB, char *base_name);

/** 
 * Funkcja alokuje pamięc na strukturę ArrayOfBases i zwraca
 * wskaźnik na nią.
 * @return Wskaźnik na zaalokowaną strukturę.
*/
ArrayOfBases *initialize_array_of_bases();

/** @brief Usuwa bazę o podanej nazwie.
 * Funkcja usuwa bazę o podanej nazwie, wyszukując ją 
 * najpierw w tablicy baz i następnie zwraca odpowiedni komunikat
 * informujący o sukcesie usuwania.
 * @param[in] AOB - wskaźnik na strukturę przechowującą tablicę baz.
 * @param[in] base_name - nazwa bazy, którą usuwamy.
 * @return SUCCESS jeśli usuwanie się udało, ERROR w przeciwnym razie,
 * np. jeśli nie ma bazy o podanej nazwie.
*/
int delete_base(ArrayOfBases *AOB, char *base_name);

/** 
 * Funkcja przyporządkowuje znak do odpowiednej kategorii.
 * @param[in] sign - znak, który analizujemy.
 * @return WHITE_SIGN jeśli znak to biały znak.
           NUMBER jeśli znak to cyfra.
           COMMENT jeśli znak to '$'.
		   Q_MARK jeśli znak to '?'.
           LARGER_CHARACTER jeśli znak to '>'.
           DEL_OPERATOR jeśli znak to 'D'.
           NEW_OPERATOR jeśli znak to 'N'.
           ERROR w przeciwnym wypadku.
*/
int recognize_input(char sign);

/** 
 * Funkcja obsługująca błąd EOF. Usuwa całą pamięć
 * przechowywaną przez strukturę ArrayOfBases i zamyka
 * program, zwracając wartość 1.
 * @param[in] AOB - wskaźnik na strukturę ArrayOfBases, która jest zwalniana.
*/
void error_eof(ArrayOfBases *AOB);

/** @brief Funkcja czytająca numer ze standardowego wejścia.
 * Funkcja czyta numer ze standardowego wejścia, przy założeniu, że @p sign jest
 * pierwszą cyfrą numeru. Uaktualnia też liczbę wczytanych bajtów poprzez wskaźnik
 * @p byte_number oraz rodzaj znaku wczytanego bezpośrednio po numerze
 * poprzez @p following_sign.
 * @param[out] byte_number - wskaźnik na liczbę bajtów.
 * @param[in] sign - pierwsza cyfra numeru.
 * @param[out] following_sign - wskaźnik na rodzaj znaku bezpośrednio po numerze.
 * @return Wskaźnik na napis reprezentujący wczytany numer.
*/
char *read_whole_number(int *byte_number, char sign, int *following_sign);

/** @brief Funkcja szuka numeru w standardowym wejściu.
 * Funkcja szukająca numeru w standardowym wejściu i uruchamiająca
 * funkcję @ref read_whole_number gdy odnajdzie pierwszą cyfrę numeru.
 * Uaktualnia też liczbę wczytanych bajtów poprzez wskaźnik
 * @p byte_number oraz rodzaj znaku wczytanego bezpośrednio po numerze
 * poprzez @p following_sign.
 * @param[out] byte_numbers - wskaźnik na liczbę bajtów.
 * @param[in] AOB - wskaźnik na strukturę ArrayOfBases, używanej do zwolnienia pamięci w przypadku błedu wczytywania.
 * @param[out] following_sign - wskaźnik na rodzaj znaku bezpośrednio po numerze.
 * @param[in] number - wskaźnik na napis reprezentujący wcześniej odnaleziony numer.
 * Używany jest on do zwolnienia pamięci w przypadku błędu.
 * @return Wskaźnik na napis reprezentujący wczytany numer.
*/
char *find_number(int *byte_numbers, ArrayOfBases *AOB, int *following_sign, char *number);

/**
 * Funkcja sprawdzająca czy podany znak jest operatorem.
 * @param[in] sign - znak do sprawdzenia.
 * @return @p true jeśli znak jest operatorem, @p false w przeciwnym razie.
*/
bool is_operator(char sign);

/** @brief Funkcja poszukująca operatora '?' albo '>'.
 * Funkcja przeszukuje standardowe wejście i zwraca odpowiedni
 * enumerator w zależności od tego co odnajdzie. Uaktualnia też liczbę wczytanych bajtów.
 * @param[in] AOB - wskaźnik na strukturę ArrayOfBases, używanej do zwolnienia pamięci w przypadku błedu wczytywania.
 * @param[out] byte_number - wskaźnik na liczbę bajtów.
 * @param[in] number - wskaźnik na napis, który trzeba zwolnić w przypadku błędu wczytywania.
 * @return LARGER_CHARACTER, jeśli pierwszym operatorem będzie znak '>'/
           Q_MARK, jeśli pierwszym operatorem będzie znak '?'.
           COMMENT, jeśli pierwszym operatorem będzie znak '$'.
           -1 w przeciwnym razie.
*/
int search_for_operator(ArrayOfBases *AOB, int *byte_number, char *number);

/**
 * Funkcja sprawdzająca czy @p enum_type jest enumeratorem odpowiadającym komentarzowi.
 * @param[in] enum_type - wartość do sprawdzenia.
 * @return @p true, jeśli enum_type równa się COMMENT, COMMENT_ON, albo COMMENT_ALMOST_DONE.
           @p false w przeciwnym razie.
*/
bool is_comment(int enum_type);

/** 
 * Funkcja sprawdzająca czy @p x jest
 * znakiem odpowiadającym numerowi.
 * @param[in] x - znak do sprawdzenia.
 * @return @p true, jeśli znak odpowiada numerowi,
	   @p false w przeciwnym razie.
*/
bool is_number(char x);


/** @brief Funkcja służąca do ingnorowania komentarzy.
 * Funkcja ignoruje wszystko co jest komentarzem na standardowym wejściu. Uaktualnia też liczbę bajtów
 * wczytywanych przez program.
 * @param[out] byte_number - wskaźnik na liczbę bajtów.
 * @param[in] AOB - wskaźnik na strukturę ArrayOfBases, używanej do zwolnienia pamięci w przypadku błedu wczytywania.
*/
void handle_comment(ArrayOfBases *AOB, int *byte_number);

/**
 * Funkcja sprawdzająca, czy podany znak jest
 * prawidłowym identyfikatorem.
 * @param[in] sign - znak, który jest badany.
 * @return @p true, jeśli znak jest prawidłowym identyfikatorem,
           @p false w przeciwnym razie.
*/
bool correct_ID(char sign);

/** @brief Funkcja wczytująca identyfikator.
 * Funkcja wczytuje identyfikator ze standardowego wejścia. Uaktualnia też liczbę bajtów
 * oraz przekazuje typ znaku wczytanego bezpośrednio po identyfikatorze.
 * @param[out] byte_numbers - wskaźnik na liczbę bajtów.
 * @param[in] AOB - wskaźnik na strukturę ArrayOfBases, używanej do zwolnienia pamięci w przypadku błedu wczytywania.
 * @param[out] type_of_input - Typ znaku wczytanego bezpośrednio po identyfikatorze.
 * @param[in] operator - typ operatora tuż przed użyciem tej funkcji.
 * @return Wskaźnik na wczytany identyfikator.
*/
char *get_string(ArrayOfBases *AOB, int *byte_numbers, int *type_of_input, size_t operator);

/** @brief Funkcja wczytuje resztę operatora.
 * Funkcja powinna zostać uruchomiona po zobaczeniu znaku 'N' albo 'D'.
 * Wczytuje ona resztę operatora i zwraca wskaźnik na wczytany napis. 
 * Uaktualnia też liczbę bajtów oraz przekazuje typ znaku wczytanego
 * bezpośrednio po operatorze.
 * @param[out] byte_numbers - wskaźnik na liczbę bajtów.
 * @param[in] AOB - wskaźnik na strukturę ArrayOfBases, używanej do zwolnienia pamięci w przypadku błedu wczytywania.
 * @param[out] type_of_input - Typ znaku wczytanego bezpośrednio po identyfikatorze.
 * @return Wskaźnik na wczytany operator.
*/
char *read_rest_of_operator(ArrayOfBases *AOB, int *byte_numbers, int *type_of_input);

/** @brief Funkcja zajmująca się obsługą wejścia.
 * Funkcja wczytuje wszystkie znaki na standardowym wejściu i zajmuje się ich
 * obsługą zgodnie z wymaganiami programu.
 * @param[in] AOB - wskaźnik na strukturę ArrayOfBases, która musi zostać zaalokowana poza tą funkcją.
 * @param[in] current_base - wskaźnik na aktualną bazę, w szczególności może byc @p NULL.
*/
void handle_input(ArrayOfBases *AOB, PfBase *current_base);

/** Funkcja zwraca maks z dwóch liczb.
 * @param[in] a - jedna z dwóch porównywanych liczb.
 * @param[in] b - druga z dwóch porównywanych liczb.
 * @return Większa z dwóch porównywanych liczb. Jeśli liczby są sobie równe, 
	     * zwraca dowolną z nich.
*/
int max(int a, int b);

/** Oblicza liczbę cyfr w napisie @p set.
 * @param[in] set - wskaźnik na napis, w którym szukamy liczby cyfr.
 * @return Liczba cyfr w napisie @p set.
*/
size_t count_digits(char *set);
#endif // _PHONE_FORWARD_PARSER_H_

