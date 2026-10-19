#include "string_view.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>


char* sv_to_c_str(const string_view sv) {

    return strndup(sv.str, sv.count);
}


string_view sv_from_c_str(char* str) {

    return (string_view) {

        .str = str,
        .count = str ? strlen(str) : 0,
    };
}


int sv_neq_cstr(const string_view s1, const char* s2, const int n) {

    assert(n >= 0);

    int len = n;

    if (n > s1.count)
        len = s1.count;

    const int len2 = (const int)strlen(s2);

    if (n > len2)
        len = len2;

    return !strncmp(s1.str, s2, len);
}


int sv_neq(const string_view s1, const string_view s2, const int n) {

    assert(n >= 0);

    int len = n;

    if (n > s1.count)
        len = s1.count;

    if (n > s2.count)
        len = s2.count;

    return !memcmp(s1.str, s2.str, len);
}


string_view sv_substr(const string_view sv, const int start, const int end) {

    assert(start <= end && start >= 0 && end >= 0 && start < sv.count && end < sv.count);

    return (string_view) {

        .count = end - start + 1,
        .str = sv.str + start,
    };
}


void sv_chop_nleft(string_view* sv, int n) {

    n = (n > sv->count) ? sv->count : n;
    sv->count -= n;
    sv->str += n;
}


void sv_chop_nright(string_view* sv, int n) {

    n = (n > sv->count) ? sv->count : n;
    sv->count -= n;
}


void sv_chop_n(string_view* sv, int n) {

    sv_chop_nleft(sv, n);
    sv_chop_nright(sv, n);
}


void sv_trim_left(string_view* sv) {

    while (sv->count > 0 && isspace((unsigned char)sv->str[0]))
        sv_chop_left(sv);
}


void sv_trim_right(string_view* sv) {

    while (sv->count > 0 && isspace((unsigned char)sv->str[sv->count - 1]))
        sv_chop_right(sv);
}


void sv_trim(string_view* sv) {

    sv_trim_left(sv);
    sv_trim_right(sv);
}


string_view sv_chop_by_delim(string_view* sv, char delim) {

    int i = 0;
    while (i < sv->count && sv->str[i] != delim)
        i++;

    if (i < sv->count) {

        string_view token = {
            
            .str = sv->str,
            .count = i,
        };

        sv_chop_nleft(sv, i + 1);
        return token;
    }

    string_view token = *sv;
    sv_chop_nleft(sv, sv->count);

    return token;
}


string_view sv_chop_by_type(string_view* sv, int (*istype)(int c)) {


    int i = 0;
    while (i < sv->count && !istype((unsigned char)sv->str[i]))
        i++;

    if (i < sv->count) {

        string_view token = {
            
            .str = sv->str,
            .count = i,
        };

        sv_chop_nleft(sv, i + 1);
        return token;
    }

    string_view token = *sv;
    sv_chop_nleft(sv, sv->count);

    return token;
}





