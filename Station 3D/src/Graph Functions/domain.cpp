#include "domain.h"

/* === SKYING DOMAIN CONSTRUCTORS AND DESTRUCTORS ===
 * Create the skying domain graph from the database filename */
domain_t *create_domain(const char *filename)
{
        /* VARIABLES */
        int i, j, length;
        FILE *fp;
        char buf[51], buf_2[5];  /* Temporary buffers for file scanning */
        domain_t *domain;        /* Future domain pointer */
        node_vector *nv;         /* Future domain's node vector pointer */
        edge_vector *ev;         /* Future domain's edge vector pointer */


        /* Main allocation */
        domain = (domain_t *) malloc_ex(sizeof(domain_t));

        /* Containers that associates for every edge name the correct edge type
         * ! Buses dont have a wait and route time
         * We'll just use the wait time place holder for the 1600-2000 BUS
         * and the route time place holder for the 1800-2000 BUS */
        struct SEdgeTypesWithStringsAndTimes *edge_type = get_domain_edge_types(domain);
        edge_type[0].type = V; strcpy(edge_type[0].str,"V"); edge_type[0].wait_time = 0; edge_type[0].route_time = 0;
        edge_type[1].type = B; strcpy(edge_type[1].str,"B"); edge_type[1].wait_time = 0; edge_type[1].route_time = 0;
        edge_type[2].type = R; strcpy(edge_type[2].str,"R"); edge_type[2].wait_time = 0; edge_type[2].route_time = 0;
        edge_type[3].type = N; strcpy(edge_type[3].str,"N"); edge_type[3].wait_time = 0; edge_type[3].route_time = 0;
        edge_type[4].type = KL; strcpy(edge_type[4].str,"KL"); edge_type[4].wait_time = 0; edge_type[4].route_time = 0;
        edge_type[5].type = SURF; strcpy(edge_type[5].str,"SURF"); edge_type[5].wait_time = 0; edge_type[5].route_time = 0;
        edge_type[6].type = TPH; strcpy(edge_type[6].str,"TPH"); edge_type[6].wait_time = 0; edge_type[6].route_time = 0;
        edge_type[7].type = TC; strcpy(edge_type[7].str,"TC"); edge_type[7].wait_time = 0; edge_type[7].route_time = 0;
        edge_type[8].type = TSD; strcpy(edge_type[8].str,"TSD"); edge_type[8].wait_time = 0; edge_type[8].route_time = 0;
        edge_type[9].type = TS; strcpy(edge_type[9].str,"TS"); edge_type[9].wait_time = 0; edge_type[9].route_time = 0;
        edge_type[10].type = TK; strcpy(edge_type[10].str,"TK"); edge_type[10].wait_time = 0; edge_type[10].route_time = 0;
        edge_type[11].type = BUS; strcpy(edge_type[11].str,"BUS"); edge_type[11].wait_time = 0; edge_type[11].route_time = 0;

        /* LOADING */
        /* Open up the database file, binary mode does not break fseek */
        fp = fopen_ex(filename, "rb");

        fscanf_ex(0, fp, "/* Donnees du graphe de la station de ski\n");
        fscanf_ex(0, fp, "Copyright: Valentin Mercier | Antoine Loret */\n");



        /* Load edge times */
        fscanf_ex(0, fp, "/* TEMPS DE PARCOURS DES PISTES ET REMONTEES */\n");
        fscanf_ex(1, fp, "PISTE_VERTE = %d\n", &edge_type[0].route_time);
        fscanf_ex(1, fp, "PISTE_BLEUE = %d\n", &edge_type[1].route_time);
        fscanf_ex(1, fp, "PISTE_ROUGE = %d\n", &edge_type[2].route_time);
        fscanf_ex(1, fp, "PISTE_NOIRE = %d\n", &edge_type[3].route_time);
        fscanf_ex(1, fp, "PISTE_KL = %d\n", &edge_type[4].route_time);
        fscanf_ex(1, fp, "PISTE_SURF = %d\n", &edge_type[5].route_time);
        fscanf_ex(1, fp, "TPH_ATTENTE = %d\n", &edge_type[6].wait_time);
        fscanf_ex(1, fp, "TPH_PARCOURS = %d\n", &edge_type[6].route_time);
        fscanf_ex(1, fp, "TC_ATTENTE = %d\n", &edge_type[7].wait_time);
        fscanf_ex(1, fp, "TC_PARCOURS = %d\n", &edge_type[7].route_time);
        fscanf_ex(1, fp, "TSD_ATTENTE = %d\n", &edge_type[8].wait_time);
        fscanf_ex(1, fp, "TSD_PARCOURS = %d\n", &edge_type[8].route_time);
        fscanf_ex(1, fp, "TS_ATTENTE = %d\n", &edge_type[9].wait_time);
        fscanf_ex(1, fp, "TS_PARCOURS = %d\n", &edge_type[9].route_time);
        fscanf_ex(1, fp, "TK_ATTENTE = %d\n", &edge_type[10].wait_time);
        fscanf_ex(1, fp, "TK_PARCOURS = %d\n", &edge_type[10].route_time);
        fscanf_ex(1, fp, "BUS_ARCS_1600_2000 = %d\n", &edge_type[11].wait_time);
        fscanf_ex(1, fp, "BUS_ARCS_1800_2000 = %d\n\n", &edge_type[11].route_time);



        /* START ALLOCATING DOMAIN GRAPH */


        /* Load nodes */
        fscanf_ex(0, fp, "/* POINTS */\n"),
        fscanf_ex(1, fp, "%d\n", &length);
        domain->nodes = make_node_vector(length, ALLOCATE_EVERY_COMPONENT);
        nv = get_domain_nodes(domain);


        /* For each node ... */
        for (i = 0; i < nv->length; i++) {
                /* Read node data from file */
                fscanf_ex(5, fp, "%*d%*[ \t\n]%50s%*[ \t\n]%d%*[ \t\n]%f%*[ \t\n]%f%*[ \t\n]%f\n",
                          buf, &nv->items[i]->altitude, &nv->items[i]->pos_x, &nv->items[i]->pos_y, &nv->items[i]->pos_z);
                nv->items[i]->name = strdup_ex(buf);

                /* Fill complementary informations */
                nv->items[i]->is_real_point = (strtol(buf, NULL, 10)) ? 0 : 1;
                nv->items[i]->id = i;
        }




        /* Load edges */
        fscanf_ex(0, fp, "\n/* PISTES ET REMONTEES */\n");
        fscanf_ex(1, fp, "%d\n", &length);
        domain->edges = make_edge_vector(length, ALLOCATE_EVERY_COMPONENT);
        ev = get_domain_edges(domain);


        /* For each edge ... */
        for (i = 0; i < ev->length; i++) {
                /* Read edge data from file */
                fscanf_ex(5, fp, "%*d%*[ \t\n]%50s%*[ \t\n]%4s%*[ \t\n]%d%*[ \t\n]%d%*[ \t\n]%d\n",
                          buf, buf_2, &ev->items[i]->parent, &ev->items[i]->child, &ev->items[i]->awesomeness);
                ev->items[i]->parent--;
                ev->items[i]->child--;
                ev->items[i]->name = strdup_ex(buf);
                ev->items[i]->id = i;

                /* Fill complementary informations */
                ev->items[i]->id = 1;
                for (j = 0; j < NUMBER_OF_EDGE_TYPES; j++) {
                        if (strcmp(buf_2, edge_type[j].str) == 0) {

                                /* Specify edge type */
                                ev->items[i]->type = edge_type[j].type;

                                /* Calculate edge length */
                                length = get_edge_length(domain, i);

                                /* Then specify edge time */
                                if (ev->items[i]->type != BUS)
                                        ev->items[i]->time = edge_type[j].wait_time + ((float)length/100.0) * edge_type[j].route_time;

                                else switch(length) {
                                        /* ARC 1800-2000 */
                                        case 102: ev->items[i]->time = edge_type[11].route_time;
                                                break;
                                        /* ARC 1600-2000 */
                                        case 507: ev->items[i]->time = edge_type[11].wait_time;
                                                break;
                                        default: die("Invalid file format");
                                }
                                break;
                        }
                }

                /* If informations could not be found then the file is
                 * corrupted */
                if (j == NUMBER_OF_EDGE_TYPES)
                        die("Invalid file format");

        }


        /* Close the database file */
        if (!feof(fp))
                die("End of file not reached");

        fclose_ex(fp);
        return domain;
}

