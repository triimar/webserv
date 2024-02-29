#include "../../include/utils.hpp"

void free_2d_array(void **array) {
    if (array == NULL) {
        return ;
    }
    for (size_t i = 0; array[i] != NULL; ++i) {
        free(array[i]) ;
    }
    free(array);
}
