/* This file defines the whole skying domain graph data scheme (called domain)
 * and the functions allocate, load, manage its content and erase it. */

#ifndef DOMAIN_H
#define DOMAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "libsaferesource.h"
#include "sha1.h"

/* === SKYING DOMAIN STRUCTURES === */
/* We're talking about a skying domain graph here
 * So we have some nodes that represent places of our domain
 * e.g. Aiguille Rouge
 * and some of them will not be named as they would represent nodes between
 * several ski track, thus the attribute is_real_point */
typedef struct SDomainNode
{
        char *name;
        int altitude, is_real_point;
        float pos_x, pos_y, pos_z;
        int id;
} node_t;




/* Every Ski track or Ski lift will be modelized as an edge of our graph,
 * However there are various types of edges so we define them all here in
 * an enum. Then we need to associate for every edge type, its name and
 * some other properties like the time it takes to go through it, so that we
 * we will read the edge name from the database we will directly be able
 * to tell what data to specify in our edge.
 * Once all these two helper enum and struct done we define the real
 * edge structure for our skying domain.
 *
 * The edges have a track or lift name, a type, and a few attributes :
 * @time: the time it takes to go through it (calculated from the edge type
 * and its respective weigth loaded from the database
 * @parent: the identifier of the source node (directed graph)
 * @child: the identifier of the destination node
 * @awesomeness: a value between 0 and 20 that describes the awesomeness
 * of the track (depends on its beauty, etc...)
 * Lifts have the worst awesomeness */

 #define NUMBER_OF_EDGE_TYPES 12

enum EDomainEdgeType
{
        V,    /* Easy tracks */
        B,    /* Intermediate tracks */
        R,    /* Advanced tracks */
        N,    /* Expert tracks */
        KL,   /* Special speed testing track */
        SURF, /* Snowboard only tracks */

        TPH,  /* Aerial Tramways */
        TC,   /* Gondola lifts */
        TSD,  /* Detachable chairlifts */
        TS,   /* Chairlifts */
        TK,   /* J-bars */

        BUS   /* Bus */
};

struct SEdgeTypesWithStringsAndTimes
{
        enum EDomainEdgeType type;
        char str[5];
        int wait_time, route_time;
};

typedef struct SDomainEdge
{
        char *name;
        enum EDomainEdgeType type;
        int time, parent, child, awesomeness;
        int id;
} edge_t;




/* Now we define two new structures that just are vectors of edges and nodes
 * and we repack them inside a big structure that will be our skying domain
 * graph.
 * ! The attribute @has_real_items tells wether or not the items were actually
 *   created for this vector. If not then they are just pointer to items of
 *   another vector items and thus data will not be removed when the vector
 *   will be deleted. */
typedef struct SNodeVector
{
        int length, has_real_items;
        node_t **items;
} node_vector;

typedef struct SEdgeVector
{
        int length, has_real_items;
        edge_t **items;
} edge_vector;

typedef struct SDomain
{
        node_vector *nodes;
        edge_vector *edges;
        struct SEdgeTypesWithStringsAndTimes edge_type[NUMBER_OF_EDGE_TYPES];
} domain_t;


/* === DOMAIN MANAGEMENT RELATED FUNCTIONS === */
/* Default constructor/destructor of the skying domain
 * Data is loaded from the file specified with the filename */
domain_t *create_domain(const char *filename);
void update_domain_route_times(domain_t *domain);
void unload_domain(domain_t *domain);

/* Getters of the domain vectors */
node_vector *get_domain_nodes(domain_t *domain);
edge_vector *get_domain_edges(domain_t *domain);
struct SEdgeTypesWithStringsAndTimes *get_domain_edge_types(domain_t *domain);


/* Helper functions that construct/destruct node and edge vectors
 * As we need to pass a not_empty boolean parameter to the constructors to
 * specify wether or not we should allocate every single node/edge in the vector
 * we define two boolean true/false maccros for better code readibility */
#define ALLOCATE_EVERY_COMPONENT 1
#define DONT_ALLOCATE_ANY_COMPONENT 0

node_vector *make_node_vector(int length, int not_empty);
edge_vector *make_edge_vector(int length, int not_empty);
void remove_node_vector(node_vector *nv);
void remove_edge_vector(edge_vector *ev);



/* Service functions that make relations between nodes and edges
 * get_node_location(): returns the name of the node and if it is not a place
 * its location between edges
 * get_nodes_from_edge(): returns the parent and the child of the specified edge
 * get_edges_from_node(): get all the edges leaving the specified node
 * get_edge_from_nodes(): get the edge id that link both specified nodes */
void get_node_location(char location[255], domain_t *domain, int id);
int get_edge_length(domain_t *domain, int id);
node_vector *get_nodes_from_edge(domain_t *domain, int id);
edge_vector *get_edges_from_node(domain_t *domain, int id);
int get_edge_from_nodes(domain_t *domain, int source, int destination);


/* Path finding function */
#define AWESOMENESS_MAX 20
enum EPathType
{
        FASTEST = 1,
        NICEST = 1 << 1,
        EASIEST = 1 << 2
};
edge_vector *get_path(enum EPathType filter, domain_t *domain, int source,int destination);


#endif