/* Domain destructor */
void unload_domain(domain_t *domain)
{
        remove_node_vector(get_domain_nodes(domain));
        remove_edge_vector(get_domain_edges(domain));
        free_ex(domain);
}

/* Domain edge route times updater */
void update_domain_route_times(domain_t *domain)
{
        int i, j, length;
        edge_vector *ev = get_domain_edges(domain);
        struct SEdgeTypesWithStringsAndTimes *edge_type = get_domain_edge_types(domain);

        /* For each edge */
        for (i = 0; i < ev->length; i++) {
                for (j = 0; j < NUMBER_OF_EDGE_TYPES; j++) {
                        if (ev->items[i]->type == edge_type[j].type) {
                                /* Calculate edge length */
                                length = get_edge_length(domain, i);

                                /* Then specify edge time */
                                ev->items[i]->time = edge_type[j].wait_time + ((float)length/100.0) * edge_type[j].route_time;
                                break;
                        }
                }
        }
}






/* === HELPER FUNCTIONS FOR NODE AND EDGES VECTORS ===
 * === VECTOR CONSTRUCTORS, GETTERS AND DESTRUCTORS ===
 * GETTERS for the node and edge vector of the domain */
node_vector *get_domain_nodes(domain_t *domain)
{
        return domain->nodes;
}

