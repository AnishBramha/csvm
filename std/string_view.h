#pragma once


typedef struct {

    char* str;
    int count;

} string_view;


string_view sv_from_c_str(char*);
void sv_chop_nleft(string_view*, int);
void sv_chop_nright(string_view*, int);
void sv_chop_n(string_view*, int);
void sv_trim_left(string_view*);
void sv_trim_right(string_view*);
void sv_trim(string_view*);
string_view sv_chop_by_delim(string_view*, char);
string_view sv_chop_by_type(string_view*, int (*istype)(int c));
int sv_neq(const string_view, const string_view, const int);
int sv_neq_cstr(const string_view, const char*, const int);
string_view sv_substr(const string_view, const int, const int);
char* sv_to_c_str(const string_view);


#define sv_chop_left(sv) sv_chop_nleft((sv), 1)
#define sv_chop_right(sv) sv_chop_nright((sv), 1)
#define sv_chop(sv) sv_chop_n((sv), 1)

#define sv_eq(s1, s2) sv_neq((s1), (s2), (s1).count)
#define sv_eq_cstr(s1, s2) sv_neq_cstr((s1), (s2), (s1).count)

#define sv_fmt "%.*s"
#define sv_arg(sv) (int)(sv).count, (sv).str









