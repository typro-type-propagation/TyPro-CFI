#include <stdlib.h>

typedef void * PTR;
typedef unsigned int hashval_t;
typedef hashval_t (*htab_hash) (const void *);
typedef int (*htab_eq) (const void *, const void *);
typedef void (*htab_del) (void *);
typedef int (*htab_trav) (void **, void *);
static int type_hash_marked_p (const void *);

#define EMPTY_ENTRY    ((PTR) 0)
#define DELETED_ENTRY  ((PTR) 1)

struct htab
{
  /* Pointer to hash function.  */
  htab_hash hash_f;

  /* Pointer to comparison function.  */
  htab_eq eq_f;

  /* Pointer to cleanup function.  */
  htab_del del_f;

  /* Table itself.  */
  PTR *entries;

  /* Current size (in entries) of the hash table */
  size_t size;

  /* Current number of elements including also deleted elements */
  size_t n_elements;

  /* Current number of deleted elements in the table */
  size_t n_deleted;

  /* The following member is used for debugging. Its value is number
     of all calls of `htab_find_slot' for the hash table. */
  unsigned int searches;

  /* The following member is used for debugging.  Its value is number
     of collisions fixed for time of work with the hash table. */
  unsigned int collisions;

  /* This is non-zero if we are allowed to return NULL for function calls
     that allocate memory.  */
  int return_allocation_failure;
};

typedef int (*ggc_htab_marked_p) (const void *);
typedef void (*ggc_htab_mark) (const void *);
typedef struct htab *htab_t;

htab_t type_hash_table;

struct d_htab_root
{
  struct d_htab_root *next;
  htab_t htab;
  ggc_htab_marked_p marked_p;
  ggc_htab_mark mark;
};

static struct d_htab_root *d_htab_roots;

void
htab_traverse (htab, callback, info)
     htab_t htab;
     htab_trav callback;
     void* info;
{
  void* *slot = htab->entries;
  void* *limit = slot + htab->size;

  do
    {
      void* x = *slot;

      if (x != EMPTY_ENTRY && x != DELETED_ENTRY)
  if (!(*callback) (slot, info))
    break;
    }
  while (++slot < limit);
}

extern void ggc_add_deletable_htab (void*, ggc_htab_marked_p, ggc_htab_mark);
extern int ggc_marked_p (const void *);
static int type_hash_marked_p (const void *);
static void type_hash_mark (const void *);

void ggc_add_deletable_htab (x, marked_p, mark)
     void* x;
     ggc_htab_marked_p marked_p;
     ggc_htab_mark mark;
{
  struct d_htab_root *r
    = (struct d_htab_root *) malloc (sizeof (struct d_htab_root));

  r->next = d_htab_roots;
  r->htab = (htab_t) x;
  r->marked_p = marked_p ? marked_p : ggc_marked_p;
  r->mark = mark;
  d_htab_roots = r;
}

void set_fp() {
  ggc_add_deletable_htab (type_hash_table, type_hash_marked_p, type_hash_mark);
}

// "target"
static int
ggc_htab_delete (slot, info)
     void **slot;
     void *info;
{
  struct d_htab_root *r = (struct d_htab_root *) info;

  if (! (*r->marked_p) (*slot)) {
  } else if (r->mark)
    (*r->mark) (*slot);

  return 1;
}

void ggc_mark_roots () {
  struct d_htab_root *y;
  for (y = d_htab_roots; y != NULL; y = y->next)
    htab_traverse (y->htab, ggc_htab_delete, (void*) y);
}