edge_vector *get_domain_edges(domain_t *domain)
{
        return domain->edges;
}
struct SEdgeTypesWithStringsAndTimes *get_domain_edge_types(domain_t *domain)
{
        return domain->edge_type;
}


/* CONSTRUCTORS of nodes/edges vectors */
node_vector *make_node_vector(int length, int not_empty)
{
        int i;
        node_vector *nv;

        nv = (node_vector *) malloc_ex(sizeof(node_vector));
        nv->length = length;
        nv->has_real_items = not_empty;
        nv->items = (node_t **) calloc_ex(length, sizeof(node_t *));

        if (not_empty)
                for (i = 0; i < length; i++)
                        nv->items[i] = (node_t *) calloc_ex(1, sizeof(node_t));

        return nv;
}
edge_vector *make_edge_vector(int length, int not_empty)
{
        int i;
        edge_vector *ev;

        ev = (edge_vector *) malloc_ex(sizeof(edge_vector));
        ev->length = length;
        ev->has_real_items = not_empty;
        ev->items = (edge_t **) calloc_ex(length, sizeof(edge_t *));

        if (not_empty)
                for (i = 0; i < length; i++)
                        ev->items[i] = (edge_t *) calloc_ex(1, sizeof(edge_t));

        return ev;
}

/* DESTRUCTORS of nodes/edges vectors */
void remove_node_vector(node_vector *nv)
{
        int i;
        if (nv->has_real_items) {
                for (i = 0; i < nv->length; i++) {
                        nv->items[i]->name ? free_ex(nv->items[i]->name) : (void)0;
                        nv->items[i] ? free_ex(nv->items[i]) : (void)0;
                }
        }

        free_ex(nv->items);
        free_ex(nv);
}
void remove_edge_vector(edge_vector *ev)
{
        int i;
        if (ev->has_real_items) {
                for (i = 0; i < ev->length; i++) {
                        ev->items[i]->name ? free_ex(ev->items[i]->name) : (void)0;
                        ev->items[i] ? free_ex(ev->items[i]) : (void)0;
                }
        }

        free_ex(ev->items);
        free_ex(ev);
}





/* === SERVICE FUNCTIONS === */
/* Gives an explicit node location string */
void get_node_location(char location[255], domain_t *domain, int id)
{
        node_vector *nv;
        edge_vector *ev;

        nv = get_domain_nodes(domain);

        if (nv->items[id]->is_real_point) {
                strcpy(location, nv->items[id]->name);
        } else {
                ev = get_edges_from_node(domain, id);
                if (ev->items[0]) {
                        sprintf(location, "En %s de %s", (ev->items[0]->type <= SURF) ? "haut" : "bas", ev->items[0]->name);
                        remove_edge_vector(ev);
                }
                else {
                        location[0] = '\0';
                        remove_edge_vector(ev);
                        return ;
                }
        }

        int i;
        location[0] = toupper(location[0]);
        for (i = 0; i < strlen(location); i++) {
                if (location[i] == '-')
                        location[i] = ' ';
                if (i >= 1 && location[i - 1] == ' ')
                        location[i] = toupper(location[i]);
        }
}

/* Gets the length in meters of an edge referenced by its ID */
int get_edge_length(domain_t *domain, int id)
{
        node_vector *nv;
        edge_vector *ev;
        node_t *parent, *child;
        nv = get_domain_nodes(domain);
        ev = get_domain_edges(domain);

        parent = nv->items[ev->items[id]->parent];
        child = nv->items[ev->items[id]->child];

        return abs(parent->altitude - child->altitude);
}

/* Get both nodes linked by the specified edge */
node_vector *get_nodes_from_edge(domain_t *domain, int id)
{
        node_vector *nv, *r_nv;
        edge_vector *ev;

        nv = get_domain_nodes(domain);
        ev = get_domain_edges(domain);
        r_nv = make_node_vector(2, DONT_ALLOCATE_ANY_COMPONENT);

        /* Get source and destination nodes */
        r_nv->items[0] = nv->items[ev->items[id]->parent];
        r_nv->items[1] = nv->items[ev->items[id]->child];

        return r_nv;
}

