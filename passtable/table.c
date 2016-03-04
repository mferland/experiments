#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 Algorithm
 =========

 This algorithm distributes a pool of characters to 'n' password
 streams. These streams can then generate passwords that are mutually
 exclusive. For example:

 pool: a,b,c

 Distribute to 4 streams of 3 characters:
          char0 char1 char2
 stream0: [a,a] [a,a] [a,c] --> aaa, baa, caa
 stream1: [a,a] [b,c] [a,c] --> aba, aca, bba, bca, cba, cca
 stream2: [b,b] [a,c] [a,c] --> aab, abb, acb, bab, bbb, bcb, cab, cbb, ccb
 stream3: [c,c] [a,c] [a,c] --> aac, abc, acc, bac, bbc, bcc, cac, cbc, ccc

 The output of the algorithm is a table of streams*characters
 entries. Each entry contains a start and end offset to the character
 pool. Using these offsets we can easily loop over all possible
 'strings' using simple for-loops.

 Pseudo-code (again for 3 characters strings):
 for (i = table[stream0, 2].first; i <= table[stream0, 2].last; ++i)
    str[0] = pool[i];
    for (j = table[stream0, 1].first; j <= table[stream0, 1].last; ++j)
       str[1] = pool[j];
       for (k = table[stream0, 0].first; k <= table[stream0, 0].last; ++k)
          str[2] = pool[k];
          do_stuff(str);

  Entry table
  ===========

  In-memory representation:
  +----+----+----+----+----+----+
  |s0c0|s1c0|s0c1|s1c1|s0c2|s1c2|
  +----+----+----+----+----+----+

  Conceptual representation:

   2 columns (streams)
   ____+____
  /         \
  +----+----+ \
  |s0c0|s1c0|  |
  +----+----+  |
  |s0c1|s1c1|  + 3 rows (characters)
  +----+----+  |
  |s0c2|s1c2|  |
  +----+----+ /

 */

struct entry {
    int first, last;
};

struct table {
    struct entry *entry;
    size_t rows;
    size_t cols;
    size_t seqlen;
};

static int compare_entries(const void *a, const void *b)
{
  const struct entry *ea = (const struct entry *)a;
  const struct entry *eb = (const struct entry *)b;

  /* since entries are always mutually exclusive, compare only the
   * 'first' member */
  return (ea->first > eb->first) - (ea->first < eb->first);
}

static void sort(struct entry *e, size_t streams)
{
    qsort(e, streams, sizeof(struct entry), compare_entries);
}

/**
 * Get entry pointer at [row][col].
 */
static struct entry *get(struct table *t, size_t row, size_t col)
{
    return &t->entry[t->cols * row + col];
}

/**
 * Split the given sequence of length 'len' in streams.
 * i.e.: 1,2,3,4,5 in 3 streams ==> [1,2],[3,4],[5]
 */
static void split_less(size_t seqlen, size_t streams, struct entry *t)
{
    for (size_t i = 0; i < streams; ++i) {
        t[i].first = (i * seqlen) / streams;
        t[i].last = ((i + 1) * seqlen) / streams - 1;
    }
}

/**
 * 1,2,3 in 4 streams ==> [1],[1],[2],[3]
 */
static void split_more(size_t seqlen, size_t streams, struct entry *e)
{
    for (size_t i = 0; i < streams; ++i) {
        e[i].first = i % seqlen;
        e[i].last = i % seqlen;
    }
    sort(e, streams);
}

/**
 * 1,2,3,4 in 4 streams ==> [1],[2],[3],[4]
 */
static void split_equal(size_t seqlen, struct entry *e)
{
    for (size_t i = 0; i < seqlen; ++i) {
        e[i].first = i;
        e[i].last = i;
    }
}

/**
 * Distribute sequence @seq in @streams and store the result at @entry.
 */
