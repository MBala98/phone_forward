#include "phone_forward_parser.h"
#include "phone_forward.h"

int main() {
    ArrayOfBases *AOB = initialize_array_of_bases();
    handle_input(AOB, NULL);
    clear(AOB);
    return 0;
}