/* Gets edges starting from the specified node */
edge_vector *get_edges_from_node(domain_t *domain, int id)
{
        edge_vector *ev, *r_ev;
        int i, j;

        ev = get_domain_edges(domain);

        /* Allocate return vector */
        r_ev = make_edge_vector(ev->length, DONT_ALLOCATE_ANY_COMPONENT);

        /* Get the edges starting from the node */
        j = 0;
        for (i = 0; i < ev->length; i++) {
                if (ev->items[i]->parent == id) {
                        r_ev->items[j] = ev->items[i];
                        j++;
                }
        }
        /* This is an abstract vector that only has edges references so
         * reseting its length will not cause any harm because there is no real
         * data allocated inside this vector.
         * Moreover, for precision purposes this must be done */
        r_ev->length = j;
        return r_ev;
}
/* Gets the edge id that link both specified nodes */
int get_edge_from_nodes(domain_t *domain, int source, int destination)
{
        int i;
        edge_vector *ev = get_domain_edges(domain);
        for (i = 0; ev->items[i]->parent != source
             || ev->items[i]->child != destination; i++);
        return i;
}



/* === PATH FINDING FUNCTIONS === */
/* Use Dijkstra's algorithm to find the fastest path between two nodes */
edge_vector *get_path(enum EPathType filter, domain_t *domain, int source, int destination)
{
        unsigned int i, remaining, shortest, n1, n2, weight;
        edge_vector *ev, *r_ev;

        /* List of distance labels and of visited nodes, initialized at 0 */
        int domain_size = get_domain_nodes(domain)->length;
        unsigned int *distance = (unsigned int *)calloc_ex(domain_size, sizeof(unsigned int));
        unsigned int *visited = (unsigned int *)calloc_ex(domain_size, sizeof(unsigned int));

        /* List of node identifiers that are predecessors of the visited node
         * referenced by its own ID */
        int *predecessors = (int *)malloc_ex(domain_size * sizeof(int));

        /* Set every distance label to "infinite" (a.k.a UINT_MAX) */
        for(i = 0; i < domain_size; i++)
            distance[i] = UINT_MAX;

        /* Set source node distance to 0 */
        distance[source] = 0;

        /* While there are still some nodes to analyze */
        remaining = domain_size;
        while (remaining) {

                /* Get closest node */
                shortest = UINT_MAX;
                for (i = 0; i < domain_size; i++) {
                        if (!visited[i] && distance[i] < shortest) {
                                n1 = i;
                                shortest = distance[i];
                        }
                }
                visited[n1] = 1;
                remaining--;

                /* Update distances */
                ev = get_edges_from_node(domain, n1);
                for (i = 0; i < ev->length; i++) {
                        n2 = ev->items[i]->child;
                        switch (filter) {
                        case FASTEST: weight = ev->items[i]->time;
                                        break;
                        case NICEST: weight = AWESOMENESS_MAX*500 + 1;
                                        weight -= ev->items[i]->awesomeness*500;
                                        break;
                        case EASIEST: weight = 1;
                                        weight += (ev->items[i]->type == R) ? 500 : 0;
                                        weight += (ev->items[i]->type == N) ? 5000 : 0;
                                        break;
                        default: die("Wrong path finding filter");
                        }

                        if (distance[n2] > distance[n1] + weight) {
                                distance[n2] = distance[n1] + weight;
                                predecessors[n2] = n1;
                                /* If the closest node is the destination one
                                 * we stop here */
                                if (n2 == destination) {
                                        remaining = 0;
                                        break;
                                }
                        }
                }
                remove_edge_vector(ev);
        }

        /* Now we can determine the fastest path, by going through the graph
         * the way back
         * Firsty we create a node vector with the potential maximum of nodes
         * inside it. This is domain_size because there is no way we pass
         * twice by the same node.
         * Although we pass the DONT_ALLOCATE_ANY_COMPONENT parameter
         * so that we dont allocate stupidly domain_size items.
         * The items will just be pointers to the actual nodes and once this
         * vector is deleted, only these pointers will be free'd and not
         * the actual items */
        ev = make_edge_vector(domain_size, DONT_ALLOCATE_ANY_COMPONENT);
        i = 0;
        int tmp_edge_id;
        while (n2 != source) {
                /* We add every node of the path to the vector */
                tmp_edge_id = get_edge_from_nodes(domain, predecessors[n2], n2);
                ev->items[i] = get_domain_edges(domain)->items[tmp_edge_id];
                i++;
                /* We find the previous node */
                n2 = predecessors[n2];
        }

        /* This is an abstract vector that only has edges references so
         * reseting its length will not cause any harm because there is no real
         * data allocated inside this vector.
         * Moreover, for precision purposes this must be done */
        ev->length = i;

        /* As the edge vector is created by going through the path backwards
         * we need to invert it. This does not cost much memory data, and is
         * much more convenient */
        r_ev = make_edge_vector(ev->length, DONT_ALLOCATE_ANY_COMPONENT);
        for (i = 0; i < ev->length; i++)
                r_ev->items[i] = ev->items[ev->length - i - 1];
        remove_edge_vector(ev);


        /* Free memory */
        free_ex(distance);
        free_ex(visited);
        free_ex(predecessors);

        return r_ev;
}
