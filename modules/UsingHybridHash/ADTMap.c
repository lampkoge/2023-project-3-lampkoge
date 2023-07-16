/////////////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Map μέσω υβριδικού Hash Table
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "ADTMap.h"
#include "ADTVector.h"

// Κάθε θέση i θεωρείται γεινοτική με όλες τις θέσεις μέχρι και την i + NEIGHBOURS
#define NEIGHBOURS 3


// Δομή του κάθε κόμβου που έχει το hash table (με το οποίο υλοιποιούμε το map)
struct map_node{
	Pointer key;
    Pointer value;
    MapNode next;
};

// Δομή του Map (περιέχει όλες τις πληροφορίες που χρεαζόμαστε για το HashTable)
struct map {
	int size;
    Vector table;
    CompareFunc compare;
    DestroyFunc destroy_key;
    DestroyFunc destroy_value;
    HashFunc hash_function;
};

MapNode map_create_node(Pointer key, Pointer value) {
    MapNode node = malloc(sizeof(struct map_node));
    node->key = key;
    node->value = value;
    node->next = NULL;
    return node;
}

Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	Map map = malloc(sizeof(struct map));
    map->table = vector_create(16, NULL);
    map->compare = compare;
    map->destroy_key = destroy_key;
    map->destroy_value = destroy_value;
    map->hash_function = NULL;
    map->size = 0;
    return map;
}

// Επιστρέφει τον αριθμό των entries του map σε μία χρονική στιγμή.
int map_size(Map map) {
	return map->size;
}

// Εισαγωγή στο hash table του ζευγαριού (key, item). Αν το key υπάρχει,
// ανανέωση του με ένα νέο value, και η συνάρτηση επιστρέφει true.

void map_insert(Map map, Pointer key, Pointer value) {
    int idx;
    if (map->hash_function == NULL) {
        idx = (size_t)key % vector_size(map->table);
    } 
	else {
        idx = map->hash_function(key) % vector_size(map->table);
    }

    MapNode curr = vector_get_at(map->table, idx);
    while (curr != NULL) {
        if (map->compare(curr->key, key) == 0) {
            if (map->destroy_value != NULL) {
                map->destroy_value(curr->value);
            }
            curr->value = value;
            return;
        }
        curr = curr->next;
    }

    for (int i = 0; i <= NEIGHBOURS; i++) {
        int hash_idx2 = (idx + i) % vector_size(map->table);
        if (vector_get_at(map->table, hash_idx2) == NULL) {
            MapNode new_node = map_create_node(key, value);
            vector_set_at(map->table, hash_idx2, new_node);
            map->size++;
            return;
        }
    }

    MapNode new_node = map_create_node(key, value);
    new_node->next = vector_get_at(map->table, idx);
    vector_set_at(map->table, idx, new_node);
    map->size++;
}

// Διαργραφή απο το Hash Table του κλειδιού με τιμή key
bool map_remove(Map map, Pointer key) {
	 if (map == NULL || key == NULL) {
        return false;
    }

    int idx;
    if (map->hash_function == NULL) {
        idx = (size_t)key % vector_size(map->table);
    } 
	else {
        idx = map->hash_function(key) % vector_size(map->table);
    }

    MapNode curr = vector_get_at(map->table, idx);
    MapNode prev = NULL;
    while (curr != NULL) {
        if (map->compare(curr->key, key) == 0) {
            if (prev != NULL) {
                prev->next = curr->next;
            } 
			else {
                vector_set_at(map->table, idx, curr->next);
            }
            if (map->destroy_key != NULL) {
                map->destroy_key(curr->key);
            }
            if (map->destroy_value != NULL) {
                map->destroy_value(curr->value);
            }
            free(curr);
            map->size--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    for (int i = 1; i <= NEIGHBOURS; i++) {
        int hash_idx2 = (idx + i) % vector_size(map->table);
        curr = vector_get_at(map->table, hash_idx2);
        prev = NULL;
        while (curr != NULL) {
            if (map->compare(curr->key, key) == 0) {
                if (prev != NULL) {
                    prev->next = curr->next;
                } 
				else {
                    vector_set_at(map->table, hash_idx2, curr->next);
                }
                if (map->destroy_key != NULL) {
                    map->destroy_key(curr->key);
                }
                if (map->destroy_value != NULL) {
                    map->destroy_value(curr->value);
                }
                free(curr);
                map->size--;
                return true;
            }

            prev = curr;
            curr = curr->next;
        }
    }

    return false;
}

// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.

Pointer map_find(Map map, Pointer key) {
	int idx;
    if (map->hash_function == NULL) {
        idx = (size_t)key % vector_size(map->table);
    } 
	else {
        idx = map->hash_function(key) % vector_size(map->table);
    }

    MapNode curr = vector_get_at(map->table, idx);
    while (curr != NULL) {
        if (map->compare(curr->key, key) == 0) {
            return curr->value;
        }
        curr = curr->next;
    }

    for (int i = 1; i <= NEIGHBOURS; i++) {
        int hash_idx2 = (idx + i) % vector_size(map->table);
        curr = vector_get_at(map->table, hash_idx2);
        while (curr != NULL) {
            if (map->compare(curr->key, key) == 0) {
                return curr->value;
            }
            curr = curr->next;
        }
    }

    return NULL;
}


DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
	DestroyFunc prev = map->destroy_key;
    map->destroy_key = destroy_key;
    return prev;
}

DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	DestroyFunc prev = map->destroy_value;
    map->destroy_value = destroy_value;
    return prev;
}