static void distribute(size_t seqlen, size_t streams, struct entry *entry)
{
    if (streams == 1) {
        entry->first = 0;
        entry->last = seqlen - 1;
    } else if (streams == seqlen) {
        split_equal(seqlen, entry);
    } else if (streams > seqlen) {
        split_more(seqlen, streams, entry);
    } else
        split_less(seqlen, streams, entry);
}

/**
 * Compares entries e1 and e2.
 */
static bool is_equal_entries(const struct entry *e1, const struct entry *e2)
{
    return (e1->first == e2->first && e1->last == e2->last);
}

/**
 * Initialize entry table with default values.
 */
static void entry_table_init(struct table *t, int first, int last)
{
    for (size_t i = 0; i < t->rows * t->cols; ++i) {
        t->entry[i].first = first;
        t->entry[i].last = last;
    }
}

/**
 * Count the number of entries identical to @e on row @row.
 */
static size_t count_identical(struct table *t, size_t row, const struct entry *e)
{
    size_t count = 0;
    struct entry *n = get(t, row, 0);

    for (size_t i = 0; i < t->cols; ++i) {
        if (is_equal_entries(&n[i], e))
            ++count;
    }
    return count;
}

/**
 * Count the number of entries identical to @e and consider @len
 * entries.
 */
static size_t count_identical_from_entry(const struct entry *e, size_t len)
{
    size_t count = 1;           /* first entry is always equal */
    for (size_t i = 1; i < len; ++i) {
        if (is_equal_entries(&e[i], e))
            ++count;
    }
    return count;
}

static void recurse(struct table *t, size_t count, struct entry *e)
{
    if (count == 1)
        return;

    distribute(t->seqlen, count, e);

    size_t identical = 0;
    for (size_t i = 0; i < count; i += identical) {
        identical = count_identical_from_entry(&e[i], count);
        recurse(t, identical, &e[i + t->cols]);
    }
}



static void generate(size_t seqlen, size_t pwlen, size_t streams,
                     struct table **table)
{
    struct table *t;

    /* TODO: sanitize input */
    /* TODO: rename cols --> streams ????? */
    /* TODO: rename rows --> ?????? */

    t = calloc(1, sizeof(struct table));

    t->rows = pwlen;
    t->cols = streams;
    t->entry = malloc(sizeof(struct entry) * t->rows * t->cols);
    t->seqlen = seqlen;

    entry_table_init(t, 0, seqlen - 1);

    /* create start point */
    distribute(seqlen, t->cols, t->entry);

    size_t count = 0;
    for (size_t i = 0; i < t->cols; i+=count) {
        count = count_identical(t, 0, &t->entry[i]);

        /* if current entry has duplicates */
        if (count != 1) {
            recurse(t, count, get(t, 1, i));
        }
    }

    *table = t;
}

static void print_entries(const struct entry *e, size_t len)
{
    for (size_t i = 0; i < len; ++i)
        printf("%d, %d\n", e[i].first, e[i].last);
    puts("");
}

static void print_table(struct table *t)
{
    for (size_t i = 0; i < t->cols; ++i) {
        for (size_t j = 0; j < t->rows; ++j) {
            const struct entry *entry = get(t, j, i);
            if (j != t->rows - 1)
                printf("%d,%d - ", entry->first, entry->last);
            else
                printf("%d,%d\n", entry->first, entry->last);
        }
    }
}

int main(int argc, char *argv[])
{
    struct table *table;
    struct entry *e;

    e = malloc(sizeof(struct entry) * 6); /* allocate for largest */

    distribute(5, 3, e);
    print_entries(e, 3);

    distribute(5, 4, e);
    print_entries(e, 4);

    distribute(5, 5, e);
    print_entries(e, 5);

    distribute(5, 6, e);
    print_entries(e, 6);

    distribute(5, 1, e);
    print_entries(e, 1);

    free(e);

    generate(3, 5, 32, &table);
    print_table(table);
    free(table->entry);
    free(table);

    return 0;
}