// Απελευθέρωση μνήμης που δεσμεύει το map
void map_destroy(Map map) {
   for (int i = 0; i < vector_size(map->table); i++) {
        MapNode curr = vector_get_at(map->table, i);
        while (curr != NULL) {
            MapNode next = curr->next;
            if (map->destroy_key != NULL) {
                map->destroy_key(curr->key);
            }
            if (map->destroy_value != NULL) {
                map->destroy_value(curr->value);
            }
            free(curr);
            curr = next;
        }
    }

    vector_destroy(map->table);
    free(map);
}

/////////////////////// Διάσχιση του map μέσω κόμβων ///////////////////////////

MapNode map_first(Map map) {
	for (int i = 0; i < vector_size(map->table); i++) {
        MapNode node = vector_get_at(map->table, i);
        if (node != NULL) {
            return node;
        }
    }

    return NULL;
}

MapNode map_next(Map map, MapNode node) {
	if (node == NULL) {
        return NULL;
    }

    if (node->next != NULL) {
        return node->next;
    }

    int idx;
    if (map->hash_function == NULL) {
        idx = (size_t)node->key % vector_size(map->table);
    } 
	else {
        idx = map->hash_function(node->key) % vector_size(map->table);
    }

    for (int i = idx + 1; i < vector_size(map->table); i++) {
        MapNode next_node = vector_get_at(map->table, i);
        if (next_node != NULL) {
            return next_node;
        }
    }

    return NULL;
}

Pointer map_node_key(Map map, MapNode node) {
	return node->key;
}

Pointer map_node_value(Map map, MapNode node) {
	return node->value;
}

MapNode map_find_node(Map map, Pointer key) {
	int idx;
    if (map->hash_function == NULL) {
        idx = (size_t)key % vector_size(map->table);
    } 
	else {
        idx = map->hash_function(key) % vector_size(map->table);
    }

    MapNode curr = vector_get_at(map->table, idx);
    while (curr != NULL) {
        if (map->compare(curr->key, key) == 0) {
            return curr;
        }
        curr = curr->next;
    }

    for (int i = 1; i <= NEIGHBOURS; i++) {
        int hash_idx2 = (idx + i) % vector_size(map->table);
        curr = vector_get_at(map->table, hash_idx2);
        while (curr != NULL) {
            if (map->compare(curr->key, key) == 0) {
                return curr;
            }
            curr = curr->next;
        }
    }

    return NULL;
}

// Αρχικοποίηση της συνάρτησης κατακερματισμού του συγκεκριμένου map.
void map_set_hash_function(Map map, HashFunc func) {
    map->hash_function = func;
}








uint hash_string(Pointer value) {
	// djb2 hash function, απλή, γρήγορη, και σε γενικές γραμμές αποδοτική
    uint hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;			// hash = (hash * 33) + *s. Το foo << 5 είναι γρηγορότερη εκδοχή του foo * 32.
    return hash;
}

uint hash_int(Pointer value) {
	return *(int*)value;
}

uint hash_pointer(Pointer value) {
	return (size_t)value;				// cast σε sizt_t, που έχει το ίδιο μήκος με έναν pointer
}